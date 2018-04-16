#ifndef _cfg_common_h_
#define _cfg_common_h_

#include "cfg.h"


/***************************************************************************
* 配置一: 定义几个公共配置变量。
***************************************************************************/

//执行命令时，是否显示详细的命令信息。
#define CFG_CMD_HIT "CFG_CMD_HIT"

//配置文件配置项。
#define CFG_FILE "CFG_FILE"

//变量初始值。
#define CFG_CMD_HIT_DFLT    "1"
#define CFG_FILE_DFLT       "cfg/cfg.txt"

//配置初始化。
int Cfg_ComInit();

/***************************************************************************
* 描述: 一些工具宏。
***************************************************************************/

#define CFG_ITEM(pcNamem, pcDesc, eValType, pcDfltVal, bConst) {(char*)pcNamem, (char*)pcDesc, eValType, pcDfltVal, bConst},

#endif // _cfg_common_h_
