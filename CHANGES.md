Mini-XML 4.0.2变更内容

- 修复了GNU make和并行构建的问题（Issue #314）


Mini-XML 4.0.1变更内容

- 在makefile中修复了"docdir"定义缺失的问题（Issue #313）
- 在makefile的CFLAGS中修复了CPPFLAGS、OPTIM和WARNINGS缺失的问题。
- 修复了configure脚本的问题。


Mini-XML 4.0.0变更内容

- 现在需要C99支持（Issue #300）
- 现在安装为"libmxml4"，以支持同时安装Mini-XML 3.x和4.x（使用`--disable-libmxml4-prefix`配置选项来禁用）
- 添加了`mxmlLoadIO`和`mxmlSaveIO`函数，通过回调函数加载和保存XML（Issue #98）
- 添加了新的`MXML_TYPE_CDATA`、`MXML_TYPE_COMMENT`、`MXML_TYPE_DECLARATION`和`MXML_TYPE_DIRECTIVE`节点类型（Issue #250）
- 添加了`mxmlLoadFilename`和`mxmlSaveFilename`函数（Issue #291）
- 添加了AFL模糊测试支持（Issue #306）
- 添加了`mxmlOptions` API，用于替代每个加载和保存函数的长列表回调函数和选项（Issue #312）
- 添加了字符串复制/释放回调函数，以支持对字符串的替代内存管理。
- 将`mxml_type_t`枚举重命名为`MXML_TYPE_xxx`（Issue #251）
- 更新API，使用bool类型代替表示布尔值的int。
- 更新SAX回调函数，返回一个bool值以控制处理（Issue #51）
- 更新加载和保存回调函数，包括上下文指针（Issue #106）
- 修复了一些警告（Issue #301）
- 修复了非英语环境中实数支持的问题（Issue #311）


Mini-XML 3.3.2变更内容

- 更新了autoconf的`config.guess`和`config.sub`脚本，以支持新平台的交叉编译（Issue #296）


Mini-XML 3.3.1变更内容

- 修复了POSIX线程清理错误（Issue #293）


Mini-XML 3.3变更内容

- 清理了整个库中的`free`用法（Issue #276）
- 在库中添加了更多的错误处理（Issue #277）
- 修复了`mxmlLoad*`函数中潜在的内存泄漏问题（Issue #278，Issue #279）
- 修复了`mxmlSaveString`在缓冲区大小为0时的问题（Issue #284）
- 修复了"mxml.h"中的`MXML_MINOR_VERSION`值（Issue #285）
- 修复了MingW下的POSIX线程支持（Issue #287）
- 修复了Coverity发现的一些轻微内存泄漏问题。


Mini-XML 3.2变更内容

- 在Haiku上添加了对共享库的支持（Issue #262）
- 修复了处理以Unicode字符开头的未引用属性值的问题（Issue #264）
- 修复了以Unicode字符开头的元素处理问题（Issue #267）
- 修复了LGTM安全扫描器发现的一些次要问题。


Mini-XML 3.1变更内容

- `mxmlLoad*`函数现在在XML不以`<`开头且没有提供父节点时打印错误（Issue #256，Issue #259）
- 修复了"make install"尝试安装旧文件的问题（Issue #257）
- 修复了Linux上的一些DSO安装问题。


Mini-XML 3.0变更内容

- 将许可证更改为Apache 2.0（带有例外）（Issue #239）
- 所有内部节点结构现在已移出公共头文件（Issue #240）
- 修复了写入浮点数据时潜在的缓冲区溢出问题（Issue #233）
- 将`mxmldoc`移至新的`codedoc`项目，其重点是生成代码文档（Issue #235，Issue #236，Issue #237）
- 错误消息现在包括错误的行号（Issue #230）
- `mxmlSetCDATA`、`mxmlSetElement`、`mxmlSetOpaque`、`mxmlSetOpaquef`、`mxmlSetText`和`mxmlSetTextf`函数在值来自同一节点时会导致使用后释放的错误（Issue #241）
- `mxmlSetOpaquef`和`mxmlSetTextf`函数不工作（Issue #244）
- `_mxml_strdupf`函数在Windows上不工作（Issue #245）


Mini-XML 2.12变更内容

