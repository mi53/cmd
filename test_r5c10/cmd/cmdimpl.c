/**************************************************************************
purpose  :	实现自定义的命令的文件。
***************************************************************************/
#include "cmddef.inc"
#include "os_thread.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alm_util.h"

#pragma warning (disable: 4100) //未引用参数
#pragma warning (disable: 4152) //非标准类型扩展

/***************************************************************************
* 描述: 初始化用户命令。
***************************************************************************/
void CmdM_R5Init()
{
    Cmd_AlmCmdInit();

    CmdM_AddCmdsByDefPara(s_astUserCmds,
                          sizeof(s_astUserCmds) / sizeof(CmdFnDefParaSt));

    ParaRunFn_SetFuncs(s_astUserFns);

    ALM_DbgSetPrint(Util_Log);
}

void CmdM_R5Cls()
{
    Cmd_AlmCmdCls();
}


/***************************************************************************
* GROUP100 - 工具类函数。
***************************************************************************/

//执行临时事务。
CMD_DEF_FUNC_HEAD(CMD_G100_Temp)
{
    LOG_INFO("This is a temp command.");
    return RET_OK;
}

//删除保密文件。
CMD_DEF_FUNC_HEAD(CMD_G100_PS_RM)
{
    int iRet;
    const char* pcPsFile = Cfg_GetStr(CFG_PS);
    FILE* pFile = NULL;

    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/
    if (!Util_FileChk(pcPsFile, true))
    {
        return RET_OK;
    }

    /***********************************************************
    * 截断文件内容。
    ***********************************************************/
    pFile = fopen(pcPsFile, "wb+");

    if (NULL == pFile)
    {
        LOG_ERR("清空文件%s失败。", pcPsFile);
        goto LAB_ERR;
    }

    /***********************************************************
    * 关闭文件。
    ***********************************************************/
    iRet = fclose(pFile);

    if (iRet != 0)
    { LOG_ERR("关闭文件%s失败。", pcPsFile); }

    return RET_OK;

LAB_ERR:

    if (NULL != pFile)
    { fclose(pFile); }

    //写入失败，会导致部分重要信息未能持久化。
    return RET_ERR;
}

//打印表格。
CMD_DEF_FUNC_HEAD(CMD_G100_TBL_SHOW)
{
    const char* pcTblName = CMD_DEF_FUNC_PARA(Str, 1);

    ALM_DbgShowTable(pcTblName);

    return RET_OK;
}

//生成bbom。
CMD_DEF_FUNC_HEAD(CMD_G100_MK_BBOM)
{
    int iRet = RET_OK;
    unsigned int uiIdx = 0;
    unsigned int uiIdx2 = 0;
    const char* pcPrdID  = CMD_DEF_FUNC_PARA(Str, ++uiIdx);
    const char* pcLicFile = CMD_DEF_FUNC_PARA(Str, ++uiIdx);
    char* pcLicStr;
    int iLicStrLen;
    ALM_LIC_STORE_STRU stLicStore = {0};
    char acCfgName_CfgID[32];
    char* pcKey = NULL;
    int iKeyLen;

    iRet = Util_ReadFileToBuffer(pcLicFile, &pcLicStr, &iLicStrLen);
    LIC_CALL_FN_RET(Util_ReadFileToBuffer, iRet);

    //获取秘钥
    GET_PRD_CFG(acCfgName_CfgID, pcPrdID, CFG_KEY);
    iRet = Util_ReadFileToBuffer(Cfg_GetStr(acCfgName_CfgID), &pcKey, &iKeyLen);
    LIC_CALL_FN_RET(Util_ReadFileToBuffer, iRet);

    ALM_StInitLicStore(&stLicStore);
    iRet = ALM_LicParser(pcLicStr, iLicStrLen, pcKey, ALM_CONST_KEY_MAX_LEN, &stLicStore);//直接调用库里的函数解析License文件
    Util_Free(pcLicStr);

    LIC_CALL_FN_RET(ALM_LicParser, iRet);

    //添加bbom。
    for (uiIdx = 0; uiIdx < stLicStore.stBbomInfo.uiNum; uiIdx++)
    {
        if (0 == stLicStore.stBbomInfo.pstArray[uiIdx].uiSameNameId)
        {

#if 1 //是否打开生成r5c01添加bbom的命令
#if R5C10_VER_CHECK_LESS(2,16)

            //r5c01添加bbom的命令
            if (stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_RESOURCE)
            {
                if (uiIdx2 <= 50)
                { LOG("BbomTmpAdd %s %d 0 %d 2 %d %d\n", stLicStore.stBbomInfo.pstArray[uiIdx].acBBomName, uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue); }
                else
                { LOG("BbomTmpAdd %s %d 0 %d 3 %d %d\n", stLicStore.stBbomInfo.pstArray[uiIdx].acBBomName, uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue); }
            }

            if (stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_FUNCTION)
            {
                LOG("BbomTmpAdd %s %d 1 %d\n", stLicStore.stBbomInfo.pstArray[uiIdx].acBBomName, uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue);
            }

#endif
#if R5C10_VER_CHECK_MORE(2, 16)

            //r5c01添加bbom的命令
            if (stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_RESOURCE)
            {
                if (uiIdx2 <= 50)
                { LOG("BbomTmpAdd %s %d 0 %d 2 %d %d\n", stLicStore.stBbomInfo.pstArray[uiIdx].pcBBomName, uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue); }
                else
                { LOG("BbomTmpAdd %s %d 0 %d 3 %d %d\n", stLicStore.stBbomInfo.pstArray[uiIdx].pcBBomName, uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue); }
            }

            if (stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_FUNCTION)
            {
                LOG("BbomTmpAdd %s %d 1 %d\n", stLicStore.stBbomInfo.pstArray[uiIdx].pcBBomName, uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue);
            }

#endif
#endif

#if 1//是否打开生成r5c10添加bbom的命令
#if R5C10_VER_CHECK_LESS(2,16)

            //r5c10添加bbom的命令
            if (stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_RESOURCE)
            {
                LOG("PrdBbomTempAdd %d \"%s\" 0 %d %d %d\n", uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].acBBomName, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue);
            }

            if (stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_FUNCTION)
            {
                LOG("PrdBbomTempAdd %d \"%s\" 1 %d %d %d\n", uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].acBBomName, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue);
            }

#endif
#if R5C10_VER_CHECK_MORE(2, 16)

            //r5c10添加bbom的命令
            if (stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_RESOURCE)
            {
                LOG("PrdBbomTempAdd %d \"%s\" 0 %d %d %d\n", uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].pcBBomName, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue);
            }

            if (stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_FUNCTION)
            {
                LOG("PrdBbomTempAdd %d \"%s\" 1 %d %d %d\n", uiIdx2, stLicStore.stBbomInfo.pstArray[uiIdx].pcBBomName, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue, stLicStore.stBbomInfo.pstArray[uiIdx].ulValue);
            }

