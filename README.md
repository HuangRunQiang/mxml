Mini-XML（迷你XML）- 轻量级XML解析库 v4
======================================

![版本](https://img.shields.io/github/v/release/michaelrsweet/mxml?include_prereleases)
![Apache 2.0](https://img.shields.io/github/license/michaelrsweet/mxml)
![构建](https://github.com/michaelrsweet/mxml/workflows/Build/badge.svg)
[![Coverity扫描状态](https://img.shields.io/coverity/scan/23959.svg)](https://scan.coverity.com/projects/michaelrsweet-mxml)


Mini-XML是一个小型的XML解析库，您可以在应用程序中使用它来读取XML数据文件或字符串，而无需依赖大型的非标准库。Mini-XML只需要一个"make"程序和一个符合C99标准的编译器 - GCC可以使用，大多数厂商的C编译器也可以使用。

Mini-XML提供以下功能：

- 支持读取UTF-8和UTF-16编码的XML文件和字符串，并支持写入UTF-8编码的XML文件和字符串。
- 数据以链接列表树结构存储，保留XML数据的层次结构。
- 支持流式（SAX）读取XML文件和字符串，以最小化内存使用。
- 支持任意元素名称、属性和属性值，没有预设限制，只受可用内存限制。
- 在"叶子"节点中支持整数、实数、不透明数据、文本和自定义数据类型。
- 提供创建和管理数据树的函数。
- 提供用于查找和遍历数据树的"查找"和"遍历"函数。
- 支持自定义字符串内存管理函数，以实现字符串池和其他减少内存使用的方案。

Mini-XML不进行基于模式文件或其他定义信息源的验证或其他类型的数据处理。

构建Mini-XML
-----------------

Mini-XML附带了一个基于autoconf的配置脚本；只需键入以下命令即可开始：

    ./configure

默认的安装前缀是`/usr/local`，可以使用`--prefix`选项进行覆盖：

    ./configure --prefix=/foo

其他配置选项可以使用`--help`选项查看：

    ./configure --help

配置软件后，键入`make`进行构建，并运行测试程序以验证是否正常工作，如下所示：

    make

如果您在Microsoft Windows下使用Visual C++使用Mini-XML，请使用`vcnet`子目录中的项目文件来构建库。注意：Windows上的静态库不是线程安全的。

安装Mini-XML
-------------------

`install`目标将Mini-XML安装在lib和include目录中：

    sudo make install

安装完成后，使用`-lmxml`选项将应用程序与Mini-XML进行链接。

文档
-------------

文档位于`doc`子目录中的`mxml.html`（HTML）和`mxml.epub`（EPUB）文件中。您还可以查看`testmxml.c`源文件，其中包含使用Mini-XML的示例。

Mini-XML提供了一个单独的头文件，您可以包含它：

    #include <mxml.h>

节点（元素、注释、声明、整数、不透明字符串、处理指令、实数和文本字符串）由`mxml_node_t`指针表示。可以使用mxmlNewXxx函数创建新节点。顶级节点必须是`<?xml ...?>`处理指令。

您可以使用mxmlLoadFilename函数加载XML文件：

    mxml_node_t *tree;

    tree = mxmlLoadFilename(/*top*/NULL, /*options*/NULL,
                            "example.xml");

类似地，您可以使用mxmlSaveFilename函数保存XML文件：

    mxml_node_t *tree;

    mxmlSaveFilename(tree, /*options*/NULL,
                     "filename.xml");

这些函数还有其他的变体，用于从文件描述符、`FILE`指针、字符串和IO回调进行加载或保存。

您可以使用mxmlFindElement函数查找具有特定名称的元素/节点：

    mxml_node_t *node = mxmlFindElement(tree, tree, "name", "attr",
					"value", MXML_DESCEND_ALL);

`name`、`attr`和`value`参数可以传递`NULL`作为通配符，例如：

    /* 查找第一个 "a" 元素 */
    node = mxmlFindElement(tree, tree, "a", NULL, NULL, MXML_DESCEND_ALL);

    /* 查找第一个带有 "href" 属性的 "a" 元素 */
    node = mxmlFindElement(tree, tree, "a", "href", NULL, MXML_DESCEND_ALL);

    /* 查找第一个带有 "href" 属性且链接到URL的 "a" 元素 */
    node = mxmlFindElement(tree, tree, "a", "href",
                           "https://www.msweet.org/mxml", MXML_DESCEND_ALL);

    /* 查找第一个带有 "src" 属性的元素 */
    node = mxmlFindElement(tree, tree, NULL, "src", NULL, MXML_DESCEND_ALL);

    /* 查找第一个带有 "src" = "foo.jpg" 的元素 */
    node = mxmlFindElement(tree, tree, NULL, "src", "foo.jpg",
                           MXML_DESCEND_ALL);

您还可以使用相同的函数进行迭代：

    mxml_node_t *node;

    for (node = mxmlFindElement(tree, tree, "name", NULL, NULL,
				MXML_DESCEND_ALL);
	 node != NULL;
	 node = mxmlFindElement(node, tree, "name", NULL, NULL,
				MXML_DESCEND_ALL))
    {
      ... 做一些操作 ...
    }

mxmlFindPath函数使用XPath在特定元素下找到（第一个）值节点：

    mxml_node_t *value = mxmlFindPath(tree, "path/to/*/foo/bar");

mxmlGetInteger、mxmlGetOpaque、mxmlGetReal和mxmlGetText函数从节点中检索相应的值：

    mxml_node_t *node;

    int intvalue = mxmlGetInteger(node);

    const char *opaquevalue = mxmlGetOpaque(node);

    double realvalue = mxmlGetReal(node);

    bool whitespacevalue;
    const char *textvalue = mxmlGetText(node, &whitespacevalue);

最后，当您完成XML数据时，使用mxmlDelete函数递归释放用于特定节点或整个树的内存：

    mxmlDelete(tree);


获取帮助和报告问题
-----------------------------------

[Mini-XML项目页面](https://www.msweet.org/mxml)提供了访问当前版本的软件、文档和Github问题跟踪页面的方式。


法律事务
-----------

版权所有 © 2003-2024 Michael R Sweet

Mini-XML库根据Apache许可证版本2.0进行许可，其中包含一个*可选*的例外，允许与仅限于GPL2/LGPL2的软件进行链接。有关更多信息，请参阅文件"LICENSE"和"NOTICE"。

> 注意：NOTICE文件中列出的例外仅适用于与GPL2/LGPL2软件进行链接的情况。一些Apache许可证纯粹主义者反对将带有这些例外的Apache许可证代码与Mini-XML进行链接，理由是这使得Mini-XML在某种程度上与Apache许可证不兼容。因此，希望保持Apache许可证纯粹性的人可以在他们的Mini-XML副本中省略这个例外。
>
> 注意2：我不是律师，但我开始讨厌他们了！
