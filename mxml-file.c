// Mini-XML的文件加载代码，一个小型的XML文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2003-2024 Michael R Sweet。
//
// 根据Apache许可证v2.0进行许可。更多信息请参见文件“LICENSE”。
//

#ifndef _WIN32
#  include <unistd.h>
#endif // !_WIN32
#include "mxml-private.h"


//
// 本地类型...
//

typedef enum _mxml_encoding_e        // 字符编码
{
  _MXML_ENCODING_UTF8,            // UTF-8
  _MXML_ENCODING_UTF16BE,        // UTF-16大端
  _MXML_ENCODING_UTF16LE        // UTF-16小端
} _mxml_encoding_t;

typedef struct _mxml_stringbuf_s    // 字符串缓冲区
{
  char        *buffer,        // 缓冲区
        *bufptr;        // 指向缓冲区的指针
  size_t    bufsize;        // 缓冲区大小
  bool        bufalloc;        // 是否分配缓冲区？
} _mxml_stringbuf_t;


//
// 宏用于检测不良的XML字符...
//

#define mxml_bad_char(ch) ((ch) < ' ' && (ch) != '\n' && (ch) != '\r' && (ch) != '\t')


//
// 本地函数...
//

static bool        mxml_add_char(mxml_options_t *options, int ch, char **ptr, char **buffer, size_t *bufsize);
static int        mxml_get_entity(mxml_options_t *options, mxml_io_cb_t io_cb, void *io_cbdata, _mxml_encoding_t *encoding, mxml_node_t *parent, int *line);
static int        mxml_getc(mxml_options_t *options, mxml_io_cb_t io_cb, void *io_cbdata, _mxml_encoding_t *encoding);
static inline int    mxml_isspace(int ch)
            {
              return (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
            }
static mxml_node_t    *mxml_load_data(mxml_node_t *top, mxml_options_t *options, mxml_io_cb_t io_cb, void *io_cbdata);
static int        mxml_parse_element(mxml_options_t *options, mxml_io_cb_t io_cb, void *io_cbdata, mxml_node_t *node, _mxml_encoding_t *encoding, int *line);
static size_t        mxml_read_cb_fd(int *fd, void *buffer, size_t bytes);
static size_t        mxml_read_cb_file(FILE *fp, void *buffer, size_t bytes);
static size_t        mxml_read_cb_string(_mxml_stringbuf_t *sb, void *buffer, size_t bytes);
static double        mxml_strtod(mxml_options_t *options, const char *buffer, char **bufptr);
static size_t        mxml_io_cb_fd(int *fd, void *buffer, size_t bytes);
static size_t        mxml_io_cb_file(FILE *fp, void *buffer, size_t bytes);
static size_t        mxml_io_cb_string(_mxml_stringbuf_t *sb, void *buffer, size_t bytes);
static int        mxml_write_node(mxml_node_t *node, mxml_options_t *options, mxml_io_cb_t io_cb, void *io_cbdata, int col);
static int        mxml_write_string(const char *s, mxml_io_cb_t io_cb, void *io_cbdata, bool use_entities, int col);
static int        mxml_write_ws(mxml_node_t *node, mxml_options_t *options, mxml_io_cb_t io_cb, void *io_cbdata, mxml_ws_t ws, int col);


//
// 'mxmlLoadFd（）' - 将文件描述符加载到XML节点树中。
//
// 此函数将文件描述符`fd`加载到XML节点树中。指定文件中的节点将添加到指定节点`top`中 - 如果为`NULL`，则XML文件必须是格式良好的，以文件开头的单个父处理指令节点（例如`<?xml version =“1.0”？>）。
//
// 加载选项通过`options`参数提供。如果为`NULL`，所有值将加载到`MXML_TYPE_TEXT`节点中。在加载XML数据时，请使用@link mxmlOptionsNew@函数创建选项。

mxml_node_t *                // O - 第一个节点，如果无法读取文件，则为`NULL`。
mxmlLoadFd(
    mxml_node_t    *top,        // I - 顶级节点
    mxml_options_t *options,    // I - 选项
    int            fd)            // I - 要读取的文件描述符
{
  // 范围检查输入...
  if (fd < 0)
    return (NULL);

  // 读取XML数据...
  return (mxml_load_data(top, options, (mxml_io_cb_t)mxml_read_cb_fd, &fd));
}


//
// 'mxmlLoadFile（）' - 将文件加载到XML节点树中。
//
// 此函数将`FILE`指针`fp`加载到XML节点树中。指定文件中的节点将添加到指定节点`top`中 - 如果为`NULL`，则XML文件必须是格式良好的，以文件开头的单个父处理指令节点（例如`<?xml version =“1.0”？>）。
//
// 加载选项通过`options`参数提供。如果为`NULL`，所有值将加载到`MXML_TYPE_TEXT`节点中。在加载XML数据时，请使用@link mxmlOptionsNew@函数创建选项。

mxml_node_t *                // O - 第一个节点，如果无法读取文件，则为`NULL`。
mxmlLoadFile(
    mxml_node_t    *top,        // I - 顶级节点
    mxml_options_t *options,    // I - 选项
    FILE           *fp)        // I - 要读取的文件
{
  // 范围检查输入...
  if (!fp)
    return (NULL);

  // 读取XML数据...
  return (mxml_load_data(top, options, (mxml_io_cb_t)mxml_read_cb_file, fp));
}


//
// 'mxmlLoadFilename（）' - 将文件加载到XML节点树中。
//
// 此函数将命名文件`filename`加载到XML节点树中。指定文件中的节点将添加到指定节点`top`中 - 如果为`NULL`，则XML文件必须是格式良好的，以文件开头的单个父处理指令节点（例如`<?xml version =“1.0”？>）。
//
// 加载选项通过`options`参数提供。如果为`NULL`，所有值将加载到`MXML_TYPE_TEXT`节点中。在加载XML数据时，请使用@link mxmlOptionsNew@函数创建选项。

mxml_node_t *                // O - 第一个节点，如果无法读取文件，则为`NULL`。
mxmlLoadFilename(
    mxml_node_t    *top,        // I - 顶级节点
    mxml_options_t *options,    // I - 选项
    const char     *filename)    // I - 要读取的文件
{
  FILE        *fp;            // 文件指针
  mxml_node_t    *ret;            // 节点


  // 范围检查输入...
  if (!filename)
    return (NULL);

  // 打开文件...
  if ((fp = fopen(filename, "r")) == NULL)
    return (NULL);

  // 读取XML数据...
  ret = mxml_load_data(top, options, (mxml_io_cb_t)mxml_read_cb_file, fp);

  // 关闭文件并返回...
  fclose(fp);

  return (ret);
}


//
// 'mxmlLoadIO（）' - 使用读取回调加载XML节点树。
//
// 此函数使用读取回调函数加载数据到XML节点树中。指定文件中的节点将添加到指定节点`top`中 - 如果为`NULL`，则XML文件必须是格式良好的，以文件开头的单个父处理指令节点（例如`<?xml version =“1.0”？>）。
//
// 加载选项通过`options`参数提供。如果为`NULL`，所有值将加载到`MXML_TYPE_TEXT`节点中。在加载XML数据时，请使用@link mxmlOptionsNew@函数创建选项。
//
// 读取回调函数`io_cb`将被调用以从源中读取一定数量的字节。回调数据指针`io_cbdata`将与指向缓冲区和要读取的最大字节数的指针一起传递给读取回调函数，例如：
//
// ```c
// size_t my_io_cb(void *cbdata, void *buffer, size_t bytes)
// {
//   ... 将最多“bytes”字节复制到缓冲区中 ...
//   ... 返回“读取”的字节数，或错误时返回0 ...
// }
// ```

mxml_node_t *                // O - 第一个节点，如果无法读取文件，则为`NULL`。
mxmlLoadIO(
    mxml_node_t    *top,        // I - 顶级节点
    mxml_options_t *options,    // I - 选项
    mxml_io_cb_t   io_cb,        // I - 读取回调函数
    void           *io_cbdata)    // I - 读取回调数据
{
  // 范围检查输入...
  if (!io_cb)
    return (NULL);

  // 读取XML数据...
  return (mxml_load_data(top, options, io_cb, io_cbdata));
}


//
// 'mxmlLoadString（）' - 将字符串加载到XML节点树中。
//
// 此函数将字符串加载到XML节点树中。指定文件中的节点将添加到指定节点`top`中 - 如果为`NULL`，则XML文件必须是格式良好的，以文件开头的单个父处理指令节点（例如`<?xml version =“1.0”？>）。
//
// 加载选项通过`options`参数提供。如果为`NULL`，所有值将加载到`MXML_TYPE_TEXT`节点中。在加载XML数据时，请使用@link mxmlOptionsNew@函数创建选项。

mxml_node_t *                // O - 第一个节点或`NULL`（如果字符串有错误）。
mxmlLoadString(
    mxml_node_t    *top,        // I - 顶级节点
    mxml_options_t *options,    // I - 选项
    const char     *s)            // I - 要加载的字符串
{
  _mxml_stringbuf_t    sb;        // 字符串缓冲区


  // 范围检查输入...
  if (!s)
    return (NULL);

  // 设置字符串缓冲区...
  sb.buffer   = (char *)s;
  sb.bufptr   = (char *)s;
  sb.bufsize  = strlen(s);
  sb.bufalloc = false;

  // 读取XML数据...
  return (mxml_load_data(top, options, (mxml_io_cb_t)mxml_read_cb_string, &sb));
}


//
// 'mxmlSaveAllocString（）' - 将XML树保存到分配的字符串中。
//
// 此函数将XML树`node`保存到分配的字符串中。使用`free`（或使用@link mxmlSetStringCallbacks@设置的字符串释放回调）释放字符串。
//
// 如果节点生成空字符串或无法分配字符串，则返回`NULL`。
//
// 保存选项通过`options`参数提供。如果为`NULL`，XML输出将在列72处换行，不添加额外的空格。请使用@link mxmlOptionsNew@函数创建保存XML数据的选项。

char *                    // O - 分配的字符串或`NULL`
mxmlSaveAllocString(
    mxml_node_t    *node,        // I - 要写入的节点
    mxml_options_t *options)    // I - 选项
{
  _mxml_stringbuf_t    sb;        // 字符串缓冲区


  // 设置字符串缓冲区
  if ((sb.buffer = malloc(1024)) == NULL)
    return (NULL);

  sb.bufptr   = sb.buffer;
  sb.bufsize  = 1024;
  sb.bufalloc = true;

  // 写入顶级节点...
  if (mxml_write_node(node, options, (mxml_io_cb_t)mxml_io_cb_string, &sb, 0) < 0)
  {
    free(sb.buffer);
    return (NULL);
  }

  // 以空字符结尾...
  *(sb.bufptr) = '\0';

  // 返回分配的字符串...
  return (sb.buffer);
}


//
// 'mxmlSaveFd（）' - 将XML树保存到文件描述符中。
//
// 此函数将XML树`node`保存到文件描述符中。
//
// 保存选项通过`options`参数提供。如果为`NULL`，XML输出将在列72处换行，不添加额外的空格。请使用@link mxmlOptionsNew@函数创建保存XML数据的选项。

bool                    // O - `true`表示成功，`false`表示错误。
mxmlSaveFd(mxml_node_t    *node,    // I - 要写入的节点
           mxml_options_t *options,    // I - 选项
           int            fd)        // I - 要写入的文件描述符
{
  int    col;            // 最终列


  // 写入节点...
  if ((col = mxml_write_node(node, options, (mxml_io_cb_t)mxml_io_cb_fd, &fd, 0)) < 0)
    return (false);

  // 确保文件以换行符结束...
  if (col > 0)
  {
    if (write(fd, "\n", 1) < 0)
      return (false);
  }

  return (true);
}


//
// 'mxmlSaveFile（）' - 将XML树保存到文件中。
//
// 此函数将XML树`node`保存到stdio `FILE`中。
//
// 保存选项通过`options`参数提供。如果为`NULL`，XML输出将在列72处换行，不添加额外的空格。请使用@link mxmlOptionsNew@函数创建保存XML数据的选项。

bool                    // O - `true`表示成功，`false`表示错误。
mxmlSaveFile(
    mxml_node_t    *node,        // I - 要写入的节点
    mxml_options_t *options,    // I - 选项
    FILE           *fp)        // I - 要写入的文件
{
  int    col;            // 最终列


  // 写入节点...
  if ((col = mxml_write_node(node, options, (mxml_io_cb_t)mxml_io_cb_file, fp, 0)) < 0)
    return (false);

  // 确保文件以换行符结束...
  if (col > 0)
  {
    if (putc('\n', fp) < 0)
      return (false);
  }

  return (true);
}


//
// 'mxmlSaveFilename（）' - 将XML树保存到文件中。
//
// 此函数将XML树`node`保存到命名文件中。
//
// 保存选项通过`options`参数提供。如果为`NULL`，XML输出将在列72处换行，不添加额外的空格。请使用@link mxmlOptionsNew@函数创建保存XML数据的选项。

bool                    // O - `true`表示成功，`false`表示错误。
mxmlSaveFilename(
    mxml_node_t    *node,        // I - 要写入的节点
    mxml_options_t *options,    // I - 选项
    const char     *filename)    // I - 要写入的文件
{
  bool        ret = true;    // 返回值
  FILE        *fp;            // 文件指针
  int        col;            // 最终列


  // 打开文件...
  if ((fp = fopen(filename, "w")) == NULL)
    return (false);

  // 写入节点...
  if ((col = mxml_write_node(node, options, (mxml_io_cb_t)mxml_io_cb_file, fp, 0)) < 0)
  {
    ret = false;
  }
  else if (col > 0)
  {
    // 确保文件以换行符结束...
    if (putc('\n', fp) < 0)
      ret = false;
  }

  fclose(fp);

  return (ret);
}


//
// 'mxmlSaveIO（）' - 使用回调保存XML树。
//
// 此函数使用写回调函数`io_cb`保存XML树`node`。回调数据指针`io_cbdata`与缓冲区指针和要写入的字节数一起传递给写回调函数，例如：
//
// ```c
// size_t my_io_cb(void *cbdata, const void *buffer, size_t bytes)
// {
//   ... 将字节从缓冲区写入输出 ...
//   ... 返回写入/复制的字节数，错误时返回0 ...
// }
// ```
//
// 保存选项通过`options`参数提供。如果为`NULL`，XML输出将在列72处换行，不添加额外的空格。请使用@link mxmlOptionsNew@函数创建保存XML数据的选项。

bool                    // O - `true`表示成功，`false`表示错误。
mxmlSaveIO(
    mxml_node_t    *node,        // I - 要写入的节点
    mxml_options_t *options,    // I - 选项
    mxml_io_cb_t   io_cb,        // I - 写回调函数
    void           *io_cbdata)    // I - 写回调数据
{
  int    col;            // 最终列


  // 范围检查输入...
  if (!node || !io_cb)
    return (false);

  // 写入节点...
  if ((col = mxml_write_node(node, options, io_cb, io_cbdata, 0)) < 0)
    return (false);

  if (col > 0)
  {
    // 确保文件以换行符结束...
    if ((io_cb)(io_cbdata, "\n", 1) != 1)
      return (false);
  }

  return (true);
}


//
// 'mxmlSaveString（）' - 将XML节点树保存到字符串中。
//
// 此函数将XML树`node`保存到固定大小的字符串缓冲区中。
//
// 保存选项通过`options`参数提供。如果为`NULL`，XML输出将在列72处换行，不添加额外的空格。请使用@link mxmlOptionsNew@函数创建保存XML数据的选项。

size_t                    // O - 字符串的大小
mxmlSaveString(
    mxml_node_t    *node,        // I - 要写入的节点
    mxml_options_t *options,    // I - 选项
    char           *buffer,    // I - 字符串缓冲区
    size_t         bufsize)    // I - 字符串缓冲区的大小
{
  _mxml_stringbuf_t    sb;        // 字符串缓冲区


  // 设置字符串缓冲区...
  sb.buffer   = buffer;
  sb.bufptr   = buffer;
  sb.bufsize  = bufsize;
  sb.bufalloc = false;

  // 写入节点...
  if (mxml_write_node(node, options, (mxml_io_cb_t)mxml_io_cb_string, &sb, 0) < 0)
    return (false);

  // 以空字符结尾...
  if (sb.bufptr < (sb.buffer + sb.bufsize))
    *(sb.bufptr) = '\0';

  // 返回字符数...
  return ((size_t)(sb.bufptr - sb.buffer));
}


//
// 'mxml_add_char（）' - 将字符添加到缓冲区中，根据需要进行扩展。
//

static bool                // O  - `true`表示成功，`false`表示错误
mxml_add_char(mxml_options_t *options,    // I  - 选项
              int            ch,        // I  - 要添加的字符
              char           **bufptr,    // IO - 缓冲区中的当前位置
              char           **buffer,    // IO - 当前缓冲区
              size_t         *bufsize)    // IO - 当前缓冲区大小
{
  char    *newbuffer;        // 新缓冲区值


  if (*bufptr >= (*buffer + *bufsize - 4))
  {
    // 增加缓冲区的大小...
    if (*bufsize < 1024)
      (*bufsize) *= 2;
    else
      (*bufsize) += 1024;

    if ((newbuffer = realloc(*buffer, *bufsize)) == NULL)
    {
      _mxml_error(options, "无法将字符串缓冲区扩展到%lu字节。", (unsigned long)*bufsize);

      return (false);
    }

    *bufptr = newbuffer + (*bufptr - *buffer);
    *buffer = newbuffer;
  }

  if (ch < 0x80)
  {
    // 单字节ASCII...
    *(*bufptr)++ = ch;
  }
  else if (ch < 0x800)
  {
    // 两字节UTF-8...
    *(*bufptr)++ = 0xc0 | (ch >> 6);
    *(*bufptr)++ = 0x80 | (ch & 0x3f);
  }
  else if (ch < 0x10000)
  {
    // 三字节UTF-8...
    *(*bufptr)++ = 0xe0 | (ch >> 12);
    *(*bufptr)++ = 0x80 | ((ch >> 6) & 0x3f);
    *(*bufptr)++ = 0x80 | (ch & 0x3f);
  }
  else
  {
    // 四字节UTF-8...
    *(*bufptr)++ = 0xf0 | (ch >> 18);
    *(*bufptr)++ = 0x80 | ((ch >> 12) & 0x3f);
    *(*bufptr)++ = 0x80 | ((ch >> 6) & 0x3f);
    *(*bufptr)++ = 0x80 | (ch & 0x3f);
  }

  return (true);
}


//
// 'mxml_get_entity（）' - 获取与实体对应的字符...
//

static int                // O  - 字符值或错误时为`EOF`
mxml_get_entity(
    mxml_options_t   *options,        // I  - 选项
    mxml_io_cb_t     io_cb,        // I  - 读取回调函数
    void             *io_cbdata,    // I  - 读取回调数据
    _mxml_encoding_t *encoding,    // IO - 字符编码
    mxml_node_t      *parent,        // I  - 父节点
    int              *line)        // IO - 当前行号
{
  int    ch;            // 当前字符
  char    entity[64],        // 实体字符串
    *entptr;        // 实体指针


  // 读取HTML字符实体，格式为“&NAME;”，“&#NUMBER;”或“&#xHEX”...
  entptr = entity;

  while ((ch = mxml_getc(options, io_cb, io_cbdata, encoding)) != EOF)
  {
    if (ch > 126 || (!isalnum(ch) && ch != '#'))
    {
      break;
    }
    else if (entptr < (entity + sizeof(entity) - 1))
    {
      *entptr++ = ch;
    }
    else
    {
      _mxml_error(options, "父节点<%s>下的实体名称过长，位于第%d行。", mxmlGetElement(parent), *line);
      break;
    }
  }

  *entptr = '\0';

  if (ch != ';')
  {
    _mxml_error(options, "父节点<%s>下的字符实体“%s”未终止，位于第%d行。", entity, mxmlGetElement(parent), *line);

    if (ch == '\n')
      (*line)++;

    return (EOF);
  }

  if ((ch = _mxml_entity_value(options, entity)) < 0)
  {
    _mxml_error(options, "父节点<%s>下的实体“&%s;”不受支持，位于第%d行。", entity, mxmlGetElement(parent), *line);
    return (EOF);
  }

  if (mxml_bad_char(ch))
  {
    _mxml_error(options, "父节点<%s>下的控制字符0x%02x不符合XML标准，位于第%d行。", ch, mxmlGetElement(parent), *line);
    return (EOF);
  }

  return (ch);
}



//
// 'mxml_getc()' - 从文件描述符中读取一个字符。
//

static int				// O  - 字符或 `EOF`
mxml_getc(mxml_options_t   *options,	// I  - 选项
          mxml_io_cb_t     io_cb,	// I  - 读取回调函数
          void             *io_cbdata,	// I  - 读取回调数据
          _mxml_encoding_t *encoding)	// IO - 编码
{
  int		ch;			// 当前字符
  unsigned char	buffer[4];		// 读取缓冲区


  // 获取下一个字符...
  read_first_byte:

  if ((io_cb)(io_cbdata, buffer, 1) != 1)
    return (EOF);

  ch = buffer[0];

  switch (*encoding)
  {
    case _MXML_ENCODING_UTF8 :
        // 获取到一个 UTF-8 字符；将 UTF-8 转换为 Unicode 并返回...
	if (!(ch & 0x80))
	{
	  // ASCII
	  break;
        }
	else if (ch == 0xfe)
	{
	  // UTF-16 大端字节顺序标记（BOM）？
	  if ((io_cb)(io_cbdata, buffer + 1, 1) != 1)
	    return (EOF);

	  if (buffer[1] != 0xff)
	    return (EOF);

          // 是的，切换到 UTF-16 大端字节顺序并尝试重新读取...
	  *encoding = _MXML_ENCODING_UTF16BE;

	  goto read_first_byte;
	}
	else if (ch == 0xff)
	{
	  // UTF-16 小端字节顺序标记（BOM）？
	  if ((io_cb)(io_cbdata, buffer + 1, 1) != 1)
	    return (EOF);

	  if (buffer[1] != 0xfe)
	    return (EOF);

          // 是的，切换到 UTF-16 小端字节顺序并尝试重新读取...
	  *encoding = _MXML_ENCODING_UTF16LE;

	  goto read_first_byte;
	}
	else if ((ch & 0xe0) == 0xc0)
	{
	  // 两字节值...
	  if ((io_cb)(io_cbdata, buffer + 1, 1) != 1)
	    return (EOF);

	  if ((buffer[1] & 0xc0) != 0x80)
	    return (EOF);

	  ch = ((ch & 0x1f) << 6) | (buffer[1] & 0x3f);

	  if (ch < 0x80)
	  {
	    _mxml_error(options, "字符 0x%04x 的 UTF-8 序列无效。", ch);
	    return (EOF);
	  }
	}
	else if ((ch & 0xf0) == 0xe0)
	{
	  // 三字节值...
	  if ((io_cb)(io_cbdata, buffer + 1, 2) != 2)
	    return (EOF);

	  if ((buffer[1] & 0xc0) != 0x80 || (buffer[2] & 0xc0) != 0x80)
	    return (EOF);

	  ch = ((ch & 0x0f) << 12) | ((buffer[1] & 0x3f) << 6) | (buffer[2] & 0x3f);

	  if (ch < 0x800)
	  {
	    _mxml_error(options, "字符 0x%04x 的 UTF-8 序列无效。", ch);
	    return (EOF);
	  }

          // 忽略字节顺序标记（BOM）...
	  if (ch == 0xfeff)
	    goto read_first_byte;
	}
	else if ((ch & 0xf8) == 0xf0)
	{
	  // 四字节值...
	  if ((io_cb)(io_cbdata, buffer + 1, 3) != 3)
	    return (EOF);

	  if ((buffer[1] & 0xc0) != 0x80 || (buffer[2] & 0xc0) != 0x80 || (buffer[3] & 0xc0) != 0x80)
	    return (EOF);

	  ch = ((ch & 0x07) << 18) | ((buffer[1] & 0x3f) << 12) | ((buffer[2] & 0x3f) << 6) | (buffer[3] & 0x3f);

	  if (ch < 0x10000)
	  {
	    _mxml_error(options, "字符 0x%04x 的 UTF-8 序列无效。", ch);
	    return (EOF);
	  }
	}
	else
	{
	  return (EOF);
	}
	break;

    case _MXML_ENCODING_UTF16BE :
        // 读取 UTF-16 大端字符...
	if ((io_cb)(io_cbdata, buffer + 1, 1) != 1)
	  return (EOF);

	ch = (ch << 8) | buffer[1];

        if (ch >= 0xd800 && ch <= 0xdbff)
	{
	  // 多字节 UTF-16 字符...
          int lch;			// 低位

	  if ((io_cb)(io_cbdata, buffer + 2, 2) != 2)
	    return (EOF);

	  lch = (buffer[2] << 8) | buffer[3];

          if (lch < 0xdc00 || lch >= 0xdfff)
	    return (EOF);

          ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
	}
	break;

    case _MXML_ENCODING_UTF16LE :
        // 读取 UTF-16 小端字符...
	if ((io_cb)(io_cbdata, buffer + 1, 1) != 1)
	  return (EOF);

	ch |= buffer[1] << 8;

        if (ch >= 0xd800 && ch <= 0xdbff)
	{
	  // 多字节 UTF-16 字符...
          int lch;			// 低位

	  if ((io_cb)(io_cbdata, buffer + 2, 2) != 2)
	    return (EOF);

	  lch = (buffer[3] << 8) | buffer[2];

          if (lch < 0xdc00 || lch >= 0xdfff)
	    return (EOF);

          ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
	}
	break;
  }

  if (mxml_bad_char(ch))
  {
    _mxml_error(options, "XML 标准不允许使用的控制字符 0x%02x。", ch);
    return (EOF);
  }

  return (ch);
}


//
// 'mxml_load_data()' - 将数据加载到 XML 节点树中。
//

static mxml_node_t *			// O - 第一个节点，如果无法读取 XML，则为 `NULL`。
mxml_load_data(
    mxml_node_t     *top,		// I - 顶级节点
    mxml_options_t  *options,		// I - 选项
    mxml_io_cb_t    io_cb,		// I - 读取回调函数
    void            *io_cbdata)		// I - 读取回调数据
{
  mxml_node_t	*node = NULL,		// 当前节点
		*first = NULL,		// 添加的第一个节点
		*parent = NULL;		// 当前父节点
  int		line = 1,		// 当前行号
		ch;			// 文件中的字符
  bool		whitespace = false;	// 是否遇到空白字符？
  char		*buffer,		// 字符串缓冲区
		*bufptr;		// 缓冲区指针
  size_t	bufsize;		// 缓冲区大小
  mxml_type_t	type;			// 当前节点类型
  _mxml_encoding_t encoding = _MXML_ENCODING_UTF8;
					// 字符编码
  static const char * const types[] =	// 类型字符串...
		{
		  "MXML_TYPE_CDATA",	// CDATA
		  "MXML_TYPE_COMMENT",	// 注释
		  "MXML_TYPE_DECLARATION",// 声明
		  "MXML_TYPE_DIRECTIVE",// 处理指令/指令
		  "MXML_TYPE_ELEMENT",	// 具有属性的 XML 元素
		  "MXML_TYPE_INTEGER",	// 整数值
		  "MXML_TYPE_OPAQUE",	// 不透明字符串
		  "MXML_TYPE_REAL",	// 实数值
		  "MXML_TYPE_TEXT",	// 文本片段
		  "MXML_TYPE_CUSTOM"	// 自定义数据
		};


  // 从文件中读取元素和其他节点...
  if ((buffer = malloc(64)) == NULL)
  {
    _mxml_error(options, "无法分配字符串缓冲区。");
    return (NULL);
  }

  bufsize    = 64;
  bufptr     = buffer;
  parent     = top;
  first      = NULL;

  if (options && options->type_cb && parent)
    type = (options->type_cb)(options->type_cbdata, parent);
  else if (options && !options->type_cb)
    type = options->type_value;
  else
    type = MXML_TYPE_IGNORE;

  if ((ch = mxml_getc(options, io_cb, io_cbdata, &encoding)) == EOF)
  {
    free(buffer);
    return (NULL);
  }
  else if (ch != '<' && !top)
  {
    free(buffer);
    _mxml_error(options, "XML 不以 '<' 开头（遇到 '%c'）。", ch);
    return (NULL);
  }

  do
  {
    if ((ch == '<' || (mxml_isspace(ch) && type != MXML_TYPE_OPAQUE && type != MXML_TYPE_CUSTOM)) && bufptr > buffer)
    {
      // 添加一个新的值节点...
      *bufptr = '\0';

      switch (type)
      {
	case MXML_TYPE_INTEGER :
            node = mxmlNewInteger(parent, strtol(buffer, &bufptr, 0));
	    break;

	case MXML_TYPE_OPAQUE :
            node = mxmlNewOpaque(parent, buffer);
	    break;

	case MXML_TYPE_REAL :
            node = mxmlNewReal(parent, mxml_strtod(options, buffer, &bufptr));
	    break;

	case MXML_TYPE_TEXT :
            node = mxmlNewText(parent, whitespace, buffer);
	    break;

	case MXML_TYPE_CUSTOM :
	    if (options && options->custload_cb)
	    {
	      // 使用回调函数填充自定义数据...
              node = mxmlNewCustom(parent, /*data*/NULL, /*free_cb*/NULL, /*free_cbdata*/NULL);

	      if (!(options->custload_cb)(options->cust_cbdata, node, buffer))
	      {
	        _mxml_error(options, "父节点 <%s> 中的自定义值 '%s' 无效，在第 %d 行。", parent ? parent->value.element.name : "null", buffer, line);
		mxmlDelete(node);
		node = NULL;
	      }
	      break;
	    }

        default : // 忽略...
	    node = NULL;
	    break;
      }

      if (*bufptr)
      {
        // 整数/实数值无效...
        _mxml_error(options, "父节点 <%s> 中的 %s 值 '%s' 无效，在第 %d 行。", parent ? parent->value.element.name : "null", type == MXML_TYPE_INTEGER ? "整数" : "实数", buffer, line);
	break;
      }

      MXML_DEBUG("mxml_load_data: node=%p(%s), parent=%p\n", node, buffer, parent);

      bufptr     = buffer;
      whitespace = mxml_isspace(ch) && type == MXML_TYPE_TEXT;

      if (!node && type != MXML_TYPE_IGNORE)
      {
        // 打印错误并返回...
	_mxml_error(options, "无法将类型为 %s 的值节点添加到父节点 <%s> 中，在第 %d 行。", types[type], parent ? parent->value.element.name : "null", line);
	goto error;
      }

      if (options && options->sax_cb)
      {
        if (!(options->sax_cb)(options->sax_cbdata, node, MXML_SAX_EVENT_DATA))
          goto error;

        if (!mxmlRelease(node))
          node = NULL;
      }

      if (!first && node)
        first = node;
    }
    else if (mxml_isspace(ch) && type == MXML_TYPE_TEXT)
    {
      whitespace = true;
    }

    if (ch == '\n')
      line ++;

    // 如果有元素并且存在空白字符，则添加一个孤立的空白节点...
    if (ch == '<' && whitespace && type == MXML_TYPE_TEXT)
    {
      if (parent)
      {
	node = mxmlNewText(parent, whitespace, "");

	if (options && options->sax_cb)
	{
	  if (!(options->sax_cb)(options->sax_cbdata, node, MXML_SAX_EVENT_DATA))
	    goto error;

	  if (!mxmlRelease(node))
	    node = NULL;
	}

	if (!first && node)
	  first = node;
      }

      whitespace = false;
    }

    if (ch == '<')
    {
      // 开始开/闭标签...
      bufptr = buffer;

      while ((ch = mxml_getc(options, io_cb, io_cbdata, &encoding)) != EOF)
      {
        if (mxml_isspace(ch) || ch == '>' || (ch == '/' && bufptr > buffer))
        {
	  break;
	}
	else if (ch == '<')
	{
	  _mxml_error(options, "元素中的单个 <。");
	  goto error;
	}
	else if (ch == '&')
	{
	  if ((ch = mxml_get_entity(options, io_cb, io_cbdata, &encoding, parent, &line)) == EOF)
	    goto error;

	  if (!mxml_add_char(options, ch, &bufptr, &buffer, &bufsize))
	    goto error;
	}
	else if (ch < '0' && ch != '!' && ch != '-' && ch != '.' && ch != '/')
	{
	  goto error;
	}
	else if (!mxml_add_char(options, ch, &bufptr, &buffer, &bufsize))
	{
	  goto error;
	}
	else if (((bufptr - buffer) == 1 && buffer[0] == '?') || ((bufptr - buffer) == 3 && !strncmp(buffer, "!--", 3)) || ((bufptr - buffer) == 8 && !strncmp(buffer, "![CDATA[", 8)))
	{
	  break;
	}

	if (ch == '\n')
	  line ++;
      }

      *bufptr = '\0';

      if (!strcmp(buffer, "!--"))
      {
        // 收集剩余的注释...
	while ((ch = mxml_getc(options, io_cb, io_cbdata, &encoding)) != EOF)
	{
	  if (ch == '>' && bufptr > (buffer + 4) && bufptr[-3] != '-' && bufptr[-2] == '-' && bufptr[-1] == '-')
	    break;
	  else if (!mxml_add_char(options, ch, &bufptr, &buffer, &bufsize))
	    goto error;

	  if (ch == '\n')
	    line ++;
	}

        // 如果没有获取到整个注释，则报错...
        if (ch != '>')
	{
	  // 打印错误并返回...
	  _mxml_error(options, "注释节点在第 %d 行遇到早期的 EOF。", line);
	  goto error;
	}

        // 否则将其作为元素添加到当前父节点下...
	bufptr[-2] = '\0';

        if (!parent && first)
	{
	  // 只能有一个根元素！
	  _mxml_error(options, "<%s--> 在 <%s> 之后不能作为第二个根节点，在第 %d 行。", buffer, first->value.element.name, line);
          goto error;
	}

	if ((node = mxmlNewComment(parent, buffer + 3)) == NULL)
	{
	  // 只打印错误...
	  _mxml_error(options, "无法将注释节点添加到父节点 <%s> 中，在第 %d 行。", parent ? parent->value.element.name : "null", line);
	  break;
	}

	MXML_DEBUG("mxml_load_data: node=%p(<%s-->), parent=%p\n", node, buffer, parent);

        if (options && options->sax_cb)
        {
          if (!(options->sax_cb)(options->sax_cbdata, node, MXML_SAX_EVENT_COMMENT))
	    goto error;

          if (!mxmlRelease(node))
            node = NULL;
        }

	if (node && !first)
	  first = node;
      }
      else if (!strcmp(buffer, "![CDATA["))
      {
        // 收集 CDATA 部分...
	while ((ch = mxml_getc(options, io_cb, io_cbdata, &encoding)) != EOF)
	{
	  if (ch == '>' && !strncmp(bufptr - 2, "]]", 2))
	  {
	    // 从 CDATA 字符串中删除终止符...
	    bufptr[-2] = '\0';
	    break;
	  }
	  else if (!mxml_add_char(options, ch, &bufptr, &buffer, &bufsize))
	  {
	    goto error;
	  }

	  if (ch == '\n')
	    line ++;
	}

        // 如果没有获取到整个 CDATA 部分，则报错...
        if (ch != '>')
	{
	  // 打印错误并返回...
	  _mxml_error(options, "CDATA 节点在第 %d 行遇到早期的 EOF。", line);
	  goto error;
	}

        // 否则将其作为元素添加到当前父节点下...
	bufptr[-2] = '\0';

        if (!parent && first)
	{
	  // 只能有一个根元素！
	  _mxml_error(options, "<%s]]> 在 <%s> 之后不能作为第二个根节点，在第 %d 行。", buffer, first->value.element.name, line);
          goto error;
	}

	if ((node = mxmlNewCDATA(parent, buffer + 8)) == NULL)
	{
	  // 打印错误并返回...
	  _mxml_error(options, "无法将 CDATA 节点添加到父节点 <%s> 中，在第 %d 行。", parent ? parent->value.element.name : "null", line);
	  goto error;
	}

	MXML_DEBUG("mxml_load_data: node=%p(<%s]]>), parent=%p\n", node, buffer, parent);

        if (options && options->sax_cb)
        {
          if (!(options->sax_cb)(options->sax_cbdata, node, MXML_SAX_EVENT_CDATA))
	    goto error;

          if (!mxmlRelease(node))
            node = NULL;
        }

	if (node && !first)
	  first = node;
      }
      else if (buffer[0] == '?')
      {
        // 收集剩余的处理指令...
	while ((ch = mxml_getc(options, io_cb, io_cbdata, &encoding)) != EOF)
	{
	  if (ch == '>' && bufptr > buffer && bufptr[-1] == '?')
	    break;
	  else if (!mxml_add_char(options, ch, &bufptr, &buffer, &bufsize))
	    goto error;

	  if (ch == '\n')
	    line ++;
	}

        // 如果没有获取到整个处理指令，则报错...
        if (ch != '>')
	{
	  // 打印错误并返回...
	  _mxml_error(options, "处理指令节点在第 %d 行遇到早期的 EOF。", line);
	  goto error;
	}

        // 否则将其作为元素添加到当前父节点下...
	bufptr[-1] = '\0';

        if (!parent && first)
	{
	  // 只能有一个根元素！
	  _mxml_error(options, "<%s?> 在 <%s> 之后不能作为第二个根节点，在第 %d 行。", buffer, first->value.element.name, line);
          goto error;
	}

	if ((node = mxmlNewDirective(parent, buffer + 1)) == NULL)
	{
	  // 打印错误并返回...
	  _mxml_error(options, "无法将处理指令节点添加到父节点 <%s> 中，在第 %d 行。", parent ? parent->value.element.name : "null", line);
	  goto error;
	}

	MXML_DEBUG("mxml_load_data: node=%p(<%s?>), parent=%p\n", node, buffer, parent);

        if (options && options->sax_cb)
        {
          if (!(options->sax_cb)(options->sax_cbdata, node, MXML_SAX_EVENT_DIRECTIVE))
	    goto error;

          if (strncmp(node->value.directive, "xml ", 4) && !mxmlRelease(node))
            node = NULL;
        }

        if (node)
	{
	  if (!first)
            first = node;

	  if (!parent)
	  {
	    parent = node;

	    if (options && options->type_cb)
	      type = (options->type_cb)(options->type_cbdata, parent);
	    else if (options)
	      type = options->type_value;
	    else
	      type = MXML_TYPE_TEXT;
	  }
	}
      }
      else if (buffer[0] == '!')
      {
        // 收集剩余的声明...
	do
	{
	  if (ch == '>')
	  {
	    break;
	  }
	  else
	  {
            if (ch == '&')
            {
	      if ((ch = mxml_get_entity(options, io_cb, io_cbdata, &encoding, parent, &line)) == EOF)
		goto error;
            }

	    if (!mxml_add_char(options, ch, &bufptr, &buffer, &bufsize))
	      goto error;
	  }

	  if (ch == '\n')
	    line ++;
	}
        while ((ch = mxml_getc(options, io_cb, io_cbdata, &encoding)) != EOF);

        // 如果没有获取到整个声明，则报错...
        if (ch != '>')
	{
	  // 打印错误并返回...
	  _mxml_error(options, "声明节点在第 %d 行遇到早期的 EOF。", line);
	  goto error;
	}

        // 否则将其作为元素添加到当前父节点下...
	*bufptr = '\0';

        if (!parent && first)
	{
	  // 只能有一个根元素！
	  _mxml_error(options, "<%s> 在 <%s> 之后不能作为第二个根节点，在第 %d 行。", buffer, first->value.element.name, line);
          goto error;
	}

	if ((node = mxmlNewDeclaration(parent, buffer + 1)) == NULL)
	{
	  // 打印错误并返回...
	  _mxml_error(options, "无法将声明节点添加到父节点 <%s> 中，在第 %d 行。", parent ? parent->value.element.name : "null", line);
	  goto error;
	}

	MXML_DEBUG("mxml_load_data: node=%p(<%s>), parent=%p\n", node, buffer, parent);

        if (options && options->sax_cb)
        {
          if (!(options->sax_cb)(options->sax_cbdata, node, MXML_SAX_EVENT_DECLARATION))
	    goto error;

          if (!mxmlRelease(node))
            node = NULL;
        }

        if (node)
	{
	  if (!first)
            first = node;

	  if (!parent)
	  {
	    parent = node;

	    if (options && options->type_cb)
	      type = (options->type_cb)(options->type_cbdata, parent);
	    else if (options)
	      type = options->type_value;
	    else
	      type = MXML_TYPE_TEXT;
	  }
	}
      }
      else if (buffer[0] == '/')
      {
        // 处理闭合标签...
	MXML_DEBUG("mxml_load_data: <%s>, parent=%p\n", buffer, parent);

        if (!parent || strcmp(buffer + 1, parent->value.element.name))
	{
	  // 闭合标签与树不匹配；暂时只打印错误...
	  _mxml_error(options, "不匹配的闭合标签 <%s>，在父节点 <%s> 下，在第 %d 行。", buffer, parent ? parent->value.element.name : "(null)", line);
          goto error;
	}

        // 继续读取，直到遇到 >...
        while (ch != '>' && ch != EOF)
	  ch = mxml_getc(options, io_cb, io_cbdata, &encoding);

        node   = parent;
        parent = parent->parent;

        if (options && options->sax_cb)
        {
          if (!(options->sax_cb)(options->sax_cbdata, node, MXML_SAX_EVENT_ELEMENT_CLOSE))
	    goto error;

          if (!mxmlRelease(node))
          {
            if (first == node)
	      first = NULL;

	    node = NULL;
	  }
        }

        // 向上进入父节点，并根据需要设置值类型...
	if (options && options->type_cb && parent)
	  type = (options->type_cb)(options->type_cbdata, parent);
	else if (options && !options->type_cb)
	  type = options->type_value;
      }
      else
      {
        // 处理开放标签...
        if (!parent && first)
	{
	  // 只能有一个根元素！
	  _mxml_error(options, "<%s> 在 <%s> 之后不能作为第二个根节点，在第 %d 行。", buffer, first->value.element.name, line);
          goto error;
	}

        if ((node = mxmlNewElement(parent, buffer)) == NULL)
	{
	  // 暂时只打印错误...
	  _mxml_error(options, "无法将元素节点添加到父节点 <%s> 中，在第 %d 行。", parent ? parent->value.element.name : "null", line);
	  goto error;
	}

        if (mxml_isspace(ch))
        {
	  MXML_DEBUG("mxml_load_data: node=%p(<%s...>), parent=%p\n", node, buffer, parent);

	  if ((ch = mxml_parse_element(options, io_cb, io_cbdata, node, &encoding, &line)) == EOF)
	    goto error;
        }
        else if (ch == '/')
	{
	  MXML_DEBUG("mxml_load_data: node=%p(<%s/>), parent=%p\n", node, buffer, parent);

	  if ((ch = mxml_getc(options, io_cb, io_cbdata, &encoding)) != '>')
	  {
	    _mxml_error(options, "元素 <%s/> 预期为 >，但实际为 '%c'，在第 %d 行。", buffer, ch, line);
            mxmlDelete(node);
            node = NULL;
            goto error;
	  }

	  ch = '/';
	}

        if (options && options->sax_cb)
        {
          if (!(options->sax_cb)(options->sax_cbdata, node, MXML_SAX_EVENT_ELEMENT_OPEN))
	    goto error;
	}

        if (!first)
	  first = node;

	if (ch == EOF)
	  break;

        if (ch != '/')
	{
	  // 进入此节点，并根据需要设置值类型...
	  parent = node;

	  if (options && options->type_cb && parent)
	    type = (options->type_cb)(options->type_cbdata, parent);
	  else if (options && !options->type_cb)
	    type = options->type_value;
	  else
	    type = MXML_TYPE_TEXT;
	}
        else if (options && options->sax_cb)
        {
          if (!(options->sax_cb)(options->sax_cbdata, node, MXML_SAX_EVENT_ELEMENT_CLOSE))
	    goto error;

          if (!mxmlRelease(node))
          {
            if (first == node)
	      first = NULL;

	    node = NULL;
	  }
        }
      }

      bufptr  = buffer;
    }
    else if (ch == '&')
    {
      // 将字符实体添加到当前缓冲区...
      if ((ch = mxml_get_entity(options, io_cb, io_cbdata, &encoding, parent, &line)) == EOF)
	goto error;

      if (!mxml_add_char(options, ch, &bufptr, &buffer, &bufsize))
	goto error;
    }
    else if (type == MXML_TYPE_OPAQUE || type == MXML_TYPE_CUSTOM || !mxml_isspace(ch))
    {
      // 将字符添加到当前缓冲区...
      if (!mxml_add_char(options, ch, &bufptr, &buffer, &bufsize))
	goto error;
    }
  }
  while ((ch = mxml_getc(options, io_cb, io_cbdata, &encoding)) != EOF);

  // 释放字符串缓冲区 - 我们不再需要它了...
  free(buffer);

  // 查找顶级元素并返回它...
  if (parent)
  {
    node = parent;

    while (parent != top && parent->parent)
      parent = parent->parent;

    if (node != parent)
    {
      _mxml_error(options, "在父节点 <%s> 下缺少闭合标签 </%s>，在第 %d 行。", mxmlGetElement(node), node->parent ? node->parent->value.element.name : "(null)", line);

      mxmlDelete(first);

      return (NULL);
    }
  }

  if (parent)
    return (parent);
  else
    return (first);

  // 常见的错误返回...
  error:

  mxmlDelete(first);

  free(buffer);

  return (NULL);
}

//
// 'mxml_parse_element()' - 解析元素的属性...
//

static int				// O  - 终止字符
mxml_parse_element(
    mxml_options_t   *options,		// I - 选项
    mxml_io_cb_t     io_cb,		// I - 读取回调函数
    void             *io_cbdata,	// I - 读取回调数据
    mxml_node_t      *node,		// I  - 元素节点
    _mxml_encoding_t *encoding,		// IO - 编码
    int              *line)		// IO - 当前行号
{
  int		ch,			// 文件中的当前字符
		quote;			// 引用字符
  char		*name,			// 属性名
		*value,			// 属性值
		*ptr;			// 指向属性名/值的指针
  size_t	namesize,		// 属性名字符串的大小
		valsize;		// 属性值字符串的大小


  // 初始化属性名和属性值缓冲区...
  if ((name = malloc(64)) == NULL)
  {
    _mxml_error(options, "无法为属性名分配内存.");
    return (EOF);
  }

  namesize = 64;

  if ((value = malloc(64)) == NULL)
  {
    free(name);
    _mxml_error(options, "无法为属性值分配内存.");
    return (EOF);
  }

  valsize = 64;

  // 循环直到遇到 >、/、? 或 EOF...
  while ((ch = mxml_getc(options, io_cb, io_cbdata, encoding)) != EOF)
  {
    MXML_DEBUG("mxml_parse_element: ch='%c'\n", ch);

    // 跳过前导空白字符...
    if (mxml_isspace(ch))
    {
      if (ch == '\n')
        (*line)++;

      continue;
    }

    // 在 /、? 或 > 处停止...
    if (ch == '/' || ch == '?')
    {
      // 获取 > 字符，如果不存在则打印错误...
      quote = mxml_getc(options, io_cb, io_cbdata, encoding);

      if (quote != '>')
      {
        _mxml_error(options, "在元素 %s 的 '%c' 后期望 '>'，但在第 %d 行得到 '%c'。", ch, mxmlGetElement(node), quote, *line);
        goto error;
      }

      break;
    }
    else if (ch == '<')
    {
      _mxml_error(options, "在元素 %s 中存在裸的 <，位于第 %d 行。", mxmlGetElement(node), *line);
      goto error;
    }
    else if (ch == '>')
    {
      break;
    }

    // 读取属性名...
    ptr = name;
    if (!mxml_add_char(options, ch, &ptr, &name, &namesize))
      goto error;

    if (ch == '\"' || ch == '\'')
    {
      // 属性名在引号中，获取带引号的字符串...
      quote = ch;

      while ((ch = mxml_getc(options, io_cb, io_cbdata, encoding)) != EOF)
      {
        if (ch == '&')
        {
	  if ((ch = mxml_get_entity(options, io_cb, io_cbdata, encoding, node, line)) == EOF)
	    goto error;
	}
	else if (ch == '\n')
	{
	  (*line)++;
	}

	if (!mxml_add_char(options, ch, &ptr, &name, &namesize))
	  goto error;

	if (ch == quote)
          break;
      }
    }
    else
    {
      // 获取普通的非引号属性名...
      while ((ch = mxml_getc(options, io_cb, io_cbdata, encoding)) != EOF)
      {
	if (mxml_isspace(ch) || ch == '=' || ch == '/' || ch == '>' || ch == '?')
	{
	  if (ch == '\n')
	    (*line)++;
          break;
        }
	else
	{
          if (ch == '&')
          {
	    if ((ch = mxml_get_entity(options, io_cb, io_cbdata, encoding, node, line)) == EOF)
	      goto error;
          }

	  if (!mxml_add_char(options, ch, &ptr, &name, &namesize))
	    goto error;
	}
      }
    }

    *ptr = '\0';

    if (mxmlElementGetAttr(node, name))
    {
      _mxml_error(options, "在元素 %s 中重复的属性 '%s'，位于第 %d 行。", name, mxmlGetElement(node), *line);
      goto error;
    }

    while (ch != EOF && mxml_isspace(ch))
    {
      ch = mxml_getc(options, io_cb, io_cbdata, encoding);

      if (ch == '\n')
        (*line)++;
    }

    if (ch == '=')
    {
      // 读取属性值...
      while ((ch = mxml_getc(options, io_cb, io_cbdata, encoding)) != EOF && mxml_isspace(ch))
      {
        if (ch == '\n')
          (*line)++;
      }

      if (ch == EOF)
      {
        _mxml_error(options, "在元素 %s 的属性 '%s' 中缺少值，位于第 %d 行。", name, mxmlGetElement(node), *line);
        goto error;
      }

      if (ch == '\'' || ch == '\"')
      {
        // 读取带引号的属性值...
        quote = ch;
	ptr   = value;

        while ((ch = mxml_getc(options, io_cb, io_cbdata, encoding)) != EOF)
        {
	  if (ch == quote)
	  {
	    break;
	  }
	  else
	  {
	    if (ch == '&')
	    {
	      if ((ch = mxml_get_entity(options, io_cb, io_cbdata, encoding, node, line)) == EOF)
	        goto error;
	    }
	    else if (ch == '\n')
	    {
	      (*line)++;
	    }

	    if (!mxml_add_char(options, ch, &ptr, &value, &valsize))
	      goto error;
	  }
	}

        *ptr = '\0';
      }
      else
      {
        // 读取不带引号的属性值...
	ptr      = value;
	if (!mxml_add_char(options, ch, &ptr, &value, &valsize))
	  goto error;

	while ((ch = mxml_getc(options, io_cb, io_cbdata, encoding)) != EOF)
	{
	  if (mxml_isspace(ch) || ch == '=' || ch == '/' || ch == '>')
	  {
	    if (ch == '\n')
	      (*line)++;

            break;
          }
	  else
	  {
	    if (ch == '&')
	    {
	      if ((ch = mxml_get_entity(options, io_cb, io_cbdata, encoding, node, line)) == EOF)
	        goto error;
	    }

	    if (!mxml_add_char(options, ch, &ptr, &value, &valsize))
	      goto error;
	  }
	}

        *ptr = '\0';
      }

      // 使用给定的字符串值设置属性...
      mxmlElementSetAttr(node, name, value);
      MXML_DEBUG("mxml_parse_element: %s=\"%s\"\n", name, value);
    }
    else
    {
      _mxml_error(options, "在元素 %s 的属性 '%s' 中缺少值，位于第 %d 行。", name, mxmlGetElement(node), *line);
      goto error;
    }

    // 检查结束字符...
    if (ch == '/' || ch == '?')
    {
      // 获取 > 字符，如果不存在则打印错误...
      quote = mxml_getc(options, io_cb, io_cbdata, encoding);

      if (quote != '>')
      {
        _mxml_error(options, "在元素 %s 的 '%c' 后期望 '>'，但在第 %d 行得到 '%c'。", ch, mxmlGetElement(node), quote, *line);
        ch = EOF;
      }

      break;
    }
    else if (ch == '>')
      break;
  }

  // 释放属性名和属性值缓冲区并返回...
  free(name);
  free(value);

  return (ch);

  // 错误返回点...
  error:

  free(name);
  free(value);

  return (EOF);
}



//
// 'mxml_read_cb_fd()' - 从文件描述符读取字节。
//

static size_t				// O - 读取的字节数
mxml_read_cb_fd(int    *fd,		// I - 文件描述符
                void   *buffer,		// I - 缓冲区
                size_t bytes)		// I - 要读取的字节数
{
#if _WIN32
  int		rbytes;			// 读取的字节数


  rbytes = read(*fd, buffer, bytes);

#else
  ssize_t	rbytes;			// 读取的字节数


  while ((rbytes = read(*fd, buffer, bytes)) < 0)
  {
    if (errno != EINTR && errno != EAGAIN)
      break;
  }
#endif // _WIN32

  if (rbytes < 0)
    return (0);
  else
    return ((size_t)rbytes);
}


//
// 'mxml_read_cb_file()' - 从文件指针读取字节。
//

static size_t				// O - 读取的字节数
mxml_read_cb_file(FILE   *fp,		// I - 文件指针
                  void   *buffer,	// I - 缓冲区
                  size_t bytes)		// I - 要读取的字节数
{
  if (feof(fp))
    return (0);
  else
    return (fread(buffer, 1, bytes, fp));
}


//
// 'mxml_read_cb_string()' - 从字符串读取字节。
//

static size_t				// O - 读取的字节数
mxml_read_cb_string(
    _mxml_stringbuf_t *sb,		// I - 字符串缓冲区
    void              *buffer,		// I - 缓冲区
    size_t            bytes)		// I - 要读取的字节数
{
  size_t	remaining;		// 缓冲区中剩余的字节数


  if ((remaining = sb->bufsize - (size_t)(sb->bufptr - sb->buffer)) < bytes)
    bytes = remaining;

  if (bytes > 0)
  {
    // 从字符串中复制字节...
    memcpy(buffer, sb->bufptr, bytes);
    sb->bufptr += bytes;
  }

  return (bytes);
}


//
// 'mxml_strtod()' - 将字符串转换为双精度浮点数，不考虑区域设置。
//

static double				// O - 实数
mxml_strtod(mxml_options_t *options,	// I - 选项
            const char     *buffer,	// I - 字符串
            char           **bufend)	// O - 数字在字符串中的结束位置
{
  const char	*bufptr;		// 指向缓冲区的指针
  char		temp[64],		// 临时缓冲区
		*tempptr;		// 指向临时缓冲区的指针


  // 检查区域设置是否有特殊的小数点字符串...
  if (!options || !options->loc)
    return (strtod(buffer, bufend));

  // 复制前导符号、数字、小数点和数字...
  tempptr                = temp;
  temp[sizeof(temp) - 1] = '\0';
  bufptr                 = buffer;

  if (*bufptr == '-' || *bufptr == '+')
    *tempptr++ = *bufptr++;

  while (*bufptr && isdigit(*bufptr & 255))
  {
    if (tempptr < (temp + sizeof(temp) - 1))
    {
      *tempptr++ = *bufptr++;
    }
    else
    {
      *bufend = (char *)bufptr;
      return (0.0);
    }
  }

  if (*bufptr == '.')
  {
    // 将小数点转换为区域设置的等效字符...
    size_t	declen = strlen(options->loc->decimal_point);
					// 小数点的长度
    bufptr ++;

    if (declen <= (sizeof(temp) - (size_t)(tempptr - temp)))
    {
      memcpy(tempptr, options->loc->decimal_point, declen);
      tempptr += declen;
    }
    else
    {
      *bufend = (char *)bufptr;
      return (0.0);
    }
  }

  // 复制剩余的字符...
  while (*bufptr && isdigit(*bufptr & 255))
  {
    if (tempptr < (temp + sizeof(temp) - 1))
      *tempptr++ = *bufptr++;
    else
      break;
  }

  *bufend = (char *)bufptr;

  if (*bufptr)
    return (0.0);

  // 在临时字符串末尾加上空字符并转换字符串...
  *tempptr = '\0';

  return (strtod(temp, NULL));
}


//
// 'mxml_io_cb_fd()' - 将字节写入文件描述符。
//

static size_t				// O - 写入的字节数
mxml_io_cb_fd(int    *fd,		// I - 文件描述符
                 void   *buffer,	// I - 缓冲区
                 size_t bytes)		// I - 要写入的字节数
{
#if _WIN32
  int		wbytes;			// 写入的字节数


  wbytes = write(*fd, buffer, bytes);

#else
  ssize_t	wbytes;			// 写入的字节数


  while ((wbytes = write(*fd, buffer, bytes)) < 0)
  {
    if (errno != EINTR && errno != EAGAIN)
      break;
  }
#endif // _WIN32

  if (wbytes < 0)
    return (0);
  else
    return ((size_t)wbytes);
}



//
// 'mxml_io_cb_file()' - 将字节写入文件指针。
//

static size_t				// O - 写入的字节数
mxml_io_cb_file(FILE   *fp,		// I - 文件指针
                   void   *buffer,	// I - 缓冲区
                   size_t bytes)	// I - 要写入的字节数
{
  return (fwrite(buffer, 1, bytes, fp));
}


//
// 'mxml_io_cb_string()' - 将字节写入字符串缓冲区。
//

static size_t				// O - 写入的字节数
mxml_io_cb_string(
    _mxml_stringbuf_t *sb,		// I - 字符串缓冲区
    void              *buffer,		// I - 缓冲区
    size_t            bytes)		// I - 要写入的字节数
{
  size_t	remaining;		// 剩余字节数


  // 根据需要扩展缓冲区...
  if ((sb->bufptr + bytes) >= (sb->buffer + sb->bufsize - 1) && sb->bufalloc)
  {
    // 重新分配缓冲区
    char	*temp;			// 新缓冲区指针
    size_t	newsize;		// 新的缓冲区大小

    newsize = (size_t)(sb->bufptr - sb->buffer) + bytes + 257;
    if ((temp = realloc(sb->buffer, newsize)) == NULL)
      return (0);

    sb->bufptr  = temp + (sb->bufptr - sb->buffer);
    sb->buffer  = temp;
    sb->bufsize = newsize;
  }

  // 复制尽可能多的字节...
  if (sb->bufptr >= (sb->buffer + sb->bufsize - 1))
    return (0);			// 没有更多空间
  else if ((remaining = (sb->bufsize - (size_t)(sb->bufptr - sb->buffer) - 1)) < bytes)
    bytes = remaining;

  memcpy(sb->bufptr, buffer, bytes);
  sb->bufptr += bytes;

  return (bytes);
}


//
// 'mxml_write_node()' - 将 XML 节点保存到文件。
//

static int				// O - 列或错误时为 -1
mxml_write_node(
    mxml_node_t    *node,		// I - 要写入的节点
    mxml_options_t *options,		// I - 选项
    mxml_io_cb_t   io_cb,		// I - 写入回调函数
    void           *io_cbdata,		// I - 写入回调数据
    int            col)			// I - 当前列
{
  mxml_node_t	*current,		// 当前节点
		*next;			// 下一个节点
  size_t	i,			// 循环变量
		width;			// 属性+值的宽度
  _mxml_attr_t	*attr;			// 当前属性
  char		s[255],			// 临时字符串
		*data;			// 自定义数据字符串
  const char	*text;			// 文本字符串
  bool		whitespace;		// 文本字符串之前的空白字符？


  // 循环遍历此节点及其所有子节点...
  for (current = node; current && col >= 0; current = next)
  {
    // 输出节点值...
    MXML_DEBUG("mxml_write_node: current=%p(%d)\n", current, current->type);

    switch (mxmlGetType(current))
    {
      case MXML_TYPE_CDATA :
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_BEFORE_OPEN, col);
	  col = mxml_write_string("<![CDATA[", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_string(mxmlGetCDATA(current), io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_string("]]>", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_AFTER_OPEN, col);
          break;

      case MXML_TYPE_COMMENT :
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_BEFORE_OPEN, col);
	  col = mxml_write_string("<!--", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_string(mxmlGetComment(current), io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_string("-->", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_AFTER_OPEN, col);
          break;

      case MXML_TYPE_DECLARATION :
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_BEFORE_OPEN, col);
	  col = mxml_write_string("<!", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_string(mxmlGetDeclaration(current), io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_string(">", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_AFTER_OPEN, col);
          break;

      case MXML_TYPE_DIRECTIVE :
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_BEFORE_OPEN, col);
	  col = mxml_write_string("<?", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_string(mxmlGetDirective(current), io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_string("?>", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_AFTER_OPEN, col);
          break;

      case MXML_TYPE_ELEMENT :
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_BEFORE_OPEN, col);
	  col = mxml_write_string("<", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_string(mxmlGetElement(current), io_cb, io_cbdata, /*use_entities*/true, col);

	  for (i = current->value.element.num_attrs, attr = current->value.element.attrs; i > 0 && col >= 0; i --, attr ++)
	  {
	    width = strlen(attr->name);

	    if (attr->value)
	      width += strlen(attr->value) + 3;

	    if (options && options->wrap > 0 && (col + (int)width) > options->wrap)
	      col = mxml_write_string("\n", io_cb, io_cbdata, /*use_entities*/false, col);
	    else
	      col = mxml_write_string(" ", io_cb, io_cbdata, /*use_entities*/false, col);

	    col = mxml_write_string(attr->name, io_cb, io_cbdata, /*use_entities*/true, col);

	    if (attr->value)
	    {
	      col = mxml_write_string("=\"", io_cb, io_cbdata, /*use_entities*/false, col);
	      col = mxml_write_string(attr->value, io_cb, io_cbdata, /*use_entities*/true, col);
	      col = mxml_write_string("\"", io_cb, io_cbdata, /*use_entities*/false, col);
	    }
	  }

	  col = mxml_write_string(current->child ? ">" : "/>", io_cb, io_cbdata, /*use_entities*/false, col);
	  col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_AFTER_OPEN, col);
	  break;

      case MXML_TYPE_INTEGER :
	  if (current->prev)
	  {
	    // 添加空格分隔符...
	    if (options && options->wrap > 0 && col > options->wrap)
	      col = mxml_write_string("\n", io_cb, io_cbdata, /*use_entities*/false, col);
	    else
	      col = mxml_write_string(" ", io_cb, io_cbdata, /*use_entities*/false, col);
	  }

          // 写入整数...
	  snprintf(s, sizeof(s), "%ld", current->value.integer);
	  col = mxml_write_string(s, io_cb, io_cbdata, /*use_entities*/true, col);
	  break;

      case MXML_TYPE_OPAQUE :
	  col = mxml_write_string(mxmlGetOpaque(current), io_cb, io_cbdata, /*use_entities*/true, col);
	  break;

      case MXML_TYPE_REAL :
	  if (current->prev)
	  {
	    // 添加空格分隔符...
	    if (options && options->wrap > 0 && col > options->wrap)
	      col = mxml_write_string("\n", io_cb, io_cbdata, /*use_entities*/false, col);
	    else
	      col = mxml_write_string(" ", io_cb, io_cbdata, /*use_entities*/false, col);
	  }

          // 写入实数...
	  snprintf(s, sizeof(s), "%g", current->value.real);

          if (options && options->loc)
          {
            char	*sptr;		// 字符串指针

	    if ((sptr = strstr(s, options->loc->decimal_point)) != NULL)
	    {
	      // 将区域设置的小数点转换为 "."
	      if (options->loc_declen > 1)
	        memmove(sptr + 1, sptr + options->loc_declen, strlen(sptr + options->loc_declen) + 1);

	      *sptr = '.';
	    }
          }

	  col = mxml_write_string(s, io_cb, io_cbdata, /*use_entities*/true, col);
	  break;

      case MXML_TYPE_TEXT :
          text = mxmlGetText(current, &whitespace);

	  if (whitespace && col > 0)
	  {
	    // 添加空格分隔符...
	    if (options && options->wrap > 0 && col > options->wrap)
	      col = mxml_write_string("\n", io_cb, io_cbdata, /*use_entities*/false, col);
	    else
	      col = mxml_write_string(" ", io_cb, io_cbdata, /*use_entities*/false, col);
	  }

	  col = mxml_write_string(text, io_cb, io_cbdata, /*use_entities*/true, col);
	  break;

      case MXML_TYPE_CUSTOM :
	  if (!options || !options->custsave_cb)
	    return (-1);

	  if ((data = (options->custsave_cb)(options->cust_cbdata, current)) == NULL)
	    return (-1);

	  col = mxml_write_string(data, io_cb, io_cbdata, /*use_entities*/true, col);

	  free(data);
	  break;

      default : // 永远不应该发生
	  return (-1);
    }

    // 确定下一个节点...
    if ((next = mxmlGetFirstChild(current)) == NULL)
    {
      if (current == node)
      {
        // 如果我们在“根”节点上，不遍历到兄弟节点...
        next = NULL;
      }
      else
      {
        // 尝试下一个兄弟节点，并根据需要继续向上遍历...
	while ((next = mxmlGetNextSibling(current)) == NULL)
	{
	  if (current == node || !mxmlGetParent(current))
	    break;

	  // 声明和指令没有结束标记...
	  current = mxmlGetParent(current);

	  if (mxmlGetType(current) == MXML_TYPE_ELEMENT)
	  {
	    col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_BEFORE_CLOSE, col);
	    col = mxml_write_string("</", io_cb, io_cbdata, /*use_entities*/false, col);
	    col = mxml_write_string(mxmlGetElement(current), io_cb, io_cbdata, /*use_entities*/true, col);
	    col = mxml_write_string(">", io_cb, io_cbdata, /*use_entities*/false, col);
	    col = mxml_write_ws(current, options, io_cb, io_cbdata, MXML_WS_AFTER_CLOSE, col);
	  }

	  if (current == node)
	    break;
	}
      }
    }
  }

  return (col);
}


//
// 'mxml_write_string()' - 写入字符串，根据需要转义 & 和 <。
//

static int				// O - 新的列数，错误时为 `-1`
mxml_write_string(
    const char      *s,			// I - 要写入的字符串
    mxml_io_cb_t    io_cb,		// I - 写入回调函数
    void            *io_cbdata,		// I - 写入回调数据
    bool            use_entities,	// I - 是否转义特殊字符？
    int             col)		// I - 当前列数
{
  const char	*frag,			// 当前字符串片段的起始位置
		*ptr,			// 字符串指针
		*ent;			// 实体，如果有的话
  size_t	fraglen;		// 片段的长度


  MXML_DEBUG("mxml_write_string(io_cb=%p, io_cbdata=%p, s=\"%s\", use_entities=%s, col=%d)\n", io_cb, io_cbdata, s, use_entities ? "true" : "false", col);

  if (col < 0)
    return (-1);

  for (frag = ptr = s; *ptr; ptr ++)
  {
    if (use_entities && (ent = _mxml_entity_string(*ptr)) != NULL)
    {
      size_t entlen = strlen(ent);	// 实体的长度

      if (ptr > frag)
      {
        // 写入当前片段
        fraglen = (size_t)(ptr - frag);

	if ((io_cb)(io_cbdata, (char *)frag, fraglen) != fraglen)
	  return (-1);
      }

      frag = ptr + 1;

      // 写入实体
      if ((io_cb)(io_cbdata, (char *)ent, entlen) != entlen)
        return (-1);

      col ++;
    }
    else if (*ptr == '\r' || *ptr == '\n')
    {
      // CR 或 LF 重置列数
      col = 0;
    }
    else if (*ptr == '\t')
    {
      // Tab 缩进列数
      col = col - (col % MXML_TAB) + MXML_TAB;
    }
    else
    {
      // 所有其他字符占据 1 列
      col ++;
    }
  }

  if (ptr > frag)
  {
    // 写入最后一个片段
    fraglen = (size_t)(ptr - frag);

    if ((io_cb)(io_cbdata, (char *)frag, fraglen) != fraglen)
      return (-1);
  }

  return (col);
}


//
// 'mxml_write_ws()' - 处理空白字符回调...
//

static int				// O - 新的列数，错误时为 `-1`
mxml_write_ws(
    mxml_node_t    *node,		// I - 当前节点
    mxml_options_t *options,		// I - 选项
    mxml_io_cb_t   io_cb,		// I - 写入回调函数
    void           *io_cbdata,		// I - 写入回调数据
    mxml_ws_t      ws,			// I - 空白字符值
    int            col)			// I - 当前列数
{
  const char	*s;			// 空白字符字符串


  if (options && options->ws_cb && (s = (options->ws_cb)(options->ws_cbdata, node, ws)) != NULL)
    col = mxml_write_string(s, io_cb, io_cbdata, /*use_entities*/false, col);

  return (col);
}
