//
// Private definitions for Mini-XML, a small XML file parsing library.
//
// https://www.msweet.org/mxml
//
// Copyright © 2003-2024 by Michael R Sweet.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

#ifndef MXML_PRIVATE_H
#  define MXML_PRIVATE_H
#  include "config.h"
#  include "mxml.h"
#  include <locale.h>


以下是给定代码的汉化版本：

//
// 私有宏...
//

#  ifdef DEBUG
#    define MXML_DEBUG(...)	fprintf(stderr, __VA_ARGS__)
#  else
#    define MXML_DEBUG(...)
#  endif // DEBUG
#  define MXML_TAB		8	// 每N列的制表符


//
// 私有结构...
//

typedef struct _mxml_attr_s		// XML元素属性值
{
  char			*name;		// 属性名称
  char			*value;		// 属性值
} _mxml_attr_t;

typedef struct _mxml_element_s		// XML元素值
{
  char			*name;		// 元素名称
  size_t		num_attrs;	// 属性数量
  _mxml_attr_t		*attrs;		// 属性
} _mxml_element_t;

typedef struct _mxml_text_s		// XML文本值
{
  bool			whitespace;	// 是否包含前导空白字符
  char			*string;	// 片段字符串
} _mxml_text_t;

typedef struct _mxml_custom_s		// XML自定义值
{
  void			*data;		// 指向（已分配的）自定义数据的指针
  mxml_custfree_cb_t	free_cb;	// 释放回调函数
  void			*free_cbdata;	// 释放回调数据
} _mxml_custom_t;

typedef union _mxml_value_u // XML节点值
{
char *cdata; // CDATA字符串
char *comment; // 注释字符串
char *declaration; // 声明字符串
char *directive; // 处理指令字符串
_mxml_element_t element; // 元素
long integer; // 整数
char *opaque; // 不透明字符串
double real; // 实数
_mxml_text_t text; // 文本片段
_mxml_custom_t custom; // 自定义数据
} _mxml_value_t;

struct _mxml_node_s // XML节点
{
mxml_type_t type; // 节点类型
struct _mxml_node_s *next; // 同一父节点下的下一个节点
struct _mxml_node_s *prev; // 同一父节点下的上一个节点
struct _mxml_node_s *parent; // 父节点
struct _mxml_node_s *child; // 第一个子节点
struct _mxml_node_s *last_child; // 最后一个子节点
_mxml_value_t value; // 节点值
size_t ref_count; // 使用计数
void *user_data; // 用户数据
};

typedef struct _mxml_global_s // 全局的、每个线程的数据
{
mxml_strcopy_cb_t strcopy_cb; // 字符串拷贝回调函数
mxml_strfree_cb_t strfree_cb; // 字符串释放回调函数
void *str_cbdata; // 字符串回调数据
} _mxml_global_t;

struct _mxml_index_s // XML节点索引
{
char *attr; // 用于索引的属性或NULL
size_t num_nodes; // 索引中的节点数量
size_t alloc_nodes; // 索引中分配的节点数量
size_t cur_node; // 当前节点
mxml_node_t **nodes; // 节点数组
};

struct _mxml_options_s // XML选项
{
struct lconv *loc; // 本地化数据
size_t loc_declen; // 小数点字符串的长度
mxml_custload_cb_t custload_cb; // 自定义加载回调函数
mxml_custsave_cb_t custsave_cb; // 自定义保存回调函数
void *cust_cbdata; // 自定义回调数据
mxml_entity_cb_t entity_cb; // 实体回调函数
void *entity_cbdata; // 实体回调数据
mxml_error_cb_t error_cb; // 错误回调函数
void *error_cbdata; // 错误回调数据
mxml_sax_cb_t sax_cb; // SAX回调函数
void *sax_cbdata; // SAX回调数据
mxml_type_cb_t type_cb; // 类型回调函数
void *type_cbdata; // 类型回调数据
mxml_type_t type_value; // 固定类型值（如果没有类型回调）
int wrap; // 换行边距
mxml_ws_cb_t ws_cb; // 空白字符回调函数
void *ws_cbdata; // 空白字符回调数据
};


//
// Private functions...
//

/**
 * @brief 获取全局变量 _mxml_global_t 的指针
 *
 * @return 返回 _mxml_global_t 的指针
 */
extern _mxml_global_t *_mxml_global(void);

/**
 * @brief 获取实体字符的字符串表示
 *
 * @param ch 实体字符的值
 * @return 返回实体字符的字符串表示
 */
extern const char *_mxml_entity_string(int ch);

/**
 * @brief 获取实体字符的值
 *
 * @param options mxml_options_t 结构体指针
 * @param name 实体字符的名称
 * @return 返回实体字符的值
 */
extern int _mxml_entity_value(mxml_options_t *options, const char *name);

/**
 * @brief 输出 mxml 错误信息
 *
 * @param options mxml_options_t 结构体指针
 * @param format 格式化字符串
 * @param ... 可变参数
 */
extern void _mxml_error(mxml_options_t *options, const char *format, ...) MXML_FORMAT(2, 3);

/**
 * @brief 复制字符串
 *
 * @param s 要复制的字符串
 * @return 返回复制后的字符串
 */
extern char *_mxml_strcopy(const char *s);

/**
 * @brief 释放字符串内存
 *
 * @param s 要释放的字符串
 */
extern void _mxml_strfree(char *s);


#endif // !MXML_PRIVATE_H
