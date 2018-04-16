#include "cfg_common.h"
#include "util.h"
#include "cmd.h"
#include <stdlib.h>


/***************************************************************************
* 描述: 函数定义。
***************************************************************************/
int Cfg_ComInit()
{
    CfgItemDescSt astItems[] =
    {
        CFG_ITEM(CFG_CMD_HIT, "执行命令时，是否显示详细的命令信息", TYPE_INT, CFG_CMD_HIT_DFLT, false)
        CFG_ITEM(CFG_FILE, "配置文件", TYPE_STR, CFG_FILE_DFLT, true)
    };
    int iItemNum = sizeof(astItems) / sizeof(CfgItemDescSt);
    int iRet;

    //添加配置项。
    iRet = Cfg_AddItems(astItems, iItemNum);

    if (iRet != iItemNum)
    {
        LOG_INFO("初始化配置项%s失败", astItems[iRet].pcName);
        Cfg_Cls();
        return RET_ERR;
    }

    //执行配置文件。
    iRet = CmdM_ExecFile(Cfg_GetStr(CFG_FILE), true);

    if (iRet != RET_OK)
    {
        LOG_INFO("初始化配置项文件失败。");
        Cfg_Cls();
        return RET_ERR;
    }

    return RET_OK;
}
