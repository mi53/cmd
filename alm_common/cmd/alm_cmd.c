#include "alm_util.h"
#include "alm_cmd.h"
#include "os.h"
#include <stdlib.h>
#include <string.h>


#pragma warning (disable: 4100) //未引用参数

/***************************************************************************
* 描述: 全局变量、内部函数等。
***************************************************************************/

//描述: timeset关联参数。
static ParaDescSt s_astTimeSet[] =
{
    {TYPE_STR, "newTime", "新的时间"}
};

//描述: timeadd关联参数。
static ParaDescSt s_astTimeAdd[] =
{
    {TYPE_INT, "times", "增加的时间", "1"},
    {TYPE_STR, "unit", "单位，d/h/m/s", "d"}
};

//描述: 互斥量id。
#define CMD_LOCKCLS_DESC "清除lock的使用记录，默认清除全部非活动锁的记录"
static ParaDescSt s_astMutexId[] =
{
    {TYPE_INT, "bForce", "是否强制清除已激活lock使用记录", "0"},
    {TYPE_INT, "bAll", "是否指定为全部lock", "1"},
    {TYPE_INT, "ulSmId", "在bAll为0时，清除该参数指定的lock", "-1"}
};

static ParaDescSt s_astMemLog[] =
{
    {TYPE_INT, "bForce", "是否打印未被释放的内存", "0"},
};

#define TMP_FMT_HELP_TITLE  "%4s %-15s %s\n", "组号", "命令", "简述"
#define TMP_FMT_HELP        "%04d %-15s %s。\n"

//描述: 内存记录。 -- 内存记录级别。
static ParaDescSt s_astMemLogLev[] =
{
    {TYPE_INT, "iLev", "内存记录级别，0 - NONE, 1 - TOTAL, 2 - DETAIL", "0"}
};

static ParaDescSt s_astThread[] =
{
    {TYPE_STR, "pcThreadCmd", "线程关联命令", "ThreadTmp"}
};
int Cmd_StartThread(ParasSt* pstPara, void* pData);

//打印文件内容。
static ParaDescSt s_astShowFile[] =
{
    {TYPE_STR, "pcFileName", "需要查看的文件路径", NULL, NULL}
};
int Cmd_ShowFile(ParasSt* pstPara, void* pData);

/***************************************************************************
* 预订命令.
***************************************************************************/
static CmdFnSt s_astAlmCmd[] =
{

    {GROUP_DFLT, "Time", "显示当前时间", NULL, Cmd_Time},
    {GROUP_DFLT, "TimeSet", "设置时间", NULL, Cmd_TimeSet, PARAS(s_astTimeSet)},
    {GROUP_DFLT, "TimeAdd", "增加天数，并正常驱动全部定时器", NULL, Cmd_TimeAdd, PARAS(s_astTimeAdd)},
    {GROUP_DFLT, "ShowFile", "打印文件内容", NULL, Cmd_ShowFile, PARAS(s_astShowFile)},

    //{GROUP_HIDE, "LockTest", "lock测试", NULL, Cmd_LicLockTest},
    {GROUP_DFLT, "Lock", "显示lock使用量", NULL, Cmd_LicLockShow},
    {GROUP_DFLT, "LockCls", "清除lock的使用记录", CMD_LOCKCLS_DESC, Cmd_LicLockCls, PARAS(s_astMutexId)},

    {GROUP_DFLT, "MemLog", "显示内存申请、释放total记录", NULL, Cmd_LicMemLog, PARAS(s_astMemLog)},
    {GROUP_DFLT, "MemLogCls", "清理内存记录器", NULL, Cmd_LicMemLogCls},
    {GROUP_DFLT, "ThreadCmd", "创建一个执行某一命令的线程", NULL, Cmd_StartThread, PARAS(s_astThread)},

    {0},//用于标记命令结尾的空行，勿删除。
};


/***************************************************************************
* 描述: 初始化ALM公共命令。
***************************************************************************/
void Cmd_AlmCmdInit()
{
    OS_Start();
    CmdM_Start();
    CmdM_AddCmds(s_astAlmCmd);

    putenv("GTL_ROOT=.");
}

void Cmd_AlmCmdCls()
{
    CmdM_Close();
    OS_Close();
}

/***************************************************************************
* 描述: 模拟时间相关命令。
***************************************************************************/
int Cmd_Time(ParasSt* pstPara, void* pData)
{
    LOG_INFO("%s", OS_TimeSimulaStr());

    return RET_OK;
}

int Cmd_TimeSet(ParasSt* pPara, void* pData)
{
    int iRet;
    OsDateSt stDate;
    const char* pcTime = Para_GetStr(pPara, 1);

    if (NULL == pcTime)
    {
        LOG_INFO("没有指定日期。");
        return RET_ERR;
    }

    iRet = Util_ChangeTime(pcTime, &stDate);

    if (RET_OK != iRet)
    {
        LOG_INFO("指定的日期无效。");
        return RET_ERR;
    }

    OS_DateSimulaSet(&stDate);

    return RET_OK;
}

