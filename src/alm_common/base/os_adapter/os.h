/**************************************************************************
    filename :  alm_common\base\os_adapter\os.h
    file path:  alm_common\base\os_adapter
    file base:  os
    file ext :  h
    purpose  :  适配不同系统函数的对外头文件。
***************************************************************************/

#ifndef _os_h_
#define _os_h_

#include "def.h"
#include "util.h"

/***************************************************************************
* 描述: 编译选择操作系统。
***************************************************************************/
#define LIC_WINDOWS 1
#define LIC_LINUX   2

#if defined(_WIN32)
#define LIC_OS LIC_WINDOWS

#elif defined(LINUX)
#define LIC_OS LIC_LINUX
#else
#error 不支持的操作系统。
#endif

/***************************************************************************
* 描述: 一些简单的系统函数定义。
***************************************************************************/
#if (LIC_OS == LIC_WINDOWS)
#include <stdlib.h>
#include <direct.h>
//#include <vld.h>
#include <io.h>

#define putenv _putenv
#define snprintf _snprintf
#define getcwd _getcwd
#define access _access
#define sleep Sleep

#else
#include <unistd.h>
//#include <dir.h>

#endif

/***************************************************************************
* 描述: OS子组件。
***************************************************************************/
#include "os_memory.h"
#include "os_date.h"
#include "os_mutex.h"
#include "os_timer.h"
#include "os_thread.h"

/***************************************************************************
* 描述: OS启动。
***************************************************************************/
int OS_Start();

/***************************************************************************
* 描述: OS关闭。
***************************************************************************/
int OS_Close();

#endif // _os_h_
