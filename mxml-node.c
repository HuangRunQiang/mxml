//
// Mini-XML的节点支持代码，一个小型的XML文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2003-2024 Michael R. Sweet。
//
// 根据Apache License v2.0许可证。有关更多信息，请参见“LICENSE”文件。
//

#include "mxml-private.h"


//
// 本地函数...
//

static void mxml_free(mxml_node_t *node);
static mxml_node_t *mxml_new(mxml_node_t *parent, mxml_type_t type);


//
// 'mxmlAdd（）' - 将节点添加到树中。
//
// 此函数将指定的节点“node”添加到父节点中。如果“child”参数不为NULL，则根据“add”参数的值，将新节点添加到指定的子节点之前或之后。
// 如果“child”参数为NULL，则将新节点放置在子节点列表的开头（MXML_ADD_BEFORE）或末尾（MXML_ADD_AFTER）。
//

void mxmlAdd(mxml_node_t *parent,		// I - 父节点
             mxml_add_t add,		// I - 添加位置，MXML_ADD_BEFORE或MXML_ADD_AFTER
             mxml_node_t *child,		// I - 用于添加的子节点或MXML_ADD_TO_PARENT
             mxml_node_t *node)		// I - 要添加的节点
{
    MXML_DEBUG("mxmlAdd(parent=%p, add=%d, child=%p, node=%p)\n", parent, add, child, node);

    // 范围检查输入...
    if (!parent || !node)
        return;

    // 从任何现有父节点中删除节点...
    if (node->parent)
        mxmlRemove(node);

    // 重置指针...
    node->parent = parent;

    switch (add)
    {
    case MXML_ADD_BEFORE:
        if (!child || child == parent->child || child->parent != parent)
        {
            // 插入为父节点下的第一个节点...
            node->next = parent->child;

            if (parent->child)
                parent->child->prev = node;
            else
                parent->last_child = node;

            parent->child = node;
        }
        else
        {
            // 在此子节点之前插入节点...
            node->next = child;
            node->prev = child->prev;

            if (child->prev)
                child->prev->next = node;
            else
                parent->child = node;

            child->prev = node;
        }
        break;

    case MXML_ADD_AFTER:
        if (!child || child == parent->last_child || child->parent != parent)
        {
            // 插入为父节点下的最后一个节点...
            node->parent = parent;
            node->prev = parent->last_child;

            if (parent->last_child)
                parent->last_child->next = node;
            else
                parent->child = node;

            parent->last_child = node;
        }
        else
        {
            // 在此子节点之后插入节点...
            node->prev = child;
            node->next = child->next;

            if (child->next)
                child->next->prev = node;
            else
                parent->last_child = node;

            child->next = node;
        }
        break;
    }
}


//
// 'mxmlDelete（）' - 删除节点及其所有子节点。
//
// 此函数删除节点“node”及其所有子节点。如果指定的节点有父节点，则此函数首先使用@link mxmlRemove@函数从其父节点中删除节点。
//

void mxmlDelete(mxml_node_t *node)		// I - 要删除的节点
{
    mxml_node_t *current,		// 当前节点
        *next;			// 下一个节点


    MXML_DEBUG("mxmlDelete(node=%p)\n", node);

    // 范围检查输入...
    if (!node)
        return;

    // 从其父节点中删除节点，如果有的话...
    mxmlRemove(node);

    // 删除子节点...
    for (current = node->child; current; current = next)
    {
        // 获取下一个节点...
        if ((next = current->child) != NULL)
        {
            // 在释放子节点后释放父节点...
            current->child = NULL;
            continue;
        }

        if ((next = current->next) == NULL)
        {
            // 下一个节点是父节点，根据需要释放...
            if ((next = current->parent) == node)
                next = NULL;
        }

        // 释放子节点...
        mxml_free(current);
    }

    // 然后释放父节点使用的内存...
    mxml_free(node);
}


//
// 'mxmlGetRefCount（）' - 获取节点的当前引用（使用）计数。
//
// 新节点的初始引用计数为1。使用@link mxmlRetain@和@link mxmlRelease@函数来增加和减少节点的引用计数。
//

