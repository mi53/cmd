/**************************************************************************
	purpose  :	工程入口。
***************************************************************************/
#include "case.h"
#include "util.h"
#include "os.h"
#include "cfg_r5.h"
#include "cmd_r5.h"
#include "lic_inc.h"
#if (LIC_OS == LIC_WINDOWS)
#include "vld.h"
#endif
/***************************************************************************
* 描述: 启动及停止全部组件。
***************************************************************************/
void start();
void stop();

/***************************************************************************
* 入口。
***************************************************************************/
int main(int argc, const char** argv)
{
    start();

    if (argc > 1)
    {
        CmdM_Run(argv + 1, argc - 1, false, OS_TimeSimulaStr);
    }
    else
    {
        CmdM_Run(NULL, 0, true, OS_TimeSimulaStr);
    }

    stop();

    return RET_OK;
}

/***************************************************************************
* 描述: 启动及停止全部组件。
***************************************************************************/
void start()
{
    //初始化命令系统。
    CmdM_R5Init();

    //初始化工程自定义配置。
    Cfg_ComInit();

    //初始化工程自定义用例。
    Case_Init();

    LOG(FMT_SEG3_E);

    CMDM_EXEC(CMD_DEF_TO_STR(CMD_G9_ALM_BaseGetVer));

    LOG("system   date: %s.\n", OS_DateStr());
    LOG("simular  date: %s.\n", OS_TimeSimulaStr());
    LOG("project  name: r5c10.\n");

    LOG(FMT_SEG3_E);
}

void stop()
{
    CMDM_EXEC(CMD_DEF_TO_STR(CMD_G1_ALM_Stop));

    //清理CASE。
    Case_Cls();

    //清理工程自定义配置。
    Cfg_Cls();

    //清理命令系统。
    CmdM_R5Cls();
}