#endif
#endif

#if 1//是否打开r5c10添加紧急/Stick bbom的命令
#if R5C10_VER_CHECK_LESS(2,16)
            //r5c10添加紧急/Stick bbom的命令
            LOG("EmStickBbomTempAdd %s %s %d\n", stLicStore.stBbomInfo.pstArray[uiIdx].acBBomName, stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_RESOURCE ? "0" : "1", stLicStore.stBbomInfo.pstArray[uiIdx].ulValue);
#endif
#if R5C10_VER_CHECK_MORE(2, 16)
            //r5c10添加紧急/Stick bbom的命令
            LOG("EmStickBbomTempAdd %s %s %d\n", stLicStore.stBbomInfo.pstArray[uiIdx].pcBBomName, stLicStore.stBbomInfo.pstArray[uiIdx].eType == ALM_LIC_BBOM_RESOURCE ? "0" : "1", stLicStore.stBbomInfo.pstArray[uiIdx].ulValue);
#endif
#endif
        }
    }

    //清理。
    ALM_StClsLicStore(&stLicStore);


    return iRet;
}


CMD_DEF_FUNC_HEAD(CMD_G100_RESET)
{
    //模拟系统复位
    int iRet = RET_OK;

    Cfg_WriteByStr(CFG_PS, Cfg_GetStr(CFG_PS_TMP));
    //后面根据添加的产品修改各种保密文件的配置
    Cfg_WriteByStr(CFG_PS_C01_1, Cfg_GetStr(CFG_PS_C01_1_TMP));
    Cfg_WriteByStr(CFG_PS_C01_2, Cfg_GetStr(CFG_PS_C01_2_TMP));

    iRet = CmdM_ExecVa(true, NULL,
                       CMD_DEF_TO_STR(CMD_G1_ALM_Stop));
    LIC_CALL_FN_RET(CMD_DEF_TO_STR(CMD_G1_ALM_Stop), iRet);

    Cfg_Recover(CFG_PS);
    Cfg_Recover(CFG_PS_C01_1);
    Cfg_Recover(CFG_PS_C01_2);

    return iRet;
}
CMD_DEF_FUNC_HEAD(CMD_G100_PRD_CFG_ADD)
{

    CfgItemDescSt stItem = {0};
    int iRet;
    char acCfgName_CfgID[32];
    unsigned int uiIdx = 0;
    const char* piCfgId = CMD_DEF_FUNC_PARA(Str, ++uiIdx);
    const char* pcCfgName = CMD_DEF_FUNC_PARA(Str, ++uiIdx);
    const char* pcCfgType = CMD_DEF_FUNC_PARA(Str, ++uiIdx);
    const char* pcCfgdlft = CMD_DEF_FUNC_PARA(Str, ++uiIdx);
    const char* pcCfgdesc = CMD_DEF_FUNC_PARA(Str, ++uiIdx);
    int* pbCont = CMD_DEF_FUNC_PARA(Int, ++uiIdx);

    strcpy(acCfgName_CfgID, pcCfgName);
    strcpy(acCfgName_CfgID + strlen(pcCfgName), "_");
    strcpy(acCfgName_CfgID + strlen(pcCfgName) + 1, piCfgId);

#if 1
    stItem.pcName = acCfgName_CfgID;
    stItem.eValType = Util_ChangStrToType(pcCfgType);
    stItem.pcDfltVal = (char*)pcCfgdlft;
    stItem.pcDesc = (char*)pcCfgdesc;
    stItem.bConst = 0 != *pbCont ? true : false;

    if (NULL == stItem.pcDfltVal)
    { stItem.pcDfltVal = (char*)"NULL"; }

    iRet = Cfg_AddItems(&stItem, 1);

    if (1 != iRet)
    {
        /* 因为本命令主要是为了做增加产品配置，在添加配置失败时可以忽略这种错误，使得配置可以继续 */
        LOG_INFO("添加配置项\"%s\"失败。", stItem.pcName);
        return RET_OK;
    }

#endif
    return RET_OK;
}

