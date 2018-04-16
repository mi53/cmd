#include "cmddflt.h"
#include "test.h"
#include "util.h"
#include "cfg.h"
#include <stdlib.h>
#include <string.h>

/***************************************************************************
* 描述: 全局变量、内部函数等。
***************************************************************************/
static ParaDescSt s_astHelpParas[] =
{
    {TYPE_STR, "option", "标记第三个参数为组编号还是命令编号，只能为-g或者命令名", "-g"},
    {TYPE_STR, "name",   "组编号，且只有第一个参数输入-g时有效", "NULL"},
};

static ParaDescSt s_astTestParas[] =
{
    {TYPE_STR, "TstName", "测试用例的名称，传入NULL执行全部用例", "NULL"},
    {TYPE_STR, "TstCdt",  "测试用例的条件，传入NULL执行全部条件", "NULL"},
};

//描述: 执行系统命令。
static ParaDescSt s_astCmd[CMD_CMD_MAX_WORD_NUM] =
{
    {TYPE_STR, "pcCmd", "所需执行的命令", "NULL"},
    {TYPE_STR, "pcOp1", "命令选项1 可选", "NULL"},
    {TYPE_STR, "pcOp2", "命令选项2 可选", "NULL"},
    {TYPE_STR, "pcOp3", "命令选项3 可选", "NULL"},
    {TYPE_STR, "pcOp4", "命令选项4 可选", "NULL"},
    {TYPE_STR, "pcOp5", "命令选项5 可选", "NULL"},
    {TYPE_STR, "pcOp6", "命令选项6 可选", "NULL"},
    {TYPE_STR, "pcOp7", "命令选项7 可选", "NULL"},
    {TYPE_STR, "pcOp8", "命令选项8 可选", "NULL"}
};

//描述: 执行一个子命令。
static ParaDescSt s_astRunSub[] =
{
    {TYPE_STR, "pcCmd", "命令，若存在空白外字符可以使用双引号", "NULL"},
    {TYPE_INT, "iRunCount", "执行次数", "1"},
    {TYPE_INT, "iFailContinue", "若单次执行失败是否继续执行", "1"}
};

//描述: 执行脚本命令关联参数。
static ParaDescSt s_astExecFile[] =
{
    {TYPE_STR, "pcFile", "脚本文件", "cmd/cmd.txt"},
    {TYPE_INT, "bQuiet", "执行命令时是否允许打印信息", "0"},
    {TYPE_INT, "bNeedOk", "执行预测成功", "1"},
};

//描述: 执行函数命令关联参数。
static ParaDescSt s_astRunFn[FN_MAX_PARA_NUM + 1] =
{
    {TYPE_STR, "pcFn",  "函数名或-h", "-h"},
    {TYPE_STR, "pcPa1", "参数1",  "i:0"},
    {TYPE_STR, "pcPa2", "参数2",  "i:0"},
    {TYPE_STR, "pcPa3", "参数3",  "i:0"},
    {TYPE_STR, "pcPa4", "参数4",  "i:0"},
    {TYPE_STR, "pcPa4", "参数5",  "i:0"}
};

#define CMD_FN_DESC "使用命令执行函数。参数格式: type:value。\n"\
    "参数类型: int/long/char/str/ptr. \n"\
    "\ti: 数据类型，short int均可，可以省略\"i:\"。\n"\
    "\tl: long类型。\n"\
    "\tc: 字符类型。\n"\
    "\ts: 字符串，可以省略\"s:\"。\n"\
    "\tp: 地址。\n"\
    "显示帮助: [-h/--help] 函数名。若省略函数名，列出全部函数。\n"\
    "例如: \n"\
    "1. Fn printf %s str:example_ptrintf.\n"\
    "2. Fn -h printf."


//描述: cfgadd关联参数。
static ParaDescSt s_astCfgAdd[] =
{
    {TYPE_STR, "name", "名称"},
    {TYPE_STR, "type", "类型，只能为:TYPE_INT或TYPE_STR"},
    {TYPE_STR, "dflt", "默认值"},
    {TYPE_STR, "desc", "描述"},
    {TYPE_INT, "bConst", "是否为常量", "0"}
};

//描述: cfg关联参数。
static ParaDescSt s_astCfg[] =
{
    {TYPE_STR, "item", "需要查看的配置项名，若为NULL查看全部", "NULL"}
};

//描述: cfgset关联参数。
static ParaDescSt s_astCfgSet[] =
{
    {TYPE_STR, "item", "需要设置的配置项"},
    {TYPE_STR, "value", "新的值"}
};

