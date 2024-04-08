//
// Mini-XML的搜索/导航函数，这是一个小型的XML文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2003-2024 Michael R Sweet.
//
// 根据Apache许可证v2.0授权。更多信息请参阅"LICENSE"文件。
//

#include "mxml-private.h"


//
// 'mxmlFindElement()' - 查找指定的元素。
//
// 此函数在从节点`node`开始的XML树`top`中查找名为`element`的元素。搜索由元素名称`element`、属性名称`attr`和属性值`value`约束 - `NULL`名称或值被视为通配符，因此可以通过查找具有给定名称的所有元素或具有特定属性的所有元素来实现不同类型的搜索。
//
// `descend`参数确定搜索是否进入子节点；通常，您将在初始搜索中使用`MXML_DESCEND_FIRST`，并使用`MXML_DESCEND_NONE`来查找节点的其他直接子节点。
//

mxml_node_t *				// 输出 - 元素节点或`NULL`
mxmlFindElement(mxml_node_t    *node,	// 输入 - 当前节点
                mxml_node_t    *top,	// 输入 - 顶级节点
                const char     *element,// 输入 - 元素名称或`NULL`表示任意元素
		const char     *attr,	// 输入 - 属性名称或`NULL`表示无属性
		const char     *value,	// 输入 - 属性值或`NULL`表示任意值
		mxml_descend_t descend)	// 输入 - 遍历树的方式 - `MXML_DESCEND_ALL`、`MXML_DESCEND_NONE`或`MXML_DESCEND_FIRST`
{
  const char	*temp;			// 当前属性值


  // 输入范围检查...
  if (!node || !top || (!attr && value))
    return (NULL);

  // 从下一个节点开始...
  node = mxmlWalkNext(node, top, descend);

  // 循环直到找到匹配的元素...
  while (node != NULL)
  {
    // 检查此节点是否匹配...
    if (node->type == MXML_TYPE_ELEMENT && node->value.element.name && (!element || !strcmp(node->value.element.name, element)))
    {
      // 检查是否需要检查属性...
      if (!attr)
        return (node);			// 无属性搜索，返回该节点...

      // 检查属性...
      if ((temp = mxmlElementGetAttr(node, attr)) != NULL)
      {
        // 属性匹配，返回该节点...
	if (!value || !strcmp(value, temp))
	  return (node);
      }
    }

    // 无匹配，继续下一个节点...
    if (descend == MXML_DESCEND_ALL)
      node = mxmlWalkNext(node, top, MXML_DESCEND_ALL);
    else
      node = node->next;
  }

  return (NULL);
}


//
// 'mxmlFindPath()' - 根据给定的路径查找节点。
//
// 此函数使用斜杠分隔的元素名称列表`path`在XML树`top`中查找节点。名称"*"被视为一个或多个级别元素的通配符，例如"foo/one/two"、"bar/two/one"、"*\/one"等等。
//
// 如果给定节点具有子节点且第一个子节点是值节点，则返回第一个子节点。
//

mxml_node_t *				// 输出 - 找到的节点或`NULL`
mxmlFindPath(mxml_node_t *top,		// 输入 - 顶级节点
	     const char  *path)		// 输入 - 元素的路径
{
  mxml_node_t	*node;			// 当前节点
  char		element[256];		// 当前元素名称
  const char	*pathsep;		// 路径中的分隔符
  mxml_descend_t descend;		// mxmlFindElement选项


  // 输入范围检查...
  if (!top || !path || !*path)
    return (NULL);

  // 搜索路径中的每个元素...
  node = top;
  while (*path)
  {
    // 处理通配符...
    if (!strncmp(path, "*/", 2))
    {
      path += 2;
      descend = MXML_DESCEND_ALL;
    }
    else
    {
      descend = MXML_DESCEND_FIRST;
    }

    // 获取路径中的下一个元素...
    if ((pathsep = strchr(path, '/')) == NULL)
      pathsep = path + strlen(path);

    if (pathsep == path || (size_t)(pathsep - path) >= sizeof(element))
      return (NULL);

    memcpy(element, path, pathsep - path);
    element[pathsep - path] = '\0';

    if (*pathsep)
      path = pathsep + 1;
    else
      path = pathsep;

    // 查找元素...
    if ((node = mxmlFindElement(node, node, element, NULL, NULL, descend)) == NULL)
      return (NULL);
  }

  // 如果执行到这里，返回节点或其第一个子节点...
  if (node->child && node->child->type != MXML_TYPE_ELEMENT)
    return (node->child);
  else
    return (node);
}


//
// 'mxmlWalkNext()' - 遍历树中的下一个逻辑节点。
//
// 此函数在树中遍历到下一个逻辑节点。`descend`参数控制是否将第一个子节点视为下一个节点。`top`参数将遍历约束为该节点的子节点。
//

mxml_node_t *				// 输出 - 下一个节点或`NULL`
mxmlWalkNext(mxml_node_t    *node,	// 输入 - 当前节点
             mxml_node_t    *top,	// 输入 - 顶级节点
             mxml_descend_t descend)	// 输入 - 遍历树的方式 - `MXML_DESCEND_ALL`、`MXML_DESCEND_NONE`或`MXML_DESCEND_FIRST`
{
  if (!node)
  {
    return (NULL);
  }
  else if (node->child && descend != MXML_DESCEND_NONE)
  {
    return (node->child);
  }
  else if (node == top)
  {
    return (NULL);
  }
  else if (node->next)
  {
    return (node->next);
  }
  else if (node->parent && node->parent != top)
  {
    node = node->parent;

    while (!node->next)
    {
      if (node->parent == top || !node->parent)
        return (NULL);
      else
        node = node->parent;
    }

    return (node->next);
  }
  else
  {
    return (NULL);
  }
}


//
// 'mxmlWalkPrev()' - 遍历树中的上一个逻辑节点。
//
// 此函数在树中遍历到上一个逻辑节点。`descend`参数控制是否将第一个子节点视为下一个节点。`top`参数将遍历约束为该节点的子节点。
//

mxml_node_t *				// 输出 - 上一个节点或`NULL`
mxmlWalkPrev(mxml_node_t    *node,	// 输入 - 当前节点
             mxml_node_t    *top,	// 输入 - 顶级节点
             mxml_descend_t descend)	// 输入 - 遍历树的方式 - `MXML_DESCEND_ALL`、`MXML_DESCEND_NONE`或`MXML_DESCEND_FIRST`
{
  if (!node || node == top)
  {
    return (NULL);
  }
  else if (node->prev)
  {
    if (node->prev->last_child && descend != MXML_DESCEND_NONE)
    {
      // 查找上一个节点下的最后一个子节点...
      node = node->prev->last_child;

      while (node->last_child)
        node = node->last_child;

      return (node);
    }
    else
    {
      return (node->prev);
    }
  }
  else if (node->parent != top)
  {
    return (node->parent);
  }
  else
  {
    return (NULL);
  }
}