size_t mxmlGetRefCount(mxml_node_t *node)	// I - 节点
{
    // 范围检查输入...
    if (!node)
        return (0);

    // 返回引用计数...
    return (node->ref_count);
}


//
// 'mxmlNewCDATA（）' - 创建一个新的CDATA节点。
//
// 将新的CDATA节点添加到指定父节点的子节点列表的末尾。常量`MXML_NO_PARENT`可用于指定新的CDATA节点没有父节点。
// 数据字符串必须以空字符结尾，并被复制到新节点中。CDATA节点当前使用`MXML_TYPE_ELEMENT`类型。
//

mxml_node_t * mxmlNewCDATA(mxml_node_t *parent,	// I - 父节点或`MXML_NO_PARENT`
                           const char *data)		// I - 数据字符串
{
    mxml_node_t *node;		// 新节点


    MXML_DEBUG("mxmlNewCDATA(parent=%p, data=\"%s\")\n", parent, data ? data : "(null)");

    // 范围检查输入...
    if (!data)
        return (NULL);

    // 创建节点并设置名称值...
    if ((node = mxml_new(parent, MXML_TYPE_CDATA)) != NULL)
    {
        if ((node->value.cdata = _mxml_strcopy(data)) == NULL)
        {
            mxmlDelete(node);
            return (NULL);
        }
    }

    return (node);
}


//
// 'mxmlNewCDATAf（）' - 创建一个新的格式化CDATA节点。
//
// 将新的CDATA节点添加到指定父节点的子节点列表的末尾。常量`MXML_NO_PARENT`可用于指定新的不透明字符串节点没有父节点。
// 格式字符串必须以空字符结尾，并被格式化到新节点中。
//

mxml_node_t * mxmlNewCDATAf(mxml_node_t *parent,	// I - 父节点或`MXML_NO_PARENT`
                            const char *format,	// I - Printf风格的格式字符串
                            ...)			// I - 需要的其他参数
{
    mxml_node_t *node;		// 新节点
    va_list ap;			// 指向参数的指针
    char buffer[16384];		// 格式缓冲区


    MXML_DEBUG("mxmlNewCDATAf(parent=%p, format=\"%s\", ...)\n", parent, format ? format : "(null)");

    // 范围检查输入...
    if (!format)
        return (NULL);

    // 创建节点并设置文本值...
    if ((node = mxml_new(parent, MXML_TYPE_CDATA)) != NULL)
    {
        va_start(ap, format);
        vsnprintf(buffer, sizeof(buffer), format, ap);
        va_end(ap);

        node->value.cdata = _mxml_strcopy(buffer);
    }

    return (node);
}


//
// 'mxmlNewComment（）' - 创建一个新的注释节点。
//
// 将新注释节点添加到指定父节点的子节点列表的末尾。常量`MXML_NO_PARENT`可用于指定新的注释节点没有父节点。
// 注释字符串必须以空字符结尾，并被复制到新节点中。
//

mxml_node_t * mxmlNewComment(mxml_node_t *parent,	// I - 父节点或`MXML_NO_PARENT`
                             const char *comment)	// I - 注释字符串
{
    mxml_node_t *node;		// 新节点


    MXML_DEBUG("mxmlNewComment(parent=%p, comment=\"%s\")\n", parent, comment ? comment : "(null)");

    // 范围检查输入...
    if (!comment)
        return (NULL);

    // 创建节点并设置名称值...
    if ((node = mxml_new(parent, MXML_TYPE_COMMENT)) != NULL)
    {
        if ((node->value.comment = _mxml_strcopy(comment)) == NULL)
        {
            mxmlDelete(node);
            return (NULL);
        }
    }

    return (node);
}


//
// 'mxmlNewCommentf（）' - 创建一个新的格式化注释字符串节点。
//
// 将新的注释字符串节点添加到指定父节点的子节点列表的末尾。常量`MXML_NO_PARENT`可用于指定新的不透明字符串节点没有父节点。
// 格式字符串必须以空字符结尾，并被格式化到新节点中。
//

