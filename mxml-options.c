
//
// Mini-XML的选项函数，一个小型的XML文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2003-2024 Michael R Sweet。
//
// 根据Apache许可证v2.0许可。有关更多信息，请参见“LICENSE”文件。
//

#include "mxml-private.h"


//
// 'mxmlOptionsDelete（）' - 释放加载/保存选项。
//

void
mxmlOptionsDelete(
    mxml_options_t *options)		// 输入 - 选项
{
  free(options);
}


//
// 'mxmlOptionsNew（）' - 分配加载/保存选项。
//
// 此函数创建一组新的加载/保存选项，用于@mxmlLoadFd@、@mxmlLoadFile@、@mxmlLoadFilename@、
// @mxmlLoadIO@、@mxmlLoadString@、@mxmlSaveAllocString@、@mxmlSaveFd@、@mxmlSaveFile@、
// @mxmlSaveFilename@、@mxmlSaveIO@和@mxmlSaveString@函数。选项可以在多次调用这些函数时重用，
// 并且应使用@mxmlOptionsDelete@函数释放。
//
// 默认的加载/保存选项使用常量类型`MXML_TYPE_TEXT`加载值，并使用72列的换行边距保存XML数据。
// 可以使用各种`mxmlOptionsSet`函数更改默认值，例如：
//
// ```c
// mxml_options_t *options = mxmlOptionsNew();
//
// /* 将值加载为不透明字符串 */
// mxmlOptionsSetTypeValue(options, MXML_TYPE_OPAQUE);
// ```
//
// 注意：使用Mini-XML库时最常见的编程错误是使用`MXML_TYPE_TEXT`节点类型加载XML文件，
// 该类型将内联文本作为一系列以空格分隔的单词返回，而不是使用`MXML_TYPE_OPAQUE`节点类型，
// 该类型将内联文本作为一个字符串返回（包括空格）。
//

mxml_options_t *			// 输出 - 选项
mxmlOptionsNew(void)
{
  mxml_options_t *options;		// 选项


  if ((options = (mxml_options_t *)calloc(1, sizeof(mxml_options_t))) != NULL)
  {
    // 设置默认值...
    options->type_value = MXML_TYPE_TEXT;
    options->wrap       = 72;

    if ((options->loc = localeconv()) != NULL)
    {
      if (!options->loc->decimal_point || !strcmp(options->loc->decimal_point, "."))
	options->loc = NULL;
      else
	options->loc_declen = strlen(options->loc->decimal_point);
    }
  }

  return (options);
}


