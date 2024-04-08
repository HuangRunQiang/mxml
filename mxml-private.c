// Mini-XML的私有函数，一个小型XML文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2003-2024 Michael R Sweet。
//
// 根据Apache License v2.0许可。有关更多信息，请参阅“LICENSE”文件。
//

#include "mxml-private.h"


//
// 一些疯狂的人认为卸载共享对象是一件好事或安全的事情。
// 不幸的是，大多数对象根本不安全，会发生糟糕的事情。
//
// 下面这段混乱的条件代码允许我们为Mini-XML提供一个析构函数，用于处理我们的线程全局存储，
// 以便可能安全地卸载它，尽管由于没有标准的方法来做到这一点，我甚至不能保证在所有平台上都可以安全地执行。
//
// 此代码目前支持AIX、HP-UX、Linux、macOS、Solaris和Windows。
// 它可能适用于BSD和IRIX，但我没有测试过。
//

// Mini-XML的私有函数，一个小型XML文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2003-2024 Michael R Sweet。
//
// 根据Apache License v2.0许可。有关更多信息，请参阅“LICENSE”文件。
//

#include "mxml-private.h"


//
// 一些疯狂的人认为卸载共享对象是一件好事或安全的事情。
// 不幸的是，大多数对象根本不安全，会发生糟糕的事情。
//
// 下面这段混乱的条件代码允许我们为Mini-XML提供一个析构函数，用于处理我们的线程全局存储，
// 以便可能安全地卸载它，尽管由于没有标准的方法来做到这一点，我甚至不能保证在所有平台上都可以安全地执行。
//
// 此代码目前支持AIX、HP-UX、Linux、macOS、Solaris和Windows。
// 它可能适用于BSD和IRIX，但我没有测试过。
//

// Mini-XML的私有函数，一个小型XML文件解析库。
//
// https://www.msweet.org/mxml
//
// 版权所有 © 2003-2024 Michael R Sweet。
//
// 根据Apache License v2.0许可。有关更多信息，请参阅“LICENSE”文件。
//

#include "mxml-private.h"


//
// 一些疯狂的人认为卸载共享对象是一件好事或安全的事情。
// 不幸的是，大多数对象根本不安全，会发生糟糕的事情。
//
// 下面这段混乱的条件代码允许我们为Mini-XML提供一个析构函数，用于处理我们的线程全局存储，
// 以便可能安全地卸载它，尽管由于没有标准的方法来做到这一点，我甚至不能保证在所有平台上都可以安全地执行。
//
// 此代码目前支持AIX、HP-UX、Linux、macOS、Solaris和Windows。
// 它可能适用于BSD和IRIX，但我没有测试过。
//

#if defined(__sun) || defined(_AIX)
#  pragma fini(_mxml_fini)
#  define _MXML_FINI _mxml_fini
#elif defined(__hpux)
#  pragma FINI _mxml_fini
#  define _MXML_FINI _mxml_fini
#elif defined(__GNUC__) // Linux and macOS
#  define _MXML_FINI __attribute((destructor)) _mxml_fini
#else
#  define _MXML_FINI _fini
#endif // __sun


// 'mxmlSetStringCallbacks()' - 设置字符串复制/释放的回调函数。
//
// 此函数为当前线程设置字符串复制/释放的回调函数。`strcopy_cb`函数用于复制提供的字符串，而`strfree_cb`函数用于释放复制的字符串。
// 每个回调函数都接受`str_cbdata`指针和字符串指针作为参数：
//
// ```c
// char *my_strcopy_cb(void *cbdata, const char *s)
// {
//   ... 复制字符串 "s" ...
// }
//
// void my_strfree_cb(void *cbdata, char *s)
// {
//   ... 释放字符串 "s" 使用的内存 ...
// }
// ```
//
// 默认的`strcopy_cb`函数调用`strdup`，而默认的`strfree_cb`函数调用`free`。

void
mxmlSetStringCallbacks(
    mxml_strcopy_cb_t strcopy_cb,	// 输入 - 字符串复制回调函数
    mxml_strfree_cb_t strfree_cb,	// 输入 - 字符串释放回调函数
    void              *str_cbdata)	// 输入 - 字符串回调数据
{
  _mxml_global_t *global = _mxml_global();
					// 全局数据


  global->strcopy_cb = strcopy_cb;
  global->strfree_cb = strfree_cb;
  global->str_cbdata = str_cbdata;
}


//
// '_mxml_strcopy()' - 复制字符串。
//

