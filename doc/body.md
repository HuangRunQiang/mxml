标题：Mini-XML 4.0 编程手册
作者：Michael R Sweet
版权：版权所有 © 2003-2024，保留所有权利。
版本：4.0
...


介绍
============

Mini-XML 是一个小型的 XML 解析库，您可以在应用程序中使用它来读取 XML 数据文件或字符串，而无需使用大型的非标准库。Mini-XML 提供以下功能：

- 支持读取 UTF-8 和 UTF-16 编码的 XML 文件和字符串，以及写入 UTF-8 编码的 XML 文件和字符串。
- 数据以链表树结构存储，保留了 XML 数据的层次结构。
- 通过 SAX（流式）读取 XML 文件和字符串，以最小化内存使用。
- 支持任意元素名称、属性和属性值，没有预设限制，只受可用内存限制。
- 在“叶子”节点中支持整数、实数、不透明（"CDATA"）、文本和自定义数据类型。
- 提供创建和管理数据树的函数。
- 提供用于查找和遍历数据树的“查找”和“遍历”函数。
- 支持自定义字符串内存管理函数，以实现字符串池和其他减少内存使用的方案。

Mini-XML 不对基于模式文件或其他定义信息源的数据进行验证或其他类型的处理。


历史
-------

Mini-XML 最初是为 [Gutenprint](http://gutenprint.sf.net/) 项目开发的，旨在用较小且易于使用的方式取代庞大而笨重的 `libxml2` 库。一切始于 2003 年 6 月的一个早晨，Robert 在开发者列表中发布了以下句子：

> 我们需要 libxml2 已经够糟糕了，但自己编写 XML 解析器有点超出我们的能力范围。

我随后回答道：

> 考虑到你在 XML 中使用的有限范围，用几百行代码编写一个迷你 XML API 应该是微不足道的。

我接受了自己的挑战，连续两天疯狂编码，最终发布了 Mini-XML 的首个公开版本，总代码行数为 696 行。Robert 随即将 Mini-XML 集成到 Gutenprint 中，并移除了 libxml2。

得益于各位开发者的大量反馈和支持，Mini-XML 自那时以来不断发展，提供了更完整的 XML 实现，目前的代码行数达到了令人惊叹的 3,491 行，而 libxml2 版本 2.11.7 的代码行数为 175,808 行。


资源
---------

您可以在 [Mini-XML 官方网站](https://www.msweet.org/mxml) 上找到 Mini-XML 的主页。在那里，您可以下载当前版本的 Mini-XML，访问问题跟踪器，并找到其他资源。

Mini-XML v4 的 API 与之前的版本略有不同。有关详细信息，请参阅 [从 Mini-XML v3.x 迁移](@) 章节。


法律事项
-----------

Mini-XML 库的版权归 Michael R Sweet 所有，版权所有 © 2003-2024，采用 Apache 许可证版本 2.0 提供，附加（可选）例外条款以允许与仅限于 GPL2/LGPL2 的软件进行链接。有关更多信息，请参阅 "LICENSE" 和 "NOTICE" 文件。


使用 Mini-XML
==============

Mini-XML 提供一个单独的头文件，您可以包含它：

```c
#include <mxml.h>
```

Mini-XML 库可以使用 `-lmxml4` 选项将其包含到您的程序中：

    gcc -o myprogram myprogram.c -lmxml4

如果您安装了 `pkg-config` 软件，可以使用它来确定适合您安装的正确编译器和链接器选项：

    gcc `pkg-config --cflags mxml4` -o myprogram myprogram.c `pkg-config --libs mxml4`

> 注意：库名 "mxml4" 是一个配置时的选项。如果您使用了 `--disable-libmxml4-prefix` 配置选项，则库名为 "mxml"。


API 基础知识
----------

XML 文件中的每个信息都以 "节点" 的形式存储在内存中。节点由 `mxml_node_t` 指针表示。每个节点都有一个关联的类型、值、父节点、兄弟节点（前一个和后一个）、可能的第一个和最后一个子节点，以及一个可选的用户数据指针。

例如，如果您有一个如下所示的 XML 文件：

```xml
<?xml version="1.0" encoding="utf-8"?>
<data>
    <node>val1</node>
    <node>val2</node>
    <node>val3</node>
    <group>
        <node>val4</node>
        <node>val5</node>
        <node>val6</node>
    </group>
    <node>val7</node>
    <node>val8</node>
</data>
```

文件的节点树在内存中的表示如下：

```
<?xml version="1.0" encoding="utf-8"?>
  |
<data>
  |
<node> - <node> - <node> - <group> - <node> - <node>
  |        |        |         |        |        |
 val1     val2     val3       |       val7     val8
                              |
                            <node> - <node> - <node>
                              |        |        |
                             val4     val5     val6
```

其中 "-" 是指向兄弟节点的指针，"|" 是指向第一个子节点或父节点的指针。

[mxmlGetType](@@) 函数获取节点的类型，表示为 `mxml_type_t` 枚举值：

- `MXML_TYPE_CDATA`：CDATA，例如 `<![CDATA[...]]>`，
- `MXML_TYPE_COMMENT`：注释，例如 `<!-- my comment -->`，
- `MXML_TYPE_CUSTOM`：应用程序定义的自定义值，
- `MXML_TYPE_DECLARATION`：声明，例如 `<!DOCTYPE html>`，
- `MXML_TYPE_DIRECTIVE`：处理指令，例如 `<?xml version="1.0" encoding="utf-8"?>`，
- `MXML_TYPE_ELEMENT`：具有可选属性的 XML 元素，例如 `<element name="value">`，
- `MXML_TYPE_INTEGER`：以空格分隔的整数值，例如 `42`，
- `MXML_TYPE_OPAQUE`：不透明的字符串值，保留所有空格，例如 `All work and no play makes Johnny a dull boy.`，
- `MXML_TYPE_REAL`：以空格分隔的浮点数值，例如 `123.4`，或
- `MXML_TYPE_TEXT`：以空格分隔的文本（片段）值，例如 `Word`。

可以使用 [mxmlGetParent](@@)、[mxmlGetNextSibling](@@)、[mxmlGetPreviousSibling](@@)、[mxmlGetFirstChild](@@) 和 [mxmlGetLastChild](@@) 函数来访问父节点、兄弟节点和子节点。

可以使用 [mxmlGetCDATA](@@)、[mxmlGetComment](@@)、[mxmlGetDeclaration](@@)、[mxmlGetDirective](@@)、[mxmlGetElement](@@)、[mxmlElementGetAttr](@@)、[mxmlGetInteger](@@)、[mxmlGetOpaque](@@)、[mxmlGetReal](@@) 和 [mxmlGetText](@@) 函数来访问节点的值。


加载 XML 文件
-------------------

您可以使用 [mxmlLoadFilename](@@) 函数加载 XML 文件：

```c
mxml_node_t *
mxmlLoadFilename(mxml_node_t *top, mxml_options_t *options,
                 const char *filename);
```

Mini-XML 还提供了从 `FILE` 指针、文件描述符、字符串或使用回调加载的函数：

```c
mxml_node_t *
mxmlLoadFd(mxml_node_t *top, mxml_options_t *options,
           int fd);

mxml_node_t *
mxmlLoadFile(mxml_node_t *top, mxml_options_t *options,
             FILE *fp);

mxml_node_t *
mxmlLoadIO(mxml_node_t *top, mxml_options_t *options,
           mxml_io_cb_t io_cb, void *io_cbdata);

mxml_node_t *
mxmlLoadString(mxml_node_t *top, mxml_options_t *options,
               const char *s);
```

每个函数接受指向顶级（"根"）节点的指针（通常为 `NULL`），任何加载选项以及要加载的内容。例如，以下代码将使用默认加载选项加载名为 "example.xml" 的 XML 文件：

```c
mxml_node_t *xml;

xml = mxmlLoadFilename(/*top*/NULL, /*options*/NULL,
                       "example.xml");
```


### 加载选项

加载选项使用 `mxml_options_t` 指针指定，可以使用 [mxmlOptionsNew](@@) 函数创建：

```c
mxml_options_t *options = mxmlOptionsNew();
```

默认加载选项将将 XML 中的任何值视为以空格分隔的文本（`MXML_TYPE_TEXT`）。您可以使用 [mxmlOptionsSetTypeValue](@@) 函数指定不同类型的值。例如，以下代码将指定值为包含空格的不透明文本字符串（`MXML_TYPE_OPAQUE`）：

```c
mxmlOptionsSetTypeValue(options, MXML_TYPE_OPAQUE);
```

对于更复杂的 XML 文档，可以指定一个回调函数，该函数根据给定元素节点返回值的类型，使用 [mxmlOptionsSetTypeCallback](@@) 函数。例如，要指定一个名为 `my_type_cb` 的回调函数，没有回调数据：

```c
mxmlOptionsSetTypeValue(options, my_type_cb, /*cbdata*/NULL);
```

`my_type_cb` 函数接受回调数据指针（在本例中为 `NULL`）和当前元素的 `mxml_node_t` 指针，并返回一个 `mxml_type_t` 枚举值，指定子节点的值类型。例如，以下函数根据 "type" 属性和元素名称确定节点的子节点的值类型：

```c
mxml_type_t
my_load_cb(void *cbdata, mxml_node_t *node)
{
  const char *type;

 /*
  * 您可以查找属性和/或使用元素名称、层次结构等...
  */

  type = mxmlElementGetAttr(node, "type");
  if (type == NULL)
    type = mxmlGetElement(node);
  if (type == NULL)
    type = "text";

  if (!strcmp(type, "integer"))
    return (MXML_TYPE_INTEGER);
  else if (!strcmp(type, "opaque"))
    return (MXML_TYPE_OPAQUE);
  else if (!strcmp(type, "real"))
    return (MXML_TYPE_REAL);
  else
    return (MXML_TYPE_TEXT);
}
```


查找节点
-------------

[mxmlFindPath](@@) 函数使用路径查找特定元素下的（第一个）值节点。路径字符串可以包含 "*" 通配符，以匹配层次结构中的单个元素节点。例如，以下代码将查找 "group" 元素下的第一个 "node" 元素，首先使用显式路径，然后使用通配符：

```c
mxml_node_t *directnode = mxmlFindPath(xml, "data/group/node");

mxml_node_t *wildnode = mxmlFindPath(xml, "data/*/node");
```

[mxmlFindElement](@@) 函数用于查找具有指定名称的元素，可选择匹配属性和值：

```c
mxml_node_t *
mxmlFindElement(mxml_node_t *node, mxml_node_t *top,
                const char *element, const char *attr,
                const char *value, int descend);
```

可以将 `element`、`attr` 和 `value` 参数传递为 `NULL`，以充当通配符，例如：

```c
mxml_node_t *node;

/* 查找第一个 "a" 元素 */
node = mxmlFindElement(tree, tree, "a", NULL, NULL,
                       MXML_DESCEND_ALL);

/* 查找具有 "href" 属性的第一个 "a" 元素 */
node = mxmlFindElement(tree, tree, "a", "href", NULL,
                       MXML_DESCEND_ALL);

/* 查找具有 "href" 属性且值为 URL 的第一个 "a" 元素 */
node = mxmlFindElement(tree, tree, "a", "href",
                       "http://msweet.org/",
                       MXML_DESCEND_ALL);

/* 查找具有 "src" 属性的第一个元素 */
node = mxmlFindElement(tree, tree, NULL, "src", NULL,
                       MXML_DESCEND_ALL);

/* 查找具有 "src" 值为 "foo.jpg" 的第一个元素 */
node = mxmlFindElement(tree, tree, NULL, "src", "foo.jpg",
                       MXML_DESCEND_ALL);
```

还可以使用相同的函数进行迭代：

```c
mxml_node_t *node;

for (node = mxmlFindElement(tree, tree, "element", NULL,
                            NULL, MXML_DESCEND_ALL);
     node != NULL;
     node = mxmlFindElement(node, tree, "element", NULL,
                            NULL, MXML_DESCEND_ALL))
{
  ... 做一些操作 ...
}
```

`descend` 参数（在前面的示例中为 `MXML_DESCEND_ALL`）可以是以下三个常量之一：

- `MXML_DESCEND_NONE`：忽略元素层次结构中的子节点，而使用同级节点（同一级别）或父节点（上一级）直到达到顶级（根）节点。
- `MXML_DESCEND_FIRST`：从节点的第一个子节点开始搜索，然后搜索兄弟节点。通常在迭代父节点的直接子节点时使用此选项，例如，在前面示例中的 `<?xml ...?>` 父节点下的所有 `<node>` 和 `<group>` 元素。
- `MXML_DESCEND_ALL`：首先搜索子节点，然后搜索兄弟节点，然后搜索父节点。


从节点获取值
-------------------------------

一旦获得节点，可以使用 mxmlGetXxx 函数之一检索其值。

元素（`MXML_TYPE_ELEMENT`）节点具有关联的名称和零个或多个具有（字符串）值的命名属性。[mxmlGetElement](@@) 函数检索元素名称，而 [mxmlElementGetAttr](@@) 函数检索命名属性的值字符串。例如，以下代码查找 HTML 标题元素，并在找到时显示标题的 "id" 属性：

```c
const char *elemname = mxmlGetElement(node);
const char *id_value = mxmlElementGetAttr(node, "id");

if ((*elemname == 'h' || *elemname == 'H') &&
    elemname[1] >= '1' && elemname[1] <= '6' &&
    id_value != NULL)
  printf("%s: %s\n", elemname, id_value);
```

[mxmlElementGetAttrByIndex](@@) 和 [mxmlElementGetAttrCount](@@) 函数允许您迭代元素的所有属性。例如，以下代码打印元素名称及其每个属性：

```c
const char *elemname = mxmlGetElement(node);
printf("%s:\n", elemname);

size_t i, count;
for (i = 0, count = mxmlElementGetAttrCount(node); i < count; i ++)
{
  const char *attrname, *attrvalue;

  attrvalue = mxmlElementGetAttrByIndex(node, i, &attrname);

  printf("    %s=\"%s\"\n", attrname, attrvalue);
}
```

CDATA（`MXML_TYPE_CDATA`）节点具有关联的字符串值，该值由 `<![CDATA[` 和 `]]>` 之间的文本组成。[mxmlGetCDATA](@@) 函数检索节点的 CDATA 字符串指针。例如，以下代码获取 CDATA 字符串值：

```c
const char *cdatavalue = mxmlGetCDATA(node);
```

注释（`MXML_TYPE_COMMENT`）节点具有关联的字符串值，该值由 `<!--` 和 `-->` 之间的文本组成。[mxmlGetComment](@@) 函数检索节点的注释字符串指针。例如，以下代码获取注释字符串值：

```c
const char *commentvalue = mxmlGetComment(node);
```

处理指令（`MXML_TYPE_DIRECTIVE`）节点具有关联的字符串值，该值由 `<?` 和 `?>` 之间的文本组成。[mxmlGetDirective](@@) 函数检索节点的处理指令字符串。例如，以下代码获取处理指令字符串值：

```c
const char *instrvalue = mxmlGetDirective(node);
```

整数（`MXML_TYPE_INTEGER`）节点具有关联的 `long` 值。[mxmlGetInteger](@@) 函数检索节点的整数值。例如，以下代码获取整数值：

```c
long intvalue = mxmlGetInteger(node);
```

不透明字符串（`MXML_TYPE_OPAQUE`）节点具有关联的字符串值，该值由元素之间的文本组成。[mxmlGetOpaque](@@) 函数检索节点的不透明字符串指针。例如，以下代码获取不透明字符串值：

```c
const char *opaquevalue = mxmlGetOpaque(node);
```

实数（`MXML_TYPE_REAL`）节点具有关联的 `double` 值。[mxmlGetReal](@@) 函数检索节点的实数值。例如，以下代码获取实数值：

```c
double realvalue = mxmlGetReal(node);
```

以空格分隔的文本字符串（`MXML_TYPE_TEXT`）节点具有与之关联的空格指示符和从元素之间的文本中提取的字符串值。[mxmlGetText](@@) 函数检索文本字符串指针和空格布尔值。例如，以下代码获取文本和空格指示符：

```c
const char *textvalue;
bool whitespace;

textvalue = mxmlGetText(node, &whitespace);
```


保存 XML 文件
------------------

您可以使用 [mxmlSaveFilename](@@) 函数保存 XML 文件：

```c
bool
mxmlSaveFilename(mxml_node_t *node, mxml_options_t *options,
                 const char *filename);
```

Mini-XML 还提供了将 XML 保存到 `FILE` 指针、文件描述符、字符串或使用回调的函数：

```c
char *
mxmlSaveAllocString(mxml_node_t *node, mxml_options_t *options);

bool
mxmlSaveFd(mxml_node_t *node, mxml_options_t *options,
           int fd);

bool
mxmlSaveFile(mxml_node_t *node, mxml_options_t *options,
             FILE *fp);

bool
mxmlSaveIO(mxml_node_t *node, mxml_options_t *options,
           mxml_io_cb_t *io_cb, void *io_cbdata);

size_t
mxmlSaveString(mxml_node_t *node, mxml_options_t *options,
               char *buffer, size_t bufsize);
```

每个函数接受指向最顶层（"根"）节点的指针，任何保存选项以及（根据需要）目标。例如，以下代码使用默认选项将 XML 文件保存到名为 "example.xml" 的文件中：

```c
mxmlSaveFile(xml, /*options*/NULL, "example.xml");
```


### 保存选项

保存选项使用 `mxml_options_t` 指针指定，可以使用 [mxmlOptionsNew](@@) 函数创建：

```c
mxml_options_t *options = mxmlOptionsNew();
```

默认保存选项会在第 72 列处换行，但不会添加其他空格。您可以使用 [mxmlOptionsSetWrapMargin](@@) 函数更改换行列。例如，以下代码将换行列设置为 0，禁用换行：

```c
mxmlOptionsSetWrapMargin(options, 0);
```

要在输出中添加其他空格，请使用 [mxmlOptionsSetWhitespaceCallback](@@) 函数设置空格回调。空格回调接受回调数据指针、当前节点以及空格位置值 `MXML_WS_BEFORE_OPEN`、`MXML_WS_AFTER_OPEN`、`MXML_WS_BEFORE_CLOSE` 或 `MXML_WS_AFTER_CLOSE`。如果不需要插入空格，则回调应返回 `NULL`；否则，返回要插入的空格、制表符、回车和换行符的字符串。

以下空格回调可用于在 XHTML 输出中添加空格，以使其在标准文本编辑器中更易读：

```c
const char *
whitespace_cb(void *cbdata, mxml_node_t *node, mxml_ws_t where)
{
  const char *element;

 /*
  * 我们可以有条件地在任何元素之前或之后断行。这些只是常见的 HTML 元素...
  */

  element = mxmlGetElement(node);

  if (!strcmp(element, "html") ||
      !strcmp(element, "head") ||
      !strcmp(element, "body") ||
      !strcmp(element, "pre") ||
      !strcmp(element, "p") ||
      !strcmp(element, "h1") ||
      !strcmp(element, "h2") ||
      !strcmp(element, "h3") ||
      !strcmp(element, "h4") ||
      !strcmp(element, "h5") ||
      !strcmp(element, "h6"))
  {
   /*
    * 在打开之前和关闭之后换行...
    */

    if (where == MXML_WS_BEFORE_OPEN ||
        where == MXML_WS_AFTER_CLOSE)
      return ("\n");
  }
  else if (!strcmp(element, "dl") ||
           !strcmp(element, "ol") ||
           !strcmp(element, "ul"))
  {
   /*
    * 在列表元素之前和之后放置换行符...
    */

    return ("\n");
  }
  else if (!strcmp(element, "dd") ||
           !strcmp(element, "dt") ||
           !strcmp(element, "li"))
  {
   /*
    * 在 <li>、<dd> 和 <dt> 前面放置制表符，并在后面放置换行符...
    */

    if (where == MXML_WS_BEFORE_OPEN)
      return ("\t");
    else if (where == MXML_WS_AFTER_CLOSE)
      return ("\n");
  }

 /*
  * 否则返回 NULL，不添加额外的空格...
  */

  return (NULL);
}
```

以下代码将为保存选项设置空格回调：

```c
mxmlOptionsSetWhitespaceCallback(options, whitespace_cb, /*cbdata*/NULL);
```


释放内存
--------------

一旦完成对 XML 数据的使用，使用 [mxmlDelete](@@) 函数释放用于特定节点及其子节点的内存。例如，以下代码释放由前面示例加载的 XML 数据：

```c
mxmlDelete(xml);
```


创建新的 XML 文档
==========================

您可以使用各种 mxmlNewXxx 函数在内存中创建新的和更新现有的 XML 文档。以下代码将创建 [使用 Mini-XML](@) 章节中描述的 XML 文档：

```c
mxml_node_t *xml;    /* <?xml version="1.0" charset="utf-8"?> */
mxml_node_t *data;   /* <data> */
mxml_node_t *node;   /* <node> */
mxml_node_t *group;  /* <group> */

xml = mxmlNewXML("1.0");

data = mxmlNewElement(xml, "data");

  node = mxmlNewElement(data, "node");
  mxmlNewText(node, false, "val1");
  node = mxmlNewElement(data, "node");
  mxmlNewText(node, false, "val2");
  node = mxmlNewElement(data, "node");
  mxmlNewText(node, false, "val3");

  group = mxmlNewElement(data, "group");

    node = mxmlNewElement(group, "node");
    mxmlNewText(node, false, "val4");
    node = mxmlNewElement(group, "node");
    mxmlNewText(node, false, "val5");
    node = mxmlNewElement(group, "node");
    mxmlNewText(node, false, "val6");

  node = mxmlNewElement(data, "node");
  mxmlNewText(node, false, "val7");
  node = mxmlNewElement(data, "node");
  mxmlNewText(node, false, "val8");
```

我们首先使用 [mxmlNewXML](@@) 函数创建所有 XML 文件共有的处理指令节点：

```c
xml = mxmlNewXML("1.0");
```

然后，我们使用 [mxmlNewElement](@@) 函数创建用于此文档的 `<data>` 节点。第一个参数指定父节点（`xml`），第二个参数指定元素名称（`data`）：

```c
data = mxmlNewElement(xml, "data");
```

每个文件中的 `<node>...</node>` 都是使用 [mxmlNewElement](@@) 和 [mxmlNewText](@@) 函数创建的。[mxmlNewText](@@) 函数的第一个参数指定父节点（`node`），第二个参数指定文本之前是否有空格（在本例中为 `false`），最后一个参数指定要添加的实际文本：

```c
node = mxmlNewElement(data, "node");
mxmlNewText(node, false, "val1");
```

然后，可以保存或处理生成的内存中的 XML 文档，就像从磁盘或字符串加载的文档一样。


元素节点
-------------

使用 [mxmlNewElement](@@) 函数创建元素（`MXML_TYPE_ELEMENT`）节点。使用 [mxmlElementSetAttr](@@) 和 [mxmlElementSetAttrf](@@) 函数设置元素属性，并使用 [mxmlElementClearAttr](@@) 函数清除属性：

```c
mxml_node_t *
mxmlNewElement(mxml_node_t *parent, const char *name);

void
mxmlElementClearAttr(mxml_node_t *node, const char *name);

void
mxmlElementSetAttr(mxml_node_t *node, const char *name,
                   const char *value);

void
mxmlElementSetAttrf(mxml_node_t *node, const char *name,
                    const char *format, ...);
```


CDATA 节点
-----------

使用 [mxmlNewCDATA](@@) 和 [mxmlNewCDATAf](@@) 函数创建 CDATA（`MXML_TYPE_CDATA`）节点，并使用 [mxmlSetCDATA](@@) 和 [mxmlSetCDATAf](@@) 函数设置 CDATA 值：

```c
mxml_node_t *
mxmlNewCDATA(mxml_node_t *parent, const char *string);

mxml_node_t *
mxmlNewCDATAf(mxml_node_t *parent, const char *format, ...);

void
mxmlSetCDATA(mxml_node_t *node, const char *string);

void
mxmlSetCDATAf(mxml_node_t *node, const char *format, ...);
```


注释节点
-------------

使用 [mxmlNewComment](@@) 和 [mxmlNewCommentf](@@) 函数创建注释（`MXML_TYPE_COMMENT`）节点，并使用 [mxmlSetComment](@@) 和 [mxmlSetCommentf](@@) 函数设置注释值：

```c
mxml_node_t *
mxmlNewComment(mxml_node_t *parent, const char *string);

mxml_node_t *
mxmlNewCommentf(mxml_node_t *parent, const char *format, ...);

void
mxmlSetComment(mxml_node_t *node, const char *string);

void
mxmlSetCommentf(mxml_node_t *node, const char *format, ...);
```

处理指令节点
----------------------------

使用[mxmlNewDirective](@@)和[mxmlNewDirectivef](@@)函数创建处理指令(`MXML_TYPE_DIRECTIVE`)节点，并使用[mxmlSetDirective](@@)和[mxmlSetDirectivef](@@)函数设置：

```c
mxml_node_t *node = mxmlNewDirective("xml-stylesheet type=\"text/css\" href=\"style.css\"");

mxml_node_t *node = mxmlNewDirectivef("xml version=\"%s\"", version);
```

[mxmlNewXML](@@)函数可用于创建顶级的"xml"处理指令，并带有关联的版本号：

```c
mxml_node_t *
mxmlNewXML(const char *version);
```


整数节点
-------------

使用[mxmlNewInteger](@@)函数创建整数(`MXML_TYPE_INTEGER`)节点，并使用[mxmlSetInteger](@@)函数设置：

```c
mxml_node_t *
mxmlNewInteger(mxml_node_t *parent, long integer);

void
mxmlSetInteger(mxml_node_t *node, long integer);
```


不透明字符串节点
-------------------

使用[mxmlNewOpaque](@@)和[mxmlNewOpaquef](@@)函数创建不透明字符串(`MXML_TYPE_OPAQUE`)节点，并使用[mxmlSetOpaque](@@)和[mxmlSetOpaquef](@@)函数设置：

```c
mxml_node_t *
mxmlNewOpaque(mxml_node_t *parent, const char *opaque);

mxml_node_t *
mxmlNewOpaquef(mxml_node_t *parent, const char *format, ...);

void
mxmlSetOpaque(mxml_node_t *node, const char *opaque);

void
mxmlSetOpaquef(mxml_node_t *node, const char *format, ...);
```


实数节点
-----------------

使用[mxmlNewReal](@@)函数创建实数(`MXML_TYPE_REAL`)节点，并使用[mxmlSetReal](@@)函数设置：

```c
mxml_node_t *
mxmlNewReal(mxml_node_t *parent, double real);

void
mxmlSetReal(mxml_node_t *node, double real);
```


文本节点
----------

使用[mxmlNewText](@@)和[mxmlNewTextf](@@)函数创建以空格分隔的文本字符串(`MXML_TYPE_TEXT`)节点，并使用[mxmlSetText](@@)和[mxmlSetTextf](@@)函数设置。每个文本节点包含文本字符串和（前导）空格布尔值。

```c
mxml_node_t *
mxmlNewText(mxml_node_t *parent, bool whitespace,
            const char *string);

mxml_node_t *
mxmlNewTextf(mxml_node_t *parent, bool whitespace,
             const char *format, ...);

void
mxmlSetText(mxml_node_t *node, bool whitespace,
            const char *string);

void
mxmlSetTextf(mxml_node_t *node, bool whitespace,
             const char *format, ...);
```


遍历和索引树
===============================


遍历节点
---------------

虽然[mxmlFindNode](@@)和[mxmlFindPath](@@)函数可以找到特定的元素节点，但有时需要遍历所有节点。可以使用[mxmlWalkNext](@@)和[mxmlWalkPrev](@@)函数来遍历XML节点树：

```c
mxml_node_t *
mxmlWalkNext(mxml_node_t *node, mxml_node_t *top,
             int descend);

mxml_node_t *
mxmlWalkPrev(mxml_node_t *node, mxml_node_t *top,
             int descend);
```

根据`descend`参数的值，这些函数将自动遍历子节点、兄弟节点和父节点，直到达到`top`节点。例如，以下代码将遍历[Using Mini-XML](@)章节中示例XML文档中的所有节点：

```c
mxml_node_t *node;

for (node = xml;
     node != NULL;
     node = mxmlWalkNext(node, xml, MXML_DESCEND_ALL))
{
  ... 做些什么 ...
}
```

节点将按以下顺序返回：

```
<?xml version="1.0" encoding="utf-8"?>
<data>
<node>
val1
<node>
val2
<node>
val3
<group>
<node>
val4
<node>
val5
<node>
val6
<node>
val7
<node>
val8
```


索引
--------

[mxmlIndexNew](@@)函数允许创建节点的索引，以便进行更快的搜索和枚举：

```c
mxml_index_t *
mxmlIndexNew(mxml_node_t *node, const char *element,
             const char *attr);
```

`element`和`attr`参数控制哪些元素包含在索引中。如果`element`不是`NULL`，则只有具有指定名称的元素会添加到索引中。类似地，如果`attr`不是`NULL`，则只有包含指定属性的元素会添加到索引中。节点在索引中排序。

例如，以下代码在XML文档中创建了所有"id"值的索引：

```c
mxml_index_t *ind = mxmlIndexNew(xml, NULL, "id");
```

创建索引后，可以使用[mxmlIndexFind](@@)函数查找匹配的节点：

```c
mxml_node_t *
mxmlIndexFind(mxml_index_t *ind, const char *element,
              const char *value);
```

例如，以下代码将找到其"id"字符串为"42"的元素：

```c
mxml_node_t *node = mxmlIndexFind(ind, NULL, "42");
```

或者，可以使用[mxmlIndexReset](@@)和[mxmlIndexEnum](@@)函数枚举索引中的节点：

```c
mxml_node_t *
mxmlIndexReset(mxml_index_t *ind);

mxml_node_t *
mxmlIndexEnum(mxml_index_t *ind);
```

通常，这些函数将在`for`循环中使用：

```c
mxml_node_t *node;

for (node = mxmlIndexReset(ind);
     node != NULL;
     node = mxmlIndexEnum(ind))
{
  ... 做些什么 ...
}
```

[mxmlIndexCount](@@)函数返回索引中的节点数：

```c
size_t
mxmlIndexGetCount(mxml_index_t *ind);
```

最后，[mxmlIndexDelete](@@)函数释放与索引关联的所有内存：

```c
void
mxmlIndexDelete(mxml_index_t *ind);
```


高级用法
==============


自定义数据类型
-----------------

Mini-XML通过加载和保存回调选项支持自定义数据类型。每个`mxml_options_t`指针一次只能激活一组回调，但是您的回调可以存储其他信息，以支持根据需要的多个自定义数据类型。`MXML_TYPE_CUSTOM`节点类型标识自定义数据节点。

[mxmlGetCustom](@@)函数检索节点的自定义值指针。

```c
const void *
mxmlGetCustom(mxml_node_t *node);
```

使用[mxmlNewCustom](@@)函数或使用[mxmlOptionsSetCustomCallbacks](@@)函数指定的自定义加载回调创建自定义(`MXML_TYPE_CUSTOM`)节点：

```c
typedef void (*mxml_custfree_cb_t)(void *cbdata, void *data);
typedef bool (*mxml_custload_cb_t)(void *cbdata, mxml_node_t *, const char *);
typedef char *(*mxml_custsave_cb_t)(void *cbdata, mxml_node_t *);

mxml_node_t *
mxmlNewCustom(mxml_node_t *parent, void *data,
              mxml_custfree_cb_t free_cb, void *free_cbdata);

int
mxmlSetCustom(mxml_node_t *node, void *data,
              mxml_custfree_cb_t free_cb, void *free_cbdata);

void
mxmlOptionsSetCustomCallbacks(mxml_option_t *options,
                              mxml_custload_cb_t load_cb,
                              mxml_custsave_cb_t save_cb,
                              void *cbdata);
```

加载回调接收回调数据指针、当前数据节点的指针和从XML源中转换为相应UTF-8字符的不透明字符数据字符串。例如，如果我们想支持一个自定义的日期/时间类型，其值编码为"yyyy-mm-ddThh:mm:ssZ"（ISO 8601格式），则加载回调将如下所示：

```c
typedef struct iso_date_time_s
{
  unsigned year,    /* 年 */
           month,   /* 月 */
           day,     /* 日 */
           hour,    /* 时 */
           minute,  /* 分 */
           second;  /* 秒 */
  time_t   unix;    /* UNIX时间 */
} iso_date_time_t;

bool
custom_load_cb(void *cbdata, mxml_node_t *node, const char *data)
{
  iso_date_time_t *dt;
  struct tm tmdata;

 /*
  * 分配数据结构...
  */

  dt = calloc(1, sizeof(iso_date_time_t));

 /*
  * 尝试从数据字符串中读取6个无符号整数...
  */

  if (sscanf(data, "%u-%u-%uT%u:%u:%uZ", &(dt->year),
             &(dt->month), &(dt->day), &(dt->hour),
             &(dt->minute), &(dt->second)) != 6)
  {
   /*
    * 无法读取数字，释放数据结构并返回错误...
    */

    free(dt);

    return (false);
  }

 /*
  * 检查值的范围...
  */

  if (dt->month < 1 || dt->month > 12 ||
      dt->day < 1 || dt->day > 31 ||
      dt->hour < 0 || dt->hour > 23 ||
      dt->minute < 0 || dt->minute > 59 ||
      dt->second < 0 || dt->second > 60)
  {
   /*
    * 日期信息超出范围...
    */

    free(dt);

    return (false);
  }

 /*
  * 将ISO时间转换为UNIX时间（以秒为单位）...
  */

  tmdata.tm_year = dt->year - 1900;
  tmdata.tm_mon  = dt->month - 1;
  tmdata.tm_day  = dt->day;
  tmdata.tm_hour = dt->hour;
  tmdata.tm_min  = dt->minute;
  tmdata.tm_sec  = dt->second;

  dt->unix = gmtime(&tmdata);

 /*
  * 分配自定义节点数据和释放回调函数/数据...
  */

  mxmlSetCustom(node, data, custom_free_cb, cbdata);

 /*
  * 无错误返回...
  */

  return (true);
}
```

函数本身在成功时返回`true`，如果无法解码自定义数据或数据包含错误，则返回`false`。自定义数据节点包含指向节点的分配的自定义数据的`void`指针和指向析构函数的指针，该析构函数将在删除节点时释放自定义数据。在本例中，我们使用标准的`free`函数，因为所有内容都包含在一个单独的`calloc`块中。

保存回调函数接收节点指针并返回一个分配的字符串，其中包含自定义数据值。以下保存回调函数可以用于我们的ISO日期/时间类型：

```c
char *
custom_save_cb(void *cbdata, mxml_node_t *node)
{
  char data[255];
  iso_date_time_t *dt;


  dt = (iso_date_time_t *)mxmlGetCustom(node);

  snprintf(data, sizeof(data),
           "%04u-%02u-%02uT%02u:%02u:%02uZ",
           dt->year, dt->month, dt->day, dt->hour,
           dt->minute, dt->second);

  return (strdup(data));
}
```

您可以使用`mxmlOptionsSetCustomCallbacks`函数注册这些回调函数：

```c
mxmlOptionsSetCustomCallbacks(options, custom_load_cb,
                              custom_save_cb, /*cbdata*/NULL);
```


SAX（流）加载文档
---------------------------------

Mini-XML支持Simple API for XML（SAX）的实现，允许您将XML文档作为节点流加载和处理。除了允许您处理任意大小的XML文档外，Mini-XML实现还允许您在内存中保留文档的部分以供以后处理。

mxmlLoadXxx函数支持通过设置回调函数和数据指针来启用SAX选项，使用[mxmlOptionsSetSAXCallback](@@)函数。回调函数接收您提供的数据指针、节点和事件代码，并返回`true`以继续处理或返回`false`以停止：

```c
bool
sax_cb(void *cbdata, mxml_node_t *node,
       mxml_sax_event_t event)
{
  ... 做一些操作 ...

  // 继续处理...
  return (true);
}
```

事件将是以下之一：

- `MXML_SAX_EVENT_CDATA`：刚刚读取了CDATA。
- `MXML_SAX_EVENT_COMMENT`：刚刚读取了注释。
- `MXML_SAX_EVENT_DATA`：刚刚读取了数据（整数、不透明、实数或文本）。
- `MXML_SAX_EVENT_DECLARATION`：刚刚读取了声明。
- `MXML_SAX_EVENT_DIRECTIVE`：刚刚读取了处理指令/指令。
- `MXML_SAX_EVENT_ELEMENT_CLOSE` - 刚刚读取了关闭元素（`</element>`）
- `MXML_SAX_EVENT_ELEMENT_OPEN` - 刚刚读取了打开元素（`<element>`）

在处理完关闭元素后，元素将被*释放*。其他所有节点在处理完后将被释放。SAX回调可以使用[mxmlRetain](@@)函数*保留*节点。例如，以下SAX回调将保留所有节点，有效地模拟了正常的内存加载：

```c
bool
sax_cb(void *cbdata, mxml_node_t *node, mxml_sax_event_t event)
{
  if (event != MXML_SAX_ELEMENT_CLOSE)
    mxmlRetain(node);

  return (true);
}
```

更典型的情况是，SAX回调只会保留文档的一小部分，以供后续处理。例如，以下SAX回调将保留XHTML文件中的标题和标题。它还保留（父）元素，如`<html>`，`<head>`和`<body>`，以及处理指令，如`<?xml ... ?>`和声明，如`<!DOCTYPE ... >`：

```c
bool
sax_cb(void *cbdata, mxml_node_t *node,
       mxml_sax_event_t event)
{
  if (event == MXML_SAX_ELEMENT_OPEN)
  {
   /*
    * 保留标题和标题...
    */

    const char *element = mxmlGetElement(node);

    if (!strcmp(element, "html") ||
        !strcmp(element, "head") ||
        !strcmp(element, "title") ||
        !strcmp(element, "body") ||
        !strcmp(element, "h1") ||
        !strcmp(element, "h2") ||
        !strcmp(element, "h3") ||
        !strcmp(element, "h4") ||
        !strcmp(element, "h5") ||
        !strcmp(element, "h6"))
      mxmlRetain(node);
  }
  else if (event == MXML_SAX_DECLARATION)
    mxmlRetain(node);
  else if (event == MXML_SAX_DIRECTIVE)
    mxmlRetain(node);
  else if (event == MXML_SAX_DATA)
  {
    if (mxmlGetRefCount(mxmlGetParent(node)) > 1)
    {
     /*
      * 如果父节点被保留，则也保留此数据节点。
      */

      mxmlRetain(node);
    }
  }

  return (true);
}
```

然后，可以像加载没有SAX回调函数的文档一样搜索生成的骨架文档树。例如，从stdin读取一个XHTML文档的过滤器，然后显示文档中的标题和标题：

```c
mxml_options_t *options;
mxml_node_t *xml, *title, *body, *heading;

options = mxmlOptionsNew();
mxmlOptionsSetSAXCallback(options, sax_cb,
                          /*cbdata*/NULL);

xml = mxmlLoadFd(/*top*/NULL, options, /*fd*/0);

title = mxmlFindElement(doc, doc, "title", NULL, NULL,
                        MXML_DESCEND_ALL);

if (title)
  print_children(title);

body = mxmlFindElement(doc, doc, "body", NULL, NULL,
                       MXML_DESCEND_ALL);

if (body)
{
  for (heading = mxmlGetFirstChild(body);
       heading;
       heading = mxmlGetNextSibling(heading))
    print_children(heading);
}

mxmlDelete(xml);
mxmlOptionsDelete(options);
```

`print_children`函数如下：

```c
void
print_children(mxml_node_t *parent)
{
  mxml_node_t *node;
  const char *text;
  bool whitespace;

  for (node = mxmlGetFirstChild(parent);
       node != NULL;
       node = mxmlGetNextSibling(node))
  {
    text = mxmlGetText(node, &whitespace);

    if (whitespace)
      putchar(' ');

    fputs(text, stdout);
  }

  putchar('\n');
}
```


用户数据
---------

每个节点都有一个关联的用户数据指针，可以用于存储应用程序的有用信息。数据指针使用的内存由Mini-XML*不*管理，因此您需要根据需要释放它。

[mxmlSetUserData](@@)函数设置与节点关联的任何用户（应用程序）数据，而[mxmlGetUserData](@@)函数获取与节点关联的任何用户（应用程序）数据：

```c
void *
mxmlGetUserData(mxml_node_t *node);

void
mxmlSetUserData(mxml_node_t *node, void *user_data);
```


内存管理
-----------------

节点支持引用计数以管理内存使用。[mxmlRetain](@@)和[mxmlRelease](@@)函数分别增加和减少节点的引用计数。当引用计数变为零时，[mxmlRelease](@@)调用[mxmlDelete](@@)来实际释放节点树使用的内存。新节点的引用计数从`1`开始。您可以使用[mxmlGetRefCount](@@)函数获取节点的当前引用计数。

字符串还可以支持不同类型的内存管理。默认情况下，使用标准C库的strdup和free函数。要使用其他机制，请调用[mxmlSetStringCallbacks](@@)函数设置字符串复制和释放回调。复制回调接收回调数据指针和要复制的字符串，并返回一个在XML数据的生命周期内将持续存在的新字符串。释放回调接收回调数据指针和复制的字符串，并可能释放用于它的内存。例如，以下代码实现了一个简单的字符串池，消除了重复字符串：

```c
typedef struct string_pool_s
{
  size_t num_strings;   // 字符串数量
  size_t alloc_strings; // 分配的字符串数量
  char   **strings;      // 字符串数组
} string_pool_t;

char *
copy_string(string_pool_t *pool, const char *s)
{
  size_t i;     // 循环变量
  char   *news; // 字符串的副本


  // 检查字符串是否已经在池中...
  for (i = 0; i < pool->num_strings; i ++)
  {
    if (!strcmp(pool->strings[i], s))
      return (pool->strings[i]);
  }

  // 不在池中，添加新字符串
  if (pool->num_strings >= pool->alloc_strings)
  {
    // 扩展字符串池...
    char **temp; // 新字符串数组

    temp = realloc(pool->strings,
                   (pool->alloc_strings + 32) *
                       sizeof(char *));

    if (temp == NULL)
      return (NULL);

    pool->alloc_strings += 32;
    pool->strings = temp;
  }

  if ((news = strdup(s)) != NULL)
    pool->strings[pool->num_strings ++] = news;

  return (news);
}

void
free_string(string_pool_t *pool, char *s)
{
  // 这里什么也不做...
}

void
free_all_strings(string_pool_t *pool)
{
  size_t i; // 循环变量


  for (i = 0; i < pool->num_strings; i ++)
    free(pool->strings[i]);
  free(pool->strings);
}

...

// 设置字符串池...
string_pool_t pool = { 0, 0, NULL };

mxmlSetStringCallbacks((mxml_strcopy_cb_t)copy_string,
                       (mxml_strfree_cb_t)free_string,
                       &pool);

// 加载XML文件...
mxml_node_t *xml;

xml = mxmlLoadFilename(/*top*/NULL, /*options*/NULL,
                       "example.xml");

// 处理XML文件...
...

// 释放XML文件使用的内存...
mxmlDelete(xml);

// 释放池中的所有字符串...
free_all_strings(&pool);
```


从Mini-XML v3.x迁移到v4.0时进行了以下不兼容的API更改：

- 现在使用`mxml_options_t`值来管理加载和保存回调函数和选项。
- 移除了mxmlSAXLoadXxx函数，而是在调用相应的mxmlLoadXxx函数之前设置`mxml_options_t`值的SAX回调函数和数据指针。
- SAX事件现在被命名为`MXML_SAX_EVENT_foo`，而不是`MXML_SAX_foo`。
- SAX回调现在返回布尔值。
- 节点类型现在被命名为`MXML_TYPE_foo`，而不是`MXML_foo`。
- 下降值现在被规范化为`MXML_DESCEND_ALL`、`MXML_DESCEND_FIRST`和`MXML_DESCEND_NONE`。
- 在成功时返回`0`、错误时返回`-1`的函数现在在成功时返回`true`，错误时返回`false`。
- CDATA节点（"`<![CDATA[...]]>`"）现在有自己的类型（`MXML_TYPE_CDATA`）。
- 注释节点（"`<!-- ... -->`"）现在有自己的类型（`MXML_TYPE_COMMENT`）。
- 声明节点（"`<!...>`"）现在有自己的类型（`MXML_TYPE_DECLARATION`）。
- 元素属性现在使用[mxmlElementClearAttr](@@)函数清除，而不是mxmlElementDeleteAttr。
- 处理指令/指示节点（"`<?...?>`"）现在有自己的类型（`MXML_TYPE_DIRECTIVE`）。
- 整数节点（`MXML_TYPE_INTEGER`）现在使用`long`类型。
- 文本节点（`MXML_TYPE_TEXT`）现在使用`bool`类型表示空白值。
- 现在使用[mxmlOptionsSetCustomCallbacks](@@)函数设置自定义节点回调，而不是线程全局的mxmlSetCustomHandlers函数。