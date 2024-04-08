//
// Mini-XML 的节点获取函数，它是一个小型的 XML 文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2014-2024 Michael R Sweet。
//
// 根据 Apache License v2.0 许可。有关更多信息，请参阅 "LICENSE" 文件。
//

#include "mxml-private.h"


//
// 'mxmlGetCDATA()' - 获取 CDATA 节点的值。
//
// 此函数获取 CDATA 节点的字符串值。如果节点不是 CDATA 元素，则返回 `NULL`。
//

const char *				// O - CDATA 值或 `NULL`
mxmlGetCDATA(mxml_node_t *node)		// I - 要获取的节点
{
  // 范围检查输入...
  if (!node || node->type != MXML_TYPE_CDATA)
    return (NULL);

  // 返回 CDATA 字符串...
  return (node->value.cdata);
}


//
// 'mxmlGetComment()' - 获取注释节点的值。
//
// 此函数获取注释节点的字符串值。如果节点不是注释，则返回 `NULL`。
//

const char *				// O - 注释值或 `NULL`
mxmlGetComment(mxml_node_t *node)	// I - 要获取的节点
{
  // 范围检查输入...
  if (!node || node->type != MXML_TYPE_COMMENT)
    return (NULL);

  // 返回注释字符串...
  return (node->value.comment);
}


//
// 'mxmlGetCustom()' - 获取自定义节点的值。
//
// 此函数获取自定义节点的二进制值。如果节点（或其第一个子节点）不是自定义值节点，则返回 `NULL`。
//

const void *				// O - 自定义值或 `NULL`
mxmlGetCustom(mxml_node_t *node)	// I - 要获取的节点
{
  // 范围检查输入...
  if (!node)
    return (NULL);

  // 返回自定义值...
  if (node->type == MXML_TYPE_CUSTOM)
    return (node->value.custom.data);
  else if (node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_CUSTOM)
    return (node->child->value.custom.data);
  else
    return (NULL);
}


//
// 'mxmlGetDeclaration()' - 获取声明节点的值。
//
// 此函数获取声明节点的字符串值。如果节点不是声明，则返回 `NULL`。
//

const char *				// O - 声明值或 `NULL`
mxmlGetDeclaration(mxml_node_t *node)	// I - 要获取的节点
{
  // 范围检查输入...
  if (!node || node->type != MXML_TYPE_DECLARATION)
    return (NULL);

  // 返回注释字符串...
  return (node->value.declaration);
}


//
// 'mxmlGetDirective()' - 获取处理指令节点的值。
//
// 此函数获取处理指令的字符串值。如果节点不是处理指令，则返回 `NULL`。
//

const char *				// O - 注释值或 `NULL`
mxmlGetDirective(mxml_node_t *node)	// I - 要获取的节点
{
  // 范围检查输入...
  if (!node || node->type != MXML_TYPE_DIRECTIVE)
    return (NULL);

  // 返回注释字符串...
  return (node->value.directive);
}


//
// 'mxmlGetElement()' - 获取元素节点的名称。
//
// 此函数获取元素节点的名称。如果节点不是元素节点，则返回 `NULL`。
//

const char *				// O - 元素名称或 `NULL`
mxmlGetElement(mxml_node_t *node)	// I - 要获取的节点
{
  // 范围检查输入...
  if (!node || node->type != MXML_TYPE_ELEMENT)
    return (NULL);

  // 返回元素名称...
  return (node->value.element.name);
}


//
// 'mxmlGetFirstChild()' - 获取节点的第一个子节点。
//
// 此函数获取节点的第一个子节点。如果节点没有子节点，则返回 `NULL`。
//

mxml_node_t *				// O - 第一个子节点或 `NULL`
mxmlGetFirstChild(mxml_node_t *node)	// I - 要获取的节点
{
  // 返回第一个子节点...
  return (node ? node->child : NULL);
}


//
// 'mxmlGetInteger()' - 获取指定节点或其第一个子节点的整数值。
//
// 此函数获取整数节点的值。如果节点（或其第一个子节点）不是整数值节点，则返回 `0`。
//

long					// O - 整数值或 `0`
mxmlGetInteger(mxml_node_t *node)	// I - 要获取的节点
{
  // 范围检查输入...
  if (!node)
    return (0);

  // 返回整数值...
  if (node->type == MXML_TYPE_INTEGER)
    return (node->value.integer);
  else if (node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_INTEGER)
    return (node->child->value.integer);
  else
    return (0);
}


//
// 'mxmlGetLastChild()' - 获取节点的最后一个子节点。
//
// 此函数获取节点的最后一个子节点。如果节点没有子节点，则返回 `NULL`。
//

mxml_node_t *				// O - 最后一个子节点或 `NULL`
mxmlGetLastChild(mxml_node_t *node)	// I - 要获取的节点
{
  return (node ? node->last_child : NULL);
}