char *					// 输出 - 字符串的副本
_mxml_strcopy(const char *s)		// 输入 - 字符串
{
  _mxml_global_t *global = _mxml_global();
					// 全局数据


  if (!s)
    return (NULL);

  if (global->strcopy_cb)
    return ((global->strcopy_cb)(global->str_cbdata, s));
  else
    return (strdup(s));
}

// '_mxml_strfree()' - 释放字符串。
//

void
_mxml_strfree(char *s)			// 输入 - 字符串
{
  _mxml_global_t *global = _mxml_global();
					// 全局数据


  if (!s)
    return;

  if (global->strfree_cb)
    (global->strfree_cb)(global->str_cbdata, s);
  else
    free((void *)s);
}


#ifdef HAVE_PTHREAD_H			// POSIX 线程
#  include <pthread.h>

static int		_mxml_initialized = 0;
					// 是否已初始化
static pthread_key_t	_mxml_key;	// 线程局部存储键
static pthread_once_t	_mxml_key_once = PTHREAD_ONCE_INIT;
					// 一次性初始化对象
static void		_mxml_init(void);
static void		_mxml_destructor(void *g);


//
// '_mxml_destructor()' - 释放全局数据使用的内存...
//

static void
_mxml_destructor(void *g)		// 输入 - 全局数据
{
  free(g);
}


//
// '_mxml_fini()' - 卸载时进行清理。
//

static void
_MXML_FINI(void)
{
  if (_mxml_initialized)
    pthread_key_delete(_mxml_key);
}


//
// '_mxml_global()' - 获取全局数据。
//

_mxml_global_t *			// 输出 - 全局数据
_mxml_global(void)
{
  _mxml_global_t	*global;	// 全局数据


  pthread_once(&_mxml_key_once, _mxml_init);

  if ((global = (_mxml_global_t *)pthread_getspecific(_mxml_key)) == NULL)
  {
    global = (_mxml_global_t *)calloc(1, sizeof(_mxml_global_t));
    pthread_setspecific(_mxml_key, global);
  }

  return (global);
}


//
// '_mxml_init()' - 初始化全局数据...
//

static void
_mxml_init(void)
{
  _mxml_initialized = 1;
  pthread_key_create(&_mxml_key, _mxml_destructor);
}


#elif defined(_WIN32) && defined(MXML1_EXPORTS) // WIN32 线程
#  include <windows.h>

static DWORD _mxml_tls_index;		// 全局存储的索引


// 'DllMain()' - 库的主要入口点。
//

BOOL WINAPI				// 输出 - 成功/失败
DllMain(HINSTANCE hinst,		// 输入 - DLL 模块句柄
        DWORD     reason,		// 输入 - 原因
        LPVOID    reserved)		// 输入 - 未使用
{
  _mxml_global_t	*global;	// 全局数据


  (void)hinst;
  (void)reserved;

  switch (reason)
  {
    case DLL_PROCESS_ATTACH :		// 在库初始化时调用
        if ((_mxml_tls_index = TlsAlloc()) == TLS_OUT_OF_INDEXES)
          return (FALSE);
        break;

    case DLL_THREAD_DETACH :		// 在线程终止时调用
        if ((global = (_mxml_global_t *)TlsGetValue(_mxml_tls_index)) != NULL)
          free(global);
        break;

    case DLL_PROCESS_DETACH :		// 在卸载库时调用
        if ((global = (_mxml_global_t *)TlsGetValue(_mxml_tls_index)) != NULL)
          free(global);

        TlsFree(_mxml_tls_index);
        break;

    default:
        break;
  }

  return (TRUE);
}


//
// '_mxml_global()' - 获取全局数据。
//

_mxml_global_t *			// 输出 - 全局数据
_mxml_global(void)
{
  _mxml_global_t	*global;	// 全局数据


  if ((global = (_mxml_global_t *)TlsGetValue(_mxml_tls_index)) == NULL)
  {
    global = (_mxml_global_t *)calloc(1, sizeof(_mxml_global_t));

    TlsSetValue(_mxml_tls_index, (LPVOID)global);
  }

  return (global);
}


#else					// 无线程支持
//
// '_mxml_global()' - 获取全局数据。
//

_mxml_global_t *			// 输出 - 全局数据
_mxml_global(void)
{
  static _mxml_global_t	global =	// 全局数据
  {
    NULL,				// strcopy_cb
    NULL,				// strfree_cb
    NULL,				// str_cbdata
  };


  return (&global);
}
#endif // HAVE_PTHREAD_H