//
// 'mxmlOptionsSetCustomCallbacks（）' - 设置自定义数据回调。
//
// 此函数设置用于加载和保存自定义数据类型的回调。加载回调`load_cb`接受回调数据指针`cbdata`、
// 节点指针和数据字符串，并在成功时返回`true`，错误时返回`false`，例如：
//
// ```c
// typedef struct
// {
//   unsigned year,    /* 年份 */
//            month,   /* 月份 */
//            day,     /* 日期 */
//            hour,    /* 小时 */
//            minute,  /* 分钟 */
//            second;  /* 秒钟 */
//   time_t   unix;    /* UNIX时间 */
// } iso_date_time_t;
//
// void
// my_custom_free_cb(void *cbdata, void *data)
// {
//   free(data);
// }
//
// bool
// my_custom_load_cb(void *cbdata, mxml_node_t *node, const char *data)
// {
//   iso_date_time_t *dt;
//   struct tm tmdata;
//
//   /* 分配自定义数据结构... */
//   dt = calloc(1, sizeof(iso_date_time_t));
//
//   /* 解析数据字符串... */
//   if (sscanf(data, "%u-%u-%uT%u:%u:%uZ", &(dt->year), &(dt->month),
//              &(dt->day), &(dt->hour), &(dt->minute), &(dt->second)) != 6)
//   {
//     /* 无法解析日期和时间数字... */
//     free(dt);
//     return (false);
//   }
//
//   /* 范围检查值... */
//   if (dt->month < 1 || dt->month > 12 || dt->day < 1 || dt->day > 31 ||
//       dt->hour < 0 || dt->hour > 23 || dt->minute < 0 || dt->minute > 59 ||
//       dt->second < 0 || dt->second > 60)
//   {
//     /* 日期信息超出范围... */
//     free(dt);
//     return (false);
//   }
//
//   /* 将ISO时间转换为UNIX时间（以秒为单位）... */
//   tmdata.tm_year = dt->year - 1900;
//   tmdata.tm_mon  = dt->month - 1;
//   tmdata.tm_day  = dt->day;
//   tmdata.tm_hour = dt->hour;
//   tmdata.tm_min  = dt->minute;
//   tmdata.tm_sec  = dt->second;
//
//   dt->unix = gmtime(&tmdata);
//
//   /* 设置自定义数据和释放函数... */
//   mxmlSetCustom(node, data, my_custom_free, /*cbdata*/NULL);
//
//   /* 返回无错误... */
//   return (true);
// }
// ```
//
// 保存回调`save_cb`接受回调数据指针`cbdata`和节点指针，并在成功时返回一个malloc的字符串，
// 错误时返回`NULL`，例如：
//
// ```c
// char *
// my_custom_save_cb(void *cbdata, mxml_node_t *node)
// {
//   char data[255];
//   iso_date_time_t *dt;
//
//   /* 获取自定义数据结构 */
//   dt = (iso_date_time_t *)mxmlGetCustom(node);
//
//   /* 生成日期/时间的字符串版本... */
//   snprintf(data, sizeof(data), "%04u-%02u-%02uT%02u:%02u:%02uZ",
//            dt->year, dt->month, dt->day, dt->hour, dt->minute, dt->second);
//
//   /* 复制字符串并返回... */
//   return (strdup(data));
// }
// ```
//

void
mxmlOptionsSetCustomCallbacks(
    mxml_options_t     *options,	// 输入 - 选项
    mxml_custload_cb_t load_cb,		// 输入 - 自定义加载回调函数
    mxml_custsave_cb_t save_cb,		// 输入 - 自定义保存回调函数
    void               *cbdata)		// 输入 - 自定义回调数据
{
  if (options)
  {
    options->custload_cb = load_cb;
    options->custsave_cb = save_cb;
    options->cust_cbdata = cbdata;
  }
}


//
// 'mxmlOptionsSetEntityCallback（）' - 设置在加载XML数据时使用的实体查找回调。
//
// 此函数设置在加载XML数据时用于查找命名XML字符实体的回调。回调函数`cb`接受回调数据指针`cbdata`
// 和实体名称。函数返回一个Unicode字符值，如果实体未知，则返回`-1`。例如，以下实体回调支持“euro”实体：
//
// ```c
// int my_entity_cb(void *cbdata, const char *name)
// {
//   if (!strcmp(name, "euro"))
//     return (0x20ac);
//   else
//     return (-1);
// }
// ```
//
// Mini-XML自动支持基本XML规范所需的“amp”、“gt”、“lt”和“quot”字符实体。
//

void
mxmlOptionsSetEntityCallback(
    mxml_options_t   *options,		// 输入 - 选项
    mxml_entity_cb_t cb,		// 输入 - 实体回调函数
    void             *cbdata)		// 输入 - 实体回调数据
{
  if (options)
  {
    options->entity_cb     = cb;
    options->entity_cbdata = cbdata;
  }
}


//
// 'mxmlOptionsSetErrorCallback（）' - 设置错误消息回调。
//
// 此函数设置在报告错误时使用的函数。回调`cb`接受数据指针`cbdata`和包含错误消息的字符串指针：
//
// ```c
// void my_error_cb(void *cbdata, const char *message)
// {
//   fprintf(stderr, "myprogram: %s\n", message);
// }
// ```
//
// 默认的错误回调将错误消息写入`stderr`文件。
//