int Cmd_TimeAdd(ParasSt* pstPara, void* pData)
{
    int iCount = 0;
    int index = 0;
    char* pcUnit = Para_GetStr(pstPara, 2);
    int iTemp;

    iCount = PARA_GETINT(pstPara, 1);

    if (NULL == pcUnit)
    {
        LOG_ERR("The 2th para should input the unit.");
        return RET_ERR;
    }

    switch (*pcUnit)
    {
        case 'd':
        case 'D':
            iTemp = 24 * 3600;
            break;

        case 'h':
        case 'H':
            iTemp = 3600;
            break;

        case 'm':
        case 'M':
            iTemp = 60;
            break;

        case 's':
        case 'S':
            iTemp = 1;
            break;

        default:
            LOG_ERR("The unit should be d/h/m/s.");
            return RET_ERR;
    }

    iTemp *= iCount;

    LOG_INFO("初始时间 : %s", OS_TimeSimulaStr());

    for (index = 0; index < iTemp / (24 * 3600); ++index)
    {
        OS_TimerRunSecs(24 * 3600);
        LOG_INFO("已增加%04d天: %s", index + 1, OS_TimeSimulaStr());
    }

    iTemp %= (24 * 3600);

    if (iTemp > 0)
    {
        OS_TimerRunSecs(iTemp);
        LOG_INFO("已增加%02d时%02d分%02d秒: %s",
                 iTemp / 3600,
                 (iTemp % 3600) / 60,
                 iTemp % 60,
                 OS_TimeSimulaStr());
    }

    return RET_OK;
}

//打印要查看的文件内容。
int Cmd_ShowFile(ParasSt* pstPara, void* pData)
{
    const char* pcTmp = NULL;
    char* pcFileBuf = NULL;
    int iFileLen = 0;
    int index;
    int iRet;

    pcTmp = Para_GetStr(pstPara, 1);

    if (NULL == pcTmp)
    {
        LOG_INFO("FILE NAME - NULL");
        return RET_ERR;
    }

    LOG_INFO("FILE NAME - \"%s\"", pcTmp);

    /***********************************************************
    * 读取文件。
    ***********************************************************/
    iRet = Util_ReadFileToBuffer(pcTmp, &pcFileBuf, &iFileLen);

    if (RET_OK != iRet)
    { return iRet; }

    /***********************************************************
    * 打印文件。
    ***********************************************************/
    pcTmp = pcFileBuf + iFileLen;
    index = iFileLen / UTIL_LOG_MAX_LEN;

    //打印前N段。
    for (; index > 0; --index)
    {
        LOG("%s", pcTmp - iFileLen);
        pcTmp += UTIL_LOG_MAX_LEN;
    }

    pcTmp -= iFileLen;

    //打印剩余部分。
    LOG("%s\n", pcTmp);

    Util_Free(pcFileBuf);

    return RET_OK;
}


/***************************************************************************
* 描述: 打印lic lock计数。
***************************************************************************/
int Cmd_LicLockShow(ParasSt* pstPara, void* pData)
{
    OS_LockLog();

    return RET_OK;
}

int Cmd_LicLockTest(ParasSt* pstPara, void* pData)
{
    return RET_OK;
}

int Cmd_LicLockCls(ParasSt* pstPara, void* pData)
{
    bool bForce;
    int iSmId;
    bool bClsAll = false;
    int* piRet;

    //bForce
    piRet = Para_GetInt(pstPara, 1);
    UTIL_CHK(NULL != piRet);
    bForce = *piRet != 0 ? true : false;

    //bAll
    piRet = Para_GetInt(pstPara, 2);
    UTIL_CHK(NULL != piRet);
    bClsAll = *piRet != 0 ? true : false;

    //id
    piRet = Para_GetInt(pstPara, 3);

    if (!bClsAll && NULL == piRet)
    {
        LOG_INFO("如果没有指定清除全部，则必须指定对应的编号。");
        return RET_ERR;
    }

    iSmId = *piRet;

    OS_LockLogCls(bForce, bClsAll, iSmId);

    return RET_OK;
}


/***************************************************************************
* 描述: 内存检测。
***************************************************************************/
int Cmd_LicMemLog(ParasSt* pstPara, void* pData)
{
    bool bShowFunc = false;
    int* piRet;

    //bForce
    piRet = Para_GetInt(pstPara, 1);
    UTIL_CHK(NULL != piRet);
    bShowFunc = *piRet != 0 ? true : false;

    OS_MemAlmLog();
//    time_consume_show_result();
    return RET_OK;
}


int Cmd_LicMemLogCls(ParasSt* pstPara, void* pData)
{
    OS_MemAlmLogCls();
    return RET_OK;
}


static void* _Cmd_StartThreadRun(void* pData)
{
    static int iRet;

    iRet = CmdM_Exec((char*)pData, NULL, true);

    Util_Free((char*)pData);

    return &iRet;
}

int Cmd_StartThread(ParasSt* pstPara, void* pData)
{
    char* pcCmd = NULL;
    HThread hThread;

    pcCmd = Para_GetStr(pstPara, 1);

    if (NULL == pcCmd)
    {
        LOG_INFO("线程未关联命令。");
        return RET_OK;
    }

    pcCmd = Util_StrCpy(pcCmd);
    hThread = thread_start(_Cmd_StartThreadRun, pcCmd);
    //thread_wait(hThread);

    //sleep(100);

    return RET_OK;
}