mxml_node_t * mxmlNewCommentf(mxml_node_t *parent,	// I - 父节点或`MXML_NO_PARENT`
                              const char *format,	// I - Printf风格的格式字符串
                              ...)			// I - 需要的其他参数
{
    mxml_node_t *node;		// 新节点
    va_list ap;			// 指向参数的指针
    char buffer[16384];		// 格式缓冲区


    MXML_DEBUG("mxmlNewCommentf(parent=%p, format=\"%s\", ...)\n", parent, format ? format : "(null)");

    // 范围检查输入...
    if (!format)
        return (NULL);

    // 创建节点并设置文本值...
    if ((node = mxml_new(parent, MXML_TYPE_COMMENT)) != NULL)
    {
        va_start(ap, format);
        vsnprintf(buffer, sizeof(buffer), format, ap);
        va_end(ap);

        node->value.comment = _mxml_strcopy(buffer);
    }

    return (node);
}


//
// 'mxmlNewCustom（）' - 创建一个新的自定义数据节点。
//
// 将新的自定义节点添加到指定父节点的子节点列表的末尾。`free_cb`参数指定在删除节点时调用的函数以释放自定义数据。
//

mxml_node_t * mxmlNewCustom(
    mxml_node_t *parent,		// I - 父节点或`MXML_NO_PARENT`
    void *data,			// I - 指向数据的指针
    mxml_custfree_cb_t free_cb,	// I - 释放回调函数或`NULL`（如果不需要）
    void *free_cbdata)		// I - 释放回调数据
{
    mxml_node_t *node;		// 新节点


    MXML_DEBUG("mxmlNewCustom(parent=%p, data=%p, free_cb=%p, free_cbdata=%p)\n", parent, data, free_cb, free_cbdata);

    // 创建节点并设置值...
    if ((node = mxml_new(parent, MXML_TYPE_CUSTOM)) != NULL)
    {
        node->value.custom.data = data;
        node->value.custom.free_cb = free_cb;
        node->value.custom.free_cbdata = free_cbdata;
    }

    return (node);
}


//
// 'mxmlNewDeclaration（）' - 创建一个新的声明节点。
//
// 将新的声明节点添加到指定父节点的子节点列表的末尾。常量`MXML_NO_PARENT`可用于指定新的声明节点没有父节点。
// 声明字符串必须以空字符结尾，并被复制到新节点中。
//

mxml_node_t * mxmlNewDeclaration(
    mxml_node_t *parent,		// I - 父节点或`MXML_NO_PARENT`
    const char *declaration)	// I - 声明字符串
{
    mxml_node_t *node;		// 新节点


    MXML_DEBUG("mxmlNewDeclaration(parent=%p, declaration=\"%s\")\n", parent, declaration ? declaration : "(null)");

    // 范围检查输入...
    if (!declaration)
        return (NULL);

    // 创建节点并设置名称值...
    if ((node = mxml_new(parent, MXML_TYPE_DECLARATION)) != NULL)
    {
        if ((node->value.declaration = _mxml_strcopy(declaration)) == NULL)
        {
            mxmlDelete(node);
            return (NULL);
        }
    }

    return (node);
}


//
// 'mxmlNewDeclarationf()' - 创建一个新的格式化声明节点。
//
// 新的声明节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的不透明字符串节点没有父节点。
// 格式化字符串必须以空字符结尾，并被格式化到新节点中。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewDeclarationf(
mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
const char *format, // 输入 - Printf 样式的格式化字符串
...) // 输入 - 需要的其他参数
{
mxml_node_t *node; // 新节点
va_list ap; // 参数指针
char buffer[16384]; // 格式化缓冲区

MXML_DEBUG("mxmlNewDeclarationf(parent=%p, format="%s", ...)\n", parent, format ? format : "(null)");

// 输入范围检查...
if (!format)
return (NULL);

// 创建节点并设置文本值...
if ((node = mxml_new(parent, MXML_TYPE_DECLARATION)) != NULL)
{
va_start(ap, format);
vsnprintf(buffer, sizeof(buffer), format, ap);
va_end(ap);

node->value.declaration = _mxml_strcopy(buffer);

}

return (node);
}

