#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "callback.h"
#include "util.h"
#include "cfg_r5.h"
#include "cmd.h"


//设置ESN。
ALM_INT32 Adapter_SetEsn(const char* pcEsn)
{
    int iRet;

    iRet = Cfg_WriteByStr(CFG_ESN, pcEsn);
    TL_CALL_RET(ALM_TRUE, iRet);

    return ALM_OK;
}

//得到ESN。
ALM_INT32 Adapter_GetEsn(OUT ALM_ENV_ESN_STRU* pstEsn)
{
    const char* pcEsn = Cfg_GetStr(CFG_ESN);

    strcpy(pstEsn->acEsn, pcEsn);

    return ALM_OK;
}

//实现持久性写。
ALM_INT32 Adapter_PsIOWrite(CONST ALM_VOID* pBuf,
                            IN ALM_UINT32 uiLen,
                            IN ALM_UINT32 uiOffSet)
{
    FILE* pFile = NULL;
    long iRet;
    const char* pcFileName = Cfg_GetStr(CFG_PS);

    if (NULL == pBuf || 0 == uiLen)
    { return ALM_ERR_UNKNOW; }

    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/
    if (!Util_FileChk(pcFileName, true))
    {
        LOG_ERR("WRITE回调: 文件%s不存在。", pcFileName);
        goto LAB_ERR;
    }

    /***********************************************************
    * 打开文件并定位文件到写的偏移位。
    ***********************************************************/
    pFile = fopen(pcFileName, "wb");

    if (NULL == pFile)
    {
        LOG_ERR("打开文件%s失败。", pcFileName);
        goto LAB_ERR;
    }

    iRet = fseek(pFile, uiOffSet, SEEK_SET);

    if (iRet != 0)
    {
        LOG_ERR("写入数据到持久化文件时定位到偏移位失败。");
        goto LAB_ERR;
    }

    /***********************************************************
    * 写入数据。
    ***********************************************************/
    iRet = (long)fwrite(pBuf, 1, uiLen, pFile);

    if (iRet != (long)uiLen)
    {
        LOG_ERR("写入数据到持久化文件失败。");
        goto LAB_ERR;
    }

    /***********************************************************
    * 关闭文件。
    ***********************************************************/
    iRet = fclose(pFile);

    if (iRet != 0)
    { LOG_ERR("关闭文件%s失败。", pcFileName); }


    return ALM_OK;

LAB_ERR:

    if (NULL != pFile)
    { fclose(pFile); }

    //写入失败，会导致部分重要信息未能持久化。
    return ALM_ERR_UNKNOW;
}

//实现持久化读。
ALM_INT32 Adapter_PsIORead(OUT ALM_VOID* pBuf,
                           INOUT ALM_UINT32* puiLen,
                           IN ALM_UINT32 uiOffSet)
{
    FILE* pFile = NULL;
    const char* pcFileName = Cfg_GetStr(CFG_PS);
    long lLenTmp;
    long lRet;
    bool bNeedCreate = true;

    if (NULL  == puiLen)
    { return ALM_ERR_UNKNOW; }

    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/
    if (!Util_FileChk(pcFileName, bNeedCreate))
    {
        LOG_ERR("READ回调: 文件%s不存在。", pcFileName);
        goto LAB_ERR;
    }

    /***********************************************************
    * 打开文件并定位到偏移位。
    ***********************************************************/
    pFile = fopen(pcFileName, "rb");

    if (NULL == pFile)
    {
        LOG_ERR("打开文件%s失败。", pcFileName);
        goto LAB_ERR;
    }

    //需要进行偏移位有效性及读取数据长度有效性判断。
    fseek(pFile, 0, SEEK_END);
    lLenTmp = ftell(pFile);

    //偏移有效性判断。
    if ((lLenTmp < 0 ) || ((long)uiOffSet > lLenTmp))
    { goto LAB_ERR; }

    //待读取长度有效性判断。
    if (0 == lLenTmp)
    {
        *puiLen = 0;
        return ALM_OK;
    }
    else if (lLenTmp < (long)*puiLen)
    {
        goto LAB_ERR;
    }

    if (NULL == pBuf)
    {
        *puiLen = lLenTmp;
        return ALM_OK;
    }

    //定位到指定偏移位。
    lRet = fseek(pFile, uiOffSet, SEEK_SET);

    if (lRet != 0)
    { goto LAB_ERR; }

    /***********************************************************
    * 读取数据
    ***********************************************************/
    lRet = fread(pBuf, 1, (long) * puiLen, pFile);

    if (lRet != (long)*puiLen)
    { goto LAB_ERR; }

    /***********************************************************
    * 关闭文件
    ***********************************************************/
    lRet = fclose(pFile);

    if (lRet != 0)
    { LOG_ERR("关闭文件%s失败。", pcFileName); }

    return ALM_OK;

LAB_ERR:

    if (NULL != pFile)
    { fclose(pFile); }

    *puiLen = 0;

    //返回错误会导致调用函数错误。与未读取到内容有不同处理。
    return ALM_ERR_UNKNOW;
}

/**
* 描述: 得到R2C05版本持久化文件相关产品数目或相关产品编号。
* piPrdIds - 存储得到的C01持久化存储相关的产品编号的数组。若为空，只输出产品数目。
* puiPrdNum - piPrdIds不为空时，输入piPrdIds的结点数目；否则用于输出产品数目。
*/
ALM_INT32 Adapter_PsR2C05GetPrds(OUT ALM_INT32* piPrdIds,
                                 INOUT ALM_UINT32* puiPrdNum)
{
    ALM_UINT32 uiIdx;

    if (ALM_NULL == piPrdIds)
    {
        *puiPrdNum = 4;
        return RET_OK;
    }

    if (*puiPrdNum < 4)
    {
        return RET_ERR;
    }

    for (uiIdx = 0; uiIdx < *puiPrdNum; ++uiIdx)
    {
        piPrdIds[uiIdx] = uiIdx;
    }

    return RET_OK;
}