//
// 'mxmlGetNextSibling()' - 获取当前父节点的下一个节点。
//
// 此函数获取当前父节点的下一个节点。如果这是当前父节点的最后一个子节点，则返回 `NULL`。
//

mxml_node_t *
mxmlGetNextSibling(mxml_node_t *node)	// I - 要获取的节点
{
  return (node ? node->next : NULL);
}


//
// 'mxmlGetOpaque()' - 获取节点或其第一个子节点的不透明字符串值。
//
// 此函数获取不透明节点的字符串值。如果节点（或其第一个子节点）不是不透明值节点，则返回 `NULL`。
//

const char *				// O - 不透明字符串或 `NULL`
mxmlGetOpaque(mxml_node_t *node)	// I - 要获取的节点
{
  // 范围检查输入...
  if (!node)
    return (NULL);

  // 返回不透明值...
  if (node->type == MXML_TYPE_OPAQUE)
    return (node->value.opaque);
  else if (node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_OPAQUE)
    return (node->child->value.opaque);
  else
    return (NULL);
}


//
// 'mxmlGetParent()' - 获取父节点。
//
// 此函数获取节点的父节点。如果是根节点，则返回 `NULL`。
//

mxml_node_t *				// O - 父节点或 `NULL`
mxmlGetParent(mxml_node_t *node)	// I - 要获取的节点
{
  return (node ? node->parent : NULL);
}


//
// 'mxmlGetPrevSibling()' - 获取当前父节点的前一个节点。
//
// 此函数获取当前父节点的前一个节点。如果这是当前父节点的第一个子节点，则返回 `NULL`。
//

mxml_node_t *				// O - 前一个节点或 `NULL`
mxmlGetPrevSibling(mxml_node_t *node)	// I - 要获取的节点
{
  return (node ? node->prev : NULL);
}


//
// 'mxmlGetReal()' - 获取节点或其第一个子节点的实数值。
//
// 此函数获取实数值节点的值。如果节点（或其第一个子节点）不是实数值节点，则返回 `0.0`。
//

double					// O - 实数值或 0.0
mxmlGetReal(mxml_node_t *node)		// I - 要获取的节点
{
  // 范围检查输入...
  if (!node)
    return (0.0);

  // 返回实数值...
  if (node->type == MXML_TYPE_REAL)
    return (node->value.real);
  else if (node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_REAL)
    return (node->child->value.real);
  else
    return (0.0);
}


//
// 'mxmlGetText()' - 获取节点或其第一个子节点的文本值。
//
// 此函数获取文本节点的字符串值和空白字符值。如果节点（或其第一个子节点）不是文本节点，则返回 `NULL` 和 `false`。
// 如果不想获取空白字符值，则可以将 `whitespace` 参数设置为 `NULL`。
//
// 注意：文本节点由空白字符分隔的单词组成。当使用 `MXML_TYPE_TEXT` 节点读取 XML 文件时，您将只获取单个单词的文本。
// 如果要获取 XML 文件中元素之间的整个字符串，您必须使用 `MXML_TYPE_OPAQUE` 节点读取 XML 文件，并使用 @link mxmlGetOpaque@ 函数获取结果字符串。
//

const char *				// O - 文本字符串或 `NULL`
mxmlGetText(mxml_node_t *node,		// I - 要获取的节点
            bool        *whitespace)	// O - 字符串之前是否有空白字符，`false` 否则为 `true`
{
  // 范围检查输入...
  if (!node)
  {
    if (whitespace)
      *whitespace = false;

    return (NULL);
  }

  // 返回整数值...
  if (node->type == MXML_TYPE_TEXT)
  {
    if (whitespace)
      *whitespace = node->value.text.whitespace;

    return (node->value.text.string);
  }
  else if (node->type == MXML_TYPE_ELEMENT && node->child && node->child->type == MXML_TYPE_TEXT)
  {
    if (whitespace)
      *whitespace = node->child->value.text.whitespace;

    return (node->child->value.text.string);
  }
  else
  {
    if (whitespace)
      *whitespace = false;

    return (NULL);
  }
}


//
// 'mxmlGetType()' - 获取节点类型。
//
// 此函数获取 `node` 的类型。如果 `node` 是 `NULL`，则返回 `MXML_TYPE_IGNORE`。
//

mxml_type_t				// O - 节点类型
mxmlGetType(mxml_node_t *node)		// I - 要获取的节点
{
  // 范围检查输入...
  if (!node)
    return (MXML_TYPE_IGNORE);

  // 返回节点类型...
  return (node->type);
}


//
// 'mxmlGetUserData()' - 获取节点的用户数据指针。
//
// 此函数获取与 `node` 相关联的用户数据指针。
//

void *					// O - 用户数据指针
mxmlGetUserData(mxml_node_t *node)	// I - 要获取的节点
{
  // 范围检查输入...
  if (!node)
    return (NULL);

  // 返回用户数据指针...
  return (node->user_data);
}