//
// 'mxmlNewDirective()' - 创建一个新的处理指令节点。
//
// 新的处理指令节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的处理指令节点没有父节点。
// 数据字符串必须以空字符结尾，并被复制到新节点中。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewDirective(mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
const char *directive)// 输入 - 指令字符串
{
mxml_node_t *node; // 新节点

MXML_DEBUG("mxmlNewDirective(parent=%p, directive="%s")\n", parent, directive ? directive : "(null)");

// 输入范围检查...
if (!directive)
return (NULL);

// 创建节点并设置名称值...
if ((node = mxml_new(parent, MXML_TYPE_DIRECTIVE)) != NULL)
{
if ((node->value.directive = _mxml_strcopy(directive)) == NULL)
{
mxmlDelete(node);
return (NULL);
}
}

return (node);
}

//
// 'mxmlNewDirectivef()' - 创建一个新的格式化处理指令节点。
//
// 新的处理指令节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的不透明字符串节点没有父节点。
// 格式化字符串必须以空字符结尾，并被格式化到新节点中。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewDirectivef(mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
const char *format, // 输入 - Printf 样式的格式化字符串
...) // 输入 - 需要的其他参数
{
mxml_node_t *node; // 新节点
va_list ap; // 参数指针
char buffer[16384]; // 格式化缓冲区

MXML_DEBUG("mxmlNewDirectivef(parent=%p, format="%s", ...)\n", parent, format ? format : "(null)");

// 输入范围检查...
if (!format)
return (NULL);

// 创建节点并设置文本值...
if ((node = mxml_new(parent, MXML_TYPE_DIRECTIVE)) != NULL)
{
va_start(ap, format);
vsnprintf(buffer, sizeof(buffer), format, ap);
va_end(ap);

node->value.directive = _mxml_strcopy(buffer);

}

return (node);
}

//
// 'mxmlNewElement()' - 创建一个新的元素节点。
//
// 新的元素节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的元素节点没有父节点。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewElement(mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
const char *name) // 输入 - 元素的名称
{
mxml_node_t *node; // 新节点

MXML_DEBUG("mxmlNewElement(parent=%p, name="%s")\n", parent, name ? name : "(null)");

// 输入范围检查...
if (!name)
return (NULL);

// 创建节点并设置元素名称...
if ((node = mxml_new(parent, MXML_TYPE_ELEMENT)) != NULL)
node->value.element.name = _mxml_strcopy(name);

return (node);
}

//
// 'mxmlNewInteger()' - 创建一个新的整数节点。
//
// 新的整数节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的整数节点没有父节点。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewInteger(mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
long integer) // 输入 - 整数值
{
mxml_node_t *node; // 新节点

MXML_DEBUG("mxmlNewInteger(parent=%p, integer=%ld)\n", parent, integer);

// 创建节点并设置元素名称...
if ((node = mxml_new(parent, MXML_TYPE_INTEGER)) != NULL)
node->value.integer = integer;

return (node);
}

//
// 'mxmlNewOpaque()' - 创建一个新的不透明字符串。
//
// 新的不透明字符串节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的不透明字符串节点没有父节点。
// 不透明字符串必须以空字符结尾，并被复制到新节点中。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewOpaque(mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
const char *opaque) // 输入 - 不透明字符串
{
mxml_node_t *node; // 新节点

MXML_DEBUG("mxmlNewOpaque(parent=%p, opaque="%s")\n", parent, opaque ? opaque : "(null)");

// 输入范围检查...
if (!opaque)
return (NULL);

// 创建节点并设置元素名称...
if ((node = mxml_new(parent, MXML_TYPE_OPAQUE)) != NULL)
node->value.opaque = _mxml_strcopy(opaque);

return (node);
}

