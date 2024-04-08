//
// Mini-XML的索引支持代码，一个小型的XML文件解析库。
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

static int index_compare(mxml_index_t *ind, mxml_node_t *first, mxml_node_t *second);
static int index_find(mxml_index_t *ind, const char *element, const char *value, mxml_node_t *node);
static void index_sort(mxml_index_t *ind, int left, int right);


//
// 'mxmlIndexDelete（）' - 删除索引。
//

void mxmlIndexDelete(mxml_index_t *ind)	// I - 要删除的索引
{
    // 范围检查输入..
    if (!ind)
        return;

    // 释放内存...
    _mxml_strfree(ind->attr);
    free(ind->nodes);
    free(ind);
}


//
// 'mxmlIndexEnum（）' - 返回索引中的下一个节点。
//
// 此函数返回索引“ind”中的下一个节点。
//
// 在使用此函数之前，应先调用@link mxmlIndexReset@以获取索引中的第一个节点。
// 节点按索引的排序顺序返回。
//

mxml_node_t * mxmlIndexEnum(mxml_index_t *ind)	// I - 要枚举的索引
{
    // 范围检查输入...
    if (!ind)
        return (NULL);

    // 返回下一个节点...
    if (ind->cur_node < ind->num_nodes)
        return (ind->nodes[ind->cur_node++]);
    else
        return (NULL);
}


//
// 'mxmlIndexFind（）' - 查找下一个匹配的节点。
//
// 此函数在索引“ind”中查找下一个匹配的节点。
//
// 在首次使用此函数时，应先调用@link mxmlIndexReset@，并使用特定的“element”和“value”字符串。
// 如果“element”和“value”都为NULL，则等效于调用@link mxmlIndexEnum@。
//

mxml_node_t * mxmlIndexFind(mxml_index_t *ind,	// I - 要搜索的索引
                            const char *element,	// I - 要查找的元素名称，如果没有则为NULL
                            const char *value)	// I - 属性值，如果没有则为NULL
{
    int diff,		// 名称之间的差异
        current,	// 搜索中的当前实体
        first,		// 搜索中的第一个实体
        last;		// 搜索中的最后一个实体


    MXML_DEBUG("mxmlIndexFind（ind =％p，element = \“％s \”，value = \“％s \”）\n", ind, element ? element : "（null）", value ? value : "（null）");

    // 范围检查输入...
    if (!ind || (!ind->attr && value))
    {
        MXML_DEBUG("mxmlIndexFind：返回NULL，ind->attr = \“％s \”...\n", ind && ind->attr ? ind->attr : "（null）");
        return (NULL);
    }

    // 如果element和value都为NULL，则只枚举索引中的节点...
    if (!element && !value)
        return (mxmlIndexEnum(ind));

    // 如果索引中没有节点，则返回NULL...
    if (!ind->num_nodes)
    {
        MXML_DEBUG("mxmlIndexFind：返回NULL，无节点...\n");
        return (NULL);
    }

    // 如果cur_node == 0，则查找第一个匹配的节点...
    if (ind->cur_node == 0)
    {
        // 使用修改过的二分搜索算法找到第一个节点...
        first = 0;
        last = ind->num_nodes - 1;

        MXML_DEBUG("mxmlIndexFind：第一次查找，num_nodes =％lu...\n", (unsigned long)ind->num_nodes);

        while ((last - first) > 1)
        {
            current = (first + last) / 2;

            MXML_DEBUG("mxmlIndexFind：first =％d，last =％d，current =％d\n", first, last, current);

            if ((diff = index_find(ind, element, value, ind->nodes[current])) == 0)
            {
                // 找到匹配项，返回到找到第一个的位置...
                MXML_DEBUG("mxmlIndexFind：匹配。\n");

                while (current > 0 && !index_find(ind, element, value, ind->nodes[current - 1]))
                    current--;

                MXML_DEBUG("mxmlIndexFind：返回第一个匹配项=％d\n", current);

                // 返回第一个匹配项并保存到下一个的索引...
                ind->cur_node = current + 1;

                return (ind->nodes[current]);
            }
            else if (diff < 0)
            {
                last = current;
            }
            else
            {
                first = current;
            }

            MXML_DEBUG("mxmlIndexFind：diff =％d\n", diff);
        }

        // 如果走到这一步，则找到了0个或1个匹配项...
        for (current = first; current <= last; current++)
        {
            if (!index_find(ind, element, value, ind->nodes[current]))
            {
                // 找到了一个（或可能两个）匹配项...
                MXML_DEBUG("mxmlIndexFind：返回唯一匹配项％d...\n", current);
                ind->cur_node = current + 1;

                return (ind->nodes[current]);
            }
        }

        // 没有匹配项...
        ind->cur_node = ind->num_nodes;
        MXML_DEBUG("mxmlIndexFind：返回NULL...\n");
        return (NULL);
    }
    else if (ind->cur_node < ind->num_nodes && !index_find(ind, element, value, ind->nodes[ind->cur_node]))
    {
        // 返回下一个匹配的节点...
        MXML_DEBUG("mxmlIndexFind：返回下一个匹配项％lu...\n", (unsigned long)ind->cur_node);
        return (ind->nodes[ind->cur_node++]);
    }

    // 如果走到这一步，那么我们没有匹配项...
    ind->cur_node = ind->num_nodes;

    MXML_DEBUG("mxmlIndexFind：返回NULL...\n");
    return (NULL);
}


//
// 'mxmlIndexGetCount（）' - 获取索引中的节点数。
//

size_t mxmlIndexGetCount(mxml_index_t *ind)	// I - 节点索引
{
    // 范围检查输入...
    if (!ind)
        return (0);

    // 返回索引中的节点数...
    return (ind->num_nodes);
}


