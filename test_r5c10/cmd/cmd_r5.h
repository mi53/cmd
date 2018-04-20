#ifndef _cmd_r5_h_
#define _cmd_r5_h_


#include "alm_cmd.h"

/***************************************************************************
* 描述: 初始化用户命令。
***************************************************************************/
void CmdM_R5Init();
void CmdM_R5Cls();


/***************************************************************************
* 定义R5C10相关命令。
***************************************************************************/



// 简单工具
#define CMD_G100_TBL_SHOW                        TblShow
#define CMD_G100_MK_BBOM                         Mkbbom
#define CMD_G100_PS_RM                           PsRm
#define CMD_G100_Temp                            Temp
#define CMD_G100_RESET                           reset
#define CMD_G100_PRD_CFG_ADD                     PrdCfgAdd


#endif // _cmd_r5_h_