//
// 'mxmlNewOpaquef()' - 创建一个新的格式化不透明字符串节点。
//
// 新的不透明字符串节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的不透明字符串节点没有父节点。
// 格式化字符串必须以空字符结尾，并被格式化到新节点中。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewOpaquef(mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
const char *format, // 输入 - Printf 样式的格式化字符串
...) // 输入 - 需要的其他参数
{
mxml_node_t *node; // 新节点
va_list ap; // 参数指针
char buffer[16384]; // 格式化缓冲区

MXML_DEBUG("mxmlNewOpaquef(parent=%p, format="%s", ...)\n", parent, format ? format : "(null)");

// 输入范围检查...
if (!format)
return (NULL);

// 创建节点并设置文本值...
if ((node = mxml_new(parent, MXML_TYPE_OPAQUE)) != NULL)
{
va_start(ap, format);
vsnprintf(buffer, sizeof(buffer), format, ap);
va_end(ap);

node->value.opaque = _mxml_strcopy(buffer);

}

return (node);
}

//
// 'mxmlNewReal()' - 创建一个新的实数节点。
//
// 新的实数节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的实数节点没有父节点。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewReal(mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
double real) // 输入 - 实数值
{
mxml_node_t *node; // 新节点

MXML_DEBUG("mxmlNewReal(parent=%p, real=%g)\n", parent, real);

// 创建节点并设置元素名称...
if ((node = mxml_new(parent, MXML_TYPE_REAL)) != NULL)
node->value.real = real;

return (node);
}

//
// 'mxmlNewText()' - 创建一个新的文本片段节点。
//
// 新的文本节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的文本节点没有父节点。
// 空格参数用于指定节点之前是否存在前导空格。
// 文本字符串必须以空字符结尾，并被复制到新节点中。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewText(mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
bool whitespace, // 输入 - true = 前导空格，false = 没有空格
const char *string) // 输入 - 字符串
{
mxml_node_t *node; // 新节点

MXML_DEBUG("mxmlNewText(parent=%p, whitespace=%s, string="%s")\n", parent, whitespace ? "true" : "false", string ? string : "(null)");

// 输入范围检查...
if (!string)
return (NULL);

// 创建节点并设置文本值...
if ((node = mxml_new(parent, MXML_TYPE_TEXT)) != NULL)
{
node->value.text.whitespace = whitespace;
node->value.text.string = _mxml_strcopy(string);
}

return (node);
}

//
// 'mxmlNewTextf()' - 创建一个新的格式化文本片段节点。
//
// 新的文本节点将被添加到指定父节点的子节点列表的末尾。
// 可以使用常量 MXML_NO_PARENT 来指定新的文本节点没有父节点。
// 空格参数用于指定节点之前是否存在前导空格。
// 格式化字符串必须以空字符结尾，并被格式化到新节点中。
//

mxml_node_t * // 返回值 - 新节点
mxmlNewTextf(mxml_node_t *parent, // 输入 - 父节点或 MXML_NO_PARENT
bool whitespace, // 输入 - true = 前导空格，false = 没有空格
const char *format, // 输入 - Printf 样式的格式化字符串
...) // 输入 - 需要的其他参数
{
mxml_node_t *node; // 新节点
va_list ap; // 参数指针
char buffer[16384]; // 格式化缓冲区

MXML_DEBUG("mxmlNewTextf(parent=%p, whitespace=%s, format="%s", ...)\n", parent, whitespace ? "true" : "false", format ? format : "(null)");

// 输入范围检查...
if (!format)
return (NULL);

// 创建节点并设置文本值...
if ((node = mxml_new(parent, MXML_TYPE_TEXT)) != NULL)
{
va_start(ap, format);
vsnprintf(buffer, sizeof(buffer), format, ap);
va_end(ap);

node->value.text.whitespace = whitespace;
node->value.text.string     = _mxml_strcopy(buffer);

}

return (node);
}

//
// 'mxmlRemove()' - 从父节点中移除一个节点。
//
// 此函数不释放节点使用的内存 - 请使用 @link mxmlDelete@ 来释放。
// 如果节点没有父节点，则此函数不执行任何操作。
//

void
mxmlRemove(mxml_node_t *node) // 输入 - 要移除的节点
{
MXML_DEBUG("mxmlRemove(node=%p)\n", node);

// 输入范围检查...
if (!node || !node->parent)
return;

// 从父节点中移除...
if (node->prev)
node->prev->next = node->next;
else
node->parent->child = node->next;

if (node->next)
node->next->prev = node->prev;
else
node->parent->last_child = node->prev;

node->parent = NULL;
node->prev = NULL;
node->next = NULL;
}