/* 读R2C05版本的持久化文件 */
ALM_INT32 Adapter_PsR2C05Read(ALM_INT32 iPrdId,
                              OUT ALM_VOID* pBuf,
                              INOUT ALM_UINT32* puiInNeedOutGetSize,
                              ALM_UINT32 uiOffSet)
{
    FILE* pFile = NULL;
    const char* pcFileName = NULL;
    long lLenTmp;
    long lRet;
    char acCfgName_CfgID[32];
    char acIntToStr[32];

    sprintf(acIntToStr, "%d", iPrdId);
    acIntToStr[sizeof(acIntToStr) - 1] = '\0';

    if (ALM_NULL == pBuf || 0 == *puiInNeedOutGetSize)
    { return RET_ERR; }

    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/
    GET_PRD_CFG(acCfgName_CfgID, acIntToStr, CFG_PS_R2C05);
    pcFileName = Cfg_GetStr(acCfgName_CfgID);

    //文件不存在返回成功，并返回0.
    if (!Util_FileChk(pcFileName, false))
    {
        *puiInNeedOutGetSize = 0;
        return RET_OK;
    }

    /***********************************************************
    * 打开文件并定位到偏移位。
    ***********************************************************/
    pFile = fopen(pcFileName, "rb");

    if (NULL == pFile)
    {
        LOG_ERR("打开文件%s失败。", pcFileName);
        goto LAB_ERR;
    }

    //需要进行偏移位有效性及读取数据长度有效性判断。
    fseek(pFile, 0, SEEK_END);
    lLenTmp = ftell(pFile);

    //偏移有效性判断。
    if ((lLenTmp < 0 ) || ((long)uiOffSet > lLenTmp))
    { goto LAB_ERR; }

    //待读取长度有效性判断。
    if (lLenTmp < (long)*puiInNeedOutGetSize)
    { goto LAB_ERR; }

    //定位到指定偏移位。
    lRet = fseek(pFile, uiOffSet, SEEK_SET);

    if (lRet != 0)
    { goto LAB_ERR; }

    /***********************************************************
    * 读取数据
    ***********************************************************/
    lRet = fread(pBuf, 1, (long) * puiInNeedOutGetSize, pFile);
    *puiInNeedOutGetSize = (ALM_UINT32)lRet;

    /***********************************************************
    * 关闭文件
    ***********************************************************/
    lRet = fclose(pFile);

    if (lRet != 0)
    { LOG_ERR("关闭文件%s失败。", pcFileName); }

    return RET_OK;

LAB_ERR:

    if (NULL != pFile)
    { fclose(pFile); }

    return RET_ERR;
}

/* 写R2C05版本的持久化文件 */
ALM_INT32 Adapter_PsR2C05Write(ALM_INT32 iPrdId,
                               OUT ALM_VOID* pBuf,
                               ALM_UINT32 uiBufSize,
                               ALM_UINT32 uiOffSet)
{
    FILE* pFile = NULL;
    long lRet;
    const char* pcFileName = NULL;
    char acCfgName_CfgID[32];
    char acIntToStr[32];

    if (ALM_NULL == pBuf || 0 == uiBufSize)
    { return RET_ERR; }

    sprintf(acIntToStr, "%d", iPrdId);
    acIntToStr[sizeof(acIntToStr) - 1] = '\0';

    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/

    GET_PRD_CFG(acCfgName_CfgID, acIntToStr, CFG_PS_R2C05);
    pcFileName = Cfg_GetStr(acCfgName_CfgID);

    //判断文件是否存在，若不存在创建。
    if (!Util_FileChk(pcFileName, true))
    {
        LOG_INFO("WRITE回调: 文件%s不存在。", pcFileName);
        goto LAB_ERR;
    }

    /***********************************************************
    * 打开文件并定位文件到写的偏移位。
    ***********************************************************/
    pFile = fopen(pcFileName, "rb+");

    if (NULL == pFile)
    {
        LOG_ERR("打开文件%s失败。", pcFileName);
        goto LAB_ERR;
    }

    lRet = fseek(pFile, (long)uiOffSet, SEEK_SET);

    if (lRet != 0)
    {
        LOG_ERR("写入数据到持久化文件时定位到偏移位失败。");
        goto LAB_ERR;
    }

    /***********************************************************
    * 写入数据。
    ***********************************************************/
    lRet = fwrite(pBuf, 1, (long)uiBufSize, pFile);

    if (lRet != (long)uiBufSize)
    {
        LOG_ERR("写入数据到持久化文件失败。");
        goto LAB_ERR;
    }

    /***********************************************************
    * 关闭文件。
    ***********************************************************/
    lRet = fclose(pFile);

    if (lRet != 0)
    { LOG_ERR("关闭文件%s失败。", pcFileName); }

    return RET_OK;

LAB_ERR:

    if (NULL != pFile)
    { fclose(pFile); }

    //写入失败，会导致部分重要信息未能持久化。
    return RET_ERR;
}

