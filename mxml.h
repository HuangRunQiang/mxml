//
// Header file for Mini-XML, a small XML file parsing library.
//
// https://www.msweet.org/mxml
//
// Copyright © 2003-2024 by Michael R Sweet.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

#ifndef MXML_H
#  define MXML_H
#  include <stdio.h>
#  include <stdlib.h>
#  include <stdbool.h>
#  include <stdint.h>
#  include <string.h>
#  include <ctype.h>
#  include <errno.h>
#  include <limits.h>
#  ifdef __cplusplus
extern "C" {
#  endif // __cplusplus


// 常量...
//

#  define MXML_MAJOR_VERSION	4	// 主版本号
#  define MXML_MINOR_VERSION	0	// 次版本号

#  ifdef __GNUC__
#    define MXML_FORMAT(a,b)	__attribute__ ((__format__ (__printf__, a, b)))
#  else
#    define MXML_FORMAT(a,b)
#  endif // __GNUC__


// 数据类型...
//

typedef enum mxml_add_e			// @link mxmlAdd@ 添加值
{
  MXML_ADD_BEFORE,			// 在指定节点之前添加节点
  MXML_ADD_AFTER			// 在指定节点之后添加节点
} mxml_add_t;

typedef enum mxml_descend_e		// @link mxmlFindElement@、@link mxmlWalkNext@ 和 @link mxmlWalkPrev@ 下降值
{
  MXML_DESCEND_FIRST = -1,		// 第一次查找时下降到子节点
  MXML_DESCEND_NONE = 0,		// 查找/遍历时不下降到子节点
  MXML_DESCEND_ALL = 1			// 查找/遍历时下降到子节点
} mxml_descend_t;

typedef enum mxml_sax_event_e		// SAX 事件类型
{
  MXML_SAX_EVENT_CDATA,			// CDATA 节点
  MXML_SAX_EVENT_COMMENT,		// 注释节点
  MXML_SAX_EVENT_DATA,			// 数据节点
  MXML_SAX_EVENT_DECLARATION,		// 声明节点
  MXML_SAX_EVENT_DIRECTIVE,		// 处理指令节点
  MXML_SAX_EVENT_ELEMENT_CLOSE,		// 元素关闭
  MXML_SAX_EVENT_ELEMENT_OPEN		// 元素打开
} mxml_sax_event_t;

typedef enum mxml_type_e		// XML节点类型
{
  MXML_TYPE_IGNORE = -1,		// 忽略/丢弃节点
  MXML_TYPE_CDATA,			// CDATA 值（"<[CDATA[...]]>"）
  MXML_TYPE_COMMENT,			// 注释（"<!--...-->"）
  MXML_TYPE_DECLARATION,		// 声明（"<!...>"）
  MXML_TYPE_DIRECTIVE,			// 处理指令（"<?...?>"）
  MXML_TYPE_ELEMENT,			// 带属性的XML元素
  MXML_TYPE_INTEGER,			// 整数值
  MXML_TYPE_OPAQUE,			// 不透明字符串
  MXML_TYPE_REAL,			// 实数值
  MXML_TYPE_TEXT,			// 文本片段
  MXML_TYPE_CUSTOM			// 自定义数据
} mxml_type_t;

typedef enum mxml_ws_e			// 空白符周期
{
  MXML_WS_BEFORE_OPEN,			// 打开标签之前的回调
  MXML_WS_AFTER_OPEN,			// 打开标签之后的回调
  MXML_WS_BEFORE_CLOSE,			// 关闭标签之前的回调
  MXML_WS_AFTER_CLOSE,			// 关闭标签之后的回调
} mxml_ws_t;

typedef void (*mxml_error_cb_t)(void *cbdata, const char *message);
					// 错误回调函数

typedef struct _mxml_node_s mxml_node_t;// XML节点

typedef struct _mxml_index_s mxml_index_t;
					// XML节点索引

typedef struct _mxml_options_s mxml_options_t;
					// XML选项

typedef void (*mxml_custfree_cb_t)(void *cbdata, void *custdata);
					// 自定义数据析构函数

typedef bool (*mxml_custload_cb_t)(void *cbdata, mxml_node_t *node, const char *s);
					// 自定义数据加载回调函数


typedef char *(*mxml_custsave_cb_t)(void *cbdata, mxml_node_t *node);
					// 自定义数据保存回调函数

typedef int (*mxml_entity_cb_t)(void *cbdata, const char *name);
					// 实体回调函数

typedef size_t (*mxml_io_cb_t)(void *cbdata, void *buffer, size_t bytes);
					// 读/写回调函数

typedef bool (*mxml_sax_cb_t)(void *cbdata, mxml_node_t *node, mxml_sax_event_t event);
					// SAX回调函数

typedef char *(*mxml_strcopy_cb_t)(void *cbdata, const char *s);
					// 字符串复制/分配回调函数
typedef void (*mxml_strfree_cb_t)(void *cbdata, char *s);
					// 字符串释放回调函数

typedef mxml_type_t (*mxml_type_cb_t)(void *cbdata, mxml_node_t *node);
					// 类型回调函数

typedef const char *(*mxml_ws_cb_t)(void *cbdata, mxml_node_t *node, mxml_ws_t when);
					// 空白符回调函数




//
// 函数原型...
//

/**
 * @brief 在父节点中添加子节点。
 *
 * @param parent 父节点指针。
 * @param add 添加方式。
 * @param child 子节点指针。
 * @param node 要添加的节点指针。
 */
extern void mxmlAdd(mxml_node_t *parent, mxml_add_t add, mxml_node_t *child, mxml_node_t *node);

/**
 * @brief 删除节点。
 *
 * @param node 要删除的节点指针。
 */
extern void mxmlDelete(mxml_node_t *node);

/**
 * @brief 清除节点的属性。
 *
 * @param node 节点指针。
 * @param name 属性名称。
 */
extern void mxmlElementClearAttr(mxml_node_t *node, const char *name);

/**
 * @brief 获取节点的属性值。
 *
 * @param node 节点指针。
 * @param name 属性名称。
 * @return 属性值的指针。
 */
extern const char *mxmlElementGetAttr(mxml_node_t *node, const char *name);

/**
 * @brief 通过索引获取节点的属性名称和值。
 *
 * @param node 节点指针。
 * @param idx 索引。
 * @param name 属性名称的指针。
 * @return 属性值的指针。
 */
extern const char *mxmlElementGetAttrByIndex(mxml_node_t *node, size_t idx, const char **name);

/**
 * @brief 获取节点的属性数量。
 *
 * @param node 节点指针。
 * @return 属性数量。
 */
extern size_t mxmlElementGetAttrCount(mxml_node_t *node);

/**
 * @brief 设置节点的属性。
 *
 * @param node 节点指针。
 * @param name 属性名称。
 * @param value 属性值。
 */
extern void mxmlElementSetAttr(mxml_node_t *node, const char *name, const char *value);

/**
 * @brief 使用格式化字符串设置节点的属性。
 *
 * @param node 节点指针。
 * @param name 属性名称。
 * @param format 格式化字符串。
 * @param ... 格式化参数。
 */
extern void mxmlElementSetAttrf(mxml_node_t *node, const char *name, const char *format, ...) MXML_FORMAT(3,4);


/**
 * @brief 在给定节点及其子节点中查找具有指定元素、属性和值的节点。
 *
 * @param node 起始节点指针。
 * @param top 顶层节点指针。
 * @param element 要查找的元素名称。
 * @param attr 要匹配的属性名称。
 * @param value 要匹配的属性值。
 * @param descend 是否向下搜索子节点。
 * @return 找到的节点指针，如果未找到则返回NULL。
 */
extern mxml_node_t *mxmlFindElement(mxml_node_t *node, mxml_node_t *top, const char *element, const char *attr, const char *value, mxml_descend_t descend);

/**
 * @brief 在给定节点及其子节点中查找具有指定路径的节点。
 *
 * @param node 起始节点指针。
 * @param path 要查找的路径。
 * @return 找到的节点指针，如果未找到则返回NULL。
 */
extern mxml_node_t *mxmlFindPath(mxml_node_t *node, const char *path);


/**
 * @brief 获取节点的CDATA内容。
 *
 * @param node 节点指针。
 * @return CDATA内容的指针。
 */
extern const char *mxmlGetCDATA(mxml_node_t *node);

/**
 * @brief 获取节点的注释内容。
 *
 * @param node 节点指针。
 * @return 注释内容的指针。
 */
extern const char *mxmlGetComment(mxml_node_t *node);

/**
 * @brief 获取节点的自定义数据。
 *
 * @param node 节点指针。
 * @return 自定义数据的指针。
 */
extern const void *mxmlGetCustom(mxml_node_t *node);

/**
 * @brief 获取节点的声明内容。
 *
 * @param node 节点指针。
 * @return 声明内容的指针。
 */
extern const char *mxmlGetDeclaration(mxml_node_t *node);

/**
 * @brief 获取节点的指令内容。
 *
 * @param node 节点指针。
 * @return 指令内容的指针。
 */
extern const char *mxmlGetDirective(mxml_node_t *node);

/**
 * @brief 获取节点的元素名称。
 *
 * @param node 节点指针。
 * @return 元素名称的指针。
 */
extern const char *mxmlGetElement(mxml_node_t *node);

/**
 * @brief 获取节点的第一个子节点。
 *
 * @param node 节点指针。
 * @return 第一个子节点的指针，如果没有子节点则返回NULL。
 */
extern mxml_node_t *mxmlGetFirstChild(mxml_node_t *node);

/**
 * @brief 获取节点的整数值。
 *
 * @param node 节点指针。
 * @return 节点的整数值。
 */
extern long mxmlGetInteger(mxml_node_t *node);

/**
 * @brief 获取节点的最后一个子节点。
 *
 * @param node 节点指针。
 * @return 最后一个子节点的指针，如果没有子节点则返回NULL。
 */
extern mxml_node_t *mxmlGetLastChild(mxml_node_t *node);

/**
 * @brief 获取节点的下一个兄弟节点。
 *
 * @param node 节点指针。
 * @return 下一个兄弟节点的指针，如果没有下一个兄弟节点则返回NULL。
 */
extern mxml_node_t *mxmlGetNextSibling(mxml_node_t *node);

/**
 * @brief 获取节点的不透明数据。
 *
 * @param node 节点指针。
 * @return 不透明数据的指针。
 */
extern const char *mxmlGetOpaque(mxml_node_t *node);

/**
 * @brief 获取节点的父节点。
 *
 * @param node 节点指针。
 * @return 父节点的指针，如果没有父节点则返回NULL。
 */
extern mxml_node_t *mxmlGetParent(mxml_node_t *node);

/**
 * @brief 获取节点的上一个兄弟节点。
 *
 * @param node 节点指针。
 * @return 上一个兄弟节点的指针，如果没有上一个兄弟节点则返回NULL。
 */
extern mxml_node_t *mxmlGetPrevSibling(mxml_node_t *node);

/**
 * @brief 获取节点的实数值。
 *
 * @param node 节点指针。
 * @return 节点的实数值。
 */
extern double mxmlGetReal(mxml_node_t *node);

/**
 * @brief 获取节点的引用计数。
 *
 * @param node 节点指针。
 * @return 节点的引用计数。
 */
extern size_t mxmlGetRefCount(mxml_node_t *node);

/**
 * @brief 获取节点的文本内容。
 *
 * @param node 节点指针。
 * @param whitespace 指向布尔值的指针，用于指示是否保留文本内容中的空白字符。
 * @return 文本内容的指针。
 */
extern const char *mxmlGetText(mxml_node_t *node, bool *whitespace);

/**
 * @brief 获取节点的类型。
 *
 * @param node 节点指针。
 * @return 节点的类型。
 */
extern mxml_type_t mxmlGetType(mxml_node_t *node);

/**
 * @brief 获取节点的用户自定义数据。
 *
 * @param node 节点指针。
 * @return 用户自定义数据的指针。
 */
extern void *mxmlGetUserData(mxml_node_t *node);

/*****************************************************/
/**
 * @brief 删除索引。
 *
 * @param ind 索引指针。
 */
extern void mxmlIndexDelete(mxml_index_t *ind);

/**
 * @brief 遍历索引中的节点。
 *
 * @param ind 索引指针。
 * @return 索引中的下一个节点指针，如果已经遍历完则返回NULL。
 */
extern mxml_node_t *mxmlIndexEnum(mxml_index_t *ind);

/**
 * @brief 在索引中查找指定元素和值的节点。
 *
 * @param ind 索引指针。
 * @param element 要查找的元素名称。
 * @param value 要查找的值。
 * @return 第一个匹配的节点指针，如果没有匹配的节点则返回NULL。
 */
extern mxml_node_t *mxmlIndexFind(mxml_index_t *ind, const char *element, const char *value);

/**
 * @brief 获取索引中的节点数量。
 *
 * @param ind 索引指针。
 * @return 索引中的节点数量。
 */
extern size_t mxmlIndexGetCount(mxml_index_t *ind);

/**
 * @brief 创建一个新的索引。
 *
 * @param node 要索引的节点指针。
 * @param element 要索引的元素名称。
 * @param attr 要索引的属性名称，如果为NULL，则索引所有具有指定元素名称的节点。
 * @return 新创建的索引指针。
 */
extern mxml_index_t *mxmlIndexNew(mxml_node_t *node, const char *element, const char *attr);

/**
 * @brief 重置索引的遍历位置。
 *
 * @param ind 索引指针。
 * @return 索引中的第一个节点指针，如果索引为空则返回NULL。
 */
extern mxml_node_t *mxmlIndexReset(mxml_index_t *ind);


/*****************************************************/
/**
 * @brief 从文件描述符加载XML数据并创建节点树。
 *
 * @param top 最顶层节点的指针，如果为NULL，则创建一个新的节点树。
 * @param options 加载选项指针，如果为NULL，则使用默认选项。
 * @param fd 文件描述符。
 * @return 创建的节点树的根节点指针。
 */
extern mxml_node_t *mxmlLoadFd(mxml_node_t *top, mxml_options_t *options, int fd);

/**
 * @brief 从文件加载XML数据并创建节点树。
 *
 * @param top 最顶层节点的指针，如果为NULL，则创建一个新的节点树。
 * @param options 加载选项指针，如果为NULL，则使用默认选项。
 * @param fp 文件指针。
 * @return 创建的节点树的根节点指针。
 */
extern mxml_node_t *mxmlLoadFile(mxml_node_t *top, mxml_options_t *options, FILE *fp);

/**
 * @brief 从文件名加载XML数据并创建节点树。
 *
 * @param top 最顶层节点的指针，如果为NULL，则创建一个新的节点树。
 * @param options 加载选项指针，如果为NULL，则使用默认选项。
 * @param filename 文件名。
 * @return 创建的节点树的根节点指针。
 */
extern mxml_node_t *mxmlLoadFilename(mxml_node_t *top, mxml_options_t *options, const char *filename);

/**
 * @brief 从自定义输入/输出回调函数加载XML数据并创建节点树。
 *
 * @param top 最顶层节点的指针，如果为NULL，则创建一个新的节点树。
 * @param options 加载选项指针，如果为NULL，则使用默认选项。
 * @param io_cb 输入/输出回调函数。
 * @param io_cbdata 输入/输出回调函数的数据指针。
 * @return 创建的节点树的根节点指针。
 */
extern mxml_node_t *mxmlLoadIO(mxml_node_t *top, mxml_options_t *options, mxml_io_cb_t io_cb, void *io_cbdata);

/**
 * @brief 从字符串加载XML数据并创建节点树。
 *
 * @param top 最顶层节点的指针，如果为NULL，则创建一个新的节点树。
 * @param options 加载选项指针，如果为NULL，则使用默认选项。
 * @param s 字符串指针。
 * @return 创建的节点树的根节点指针。
 */
extern mxml_node_t *mxmlLoadString(mxml_node_t *top, mxml_options_t *options, const char *s);

/*****************************************************/
/**
 * @brief 删除选项。
 *
 * @param options 选项指针。
 */
extern void mxmlOptionsDelete(mxml_options_t *options);

/**
 * @brief 创建新的选项。
 *
 * @return 新创建的选项指针。
 */
extern mxml_options_t *mxmlOptionsNew(void);

/**
 * @brief 设置自定义回调函数。
 *
 * @param options 选项指针。
 * @param load_cb 自定义加载回调函数。
 * @param save_cb 自定义保存回调函数。
 * @param cbdata 回调函数的数据指针。
 */
extern void mxmlOptionsSetCustomCallbacks(mxml_options_t *options, mxml_custload_cb_t load_cb, mxml_custsave_cb_t save_cb, void *cbdata);

/**
 * @brief 设置实体回调函数。
 *
 * @param options 选项指针。
 * @param cb 实体回调函数。
 * @param cbdata 回调函数的数据指针。
 */
extern void mxmlOptionsSetEntityCallback(mxml_options_t *options, mxml_entity_cb_t cb, void *cbdata);

/**
 * @brief 设置错误回调函数。
 *
 * @param options 选项指针。
 * @param cb 错误回调函数。
 * @param cbdata 回调函数的数据指针。
 */
extern void mxmlOptionsSetErrorCallback(mxml_options_t *options, mxml_error_cb_t cb, void *cbdata);

/**
 * @brief 设置SAX回调函数。
 *
 * @param options 选项指针。
 * @param cb SAX回调函数。
 * @param cbdata 回调函数的数据指针。
 */
extern void mxmlOptionsSetSAXCallback(mxml_options_t *options, mxml_sax_cb_t cb, void *cbdata);

/**
 * @brief 设置类型回调函数。
 *
 * @param options 选项指针。
 * @param cb 类型回调函数。
 * @param cbdata 回调函数的数据指针。
 */
extern void mxmlOptionsSetTypeCallback(mxml_options_t *options, mxml_type_cb_t cb, void *cbdata);

/**
 * @brief 设置选项的类型值。
 *
 * @param options 选项指针。
 * @param type 类型值。
 */
extern void mxmlOptionsSetTypeValue(mxml_options_t *options, mxml_type_t type);

/**
 * @brief 设置空白回调函数。
 *
 * @param options 选项指针。
 * @param cb 空白回调函数。
 * @param cbdata 回调函数的数据指针。
 */
extern void mxmlOptionsSetWhitespaceCallback(mxml_options_t *options, mxml_ws_cb_t cb, void *cbdata);

/**
 * @brief 设置换行边距。
 *
 * @param options 选项指针。
 * @param column 换行边距列数。
 */
extern void mxmlOptionsSetWrapMargin(mxml_options_t *options, int column);

/*****************************************************/
/**
 * @brief 在父节点下创建一个包含CDATA的节点。
 *
 * @param parent 父节点指针。
 * @param string CDATA字符串。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewCDATA(mxml_node_t *parent, const char *string);

/**
 * @brief 在父节点下创建一个包含格式化CDATA的节点。
 *
 * @param parent 父节点指针。
 * @param format 格式化字符串。
 * @param ... 格式化参数。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewCDATAf(mxml_node_t *parent, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 在父节点下创建一个注释节点。
 *
 * @param parent 父节点指针。
 * @param comment 注释字符串。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewComment(mxml_node_t *parent, const char *comment);

/**
 * @brief 在父节点下创建一个格式化注释节点。
 *
 * @param parent 父节点指针。
 * @param format 格式化字符串。
 * @param ... 格式化参数。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewCommentf(mxml_node_t *parent, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 在父节点下创建一个自定义节点。
 *
 * @param parent 父节点指针。
 * @param data 自定义数据指针。
 * @param free_cb 自定义释放回调函数。
 * @param free_cbdata 释放回调函数的数据指针。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewCustom(mxml_node_t *parent, void *data, mxml_custfree_cb_t free_cb, void *free_cbdata);

/**
 * @brief 在父节点下创建一个声明节点。
 *
 * @param parent 父节点指针。
 * @param declaration 声明字符串。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewDeclaration(mxml_node_t *parent, const char *declaration);

/**
 * @brief 在父节点下创建一个格式化声明节点。
 *
 * @param parent 父节点指针。
 * @param format 格式化字符串。
 * @param ... 格式化参数。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewDeclarationf(mxml_node_t *parent, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 在父节点下创建一个指令节点。
 *
 * @param parent 父节点指针。
 * @param directive 指令字符串。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewDirective(mxml_node_t *parent, const char *directive);

/**
 * @brief 在父节点下创建一个格式化指令节点。
 *
 * @param parent 父节点指针。
 * @param format 格式化字符串。
 * @param ... 格式化参数。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewDirectivef(mxml_node_t *parent, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 在父节点下创建一个元素节点。
 *
 * @param parent 父节点指针。
 * @param name 元素名称。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewElement(mxml_node_t *parent, const char *name);

/**
 * @brief 在父节点下创建一个整数节点。
 *
 * @param parent 父节点指针。
 * @param integer 整数值。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewInteger(mxml_node_t *parent, long integer);

/**
 * @brief 在父节点下创建一个不透明节点。
 *
 * @param parent 父节点指针。
 * @param opaque 不透明字符串。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewOpaque(mxml_node_t *parent, const char *opaque);

/**
 * @brief 在父节点下创建一个格式化不透明节点。
 *
 * @param parent 父节点指针。
 * @param format 格式化字符串。
 * @param ... 格式化参数。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewOpaquef(mxml_node_t *parent, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 在父节点下创建一个实数节点。
 *
 * @param parent 父节点指针。
 * @param real 实数值。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewReal(mxml_node_t *parent, double real);

/**
 * @brief 在父节点下创建一个文本节点。
 *
 * @param parent 父节点指针。
 * @param whitespace 是否保留空白。
 * @param string 文本字符串。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewText(mxml_node_t *parent, bool whitespace, const char *string);

/**
 * @brief 在父节点下创建一个格式化文本节点。
 *
 * @param parent 父节点指针。
 * @param whitespace 是否保留空白。
 * @param format 格式化字符串。
 * @param ... 格式化参数。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewTextf(mxml_node_t *parent, bool whitespace, const char *format, ...) MXML_FORMAT(3,4);

/**
 * @brief 创建一个XML节点。
 *
 * @param version XML版本字符串。
 * @return 创建的节点指针。
 */
extern mxml_node_t *mxmlNewXML(const char *version);

/*****************************************************/
/**
 * @brief 释放节点的内存。
 *
 * @param node 节点指针。
 * @return 成功释放返回0，否则返回-1。
 */
extern int mxmlRelease(mxml_node_t *node);

/**
 * @brief 从父节点中移除节点。
 *
 * @param node 节点指针。
 */
extern void mxmlRemove(mxml_node_t *node);

/**
 * @brief 增加节点的引用计数。
 *
 * @param node 节点指针。
 * @return 增加引用计数后的值。
 */
extern int mxmlRetain(mxml_node_t *node);

/*****************************************************/
/**
 * @brief 将节点保存为动态分配的字符串。
 *
 * @param node 节点指针。
 * @param options 保存选项指针。
 * @return 分配的字符串指针，保存失败返回NULL。
 */
extern char *mxmlSaveAllocString(mxml_node_t *node, mxml_options_t *options);

/**
 * @brief 将节点保存到文件描述符。
 *
 * @param node 节点指针。
 * @param options 保存选项指针。
 * @param fd 文件描述符。
 * @return 保存成功返回true，否则返回false。
 */
extern bool mxmlSaveFd(mxml_node_t *node, mxml_options_t *options, int fd);

/**
 * @brief 将节点保存到文件。
 *
 * @param node 节点指针。
 * @param options 保存选项指针。
 * @param fp 文件指针。
 * @return 保存成功返回true，否则返回false。
 */
extern bool mxmlSaveFile(mxml_node_t *node, mxml_options_t *options, FILE *fp);

/**
 * @brief 将节点保存到指定的文件名。
 *
 * @param node 节点指针。
 * @param options 保存选项指针。
 * @param filename 文件名。
 * @return 保存成功返回true，否则返回false。
 */
extern bool mxmlSaveFilename(mxml_node_t *node, mxml_options_t *options, const char *filename);

/**
 * @brief 将节点通过自定义的I/O回调函数保存。
 *
 * @param node 节点指针。
 * @param options 保存选项指针。
 * @param io_cb 自定义I/O回调函数。
 * @param io_cbdata I/O回调函数的数据指针。
 * @return 保存成功返回true，否则返回false。
 */
extern bool mxmlSaveIO(mxml_node_t *node, mxml_options_t *options, mxml_io_cb_t io_cb, void *io_cbdata);

/**
 * @brief 将节点保存到指定的字符串缓冲区。
 *
 * @param node 节点指针。
 * @param options 保存选项指针。
 * @param buffer 字符串缓冲区。
 * @param bufsize 缓冲区大小。
 * @return 实际保存的字符数，保存失败返回0。
 */
extern size_t mxmlSaveString(mxml_node_t *node, mxml_options_t *options, char *buffer, size_t bufsize);

/*****************************************************/
/**
 * @brief 设置节点的CDATA数据。
 *
 * @param node 节点指针。
 * @param data CDATA数据。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetCDATA(mxml_node_t *node, const char *data);

/**
 * @brief 使用格式化字符串设置节点的CDATA数据。
 *
 * @param node 节点指针。
 * @param format 格式化字符串。
 * @param ... 可变参数列表。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetCDATAf(mxml_node_t *node, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 设置节点的注释。
 *
 * @param node 节点指针。
 * @param comment 注释字符串。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetComment(mxml_node_t *node, const char *comment);

/**
 * @brief 使用格式化字符串设置节点的注释。
 *
 * @param node 节点指针。
 * @param format 格式化字符串。
 * @param ... 可变参数列表。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetCommentf(mxml_node_t *node, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 设置节点的声明。
 *
 * @param node 节点指针。
 * @param declaration 声明字符串。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetDeclaration(mxml_node_t *node, const char *declaration);

/**
 * @brief 使用格式化字符串设置节点的声明。
 *
 * @param node 节点指针。
 * @param format 格式化字符串。
 * @param ... 可变参数列表。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetDeclarationf(mxml_node_t *node, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 设置节点的指令。
 *
 * @param node 节点指针。
 * @param directive 指令字符串。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetDirective(mxml_node_t *node, const char *directive);

/**
 * @brief 使用格式化字符串设置节点的指令。
 *
 * @param node 节点指针。
 * @param format 格式化字符串。
 * @param ... 可变参数列表。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetDirectivef(mxml_node_t *node, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 设置节点的自定义数据。
 *
 * @param node 节点指针。
 * @param data 自定义数据指针。
 * @param free_cb 自定义数据释放回调函数。
 * @param free_cbdata 释放回调函数的数据指针。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetCustom(mxml_node_t *node, void *data, mxml_custfree_cb_t free_cb, void *free_cbdata);

/**
 * @brief 设置节点的元素名称。
 *
 * @param node 节点指针。
 * @param name 元素名称字符串。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetElement(mxml_node_t *node, const char *name);

/**
 * @brief 设置节点的整数值。
 *
 * @param node 节点指针。
 * @param integer 整数值。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetInteger(mxml_node_t *node, long integer);

/**
 * @brief 设置节点的不透明数据。
 *
 * @param node 节点指针。
 * @param opaque 不透明数据字符串。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetOpaque(mxml_node_t *node, const char *opaque);

/**
 * @brief 使用格式化字符串设置节点的不透明数据。
 *
 * @param node 节点指针。
 * @param format 格式化字符串。
 * @param ... 可变参数列表。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetOpaquef(mxml_node_t *node, const char *format, ...) MXML_FORMAT(2,3);

/**
 * @brief 设置字符串相关的回调函数。
 *
 * @param strcopy_cb 字符串复制回调函数。
 * @param strfree_cb 字符串释放回调函数。
 * @param str_cbdata 字符串回调函数的数据指针。
 */
extern void mxmlSetStringCallbacks(mxml_strcopy_cb_t strcopy_cb, mxml_strfree_cb_t strfree_cb, void *str_cbdata);

/**
 * @brief 设置节点的文本内容。
 *
 * @param node 节点指针。
 * @param whitespace 是否保留空白字符。
 * @param string 文本字符串。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetText(mxml_node_t *node, bool whitespace, const char *string);

/**
 * @brief 使用格式化字符串设置节点的文本内容。
 *
 * @param node 节点指针。
 * @param whitespace 是否保留空白字符。
 * @param format 格式化字符串。
 * @param ... 可变参数列表。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetTextf(mxml_node_t *node, bool whitespace, const char *format, ...) MXML_FORMAT(3,4);

/**
 * @brief 设置节点的用户自定义数据。
 *
 * @param node 节点指针。
 * @param data 用户自定义数据指针。
 * @return 设置成功返回true，否则返回false。
 */
extern bool mxmlSetUserData(mxml_node_t *node, void *data);

/*****************************************************/
/**
 * @brief 获取节点在遍历中的下一个节点。
 *
 * @param node 当前节点指针。
 * @param top 顶层节点指针。
 * @param descend 遍历方向。
 * @return 下一个节点指针，如果没有下一个节点则返回NULL。
 */
extern mxml_node_t *mxmlWalkNext(mxml_node_t *node, mxml_node_t *top, mxml_descend_t descend);

/**
 * @brief 获取节点在遍历中的上一个节点。
 *
 * @param node 当前节点指针。
 * @param top 顶层节点指针。
 * @param descend 遍历方向。
 * @return 上一个节点指针，如果没有上一个节点则返回NULL。
 */
extern mxml_node_t *mxmlWalkPrev(mxml_node_t *node, mxml_node_t *top, mxml_descend_t descend);



#  ifdef __cplusplus
}
#  endif // __cplusplus
#endif // !MXML_H