//
// 'mxmlNewXML()' - 创建一个新的 XML 文档树。
//
// "version" 参数指定要放入 ?xml 指令节点中的版本号。如果为 NULL，则默认为 "1.0"。
//

mxml_node_t * // 返回值 - 新的 ?xml 节点
mxmlNewXML(const char *version) // 输入 - 要使用的版本号
{
char directive[1024]; // 指令文本

snprintf(directive, sizeof(directive), "xml version="%s" encoding="utf-8"", version ? version : "1.0");

return (mxmlNewDirective(NULL, directive));
}

//
// 'mxmlRelease()' - 释放一个节点。
//
// 当引用计数达到零时，节点（及其任何子节点）将通过 @link mxmlDelete@ 删除。
//

int // 返回值 - 新的引用计数
mxmlRelease(mxml_node_t *node) // 输入 - 节点
{
if (node)
{
if ((-- node->ref_count) <= 0)
{
mxmlDelete(node);
return (0);
}
else
{
return (node->ref_count);
}
}
else
{
return (-1);
}
}


//
// 'mxmlRetain()' - 保留一个节点。
//

int					// 返回值 - 新的引用计数
mxmlRetain(mxml_node_t *node)		// 输入参数 - 节点
{
  if (node)
    return (++ node->ref_count);
  else
    return (-1);
}


//
// 'mxml_free()' - 释放节点使用的内存。
//
// 注意：不释放子节点，不从父节点中移除。
//

static void
mxml_free(mxml_node_t *node)		// 输入参数 - 节点
{
  size_t	i;			// 循环变量


  switch (node->type)
  {
    case MXML_TYPE_CDATA :
	_mxml_strfree(node->value.cdata);
        break;
    case MXML_TYPE_COMMENT :
	_mxml_strfree(node->value.comment);
        break;
    case MXML_TYPE_DECLARATION :
	_mxml_strfree(node->value.declaration);
        break;
    case MXML_TYPE_DIRECTIVE :
	_mxml_strfree(node->value.directive);
        break;
    case MXML_TYPE_ELEMENT :
	_mxml_strfree(node->value.element.name);

	if (node->value.element.num_attrs)
	{
	  for (i = 0; i < node->value.element.num_attrs; i ++)
	  {
	    _mxml_strfree(node->value.element.attrs[i].name);
	    _mxml_strfree(node->value.element.attrs[i].value);
	  }

          free(node->value.element.attrs);
	}
        break;
    case MXML_TYPE_INTEGER :
       // 无需处理
        break;
    case MXML_TYPE_OPAQUE :
	_mxml_strfree(node->value.opaque);
        break;
    case MXML_TYPE_REAL :
       // 无需处理
        break;
    case MXML_TYPE_TEXT :
	_mxml_strfree(node->value.text.string);
        break;
    case MXML_TYPE_CUSTOM :
        if (node->value.custom.data && node->value.custom.free_cb)
	  (node->value.custom.free_cb)(node->value.custom.free_cbdata, node->value.custom.data);
	break;
    default :
        break;
  }

  // 释放该节点...
  free(node);
}


//
// 'mxml_new()' - 创建一个新节点。
//

static mxml_node_t *			// 返回值 - 新节点
mxml_new(mxml_node_t *parent,		// 输入参数 - 父节点
         mxml_type_t type)		// 输入参数 - 节点类型
{
  mxml_node_t	*node;			// 新节点


  MXML_DEBUG("mxml_new(parent=%p, type=%d)\n", parent, type);

  // 为节点分配内存...
  if ((node = calloc(1, sizeof(mxml_node_t))) == NULL)
  {
    MXML_DEBUG("mxml_new: 返回 NULL\n");
    return (NULL);
  }

  MXML_DEBUG("mxml_new: 返回 %p\n", node);

  // 设置节点类型...
  node->type      = type;
  node->ref_count = 1;

  // 如果存在父节点，则添加到父节点中...
  if (parent)
    mxmlAdd(parent, MXML_ADD_AFTER, /*child*/NULL, node);

  // 返回新节点...
  return (node);
}