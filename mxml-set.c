//
// Mini-XML的节点设置函数，一个小型的XML文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2003-2024 Michael R. Sweet。
//
// 根据Apache许可证v2.0进行许可。有关更多信息，请参阅“LICENSE”文件。
//

#include "mxml-private.h"


//
// 'mxmlSetCDATA（）' - 设置CDATA节点的数据。
//
// 此函数设置CDATA节点的值字符串。如果节点（或其第一个子节点）不是CDATA节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetCDATA（mxml_node_t *node，		// 要设置的节点
             const char  *data）		// 新数据字符串
{
  char	*s;				// 新元素名称


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_CDATA)
    node = node->child;

  if (!node || node->type != MXML_TYPE_CDATA)
    return (false);
  else if (!data)
    return (false);

  if (data == node->value.cdata)
  {
    // 不要更改值...
    return (true);
  }

  // 分配新值，释放任何旧元素值，并设置新值...
  if ((s = _mxml_strcopy（data）) == NULL)
    return (false);

  _mxml_strfree（node->value.cdata）;
  node->value.cdata = s;

  return (true);
}


//
// 'mxmlSetCDATAf（）' - 将CDATA数据设置为格式化字符串。
//
// 此函数将格式化字符串值设置为CDATA节点。如果节点（或其第一个子节点）不是CDATA节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetCDATAf（mxml_node_t *node，	// 节点
	      const char *format，	// `printf`样式格式字符串
	      ...）			// 根据需要的其他参数
{
  va_list	ap;			// 参数指针
  char		buffer[16384];		// 格式缓冲区
  char		*s;			// 临时字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_CDATA)
    node = node->child;

  if (!node || node->type != MXML_TYPE_CDATA)
    return (false);
  else if (!format)
    return (false);

  // 格式化新字符串，释放任何旧字符串值，并设置新值...
  va_start(ap, format);
  vsnprintf(buffer, sizeof(buffer), format, ap);
  va_end(ap);

  if ((s = _mxml_strcopy（buffer）) == NULL)
    return (false);

  _mxml_strfree（node->value.cdata）;
  node->value.cdata = s;

  return (true);
}


//
// 'mxmlSetComment（）' - 将注释设置为字面字符串。
//
// 此函数设置注释节点的字符串值。

bool					// 成功返回true，失败返回false
mxmlSetComment（mxml_node_t *node，	// 节点
               const char  *comment）	// 字面字符串
{
  char *s;				// 新字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_COMMENT)
    node = node->child;

  if (!node || node->type != MXML_TYPE_COMMENT)
    return (false);
  else if (!comment)
    return (false);

  if (comment == node->value.comment)
    return (true);

  // 释放任何旧字符串值并设置新值...
  if ((s = _mxml_strcopy（comment）) == NULL)
    return (false);

  _mxml_strfree（node->value.comment）;
  node->value.comment = s;

  return (true);
}


//
// 'mxmlSetCommentf（）' - 将注释设置为格式化字符串。
//
// 此函数将格式化字符串值设置为注释节点。

bool					// 成功返回true，失败返回false
mxmlSetCommentf（mxml_node_t *node，	// 节点
                const char *format，	// `printf`样式格式字符串
		...）			// 根据需要的其他参数
{
  va_list	ap;			// 参数指针
  char		buffer[16384];		// 格式缓冲区
  char		*s;			// 临时字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_COMMENT)
    node = node->child;

  if (!node || node->type != MXML_TYPE_COMMENT)
    return (false);
  else if (!format)
    return (false);

  // 格式化新字符串，释放任何旧字符串值，并设置新值...
  va_start(ap, format);
  vsnprintf(buffer, sizeof(buffer), format, ap);
  va_end(ap);

  if ((s = _mxml_strcopy（buffer）) == NULL)
    return (false);

  _mxml_strfree（node->value.comment）;
  node->value.comment = s;

  return (true);
}


//
// 'mxmlSetCustom（）' - 设置自定义数据节点的数据和析构函数。
//
// 此函数设置自定义数据节点的数据指针`data`和析构函数回调`destroy_cb`。如果节点（或其第一个子节点）不是自定义节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetCustom(
    mxml_node_t        *node,		// 要设置的节点
    void               *data,		// 新数据指针
    mxml_custfree_cb_t free_cb,		// 释放回调函数
    void               *free_cbdata)	// 释放回调数据
{
  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_CUSTOM)
    node = node->child;

  if (!node || node->type != MXML_TYPE_CUSTOM)
    return (false);

  if (data == node->value.custom.data)
    goto set_free_callback;

  // 释放任何旧元素值并设置新值...
  if (node->value.custom.data && node->value.custom.free_cb)
    (node->value.custom.free_cb)(node->value.custom.free_cbdata, node->value.custom.data);

  node->value.custom.data = data;

  set_free_callback:

  node->value.custom.free_cb     = free_cb;
  node->value.custom.free_cbdata = free_cbdata;

  return (true);
}