/**
* 描述: 得到C01版本持久化文件相关产品数目或相关产品编号。
* piPrdIds - 存储得到的C01持久化存储相关的产品编号的数组。若为空，只输出产品数目。
* puiPrdNum - piPrdIds不为空时，输入piPrdIds的结点数目；否则用于输出产品数目。
*/
ALM_INT32 Adapter_PsC01GetPrds(OUT ALM_INT32* piPrdIds,
                               INOUT ALM_UINT32* puiPrdNum)
{
    ALM_UINT32 uiIdx;

    if (ALM_NULL == piPrdIds)
    {
        *puiPrdNum = 4;
        return RET_OK;
    }

    if (*puiPrdNum < 3)
    {
        return RET_ERR;
    }

    for (uiIdx = 0; uiIdx < *puiPrdNum; ++uiIdx)
    {
        piPrdIds[uiIdx] = uiIdx;
    }

    return RET_OK;
}

/* 读R5C01版本的持久化文件 */
ALM_INT32 Adapter_PsC01Read(ALM_INT32 iPrdId,
                            ALM_ENUM(ALM_R5C01_PERSISTENT_TYPE_ENUM) eType,
                            OUT ALM_VOID* pBuf,
                            INOUT ALM_UINT32* puiInNeedOutGetSize,
                            ALM_UINT32 uiOffSet)
{
    FILE* pFile = NULL;
    const char* pcFileName = NULL;
    long lLenTmp;
    long lRet;
    char acCfgName_CfgID[32];
    char acIntToStr[32];

    sprintf(acIntToStr, "%d", iPrdId);
    acIntToStr[sizeof(acIntToStr) - 1] = '\0';

    if (ALM_NULL == pBuf || 0 == *puiInNeedOutGetSize)
    { return RET_ERR; }

    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/
    if (ALM_R5C01_PERSISTENT_FRIST == eType)
    {
        GET_PRD_CFG(acCfgName_CfgID, acIntToStr, CFG_PS_C01_1);
        pcFileName = Cfg_GetStr(acCfgName_CfgID);
    }
    else if (ALM_R5C01_PERSISTENT_SECOND == eType)
    {
        GET_PRD_CFG(acCfgName_CfgID, acIntToStr, CFG_PS_C01_2);
        pcFileName = Cfg_GetStr(acCfgName_CfgID);
    }
    else
    {
        LOG_ERR("无效的持久化文件类型。");
        return RET_ERR;
    }

    //文件不存在返回成功，并返回0.
    if (!Util_FileChk(pcFileName, false))
    {
        *puiInNeedOutGetSize = 0;
        return RET_OK;
    }

    /***********************************************************
    * 打开文件并定位到偏移位。
    ***********************************************************/
    pFile = fopen(pcFileName, "rb");

    if (NULL == pFile)
    {
        LOG_ERR("打开文件%s失败。", pcFileName);
        goto LAB_ERR;
    }

    //需要进行偏移位有效性及读取数据长度有效性判断。
    fseek(pFile, 0, SEEK_END);
    lLenTmp = ftell(pFile);

    //偏移有效性判断。
    if ((lLenTmp < 0 ) || ((long)uiOffSet > lLenTmp))
    { goto LAB_ERR; }

    //待读取长度有效性判断。
    if (lLenTmp < (long)*puiInNeedOutGetSize)
    { goto LAB_ERR; }

    //定位到指定偏移位。
    lRet = fseek(pFile, uiOffSet, SEEK_SET);

    if (lRet != 0)
    { goto LAB_ERR; }

    /***********************************************************
    * 读取数据
    ***********************************************************/
    lRet = fread(pBuf, 1, (long) * puiInNeedOutGetSize, pFile);
    *puiInNeedOutGetSize = (ALM_UINT32)lRet;

    /***********************************************************
    * 关闭文件
    ***********************************************************/
    lRet = fclose(pFile);

    if (lRet != 0)
    { LOG_ERR("关闭文件%s失败。", pcFileName); }

    return RET_OK;

LAB_ERR:

    if (NULL != pFile)
    { fclose(pFile); }

    return RET_ERR;
}

/* 写R5C01版本的持久化文件 */
ALM_INT32 Adapter_PsC01Write(ALM_INT32 iPrdId,
                             ALM_ENUM(ALM_R5C01_PERSISTENT_TYPE_ENUM) eType,
                             OUT ALM_VOID* pBuf,
                             ALM_UINT32 uiBufSize,
                             ALM_UINT32 uiOffSet)
{
    FILE* pFile = NULL;
    long lRet;
    const char* pcFileName = NULL;
    char acCfgName_CfgID[32];
    char acIntToStr[32];

    if (ALM_NULL == pBuf || 0 == uiBufSize)
    { return RET_ERR; }

    sprintf(acIntToStr, "%d", iPrdId);
    acIntToStr[sizeof(acIntToStr) - 1] = '\0';

    /***********************************************************
    * 选择并验证文件有效性。
    ***********************************************************/
    if (ALM_R5C01_PERSISTENT_FRIST == eType)
    {
        GET_PRD_CFG(acCfgName_CfgID, acIntToStr, CFG_PS_C01_1);
        pcFileName = Cfg_GetStr(acCfgName_CfgID);
    }
    else if (ALM_R5C01_PERSISTENT_SECOND == eType)
    {
        GET_PRD_CFG(acCfgName_CfgID, acIntToStr, CFG_PS_C01_2);
        pcFileName = Cfg_GetStr(acCfgName_CfgID);
    }
    else
    {
        LOG_ERR("无效的持久化文件类型。");
        return RET_ERR;
    }

    //判断文件是否存在，若不存在创建。
    if (!Util_FileChk(pcFileName, true))
    {
        LOG_INFO("WRITE回调: 文件%s不存在。", pcFileName);
        goto LAB_ERR;
    }

    /***********************************************************
    * 打开文件并定位文件到写的偏移位。
    ***********************************************************/
    pFile = fopen(pcFileName, "rb+");

    if (NULL == pFile)
    {
        LOG_ERR("打开文件%s失败。", pcFileName);
        goto LAB_ERR;
    }

    lRet = fseek(pFile, (long)uiOffSet, SEEK_SET);

    if (lRet != 0)
    {
        LOG_ERR("写入数据到持久化文件时定位到偏移位失败。");
        goto LAB_ERR;
    }

    /***********************************************************
    * 写入数据。
    ***********************************************************/
    lRet = fwrite(pBuf, 1, (long)uiBufSize, pFile);

    if (lRet != (long)uiBufSize)
    {
        LOG_ERR("写入数据到持久化文件失败。");
        goto LAB_ERR;
    }

    /***********************************************************
    * 关闭文件。
    ***********************************************************/
    lRet = fclose(pFile);

    if (lRet != 0)
    { LOG_ERR("关闭文件%s失败。", pcFileName); }

    return RET_OK;

LAB_ERR:

    if (NULL != pFile)
    { fclose(pFile); }

    //写入失败，会导致部分重要信息未能持久化。
    return RET_ERR;
}