void
mxmlOptionsSetErrorCallback(
    mxml_options_t  *options,		// 输入 - 选项
    mxml_error_cb_t cb,			// 输入 - 错误回调函数
    void            *cbdata)		// 输入 - 错误回调数据
{
  if (options)
  {
    options->error_cb     = cb;
    options->error_cbdata = cbdata;
  }
}


//
// 'mxmlOptionsSetSAXCallback（）' - 设置在读取XML数据时使用的SAX回调。
//
// 此函数设置在读取XML数据时使用的SAX回调。SAX回调函数`cb`和相关的回调数据`cbdata`用于启用
// 简单API for XML流模式。当解析XML节点树时，回调在调用时接收`cbdata`指针、`mxml_node_t`指针
// 和事件代码。函数返回`true`以继续处理，返回`false`以停止：
//
// ```c
// bool
// sax_cb(void *cbdata, mxml_node_t *node,
//        mxml_sax_event_t event)
// {
//   ... do something ...
//
//   /* 继续处理... */
//   return (true);
// }
// ```
//
// 事件将是以下之一：
//
// - `MXML_SAX_EVENT_CDATA`：刚刚读取CDATA。
// - `MXML_SAX_EVENT_COMMENT`：刚刚读取注释。
// - `MXML_SAX_EVENT_DATA`：刚刚读取数据（整数、不透明、实数或文本）。
// - `MXML_SAX_EVENT_DECLARATION`：刚刚读取声明。
// - `MXML_SAX_EVENT_DIRECTIVE`：刚刚读取处理指令/指令。
// - `MXML_SAX_EVENT_ELEMENT_CLOSE` - 刚刚读取关闭元素（`</element>`）
// - `MXML_SAX_EVENT_ELEMENT_OPEN` - 刚刚读取打开元素（`<element>`）
//
// 元素在处理关闭元素后*释放*。所有其他节点在处理后释放。SAX回调可以使用[mxmlRetain](@@)函数*保留*节点。
//

void
mxmlOptionsSetSAXCallback(
    mxml_options_t *options,		// 输入 - 选项
    mxml_sax_cb_t  cb,			// 输入 - SAX回调函数
    void           *cbdata)		// 输入 - SAX回调数据
{
  if (options)
  {
    options->sax_cb     = cb;
    options->sax_cbdata = cbdata;
  }
}


//
// 'mxmlOptionsSetTypeCallback（）' - 设置用于子节点/值节点的类型回调。
//
// 加载回调函数`cb`被调用以获取节点类型的子节点/值节点，并接收`cbdata`指针和`mxml_node_t`指针，例如：
//
// ```c
// mxml_type_t
// my_type_cb(void *cbdata, mxml_node_t *node)
// {
//   const char *type;
//
//  /*
//   * 可以查找属性和/或使用元素名称、层次结构等...
//   */
//
//   type = mxmlElementGetAttr(node, "type");
//   if (type == NULL)
//     type = mxmlGetElement(node);
//   if (type == NULL)
//     type = "text";
//
//   if (!strcmp(type, "integer"))
//     return (MXML_TYPE_INTEGER);
//   else if (!strcmp(type, "opaque"))
//     return (MXML_TYPE_OPAQUE);
//   else if (!strcmp(type, "real"))
//     return (MXML_TYPE_REAL);
//   else
//     return (MXML_TYPE_TEXT);
// }
// ```
//

void
mxmlOptionsSetTypeCallback(
    mxml_options_t *options,		// 输入 - 选项
    mxml_type_cb_t cb,			// 输入 - 类型回调函数
    void           *cbdata)		// 输入 - 类型回调数据
{
  if (options)
  {
    options->type_cb     = cb;
    options->type_cbdata = cbdata;
  }
}


//
// 'mxmlOptionsSetTypeValue（）' - 设置用于所有子节点/值节点的类型。
//
// 此函数为所有子节点/值节点设置一个常量节点类型。
//

void
mxmlOptionsSetTypeValue(
    mxml_options_t *options,		// 输入 - 选项
    mxml_type_t    type)		// 输入 - 值节点类型
{
  if (options)
  {
    options->type_cb    = NULL;
    options->type_value = type;
  }
}


