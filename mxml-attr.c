// Mini-XML的属性支持代码，一个小型的XML文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2003-2024 Michael R Sweet。
//
// 根据Apache许可证v2.0进行许可。更多信息请参见文件“LICENSE”。
//

#include "mxml-private.h"


//
// 本地函数...
//

static bool mxml_set_attr(mxml_node_t *node, const char *name, char *value);


//
// 'mxmlElementClearAttr（）' - 从元素中删除属性。
//
// 此函数从元素`node`中删除属性`name`。

void mxmlElementClearAttr(mxml_node_t *node, const char *name)
{
    size_t i; // 循环变量
    _mxml_attr_t *attr; // 当前属性

    MXML_DEBUG("mxmlElementClearAttr（node = %p，name = \"%s\"）\n", node, name ? name : "(null)");

    // 范围检查输入...
    if (!node || node->type != MXML_TYPE_ELEMENT || !name)
        return;

    // 查找属性...
    for (i = node->value.element.num_attrs, attr = node->value.element.attrs; i > 0; i--, attr++)
    {
        MXML_DEBUG("mxmlElementClearAttr：%s = \"%s\" \n", attr->name, attr->value);

        if (!strcmp(attr->name, name))
        {
            // 删除此属性...
            _mxml_strfree(attr->name);
            _mxml_strfree(attr->value);

            i--;
            if (i > 0)
                memmove(attr, attr + 1, i * sizeof(_mxml_attr_t));

            node->value.element.num_attrs--;

            if (node->value.element.num_attrs == 0)
                free(node->value.element.attrs);
            return;
        }
    }
}


//
// 'mxmlElementGetAttr（）' - 获取属性的值。
//
// 此函数从元素`node`中获取属性`name`的值。如果节点不是元素或指定的属性不存在，则返回`NULL`。

const char * mxmlElementGetAttr(mxml_node_t *node, const char *name)
{
    size_t i; // 循环变量
    _mxml_attr_t *attr; // 当前属性

    MXML_DEBUG("mxmlElementGetAttr（node = %p，name = \"%s\"）\n", node, name ? name : "(null)");

    // 范围检查输入...
    if (!node || node->type != MXML_TYPE_ELEMENT || !name)
        return NULL;

    // 查找属性...
    for (i = node->value.element.num_attrs, attr = node->value.element.attrs; i > 0; i--, attr++)
    {
        MXML_DEBUG("mxmlElementGetAttr：%s = \"%s\" \n", attr->name, attr->value);

        if (!strcmp(attr->name, name))
        {
            MXML_DEBUG("mxmlElementGetAttr：返回\"%s\"。 \n", attr->value);
            return attr->value;
        }
    }

    // 未找到属性，因此返回NULL...
    MXML_DEBUG("mxmlElementGetAttr：返回NULL。 \n");

    return NULL;
}


//
// 'mxmlElementGetAttrByIndex（）' - 按索引获取属性。
//
// 此函数返回元素`node`的第N个（`idx`）属性。属性名称可以选择在`name`参数中返回。如果节点不是元素或指定的索引超出范围，则返回NULL。

const char * // O - 属性值
mxmlElementGetAttrByIndex(
    mxml_node_t *node, // I - 节点
    size_t idx, // I - 属性索引，从`0`开始
    const char **name) // O - 属性名称或`NULL`以不返回它
{
    if (!node || node->type != MXML_TYPE_ELEMENT || idx >= node->value.element.num_attrs)
        return NULL;

    if (name)
        *name = node->value.element.attrs[idx].name;

    return node->value.element.attrs[idx].value;
}


//
// 'mxmlElementGetAttrCount（）' - 获取元素属性的数量。
//
// 此函数返回元素`node`的属性数量。如果节点不是元素或元素没有属性，则返回0。

size_t // O - 属性数量
mxmlElementGetAttrCount(
    mxml_node_t *node) // I - 节点
{
    if (node && node->type == MXML_TYPE_ELEMENT)
        return node->value.element.num_attrs;
    else
        return 0;
}


//
// 'mxmlElementSetAttr（）' - 为元素设置属性。
//
// 此函数为元素`node`设置属性`name`为字符串`value`。如果已存在具有指定名称的属性，则属性的值将被新的字符串值替换。字符串值将被复制。

void mxmlElementSetAttr(mxml_node_t *node, const char *name, const char *value)
{
    char *valuec; // value的副本

    MXML_DEBUG("mxmlElementSetAttr（node = %p，name = \"%s\"，value = \"%s\"）\n", node, name ? name : "(null)", value ? value : "(null)");

    // 范围检查输入...
    if (!node || node->type != MXML_TYPE_ELEMENT || !name)
        return;

    if (value)
    {
        if ((valuec = _mxml_strcopy(value)) == NULL)
            return;
    }
    else
    {
        valuec = NULL;
    }

    if (!mxml_set_attr(node, name, valuec))
        _mxml_strfree(valuec);
}


//
// 'mxmlElementSetAttrf（）' - 使用格式化的值设置属性。
//
// 此函数将属性`name`设置为元素`node`的`format`的格式化值。如果已存在具有指定名称的属性，则属性的值将被新的格式化字符串值替换。

void mxmlElementSetAttrf(mxml_node_t *node, const char *name, const char *format, ...) // I - 格式化的属性值，如有需要，还有其他参数
{
    va_list ap; // 参数指针
    char buffer[16384]; // 格式化缓冲区
    char *value; // 值

    MXML_DEBUG("mxmlElementSetAttrf（node = %p，name = \"%s\"，format = \"%s\"，...）\n", node, name ? name : "(null)", format ? format : "(null)");

    // 范围检查输入...
    if (!node || node->type != MXML_TYPE_ELEMENT || !name || !format)
        return;

    // 格式化值...
    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    if ((value = _mxml_strcopy(buffer)) != NULL)
    {
        if (!mxml_set_attr(node, name, value))
            _mxml_strfree(value);
    }
}


//
// 'mxml_set_attr（）' - 设置或添加属性名/值对。
//

static bool // O - 成功时为`true`，失败时为`false`
mxml_set_attr(mxml_node_t *node, const char *name, char *value) // I - 元素节点，属性名称，属性值
{
    int i; // 循环变量
    _mxml_attr_t *attr; // 新属性

    // 查找属性...
    for (i = node->value.element.num_attrs, attr = node->value.element.attrs; i > 0; i--, attr++)
    {
        if (!strcmp(attr->name, name))
        {
            // 根据需要释放旧值...
            _mxml_strfree(attr->value);
            attr->value = value;

            return true;
        }
    }

    // 添加新属性...
    if ((attr = realloc(node->value.element.attrs, (node->value.element.num_attrs + 1) * sizeof(_mxml_attr_t))) == NULL)
        return false;

    node->value.element.attrs = attr;
    attr += node->value.element.num_attrs;

    if ((attr->name = _mxml_strcopy(name)) == NULL)
        return false;

    attr->value = value;

    node->value.element.num_attrs++;

    return true;
}