//打印日志。
ALM_INT32 Adapter_Log(ALM_ENUM(ALM_LOG_LEVEL_ENUM) eLevel, CONST ALM_CHAR* pcLogStr)
{
    switch (eLevel)
    {
        case ALM_LOG_LEVEL_TRACE:
            LOG_FIX_LINE_BS("[%s: TRACE] %s", OS_TimeSimulaStr(), pcLogStr);
            break;

        case ALM_LOG_LEVEL_DEBUG:
            LOG_FIX_LINE_BS("[%s: DEBUG] %s", OS_TimeSimulaStr(), pcLogStr);
            break;

        case ALM_LOG_LEVEL_ERROR:
            LOG_FIX_LINE_BS("[%s: ERROR] %s", OS_TimeSimulaStr(), pcLogStr);
            break;

        case ALM_LOG_LEVEL_WARNING:
            LOG_FIX_LINE_BS("[%s:  WARN] %s", OS_TimeSimulaStr(), pcLogStr);
            break;

        case ALM_LOG_LEVEL_INFO:
            LOG_FIX_LINE_BS("[%s: INFO] %s", OS_TimeSimulaStr(), pcLogStr);
            break;

        default:
            return ALM_ERR_UNKNOW;
    }

    return ALM_OK;
}

/* 状态变更通知 */
ALM_INT32 Adapter_NotifyChange(CONST ALM_NOTIFY_CHANGES_STRU* pstChanges)
{
    ALM_UINT32 uiIdx;
    ALM_UINT32 uiIdx2;
    CONST ALM_NOTIFY_CHANGE_STRU* pstPrd;
    ALM_BOOL bTmp;

#define RSN_MAX_LEN 256

    ALM_CHAR acRsn[RSN_MAX_LEN + 1] = {0};

    if (NULL == pstChanges && 0 == pstChanges->uiNum)
    {
        return ALM_OK;
    }

#define ALMRsn2Str_M(ulRsn) \
    ALMDbgRsnAndOptToRsnStr(pstPrd->eOperate, ulRsn, acRsn, sizeof(acRsn))

    /* License状态转换为字符串, ALM_STATE_LIC_STRU */
#define ALMLicState2Str_M(stLicState) \
    stLicState.bUsing ? Tl_AlmState2Str(stLicState.stBase.eState) : "NotUsing"

    LOG("\n%s:%s\n", "Notify_Change_Start", OS_TimeSimulaStr());

    /* 打印产品状态变更。 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        if (pstPrd->pstPrd != NULL)
        {
            /* 打印头 */
            if (!bTmp)
            {
                LOG("\n\t%s\n", "Notify: Prd_State_Change");

                LOG("\t%-10s %-15s %-15s %-20s %-5s %s\n",
                    "PrdId", "Old", "New", "enter",
                    "perm", "left");

                LOG("\t%-10s %-15s %-15s %-20s %-5s %s\n",
                    "----------", "---------------", "---------------",
                    "--------------------",
                    "-----", "---------------");

                bTmp = ALM_TRUE;
            }

            /* 打印条目 */
            LOG("\t%-10d %-15s %-15s %-20s %-5s %-15s\n",
                pstPrd->iPrdId,
                Tl_AlmPrdState2Str(pstPrd->pstPrd->stOld.eState),
                Tl_AlmPrdState2Str(pstPrd->pstPrd->stNew.eState),
                pstPrd->pstPrd->stNew.acEnterTime,
                pstPrd->pstPrd->stNew.bPerm ? "true" : "not",
                Tl_AlmMins2DayStr(pstPrd->pstPrd->stNew.uiLeftMins));
        }
    }

    /* 打印BBOM状态变更 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (uiIdx2 = 0; uiIdx2 < pstPrd->stBboms.uiNum; ++uiIdx2)
        {
            if (!pstPrd->stBboms.pstArray[uiIdx2].bStateChange)
            {
                continue;
            }

            /* 打印头 */
            if (!bTmp)
            {
                LOG("\n\t%s\n", "Notify: Bbom_State_Change");

                LOG("\t%-10s %-20s %-10s %-15s %-15s %-20s %-5s %s\n",
                    "PrdId", "Name", "Id", "Old", "New", "enter",
                    "perm", "left");

                LOG("\t%-10s %-20s %-10s %-15s %-15s %-20s %-5s %s\n",
                    "----------", "--------------------", "----------",
                    "---------------", "---------------",
                    "--------------------", "-----",
                    "-------------------------");
                bTmp = ALM_TRUE;
            }

            /* 打印条目 */
            LOG("\t%-10d %-20s %-10d %-15s %-15s %-20s %-5s %-15s\n",
                pstPrd->iPrdId,
                pstPrd->stBboms.pstArray[uiIdx2].acName,
                pstPrd->stBboms.pstArray[uiIdx2].iBbomId,
                Tl_AlmPrdState2Str(pstPrd->stBboms.pstArray[uiIdx2].stOld.stBase.eState),
                Tl_AlmPrdState2Str(pstPrd->stBboms.pstArray[uiIdx2].stNew.stBase.eState),
                pstPrd->stBboms.pstArray[uiIdx2].stNew.stBase.acEnterTime,
                pstPrd->stBboms.pstArray[uiIdx2].stNew.stBase.bPerm ? "true" : "not",
                Tl_AlmMins2DayStr(pstPrd->stBboms.pstArray[uiIdx2].stNew.stBase.uiLeftMins));
        }
    }

    /* 打印BBOM值变更 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (uiIdx2 = 0; uiIdx2 < pstPrd->stBboms.uiNum; ++uiIdx2)
        {
            if (!pstPrd->stBboms.pstArray[uiIdx2].bValueChange)
            {
                continue;
            }

            /* 打印头 */
            if (!bTmp)
            {
                LOG("\n\t%s\n", "Notify: Bbom_Value_Change");

                LOG("\t%-10s %-20s %-10s %-10s %-10s\n",
                    "PrdId", "Name", "Id", "OldVal", "NewVal");

                LOG("\t%-10s %-20s %-10s %-10s %-10s\n",
                    "----------", "--------------------",
                    "----------", "----------", "----------");
                bTmp = ALM_TRUE;
            }

            /* 打印条目 */
            LOG("\t%-10d %-20s %-10d %-10llu %-10llu\n",
                pstPrd->iPrdId,
                pstPrd->stBboms.pstArray[uiIdx2].acName,
                pstPrd->stBboms.pstArray[uiIdx2].iBbomId,
                pstPrd->stBboms.pstArray[uiIdx2].stOld.ulVal,
                pstPrd->stBboms.pstArray[uiIdx2].stNew.ulVal);
        }
    }

    /* 打印License状态变更 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (uiIdx2 = 0; uiIdx2 < pstPrd->stLics.uiNum; ++uiIdx2)
        {
            if (!pstPrd->stLics.pstArray[uiIdx2].bStateChange)
            {
                continue;
            }

            /* 打印头 */
            if (!bTmp)
            {
                LOG("\n\t%s\n", "Notify: Lic_State_Change");

                LOG("\t%-10s %-20s %-5s %-15s %-15s %-10s %-20s %-5s %-15s\n",
                    "PrdId", "Lsn", "Type", "Old", "New", "Recover", "Enter",
                    "Perm", "Left");

                LOG("\t%-10s %-20s %-5s %-15s %-15s %-10s %-20s %-5s %s\n",
                    "----------", "--------------------", "-----",
                    "---------------", "---------------",
                    "----------", "--------------------",
                    "-----", "---------------");
                bTmp = ALM_TRUE;
            }

            /* 打印条目 */
            LOG("\t%-10d %-20s %-5s %-15s %-15s %-10s %-20s %-5s %-15s <-- Rsn:%s\n",
                pstPrd->iPrdId,
                pstPrd->stLics.pstArray[uiIdx2].acLsn,
                Tl_AlmLicType2Str(pstPrd->stLics.pstArray[uiIdx2].eLicType),
                ALMLicState2Str_M(pstPrd->stLics.pstArray[uiIdx2].stOld),
                ALMLicState2Str_M(pstPrd->stLics.pstArray[uiIdx2].stNew),
                pstPrd->stLics.pstArray[uiIdx2].bRecoverUsing ? "yes" : "not",
                pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.acEnterTime,
                pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.bPerm ? "true" : "false",
                Tl_AlmMins2DayStr(pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.uiLeftMins),
                ALMRsn2Str_M(pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.ulStateRsn));
        }
    }

    /* 打印License校验项 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (bTmp = ALM_FALSE, uiIdx2 = 0; uiIdx2 < pstPrd->stLics.uiNum; ++uiIdx2)
        {
            if (pstPrd->stLics.pstArray[uiIdx2].bVerifyChange)
            {
                /* 打印头 */
                if (!bTmp)
                {
                    LOG("\n\t%s\n", "Notify: Lic_Verify_Change");
                    LOG("\t%-10s %-20s\n\t%-10s %-20s\n", "PrdId", "Lsn",
                        "----------",  "--------------------");
                    bTmp = ALM_TRUE;
                }

                /* 打印条目 */
                LOG("\t%-10d %-20s <-- Rsn:%s ",
                    pstPrd->iPrdId,
                    pstPrd->stLics.pstArray[uiIdx2].acLsn,
                    ALMRsn2Str_M(pstPrd->stLics.pstArray[uiIdx2].stNew.stBase.ulStateRsn));
            }
        }
    }

    /* 打印License Bbom状态变更 */
    for (bTmp = ALM_FALSE, uiIdx = 0; uiIdx < pstChanges->uiNum; ++uiIdx)
    {
        pstPrd = &pstChanges->pstArray[uiIdx];

        for (uiIdx2 = 0; uiIdx2 < pstPrd->stLicBboms.uiNum; ++uiIdx2)
        {
            /* 打印头 */
            if (!bTmp)
            {
                LOG("\n\t%s\n", "Notify: Lic_Bbom_State_Change");

                LOG("\t%-10s %-10s %-20s %-6s %-10s %-10s %-15s\n"
                    "\t%-10s %-10s %-20s %-6s %-10s %-10s %-15s\n",
                    "PrdId", "BbomId", "BbomName", "SameId", "OldState",
                    "NewState", "Left",
                    "----------", "----------", "--------------------",
                    "------", "----------", "----------", "---------------");

                bTmp = ALM_TRUE;
            }

#if R5C10_VER_CHECK_LESS(2,16)
            /* 打印条目 */
            LOG("\t%-10d %-10d %-20s %-6d %-10s %-10s %-15s <-- Rsn:%s\n",
                pstPrd->iPrdId,
                pstPrd->stLicBboms.pstArray[uiIdx2].iBbomId,
                pstPrd->stLicBboms.pstArray[uiIdx2].pstBbomInfo->acBBomName,
                pstPrd->stLicBboms.pstArray[uiIdx2].pstBbomInfo->uiSameNameId,
                Tl_AlmState2Str(pstPrd->stLicBboms.pstArray[uiIdx2].stOld.eState),
                Tl_AlmState2Str(pstPrd->stLicBboms.pstArray[uiIdx2].stNew.eState),
                Tl_AlmMins2DayStr(pstPrd->stLicBboms.pstArray[uiIdx2].stNew.uiLeftMins),
                ALMRsn2Str_M(pstPrd->stLicBboms.pstArray[uiIdx2].stNew.ulStateRsn));
#endif
#if R5C10_VER_CHECK_MORE(2,16)
            /* 打印条目 */
            LOG("\t%-10d %-10d %-20s %-6d %-10s %-10s %-15s <-- Rsn:%s\n",
                pstPrd->iPrdId,
                pstPrd->stLicBboms.pstArray[uiIdx2].iBbomId,
                pstPrd->stLicBboms.pstArray[uiIdx2].pstBbomInfo->pcBBomName,
                pstPrd->stLicBboms.pstArray[uiIdx2].pstBbomInfo->uiSameNameId,
                Tl_AlmState2Str(pstPrd->stLicBboms.pstArray[uiIdx2].stOld.eState),
                Tl_AlmState2Str(pstPrd->stLicBboms.pstArray[uiIdx2].stNew.eState),
                Tl_AlmMins2DayStr(pstPrd->stLicBboms.pstArray[uiIdx2].stNew.uiLeftMins),
                ALMRsn2Str_M(pstPrd->stLicBboms.pstArray[uiIdx2].stNew.ulStateRsn));
#endif
        }
    }

    LOG("\n%s\n\n\n", "Notify_Change_End");

    return ALM_OK;
}