//
// 'mxmlOptionsSetWhitespaceCallback（）' - 设置空白回调。
//
// 此函数设置保存XML数据时使用的空白回调。回调函数`cb`指定在每个元素之前和之后返回一个空白字符串
// 或`NULL`。函数接收回调数据指针`cbdata`、`mxml_node_t`指针和指示空白添加位置的“when”值，例如：
//
// ```c
// const char *my_whitespace_cb(void *cbdata, mxml_node_t *node, mxml_ws_t when)
// {
//   if (when == MXML_WS_BEFORE_OPEN || when == MXML_WS_AFTER_CLOSE)
//     return ("\n");
//   else
//     return (NULL);
// }
// ```
//

void
mxmlOptionsSetWhitespaceCallback(
    mxml_options_t *options,		// 输入 - 选项
    mxml_ws_cb_t   cb,			// 输入 - 空白回调函数
    void           *cbdata)		// 输入 - 空白回调数据
{
  if (options)
  {
    options->ws_cb     = cb;
    options->ws_cbdata = cbdata;
  }
}


//
// 'mxmlOptionsSetWrapMargin（）' - 设置保存XML数据时的换行边距。
//
// 此函数设置保存XML数据时使用的换行边距。当`column`为`0`时，禁用换行。
//

void
mxmlOptionsSetWrapMargin(
    mxml_options_t *options,		// 输入 - 选项
    int            column)		// 输入 - 换行列
{
  if (options)
    options->wrap = column;
}


//
// '_mxml_entity_string（）' - 获取与字符对应的实体（如果有）。
//

const char *				// 输出 - 实体或`NULL`（如果没有）
_mxml_entity_string(int ch)		// 输入 - 字符
{
  switch (ch)
  {
    case '&' :
        return ("&amp;");

    case '<' :
        return ("&lt;");

    case '>' :
        return ("&gt;");

    case '\"' :
        return ("&quot;");

    default :
        return (NULL);
  }
}


//
// '_mxml_entity_value（）' - 获取与命名实体对应的字符。
//
// 实体名称也可以是数字常量。如果名称未知，则返回`-1`。
//

int					// 输出 - Unicode字符
_mxml_entity_value(
    mxml_options_t *options,		// 输入 - 选项
    const char     *name)		// 输入 - 实体名称
{
  int		ch = -1;		// Unicode字符


  if (!name)
  {
    // 没有名称...
    return (-1);
  }
  else if (*name == '#')
  {
    // 数字实体...
    if (name[1] == 'x')
      ch = (int)strtol(name + 2, NULL, 16);
    else
      ch = (int)strtol(name + 1, NULL, 10);
  }
  else if (!strcmp(name, "amp"))
  {
    // Ampersand
    ch = '&';
  }
  else if (!strcmp(name, "gt"))
  {
    // Greater than
    ch = '>';
  }
  else if (!strcmp(name, "lt"))
  {
    // Less than
    ch = '<';
  }
  else if (!strcmp(name, "quot"))
  {
    // Double quote
    ch = '\"';
  }
  else if (options && options->entity_cb)
  {
    // 使用回调
    ch = (options->entity_cb)(options->entity_cbdata, name);
  }

  return (ch);
}


//
// '_mxml_error（）' - 显示错误消息。
//

void
_mxml_error(mxml_options_t *options,	// 输入 - 加载/保存选项
            const char     *format,	// 输入 - Printf样式的格式字符串
            ...)			// 输入 - 需要的其他参数
{
  va_list	ap;			// 指向参数的指针
  char		s[1024];		// 消息字符串


  // 检查输入范围...
  if (!format)
    return;

  // 格式化错误消息字符串...
  va_start(ap, format);
  vsnprintf(s, sizeof(s), format, ap);
  va_end(ap);

  // 然后显示错误消息...
  if (options->error_cb)
    (options->error_cb)(options->error_cbdata, s);
  else
    fprintf(stderr, "%s\n", s);
}