//描述: timeset关联参数。
static ParaDescSt s_astTimeSet[] =
{
    {TYPE_STR, "newTime", "新的时间"}
};

//描述: timeadd关联参数。
static ParaDescSt s_astTimeAdd[] =
{
    {TYPE_INT, "days", "增加的天数", "1"}
};

//描述: 互斥量id。
#define CMD_LOCKCLS_DESC "清除lock的使用记录，默认清除全部非活动锁的记录"
static ParaDescSt s_astMutexId[] =
{
    {TYPE_INT, "bForce", "是否强制清除已激活lock使用记录", "0"},
    {TYPE_INT, "bAll", "是否指定为全部lock", "1"},
    {TYPE_INT, "ulSmId", "在bAll为0时，清除该参数指定的lock", "-1"}
};

#define TMP_FMT_HELP_TITLE  "%4s %-15s %s\n", "组号", "命令", "简述"
#define TMP_FMT_HELP        "%04d %-15s %s。\n"
/***************************************************************************
* 预订命令.
***************************************************************************/
CmdFnSt g_astDfltCmd[] =
{
    {GROUP_DFLT, "ExecF", "执行脚本", NULL, Cmd_ExecFile, PARAS(s_astExecFile)},
    {GROUP_DFLT, CMD_HELP,  "显示帮助信息", NULL, Cmd_Help, PARAS(s_astHelpParas)},
    {GROUP_DFLT, "Exit",  "退出", NULL, Cmd_Exit},
    {GROUP_DFLT, ":",   "运行系统命令", NULL, Cmd_Cmd, PARAS(s_astCmd)},
    {GROUP_DFLT, CMD_RUN_SUB, "执行子命令", NULL, Cmd_RunCmd, PARAS(s_astRunSub)},

    {GROUP_HIDE, "Cfg", "显示配置项", NULL, Cmd_CfgShow, PARAS(s_astCfg)},
    {GROUP_HIDE, "CfgAdd", "添加配置项", NULL, Cmd_CfgAdd, PARAS(s_astCfgAdd)},
    {GROUP_HIDE, "CfgSet", "设置配置项的值", NULL, Cmd_CfgSet, PARAS(s_astCfgSet)},
    {GROUP_HIDE, "CfgRecover", "还原全部配置为默认配置", NULL, Cmd_CfgRecover},

    {GROUP_HIDE, "Case",  "运行测试用例", NULL, Cmd_Test, PARAS(s_astTestParas)},
    {GROUP_HIDE, "Fn", "使用命令执行函数", CMD_FN_DESC, Cmd_RunFn, PARAS(s_astRunFn)},

    {0},//用于标记命令结尾的空行，勿删除。
};