//
// 'mxmlSetDeclaration（）' - 将声明设置为字面字符串。
//
// 此函数设置声明节点的字符串值。

bool					// 成功返回true，失败返回false
mxmlSetDeclaration(
    mxml_node_t *node,			// 节点
    const char  *declaration)		// 字面字符串
{
  char *s;				// 新字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_DECLARATION)
    node = node->child;

  if (!node || node->type != MXML_TYPE_DECLARATION)
    return (false);
  else if (!declaration)
    return (false);

  if (declaration == node->value.declaration)
    return (true);

  // 释放任何旧字符串值并设置新值...
  if ((s = _mxml_strcopy（declaration）) == NULL)
    return (false);

  _mxml_strfree（node->value.declaration）;
  node->value.declaration = s;

  return (true);
}


//
// 'mxmlSetDeclarationf（）' - 将声明设置为格式化字符串。
//
// 此函数将格式化字符串值设置为声明节点。

bool					// 成功返回true，失败返回false
mxmlSetDeclarationf(mxml_node_t *node,	// 节点
                    const char *format,	// `printf`样式格式字符串
		    ...)		// 根据需要的其他参数
{
  va_list	ap;			// 参数指针
  char		buffer[16384];		// 格式缓冲区
  char		*s;			// 临时字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_COMMENT)
    node = node->child;

  if (!node || node->type != MXML_TYPE_COMMENT)
    return (false);
  else if (!format)
    return (false);

  // 格式化新字符串，释放任何旧字符串值，并设置新值...
  va_start(ap, format);
  vsnprintf(buffer, sizeof(buffer), format, ap);
  va_end(ap);

  if ((s = _mxml_strcopy（buffer）) == NULL)
    return (false);

  _mxml_strfree（node->value.declaration）;
  node->value.declaration = s;

  return (true);
}


//
// 'mxmlSetDirective（）' - 将处理指令设置为字面字符串。
//
// 此函数设置处理指令节点的字符串值。

bool					// 成功返回true，失败返回false
mxmlSetDirective(mxml_node_t *node,	// 节点
                 const char  *directive)// 字面字符串
{
  char *s;				// 新字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_DIRECTIVE)
    node = node->child;

  if (!node || node->type != MXML_TYPE_DIRECTIVE)
    return (false);
  else if (!directive)
    return (false);

  if (directive == node->value.directive)
    return (true);

  // 释放任何旧字符串值并设置新值...
  if ((s = _mxml_strcopy（directive）) == NULL)
    return (false);

  _mxml_strfree（node->value.directive）;
  node->value.directive = s;

  return (true);
}


//
// 'mxmlSetDirectivef（）' - 将处理指令设置为格式化字符串。
//
// 此函数将格式化字符串值设置为处理指令节点。

bool					// 成功返回true，失败返回false
mxmlSetDirectivef(mxml_node_t *node,	// 节点
                  const char *format,	// `printf`样式格式字符串
		  ...)			// 根据需要的其他参数
{
  va_list	ap;			// 参数指针
  char		buffer[16384];		// 格式缓冲区
  char		*s;			// 临时字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_DIRECTIVE)
    node = node->child;

  if (!node || node->type != MXML_TYPE_DIRECTIVE)
    return (false);
  else if (!format)
    return (false);

  // 格式化新字符串，释放任何旧字符串值，并设置新值...
  va_start(ap, format);
  vsnprintf(buffer, sizeof(buffer), format, ap);
  va_end(ap);

  if ((s = _mxml_strcopy（buffer）) == NULL)
    return (false);

  _mxml_strfree（node->value.directive）;
  node->value.directive = s;

  return (true);
}


//
// 'mxmlSetElement（）' - 设置元素节点的名称。
//
// 此函数设置元素节点的名称。如果节点不是元素节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetElement(mxml_node_t *node,	// 要设置的节点
               const char  *name)	// 新名称字符串
{
  char *s;				// 新名称字符串


  // 输入范围检查...
  if (!node || node->type != MXML_TYPE_ELEMENT)
    return (false);
  else if (!name)
    return (false);

  if (name == node->value.element.name)
    return (true);

  // 释放任何旧元素值并设置新值...
  if ((s = _mxml_strcopy（name）) == NULL)
    return (false);

  _mxml_strfree（node->value.element.name）;
  node->value.element.name = s;

  return (true);
}


//
// 'mxmlSetInteger（）' - 设置整数节点的值。
//
// 此函数设置整数节点的值。如果节点（或其第一个子节点）不是整数节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetInteger(mxml_node_t *node,	// 要设置的节点
               long        integer)	// 整数值
{
  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_INTEGER)
    node = node->child;

  if (!node || node->type != MXML_TYPE_INTEGER)
    return (false);

  // 设置新值并返回...
  node->value.integer = integer;

  return (true);
}