- 添加了关于在需要获取内联文本的完整字符串时使用`MXML_OPAQUE_CALLBACK`的更多文档（Issue #190）
- 在macOS上不再构建文档集，因为Xcode不再支持它们（Issue #198）
- 更新了与BCC一起使用的`va_copy`宏（Issue #211）
- `mxmlNewCDATA`和`mxmlSetCDATA`函数不正确地将XML尾部" ]]"添加到字符串中（Issue #216）
- 在安装过程中交叉编译失败（Issue #218）
- 修复了`mxmlWrite`函数中的崩溃错误（Issue #228）
- `mxmlWrite`函数不再写入传递节点的所有兄弟节点，只写入该节点及其子节点（Issue #228）
- 更新了mxmldoc使用的markdown和ZIP容器库。


Mini-XML 2.11变更内容

- 为方便起见，CDATA节点现在省略尾部的" ]]"（Issue #170）
- 修复了`mxmlDelete`中的内存泄漏（Issue #183）
- `mxmlElementSetAttrf`在某些Visual Studio版本中无法工作（Issue #184）
- 添加了`mxmlElementGetAttrByIndex`和`mxmlELementGetAttrCount`函数（Issue #185）
- configure脚本现在正确支持交叉编译（Issue #188）
- mxmldoc工具现在支持生成EPUB文件（Issue #189）
- mxmldoc工具现在支持`SOURCE_DATE_EPOCH`环境变量用于可重复构建（Issue #193）
- mxmldoc工具现在支持Markdown（Issue #194）
- 修复了自定义数据值的写入问题（Issue #201）
- 添加了`mxmlNewOpaquef`和`mxmlSetOpaquef`函数以添加和设置格式化的不透明字符串值。
- mxmldoc工具以不同方式扫描和加载描述性文本，导致生成的文档中丢失了详细描述（"discussion"）。
- mxmldoc工具现在支持`@exclude format@`注释，根据输出格式排除文档。格式字符串可以是`all`以排除所有格式的文档，或者是逗号分隔的列表，例如`@exclude man,html@`。


Mini-XML 2.10变更内容

- mxml.h中的版本号有误。
- mxml.spec文件已过时。
- Mini-XML不再允许格式错误的元素名称。
- 当指定了MXML_TEXT_CALLBACK时，`mxmlLoad*`和`mxmlSAXLoad*`在创建文本节点时没有正确工作。
- `mxmlDelete`使用了递归算法，根据文件的大小可能需要大量的堆栈空间。（CVE-2016-4570）
- `mxmlWrite*`使用了递归算法，根据文件的大小可能需要大量的堆栈空间。（CVE-2016-4571）


Mini-XML 2.9变更内容

- `mxmlLoad*`在使用`MXML_NO_CALLBACK`或`MXML_TEXT_CALLBACK`加载值节点时没有正确工作。


Mini-XML 2.8变更内容

- 现在在macOS上使用xcrun调用docsetutil。
- mxmldoc在处理@code foo@注释中的特殊HTML字符时未正确转义。
- 在mxmlElementDeleteAttr中修复了内存泄漏问题。
- 在mxml.h中添加了MXML_MAJOR/MINOR_VERSION定义。
- 修复了从文件读取UTF-16字符的错误。
- 在加载无效XML时修复了内存泄漏问题。
- 修复了在mxmldoc中加载XML片段的问题。


Mini-XML 2.7变更内容

- 在VC++项目文件中添加了64位配置。
- 修复了mxmldoc的HTML和CSS输出的一致性。
- 为Mini-XML头文件中的`mxml_node_t`和`mxml_index_t`结构体添加了私有性，但仍可在Mini-XML头文件中使用，以保持源代码兼容性。
- 更新了源文件头部，引用了Mini-XML许可证及其对LGPL2的例外。
- 在加载格式错误的XML文件时修复了内存泄漏问题。
- 添加了新的mxmlFindPath函数，用于查找具有指定元素名称的值节点。
- 在Windows上构建静态版本的库无法正常工作。
- 共享库在UNIX类操作系统上未包含线程特定数据键的析构函数。
- mxmlLoad*在XML中包含多个根节点时未报错。
- 修复了_mxml_vstrdupf函数中的问题。
- `mxmlSave*`不再写入传递节点的所有兄弟节点，只写入该节点及其子节点。


Mini-XML 2.6变更内容