/***************************************************************************
* 描述: 默认命令 - help.
***************************************************************************/
int Cmd_Help(ParasSt* pPara, void* pData)
{
    const char* pcPara1 = NULL;
    const char* pcPara2 = NULL;
    const CmdFnSt* pstCmdDesc = NULL;
    const CmdFnSt* pstCmdDescLast = NULL;
    int index;
    int iCmp;
    const char* pcType = NULL;
    const char* pcDfltVal = NULL;

    /***********************************************************
    * 获取参数。
    ***********************************************************/
    pcPara1 = Para_GetStr(pPara, 1);
    pcPara2 = Para_GetStr(pPara, 2);

LAB_HELP_START:

    /***********************************************************
    * 若第二个参数为整数，将字符串转换为整数。
    * 否则找到对应的命令索引。
    ***********************************************************/
    if (pcPara1 != NULL)
    {
        pstCmdDesc = CmdM_CmdGet(pcPara1, strlen(pcPara1));
    }

    /***********************************************************
    * 显示命令组的信息。
    ***********************************************************/
    iCmp = Util_StrCmpIgrUp(pcPara1, "-g");

    if (0 == iCmp)
    {
        bool bListAll = false;
        long iGroupId = -1;

        if (NULL == pcPara2 || !Util_StrGetLong(pcPara2, &iGroupId))
        { bListAll = true; }

        LOG(TMP_FMT_HELP_TITLE);

        pstCmdDescLast = NULL;
        pstCmdDesc = CmdM_CmdItr(pstCmdDescLast);

        while (NULL != pstCmdDesc)
        {
            //打印组分隔线。负数组命令不显示于help全索引中。
            if ((bListAll && pstCmdDesc->iGroup >= 0)
                || (!bListAll && pstCmdDesc->iGroup == iGroupId))
            {
                if (NULL == pstCmdDescLast
                    || pstCmdDescLast->iGroup != pstCmdDesc->iGroup)
                { LOG(FMT_SEG1_E); }

                //显示本组命令。
                LOG(TMP_FMT_HELP,
                    pstCmdDesc->iGroup,
                    pstCmdDesc->pcCmdStr,
                    pstCmdDesc->pcTitle);
            }

            pstCmdDescLast = pstCmdDesc;
            pstCmdDesc = CmdM_CmdItr(pstCmdDescLast);
        }

        return RET_OK;
    }

    /***********************************************************
    * 显示单个命令的帮助信息。
    ***********************************************************/
    if (NULL == pstCmdDesc)
    {
        LOG_INFO("要查看的命令不存在。");

        return RET_OK;
    }

    LOG("命  令: %s\n", pstCmdDesc->pcCmdStr);
    LOG_FIX_LINE("说  明: %s。\n", pstCmdDesc->pcDesc);
    LOG("组  号: %d\n", pstCmdDesc->iGroup);

    for (index = 0; index < pstCmdDesc->stParasDesc.iNum; ++index)
    {
        switch (pstCmdDesc->stParasDesc.pstParas[index].eType)
        {
            case TYPE_INT:
                pcType = "int";
                break;

            case TYPE_STR:
                pcType = "string";
                break;

            default:
                pcType = "unkonw_type";
                break;
        }

        if (NULL != pstCmdDesc->stParasDesc.pstParas[index].pcAssioCfg)
        {
            pcDfltVal = Cfg_GetStr(pstCmdDesc->stParasDesc.pstParas[index].pcAssioCfg);

            LOG_FIX_LINE("\n参数%02d: %s。\n"
                         "原  型: %s %s.\n"
                         "默  认: 关联配置项 %s - %s.\n",
                         index,
                         pstCmdDesc->stParasDesc.pstParas[index].pcDesc,
                         pcType,
                         pstCmdDesc->stParasDesc.pstParas[index].pcName,
                         pstCmdDesc->stParasDesc.pstParas[index].pcAssioCfg,
                         pcDfltVal);
        }
        else
        {
            if (NULL != pstCmdDesc->stParasDesc.pstParas[index].pcDflt)
            { pcDfltVal = pstCmdDesc->stParasDesc.pstParas[index].pcDflt; }
            else
            { pcDfltVal = "无"; }

            LOG_FIX_LINE("\n参数%02d: %s。\n"
                         "原  型: %s %s.\n"
                         "默  认: %s.\n",
                         index,
                         pstCmdDesc->stParasDesc.pstParas[index].pcDesc,
                         pcType,
                         pstCmdDesc->stParasDesc.pstParas[index].pcName,
                         pcDfltVal);
        }
    }

    return RET_OK;

    /***********************************************************
    * 参数错误显示help命令的帮助信息。
    ***********************************************************/
    LOG_INFO("解析参数失败。");
    LOG(FMT_SEG1_E);

    pcPara1 = CMD_HELP;
    pcPara2 = "NULL";

    goto LAB_HELP_START;


    return RET_OK;
}

/***************************************************************************
* 描述: 默认命令 - quit.
* CmdM_Run函数的退出命令，CmdM_Exec函数接收到该命令会停止后续命令的执行。
***************************************************************************/
int Cmd_Exit(ParasSt* pPara, void* pData)
{
    CmdM_Exit();
    return RET_OK;
}


/***************************************************************************
* 描述: 执行脚本。
***************************************************************************/
int Cmd_ExecFile(ParasSt* pPara, void* pData)
{
    int iRet = RET_OK;
    const char* pcFile = Para_GetStr(pPara, 1);
    int bQuiet = PARA_GETINT(pPara, 2);
    int bNeedOk = PARA_GETINT(pPara, 3);

    if (NULL == pcFile)
    {
        LOG_INFO("没有执行需要执行的文件。");
        return RET_ERR;
    }

    iRet = CmdM_ExecFile(pcFile,  (bool)bQuiet);

    if ((!bNeedOk && iRet != RET_OK) || (bNeedOk && RET_OK == iRet))
    {
        iRet = RET_OK;
    }
    else
    {
        iRet = RET_ERR;
    }

    return iRet;
}