/* ALM告警回调 */
ALM_INT32 Adapter_Alarm(CONST ALM_ALARM_PRD_CHANGE_STRU* pstArray,
                        ALM_UINT32 uiNum)
{
    ALM_UINT32 uiIdx;
    ALM_UINT32 uiIdx2;
    CONST ALM_ALARM_PRD_CHANGE_STRU* pstAlarmChangeTem = NULL;
    ALM_ALARM_LK_STRU* pstRecoveryLkAlarmTem = NULL;
    ALM_ALARM_LK_STRU* pstFaultLkAlarmTem = NULL;
    ALM_ALARM_BASE_STRU* pstEmAlarmTem = NULL;
    ALM_ALARM_BASE_STRU* pstStickAlarmTem = NULL;
    ALM_ALARM_FEATURE_STRU* pstFeatureAlarmTem = NULL;
    ALM_ALARM_BASE_FEATURE_STRU* pstSingleFeatureTem = NULL;

#define RSN_MAX_LEN 256

    ALM_CHAR acRsn[RSN_MAX_LEN + 1] = {0};

    if (pstArray == NULL || uiNum == 0)
    {
        return ALM_ERR_NULL_PTR;
    }

    for (uiIdx = 0; uiIdx < uiNum; uiIdx++)
    {
        pstAlarmChangeTem = pstArray + uiIdx;

        if (NULL != pstAlarmChangeTem)
        {
            /*打印告警的产品状态*/
            LOG("**************** Alarm Prd  ***********\n");
            LOG("id: %d\n", pstAlarmChangeTem->iPrdId);
            LOG("id: %d\n", pstAlarmChangeTem->pstPrdState->iPrdId);
            LOG("EnterTime: %s\n", pstAlarmChangeTem->pstPrdState->stBase.acEnterTime);
            LOG("Lsn: %s\n", pstAlarmChangeTem->pstPrdState->stBase.acLsn);
            LOG("Perm: %d\n", pstAlarmChangeTem->pstPrdState->stBase.bPerm);
            LOG("State: %s\n", Tl_AlmPrdState2Str(pstAlarmChangeTem->pstPrdState->stBase.eState));
            LOG("LeftTime: %s\n", Tl_AlmMins2DayStr(pstAlarmChangeTem->pstPrdState->stBase.uiLeftMins));
            LOG("**************** Alarm Prd  ***********\n\n");

            /*打印告警文件状态*/
            if (NULL != &pstArray->stAlarm.stLkAlarm)
            {
                if (NULL != pstArray->stAlarm.stLkAlarm.pstRecoveryLkAlarm)
                {
                    pstRecoveryLkAlarmTem = pstArray->stAlarm.stLkAlarm.pstRecoveryLkAlarm;

                    LOG("***************** Lic_Recovery **********\n");
                    LOG("acGraceEnterTime: %s\n", pstRecoveryLkAlarmTem->acGraceEnterTime);
                    LOG("acGraceDeadLine: %s\n", pstRecoveryLkAlarmTem->acGraceDeadLine);
                    LOG("AlarmType: %s\n", Tl_AlmAlarmType2Str(pstRecoveryLkAlarmTem->stBaseAlarm.eAlarmType));
                    LOG("AlarmName: %s\n", Tl_AlmAlarmName2Str(pstRecoveryLkAlarmTem->stBaseAlarm.eAlarmName));
                    LOG("AlarmLevel: %d\n", pstRecoveryLkAlarmTem->stBaseAlarm.eAlarmLevel);
                    LOG("AlarmReason: %s\n",
                        Tl_AlmAlarmRsnToStr(pstRecoveryLkAlarmTem->stBaseAlarm.ulAlarmReason, acRsn, RSN_MAX_LEN + 1));
                    LOG("State: %s\n", Tl_AlmState2Str(pstRecoveryLkAlarmTem->stBaseAlarm.eState));
                    LOG("EnterTime: %s\n", pstRecoveryLkAlarmTem->stBaseAlarm.acStateStartTime);
                    LOG("LeftTime: %s\n", Tl_AlmMins2DayStr(pstRecoveryLkAlarmTem->stBaseAlarm.uiStateLeftMins));
                    LOG("Lsn: %s\n", pstRecoveryLkAlarmTem->stBaseAlarm.acLsn);
                    LOG("***************** Lic_Recovery **********\n\n");


                }

                if (NULL != pstArray->stAlarm.stLkAlarm.pstFaultLkAlarm)
                {
                    pstFaultLkAlarmTem = pstArray->stAlarm.stLkAlarm.pstFaultLkAlarm;

                    LOG("***************** Lic_Fault **********\n");
                    LOG("acGraceEnterTime: %s\n", pstFaultLkAlarmTem->acGraceEnterTime);
                    LOG("acGraceDeadLine: %s\n", pstFaultLkAlarmTem->acGraceDeadLine);
                    LOG("AlarmType: %s\n", Tl_AlmAlarmType2Str(pstFaultLkAlarmTem->stBaseAlarm.eAlarmType));
                    LOG("AlarmName: %s\n", Tl_AlmAlarmName2Str(pstFaultLkAlarmTem->stBaseAlarm.eAlarmName));
                    LOG("AlarmLevel: %d\n", pstFaultLkAlarmTem->stBaseAlarm.eAlarmLevel);
                    LOG("AlarmReason: %s\n",
                        Tl_AlmAlarmRsnToStr(pstFaultLkAlarmTem->stBaseAlarm.ulAlarmReason, acRsn, RSN_MAX_LEN + 1));
                    LOG("State: %s\n", Tl_AlmState2Str(pstFaultLkAlarmTem->stBaseAlarm.eState));
                    LOG("EnterTime: %s\n", pstFaultLkAlarmTem->stBaseAlarm.acStateStartTime);
                    LOG("LeftTime: %s\n", Tl_AlmMins2DayStr(pstFaultLkAlarmTem->stBaseAlarm.uiStateLeftMins));
                    LOG("Lsn: %s\n", pstFaultLkAlarmTem->stBaseAlarm.acLsn);
                    LOG("***************** Lic_Fault **********\n");

                }
            }

            if (NULL != pstArray->stAlarm.pstEmAlarm)
            {
                pstEmAlarmTem = pstArray->stAlarm.pstEmAlarm;

                LOG("***************** EM **********\n");
                LOG("AlarmType: %s\n", Tl_AlmAlarmType2Str(pstEmAlarmTem->eAlarmType));
                LOG("AlarmName: %s\n", Tl_AlmAlarmName2Str(pstEmAlarmTem->eAlarmName));
                LOG("AlarmLevel: %d\n", pstEmAlarmTem->eAlarmLevel);
                LOG("AlarmReason: %s\n",
                    Tl_AlmAlarmRsnToStr(pstEmAlarmTem->ulAlarmReason, acRsn, RSN_MAX_LEN + 1));
                LOG("State: %s\n", Tl_AlmState2Str(pstEmAlarmTem->eState));
                LOG("EnterTime: %s\n", pstEmAlarmTem->acStateStartTime);
                LOG("LeftTime: %s\n", Tl_AlmMins2DayStr(pstEmAlarmTem->uiStateLeftMins));
                LOG("Lsn: %s\n", pstEmAlarmTem->acLsn);
                LOG("***************** EM **********\n\n");
            }

            if (NULL != pstArray->stAlarm.pstStickAlarm)
            {
                pstStickAlarmTem = pstArray->stAlarm.pstStickAlarm;

                LOG("***************** STICK **********\n");
                LOG("AlarmType: %s\n", Tl_AlmAlarmType2Str(pstStickAlarmTem->eAlarmType));
                LOG("AlarmName: %s\n", Tl_AlmAlarmName2Str(pstStickAlarmTem->eAlarmName));
                LOG("AlarmLevel: %d\n", pstStickAlarmTem->eAlarmLevel);
                LOG("AlarmReason: %s\n",
                    Tl_AlmAlarmRsnToStr(pstStickAlarmTem->ulAlarmReason, acRsn, RSN_MAX_LEN + 1));
                LOG("State: %s\n", Tl_AlmState2Str(pstStickAlarmTem->eState));
                LOG("EnterTime: %s\n", pstStickAlarmTem->acStateStartTime);
                LOG("LeftTime: %s\n", Tl_AlmMins2DayStr(pstStickAlarmTem->uiStateLeftMins));
                LOG("Lsn: %s\n", pstStickAlarmTem->acLsn);
                LOG("***************** STICK **********\n\n");
            }


            if (NULL != pstArray->stAlarm.pstFeatureAlarms)
            {
                for (uiIdx2 = 0; uiIdx2 < pstArray->stAlarm.uiFeatureNum; uiIdx2++)
                {
                    pstFeatureAlarmTem = pstArray->stAlarm.pstFeatureAlarms + uiIdx2;

                    if (NULL != pstFeatureAlarmTem->pstRecoveryFeature)
                    {
                        pstSingleFeatureTem = pstFeatureAlarmTem->pstRecoveryFeature;

                        LOG("***************** FEATURE_RECOVERY_START**********\n");
                        LOG("Feature Name: %s\n", pstFeatureAlarmTem->acFeatureName);
                        LOG("Feature DeadLine: %s\n", pstFeatureAlarmTem->acFeatureDeadLine);
                        LOG("Feature ID: %d\n", pstFeatureAlarmTem->uiFeatureId);
                        LOG("AlarmType: %s\n", Tl_AlmAlarmType2Str(pstSingleFeatureTem->eAlarmType));
                        LOG("AlarmName: %s\n", Tl_AlmAlarmName2Str(pstSingleFeatureTem->eAlarmName));
                        LOG("AlarmLevel: %d\n", pstSingleFeatureTem->eAlarmLevel);
                        LOG("AlarmReason: %s\n",
                            Tl_AlmAlarmRsnToStr(pstSingleFeatureTem->ulAlarmReason, acRsn, RSN_MAX_LEN + 1));
                        LOG("State: %s\n", Tl_AlmState2Str(pstSingleFeatureTem->eFeatureState));
                        LOG("LeftTime: %s\n", Tl_AlmMins2DayStr(pstSingleFeatureTem->uiStateLeftMins));
                        LOG("acLsn: %s\n", pstSingleFeatureTem->acLsn);
                        LOG("***************** FEATURE_RECOVERY_END **********\n\n");

                    }

                    if (NULL != pstFeatureAlarmTem->pstFaultFeature)
                    {
                        pstSingleFeatureTem = pstFeatureAlarmTem->pstFaultFeature;

                        LOG("***************** FEATURE_FAULT **********\n");
                        LOG("Feature Name: %s\n", pstFeatureAlarmTem->acFeatureName);
                        LOG("Feature DeadLine: %s\n", pstFeatureAlarmTem->acFeatureDeadLine);
                        LOG("Feature ID: %d\n", pstFeatureAlarmTem->uiFeatureId);
                        LOG("AlarmType: %s\n", Tl_AlmAlarmType2Str(pstSingleFeatureTem->eAlarmType));
                        LOG("AlarmName: %s\n", Tl_AlmAlarmName2Str(pstSingleFeatureTem->eAlarmName));
                        LOG("AlarmLevel: %d\n", pstSingleFeatureTem->eAlarmLevel);
                        LOG("AlarmReason: %s\n",
                            Tl_AlmAlarmRsnToStr(pstSingleFeatureTem->ulAlarmReason, acRsn, RSN_MAX_LEN + 1));
                        LOG("State: %s\n", Tl_AlmState2Str(pstSingleFeatureTem->eFeatureState));
                        LOG("LeftTime: %s\n", Tl_AlmMins2DayStr(pstSingleFeatureTem->uiStateLeftMins));
                        LOG("acLsn: %s\n", pstSingleFeatureTem->acLsn);
                        LOG("***************** FEATURE_FAULT **********\n\n");
                    }

                }
            }
        }
    }

    return ALM_OK;
}