/***************************************************************************
* 辅助函数。
***************************************************************************/

//添加BBOM到临时对象。
static void _Cmd_PrdTempBbomsAdd(const ALM_PRD_BBOM_STRU* pstBbom)
{
    TL_CHK_VOID(pstBbom != NULL, NULL);

    if (s_stPrdBboms.uiNum == s_stPrdBboms.uiCap)
    {
        Util_ArrayExpandAuto((void**)&s_stPrdBboms.pstArray,
                             sizeof(ALM_PRD_BBOM_STRU),
                             s_stPrdBboms.uiNum,
                             &s_stPrdBboms.uiCap);
    }

    memcpy(s_stPrdBboms.pstArray + s_stPrdBboms.uiNum,
           pstBbom,
           sizeof(ALM_PRD_BBOM_STRU));
    ++s_stPrdBboms.uiNum;
}

//添加BBOM到临时对象。
static void _Cmd_EmOrStickAddBbomToTempObj(const ALM_SLIC_BBOM_STRU* pstBbom)
{
    TL_CHK_VOID(pstBbom != NULL, NULL);

    if (s_stEmOrStickBboms.uiNum == s_stEmOrStickBboms.uiCap)
    {
        Util_ArrayExpandAuto((void**)&s_stEmOrStickBboms.pstArray,
                             sizeof(ALM_SLIC_BBOM_STRU),
                             s_stEmOrStickBboms.uiNum,
                             &s_stEmOrStickBboms.uiCap);
    }

    memcpy(s_stEmOrStickBboms.pstArray + s_stEmOrStickBboms.uiNum,
           pstBbom,
           sizeof(ALM_SLIC_BBOM_STRU));
    ++s_stEmOrStickBboms.uiNum;
}

static void _Cmd_PrdTempBbomsShow()
{
    ALM_UINT32 idx;

    //显示资源项。
    LOG("%-5s %-15s %-4s %-6s %-6s %-6s\n",
        "ID", "NAME", "TYPE", "DFLT", "MIN", "MAX");
    LOG("%-5s %-15s %-4s %-6s %-6s %-6s\n",
        "=====", "===============", "====", "======", "======", "======");

    for (idx = 0; idx < s_stPrdBboms.uiNum; ++idx)
    {
        LOG("%-5d %-15s %-4s %-6d %-6d %-6d\n",
            s_stPrdBboms.pstArray[idx].iBbomId,
            s_stPrdBboms.pstArray[idx].acName,
            s_stPrdBboms.pstArray[idx].eType ? "FUN" : "RES",
            s_stPrdBboms.pstArray[idx].ulDfltVal,
            s_stPrdBboms.pstArray[idx].ulMinVal,
            s_stPrdBboms.pstArray[idx].ulMaxVal);
    }

}

static void _Cmd_EmOrStickBbomsShow()
{
    ALM_UINT32 idx;

    //显示资源项。
    LOG("%-15s %-4s %-6s\n",
        "NAME", "TYPE", "VALUE");
    LOG("%-15s %-4s %-6s\n",
        "===============", "====", "======");

    for (idx = 0; idx < s_stEmOrStickBboms.uiNum; ++idx)
    {
        LOG("%-5d %-15s %-4s %-6d %-6d %-6d\n",
            s_stEmOrStickBboms.pstArray[idx].acBBomName,
            s_stEmOrStickBboms.pstArray[idx].eType ? "FUN" : "RES",
            s_stEmOrStickBboms.pstArray[idx].ulValue);
    }

}
