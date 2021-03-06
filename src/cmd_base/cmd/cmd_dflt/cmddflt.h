#ifndef _cmddflt_h_
#define _cmddflt_h_

#include "cmd.h"
#include "cmdfn.h"

/***************************************************************************
* 描述: 默认命令存储单元。
***************************************************************************/
extern CmdFnSt g_astDfltCmd[];

/***************************************************************************
* 描述: 默认命令 - help. help命令为预订命令。
***************************************************************************/
int Cmd_Help(ParasSt* pPara, void* pData);
#define CMD_HELP "Help"

/***************************************************************************
* 描述: 默认命令 - quit. quit命令为预订命令。
***************************************************************************/
int Cmd_Exit(ParasSt* pPara, void* pData);

/***************************************************************************
* 描述: 执行脚本。
***************************************************************************/
int Cmd_ExecFile(ParasSt* pPara, void* pData);

/***************************************************************************
* 描述: 执行系统命令。最多支持8个参数或选项。
***************************************************************************/
int Cmd_Cmd(ParasSt* pstPara, void* pData);
#define CMD_CMD_MAX_WORD_NUM 9

/***************************************************************************
* 描述: 执行子命令。
***************************************************************************/
int Cmd_RunCmd(ParasSt* pstPara, void* pData);
#define CMD_RUN_SUB "Run"

/***************************************************************************
* 描述: 执行测试用例。
***************************************************************************/
int Cmd_Test(ParasSt* pstPara, void* pData);

/***************************************************************************
* 描述: 通过命令执行函数。该函数需要在文件cmdfn.inc中添加。
***************************************************************************/
int Cmd_RunFn(ParasSt* pstPara, void* pData);

/***************************************************************************
* 描述: 配置相关操作。
***************************************************************************/
int Cmd_CfgAdd(ParasSt* pstPara, void* pData);
int Cmd_CfgShow(ParasSt* pstPara, void* pData);
int Cmd_CfgSet(ParasSt* pstPara, void* pData);
int Cmd_CfgRecover(ParasSt* pstPara, void* pData);


#endif // _cmddflt_h_
