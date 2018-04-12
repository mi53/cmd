#ifndef _cmdrun_inc_
#define _cmdrun_inc_


#include "cmd.h"
#include "util.h"
#include "cfg.h"  //完成配置初始化。
#include "cmddflt.h"
#include "cfg_common.h"

/***************************************************************************
* 外部依赖声明
***************************************************************************/
extern CmdFnSt g_astDfltCmd[];

/***************************************************************************
* 变量定义。
***************************************************************************/
static bool s_bHaveInit = false;
static bool s_bRunContinue = true;//执行单个命令后是否退出命令系统。
//static CmdFnSt** s_apstCmds = NULL;

static CmdFnSt* s_astCmds = NULL;

/***************************************************************************
* 内部函数声明。
***************************************************************************/
void _CmdM_InitCmds(CmdFnSt* pstCmds);

#define MAX_CMD_LEN 1024

#endif // _cmdrun_inc_