//
// 'mxmlSetOpaque（）' - 设置不透明节点的值。
//
// 此函数设置不透明节点的字符串值。如果节点（或其第一个子节点）不是不透明节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetOpaque(mxml_node_t *node,	// 要设置的节点
              const char  *opaque)	// 不透明字符串
{
  char *s;				// 新不透明字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_OPAQUE)
    node = node->child;

  if (!node || node->type != MXML_TYPE_OPAQUE)
    return (false);
  else if (!opaque)
    return (false);

  if (node->value.opaque == opaque)
    return (true);

  // 释放任何旧不透明值并设置新值...
  if ((s = _mxml_strcopy（opaque）) == NULL)
    return (false);

  _mxml_strfree（node->value.opaque）;
  node->value.opaque = s;

  return (true);
}


//
// 'mxmlSetOpaquef（）' - 将不透明字符串节点的值设置为格式化字符串。
//
// 此函数将格式化字符串值设置为不透明节点。如果节点（或其第一个子节点）不是不透明节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetOpaquef(mxml_node_t *node,	// 要设置的节点
               const char  *format,	// `printf`样式格式字符串
	       ...)			// 根据需要的其他参数
{
  va_list	ap;			// 参数指针
  char		buffer[16384];		// 格式缓冲区
  char		*s;			// 临时字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_OPAQUE)
    node = node->child;

  if (!node || node->type != MXML_TYPE_OPAQUE)
    return (false);
  else if (!format)
    return (false);

  // 格式化新字符串，释放任何旧字符串值，并设置新值...
  va_start(ap, format);
  vsnprintf(buffer, sizeof(buffer), format, ap);
  va_end(ap);

  if ((s = _mxml_strcopy（buffer）) == NULL)
    return (false);

  _mxml_strfree（node->value.opaque）;
  node->value.opaque = s;

  return (true);
}


//
// 'mxmlSetReal（）' - 设置实数值节点的值。
//
// 此函数设置实数值节点的值。如果节点（或其第一个子节点）不是实数值节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetReal(mxml_node_t *node,		// 要设置的节点
            double      real)		// 实数值
{
  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_REAL)
    node = node->child;

  if (!node || node->type != MXML_TYPE_REAL)
    return (false);

  // 设置新值并返回...
  node->value.real = real;

  return (true);
}


//
// 'mxmlSetText（）' - 设置文本节点的值。
//
// 此函数设置文本节点的字符串和空白值。如果节点（或其第一个子节点）不是文本节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetText(mxml_node_t *node,		// 要设置的节点
            bool        whitespace,	// `true` = 前导空白，`false` = 无空白
	    const char  *string)	// 字符串
{
  char *s;				// 新字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_TEXT)
    node = node->child;

  if (!node || node->type != MXML_TYPE_TEXT)
    return (false);
  else if (!string)
    return (false);

  if (string == node->value.text.string)
  {
    node->value.text.whitespace = whitespace;
    return (true);
  }

  // 释放任何旧字符串值并设置新值...
  if ((s = _mxml_strcopy（string）) == NULL)
    return (false);

  _mxml_strfree（node->value.text.string）;

  node->value.text.whitespace = whitespace;
  node->value.text.string     = s;

  return (true);
}


//
// 'mxmlSetTextf（）' - 将文本节点的值设置为格式化字符串。
//
// 此函数将格式化字符串和空白值设置为文本节点的值。如果节点（或其第一个子节点）不是文本节点，则不会更改该节点。

bool					// 成功返回true，失败返回false
mxmlSetTextf(mxml_node_t *node,		// 要设置的节点
             bool        whitespace,	// `true` = 前导空白，`false` = 无空白
             const char  *format,	// `printf`样式格式字符串
	     ...)			// 根据需要的其他参数
{
  va_list	ap;			// 参数指针
  char		buffer[16384];		// 格式缓冲区
  char		*s;			// 临时字符串


  // 输入范围检查...
  if (node && node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_TEXT)
    node = node->child;

  if (!node || node->type != MXML_TYPE_TEXT)
    return (false);
  else if (!format)
    return (false);

  // 释放任何旧字符串值并设置新值...
  va_start(ap, format);
  vsnprintf(buffer, sizeof(buffer), format, ap);
  va_end(ap);

  if ((s = _mxml_strcopy（buffer）) == NULL)
    return (false);

  _mxml_strfree（node->value.text.string）;

  node->value.text.whitespace = whitespace;
  node->value.text.string     = s;

  return (true);
}


//
// 'mxmlSetUserData（）' - 设置节点的用户数据指针。
//

bool					// 成功返回true，失败返回false
mxmlSetUserData(mxml_node_t *node,	// 要设置的节点
                void        *data)	// 用户数据指针
{
  // 输入范围检查...
  if (!node)
    return (false);

  // 设置用户数据指针并返回...
  node->user_data = data;
  return (true);
}