- 文档修复。
- mxmldoc程序未正确处理typedef注释。
- 添加了对"long long" printf格式的支持。
- XML解析器现在忽略UTF-8 XML文件中的BOM。
- mxmldoc程序现在支持生成Xcode文档集。
- `mxmlSave*`在某些平台上未正确输出UTF-8。
- `mxmlNewXML`现在在?xml指令中添加encoding="utf-8"，以避免与不符合规范的XML解析器产生问题，这些解析器假设默认编码不是UTF-8。
- 当调用mxmlSetWrapMargin(0)时，不再禁用换行，并且"<?xml ... ?>"之后总是跟着一个换行符。
- mxml.pc.in文件有问题。
- mxmldoc程序现在正确处理"typedef enum name {} name"。


Mini-XML 2.5变更内容

- mxmldoc程序现在更多地使用CSS，并支持`--css`选项以嵌入替代样式表。
- mxmldoc程序现在支持`--header`和`--footer`选项，在生成的内容之前和之后插入文档内容。
- mxmldoc程序现在支持`--framed`选项以生成带框架的HTML输出。
- mxmldoc程序现在在生成HTML输出时创建包含`--intro`文件中的所有标题的目录。
- man页和mxmldoc生成的man页输出不再使用"\-"表示破折号。
- Mini-XML DLL的调试版本无法构建。
- 当不在文档的顶层时，处理指令和指示时出现问题。
- 不支持属性周围的空格。


Mini-XML 2.4变更内容

- 修复了在HP-UX和Mac macOS上构建共享库的问题。
- mxmldoc程序未正确输出函数的参数描述。
- 所有全局设置（自定义、错误和实体回调以及换行边距）现在针对每个线程单独管理。
- 添加了`mxmlElementDeleteAttr`函数。
- `mxmlElementSetAttrf`未正常工作。
- `mxmlLoad*`错误地将声明视为父元素。
- `mxmlLoad*`错误地允许没有值的属性。
- 修复了Visual C++构建问题。
- `mxmlLoad*`在元素包含错误时未返回NULL。
- 添加了对apos字符实体的支持。
- 修复了对Unicode字符的空白检测。
- 当顶部节点没有子节点时，`mxmlWalkNext`和`mxmlWalkPrev`无法正确工作。


Mini-XML 2.3变更内容

- 添加了两个LGPL的例外，以支持应用程序静态链接Mini-XML。
- mxmldoc工具现在也可以生成man页。
- 添加了mxmlNewXML函数。
- 添加了mxmlElementSetAttrf函数。
- 为测试程序添加了snprintf()仿真函数。
- 在VC++ 2005上构建时添加了_CRT_SECURE_NO_DEPRECATE定义。
- `mxmlLoad*`未正确加载XML流中包含非法控制字符的情况。
- `mxmlLoad*`未正确处理XML元素中具有相同名称的两个属性，与XML规范一致。
- 添加了对CDATA的支持。
- 更新了注释和处理指令处理-根据XML规范，不支持实体。
- 添加了对无效注释终止的检查："--->"是不允许的。


Mini-XML 2.1变更内容

- 添加了对自定义数据节点的支持。
- 现在将超过必要长度的UTF-8序列视为错误。
- 修复了实体编号支持。
- 修复了mxmlLoadString()在UTF-8中的错误。
- 修复了实体查找错误。
- 添加了`mxmlLoadFd`和`mxmlSaveFd`函数。
- 修复了多个字的UTF-16处理。


Mini-XML 2.0变更内容

- 新的程序员手册。
- 为Microsoft Windows用户添加了Visual C++项目文件。
- 对mxmldoc、`mxmlSaveFile`和`mxmlIndexNew`进行了优化。
- `mxmlEntityAddCallback`现在返回一个整数状态。
- 添加了UTF-16支持（仅输入；所有输出均为UTF-8）。
- 添加了用于构建可搜索XML节点索引的索引函数。
- 添加了字符实体回调接口，以支持除XHTML规范中定义的字符实体之外的其他字符实体。
- 添加了对XHTML字符实体的支持。
- mxmldoc工具现在生成符合更新的XML模式的XML输出，该模式在文件"doc/mxmldoc.xsd"中描述。
- 将空白回调接口更改为返回字符串而不是单个字符，以便更好地控制使用Mini-XML编写的XML文件的格式。如果使用了空白回调，这个改变将需要对1.x代码进行更改。
- mxmldoc工具现在能够记录C++类、函数和结构，并正确处理C++注释。
- 为mxmldoc添加了新的模块化测试。
- 更新了mxmldoc输出，使其更适用于在使用HTMLDOC生成的手册中嵌入。
- makefile在目标路径和安装路径之间错误地包含了"/"分隔符，这在使用MingW构建和安装时会导致问题。