//
// 'mxmlIndexNew（）' - 创建一个新索引。
//
// 此函数为XML树“node”创建一个新索引。
//
// 索引将包含包含指定元素和/或属性的所有节点。如果“element”和“attr”都为NULL，则索引将包含节点树中元素的排序列表。
// 节点按元素名称排序，如果“attr”参数不为NULL，则按属性值排序。
//

mxml_index_t * mxmlIndexNew(mxml_node_t *node,		// I - XML节点树
                            const char *element,	// I - 要索引的元素或NULL以获取所有
                            const char *attr)		// I - 要索引的属性或NULL以获取所有
{
    mxml_index_t *ind;		// 新索引
    mxml_node_t **temp;		// 临时节点指针数组
    mxml_node_t *current;	// 索引中的当前节点


    // 范围检查输入...
    MXML_DEBUG("mxmlIndexNew（node =％p，element = \“％s \”，attr = \“％s \”）\n", node, element ? element : "（null）", attr ? attr : "（null）");

    if (!node)
        return (NULL);

    // 创建一个新索引...
    if ((ind = calloc(1, sizeof(mxml_index_t))) == NULL)
        return (NULL);

    if (attr)
    {
        if ((ind->attr = _mxml_strcopy(attr)) == NULL)
        {
            free(ind);
            return (NULL);
        }
    }

    if (!element && !attr)
        current = node;
    else
        current = mxmlFindElement(node, node, element, attr, NULL, MXML_DESCEND_ALL);

    while (current)
    {
        if (ind->num_nodes >= ind->alloc_nodes)
        {
            if ((temp = realloc(ind->nodes, (ind->alloc_nodes + 64) * sizeof(mxml_node_t *))) == NULL)
            {
                // 无法为索引分配内存，因此中止...
                mxmlIndexDelete(ind);
                return (NULL);
            }

            ind->nodes = temp;
            ind->alloc_nodes += 64;
        }

        ind->nodes[ind->num_nodes++] = current;

        current = mxmlFindElement(current, node, element, attr, NULL, MXML_DESCEND_ALL);
    }

    // 根据搜索条件对节点进行排序...
    if (ind->num_nodes > 1)
        index_sort(ind, 0, ind->num_nodes - 1);

    // 返回新索引...
    return (ind);
}


//
// 'mxmlIndexReset（）' - 重置索引中的枚举/查找指针并返回索引中的第一个节点。
//
// 此函数重置索引“ind”中的枚举/查找指针，并在首次使用@link mxmlIndexEnum@或@link mxmlIndexFind@之前调用。
//

mxml_node_t * mxmlIndexReset(mxml_index_t *ind)	// I - 要重置的索引
{
    MXML_DEBUG("mxmlIndexReset（ind =％p）\n", ind);

    // 范围检查输入...
    if (!ind)
        return (NULL);

    // 将索引设置为第一个元素...
    ind->cur_node = 0;

    // 返回第一个节点...
    if (ind->num_nodes)
        return (ind->nodes[0]);
    else
        return (NULL);
}


//
// 'index_compare（）' - 比较两个节点。
//

static int index_compare(mxml_index_t *ind, mxml_node_t *first, mxml_node_t *second)
{
    int diff;		// 差异


    // 检查元素名称...
    if ((diff = strcmp(first->value.element.name, second->value.element.name)) != 0)
        return (diff);

    // 检查属性值...
    if (ind->attr)
    {
        if ((diff = strcmp(mxmlElementGetAttr(first, ind->attr), mxmlElementGetAttr(second, ind->attr))) != 0)
            return (diff);
    }

    // 没有差异，返回0...
    return (0);
}


//
// 'index_find（）' - 将节点与索引值进行比较。
//

static int index_find(mxml_index_t *ind, const char *element, const char *value, mxml_node_t *node)
{
    int diff;		// 差异


    // 检查元素名称...
    if (element)
    {
        if ((diff = strcmp(element, node->value.element.name)) != 0)
            return (diff);
    }

    // 检查属性值...
    if (value)
    {
        if ((diff = strcmp(value, mxmlElementGetAttr(node, ind->attr))) != 0)
            return (diff);
    }

    // 没有差异，返回0...
    return (0);
}


//
// 'index_sort（）' - 对索引中的节点进行排序...
//
// 此函数实现了经典的快速排序算法...
//

static void index_sort(mxml_index_t *ind, int left, int right)
{
    mxml_node_t *pivot,		// 枢轴节点
        *temp;			// 交换节点
    int templ,			// 临时左节点
        tempr;			// 临时右节点


    // 循环直到我们完全排序到右边...
    do
    {
        // 在当前分区中对枢轴进行排序...
        pivot = ind->nodes[left];

        for (templ = left, tempr = right; templ < tempr;)
        {
            // 当左节点<=枢轴节点时向左移动...
            while ((templ < right) && index_compare(ind, ind->nodes[templ], pivot) <= 0)
                templ++;

            // 当右节点>枢轴节点时向右移动...
            while ((tempr > left) && index_compare(ind, ind->nodes[tempr], pivot) > 0)
                tempr--;

            // 如果需要，交换节点...
            if (templ < tempr)
            {
                temp = ind->nodes[templ];
                ind->nodes[templ] = ind->nodes[tempr];
                ind->nodes[tempr] = temp;
            }
        }

        // 当我们到达这里时，右侧（tempr）节点是枢轴节点的新位置...
        if (index_compare(ind, pivot, ind->nodes[tempr]) > 0)
        {
            ind->nodes[left] = ind->nodes[tempr];
            ind->nodes[tempr] = pivot;
        }

        // 根据需要递归排序左分区...
        if (left < (tempr - 1))
            index_sort(ind, left, tempr - 1);
    } while (right > (left = tempr + 1));
}