/***************************************************************************
* 描述: 执行系统命令。
***************************************************************************/
int Cmd_Cmd(ParasSt* pstPara, void* pData)
{
    const char* pcCmd = NULL;
    const char* pcPara = NULL;

    char* pcCmdTmp = NULL;
    int iCmdLen = 0;
    int index;
    int index2;
    int iRet = RET_OK;

    for (index = 0; index < CMD_CMD_MAX_WORD_NUM; ++index)
    {
        pcPara = Para_GetStr(pstPara, index + 1);

        if (NULL != pcPara)
        { iCmdLen += (strlen(pcPara) + 1); }//增加一个空格。
        else if (NULL == pcPara && iCmdLen > 0)
        {
            pcCmdTmp = (char*)malloc(iCmdLen + 1);
            pcCmdTmp[0] = '\0';

            for (index2 = 0; index2 < index; ++index2)
            {
                pcPara = Para_GetStr(pstPara, index2 + 1);
                strcat(pcCmdTmp, pcPara);
                strcat(pcCmdTmp, " ");
            }

            iRet = system(pcCmdTmp);
            Util_Free(pcCmdTmp);

            return iRet;
        }
        else
        {
            LOG_INFO("命令为空。");
            return RET_ERR;
        }
    }

    return RET_ERR;
}


/***************************************************************************
* 描述: 执行子命令。
***************************************************************************/
int Cmd_RunCmd(ParasSt* pstPara, void* pData)
{
    const char* pcCmd = NULL;
    int* piCount;
    int* piFailContinue;
    int idx;
    int iRet;

    pcCmd = Para_GetStr(pstPara, 1);
    piCount = Para_GetInt(pstPara, 2);
    piFailContinue = Para_GetInt(pstPara, 3);

    if (NULL == pcCmd || NULL == piCount)
    {
        LOG_ERR("命令不正确");
        CmdM_ExecVa(true, NULL, "%s %s", CMD_HELP, CMD_RUN_SUB);
        return RET_ERR;
    }

    for (idx = 0; idx < *piCount; ++idx)
    {
        iRet = CmdM_ExecVa(true, NULL, "%s", pcCmd);

        if (iRet != RET_OK && !*piFailContinue)
        {
            return iRet;
        }
    }

    return RET_OK;
}


/***************************************************************************
* 描述: 执行测试函数。
***************************************************************************/
int Cmd_Test(ParasSt* pstPara, void* pData)
{
    const char* pcTstName = NULL;
    const char* pcCdt = NULL;

    pcTstName = Para_GetStr(pstPara, 1);
    pcCdt = Para_GetStr(pstPara, 2);

    Tst_CaseRun(pcTstName, pcCdt);

    return RET_OK;
}