/* 将原因附加操作转换为原因字符串 */
static CONST ALM_CHAR* ALMDbgRsnAndOptToRsnStr(
    ALM_ENUM(ALM_STATE_OPERATE_ENUM) eOperate,
    ALM_UINT64 ulRsn,
    OUT ALM_CHAR* pcRsn,
    ALM_UINT32 uiRsnSize)
{
    if (NULL == pcRsn || 0 == uiRsnSize)
    {
        return NULL;
    }

    Tl_AlmRsnToStr(ulRsn, pcRsn, uiRsnSize);

    (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize,  ", by ");

    switch (eOperate)
    {
            /* 激活License */
        case ALM_STATE_OPERATE_ACTIVE_LIC:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "active license");
            break;

            /* 失效License文件 */
        case ALM_STATE_OPERATE_REVOKE_LIC:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "revoke license");
            break;

            /* 启动紧急 */
        case ALM_STATE_OPERATE_START_EMERGENCY:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "start emergency");
            break;

            /* 停止紧急 */
        case ALM_STATE_OPERATE_STOP_EMERGENCY:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "stop emergency");
            break;

            /* 启动Stick */
        case ALM_STATE_OPERATE_START_STICK:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "start stick");
            break;

            /* 停止Stick */
        case ALM_STATE_OPERATE_STOP_STICK:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "stop stick");
            break;

            /* 执行恢复动作 */
        case ALM_STATE_OPERATE_RECOVER:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "recover");
            break;

            /* 刷新状态 */
        case ALM_STATE_OPERATE_REFRESH:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "flash");
            break;

        case ALM_STATE_OPERATE_DISABLE:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "disable");
            break;

        default:
            (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "unknow operator");
            break;
    }

    (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, ".");

    return pcRsn;
}