/***************************************************************************
* 描述: 通过命令执行函数。该函数需要在文件cmdfn.inc中添加。
* 最多支持6个参数。
***************************************************************************/
int Cmd_RunFn(ParasSt* pstPara, void* pData)
{
    long lRet;
    const char* pcFn  = Para_GetStr(pstPara, 1);
    const char* pcPa[FN_MAX_PARA_NUM] = {0};

    int iCmp;
    int index;

    bool bShowHelp;

    FnDescSt* pstFnDesc = NULL;
    FnDescSt* astAllFns[] = {g_astDfltFns, g_pstUserFns};

    /***********************************************************
    * 获取输入。
    ***********************************************************/
    for (index = 0; index < FN_MAX_PARA_NUM; ++index)
    {
        //索引0对应参数编号为2.
        pcPa[index] = Para_GetStr(pstPara, 1 + 1 + index);
    }

    /***********************************************************
    * 判断是否为函数帮助。
    ***********************************************************/
    bShowHelp = false;

    iCmp = Util_StrCmpIgrUp(pcFn, "-h");

    if (0 != iCmp)
    { iCmp = Util_StrCmpIgrUp(pcFn, "--help"); }

    if (0 == iCmp)
    {
        bShowHelp = true;
    }

    /***********************************************************
    * 找到对应的函数。
    ***********************************************************/
    if (bShowHelp)
    {
        //若为函数帮助，第三项内容是函数名。
        pcFn = pcPa[0];
    }

    if (NULL == pcFn)
    {
        LOG_INFO("请输入有效的函数名。");
        return RET_ERR;
    }

    //依据函数名查找函数，不忽略大小写。
    for (index = 0; index < sizeof(astAllFns) / sizeof(FnDescSt*); ++index)
    {
        pstFnDesc = astAllFns[index];

        while (NULL != pstFnDesc && pstFnDesc->pcFnName != NULL)
        {
            iCmp = strcmp(pstFnDesc->pcFnName, pcFn);

            if (0 == iCmp)
            {
                break;
            }

            ++pstFnDesc;
        }

        if (NULL != pstFnDesc && pstFnDesc->pcFnName != NULL)
        { break; }
        else
        { pstFnDesc = NULL; }
    }

    //查找用户自定义函数。
    if (NULL == pstFnDesc && !bShowHelp)
    {
        LOG_INFO("不存在该函数，或者未添加，请在文件cmdfn.inc中添加该函数。");
        return RET_ERR;
    }

    /***********************************************************
    * 显示函数说明。
    ***********************************************************/
    if (bShowHelp)
    {
        LOG("%20s|%-4s|%s\n", "函数名", "参数", "描述");
        LOG(FMT_SEG1_E);

        //打印函数信息。
        if (pstFnDesc != NULL)
        {
            LOG("%20s|%04d|%s\n",
                pstFnDesc->pcFnName,
                pstFnDesc->iParaNum,
                pstFnDesc->pcDesc);
        }
        //显示函数列表。
        else
        {
            for (index = 0; index < sizeof(astAllFns) / sizeof(FnDescSt*); ++index)
            {
                pstFnDesc = astAllFns[index];

                while (NULL != pstFnDesc && pstFnDesc->pcFnName != NULL)
                {
                    LOG("%20s|%04d|%s\n",
                        pstFnDesc->pcFnName,
                        pstFnDesc->iParaNum,
                        pstFnDesc->pcDesc);
                    ++pstFnDesc;
                }
            }
        }

        return RET_OK;
    }

    /***********************************************************
    * 解析参数并执行函数。
    ***********************************************************/
    {
        FnParaSt stPara = {0};
        int iPaNum;
        FnParaAddRsltSt stAddRslt = {0};
        int* piVal;

        iPaNum = pstFnDesc->iParaNum;

        if (iPaNum > FN_MAX_PARA_NUM)
        {
            LOG_ERR("参数数目大于所允许的最大数目。");
            return RET_ERR;
        }

        //解析参数。
        while (stAddRslt.iParaIdx < iPaNum)
        {
            //此处不屏蔽null。
            if (NULL == pcPa[stAddRslt.iParaIdx])
            { pcPa[index] = "null"; }

            lRet = ParaRunFn_ParaAdd(&stPara, pcPa[stAddRslt.iParaIdx], &stAddRslt);

            if (RET_OK != lRet)
            {
                LOG_INFO("参数解析失败。");
                return RET_ERR;
            }
        }

        //执行函数。 找到参数值结尾处，后续默认为Int.
        piVal = stPara.aiPara + stAddRslt.iParaIntIdx
                + (FN_MAX_PARA_NUM - stAddRslt.iParaIdx);

        lRet = FN_CALL(pstFnDesc->fnFunc, piVal, stAddRslt.iStype);
        LOG("\n返回值:%ld\n", lRet);
    }

    return RET_OK;
}

/***************************************************************************
* 描述: 显示配置。传入NULL显示全部配置项。
***************************************************************************/
int Cmd_CfgShow(ParasSt* pstPara, void* pData)
{
    const char* pcCfgName = Para_GetStr(pstPara, 1);

    Cfg_Show(pcCfgName);

    return RET_OK;
}

/***************************************************************************
* 描述: 增添一个配置项。参数不能为空。
***************************************************************************/
int Cmd_CfgAdd(ParasSt* pstPara, void* pData)
{
    CfgItemDescSt stItem = {0};
    int iRet;
    int* piParaVal;

    stItem.pcName = Para_GetStr(pstPara, 1);
    stItem.eValType = Util_ChangStrToType(Para_GetStr(pstPara, 2));
    stItem.pcDfltVal = Para_GetStr(pstPara, 3);
    stItem.pcDesc = Para_GetStr(pstPara, 4);
    piParaVal = Para_GetInt(pstPara, 5);
    stItem.bConst = 0 != *piParaVal ? true : false;

    if (NULL == stItem.pcDfltVal)
    { stItem.pcDfltVal = (char*)"NULL"; }

    iRet = Cfg_AddItems(&stItem, 1);

    if (1 != iRet)
    {
        LOG_INFO("添加配置项\"%s\"失败。", stItem.pcName);
        return RET_ERR;
    }

    return RET_OK;
}

/***************************************************************************
* 描述: 还原为初始配置。
***************************************************************************/
int Cmd_CfgRecover(ParasSt* pstPara, void* pData)
{
    Cfg_RecoverAll();

    Cfg_Show(NULL);

    return RET_OK;
}

/***************************************************************************
* 描述: 设置配置值。
***************************************************************************/
int Cmd_CfgSet(ParasSt* pstPara, void* pData)
{
    const char* pcItem = Para_GetStr(pstPara, 1);
    const char* pcVal = Para_GetStr(pstPara, 2);
    int iRet;

    iRet = Cfg_WriteByStr(pcItem, pcVal);

    LOG("\n");
    Cfg_Show(pcItem);

    return iRet;
}
