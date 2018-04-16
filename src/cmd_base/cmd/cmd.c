#include "cmd.inc"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/***************************************************************************
* 描述: 初始化.
***************************************************************************/
void CmdM_Start()
{
    if (!s_bHaveInit)
    {
        //初始化命令。
        CmdM_AddCmds(g_astDfltCmd);

        s_bHaveInit = true;
    }
}

/***************************************************************************
* 描述: 使用CmdFnSt对象设置用户命令。
***************************************************************************/
void CmdM_AddCmds(CmdFnSt* pstCmds)
{
    CmdFnSt* pstNew;
    int iCntNum = 0;
    int iNewNum = 0;
    int idx;
    int idx2;
    CmdFnSt stTemp;

    if (NULL == pstCmds)
    {
        LOG_INFO("传入的用户命令为空。");
        return;
    }

    //得到命令数目
    while (pstCmds[iNewNum].pfn != NULL)
    {
        ++iNewNum;
    }

    if (0 == iNewNum)
    { return; }

    //初始化命令。
    _CmdM_InitCmds(pstCmds);

    //扩展内存，末位保留一个空位。
    while (NULL != s_astCmds && NULL != s_astCmds[iCntNum].pfn)
    { ++iCntNum; }

    pstNew = (CmdFnSt*)malloc(sizeof(CmdFnSt) * (iCntNum + iNewNum + 1));

    if (iCntNum > 0)
    { memcpy(pstNew, s_astCmds, sizeof(CmdFnSt) * iCntNum); }

    memcpy(pstNew + iCntNum, pstCmds, sizeof(CmdFnSt) * iNewNum);
    pstNew[iCntNum + iNewNum].pfn = NULL;

    //命令按组由小到大排序。
    iCntNum += iNewNum;

    for (idx = 0; idx < iCntNum; ++idx)
    {
        for (idx2 = idx + 1; idx2 < iCntNum; ++idx2)
        {
            if (pstNew[idx2].iGroup < pstNew[idx].iGroup)
            {
                memcpy(&stTemp, &pstNew[idx2], sizeof(CmdFnSt));
                memcpy(&pstNew[idx2], &pstNew[idx], sizeof(CmdFnSt));
                memcpy(&pstNew[idx], &stTemp, sizeof(CmdFnSt));
            }
        }
    }

    Util_Free(s_astCmds);
    s_astCmds = pstNew;
}

/***************************************************************************
* 描述: 使用CmdFnDefParaSt对象设置用户命令。
***************************************************************************/
void CmdM_AddCmdsByDefPara(CmdFnDefParaSt* pstCmds, unsigned int uiNum)
{
    CmdFnSt* pstCmdsTemp = NULL;
    unsigned int uiIdx;
    unsigned int uiIdxPara;

    UTIL_CHK(uiNum >= 0 && uiNum < 10000);

    if (0 == uiNum)
    {
        return;
    }

    //将CmdFnDefParaSt转会为CmdFnSt
    pstCmdsTemp = (CmdFnSt*)malloc(sizeof(CmdFnSt) * (uiNum + 1));
    UTIL_CHK(pstCmdsTemp != NULL);

    memset(pstCmdsTemp, 0, sizeof(CmdFnSt) * (uiNum + 1));

    for (uiIdx = 0; uiIdx < uiNum; ++uiIdx)
    {
        pstCmdsTemp[uiIdx].iGroup = pstCmds[uiIdx].iGroup;
        pstCmdsTemp[uiIdx].pcCmdStr = pstCmds[uiIdx].pcCmdStr;
        pstCmdsTemp[uiIdx].pcDesc = pstCmds[uiIdx].pcDesc;
        pstCmdsTemp[uiIdx].pcTitle = pstCmds[uiIdx].pcTitle;
        pstCmdsTemp[uiIdx].pfn = pstCmds[uiIdx].pfn;
        pstCmdsTemp[uiIdx].stParasDesc.pstParas = pstCmds[uiIdx].astParas;

        for (uiIdxPara = 0; uiIdxPara < sizeof(pstCmds[uiIdx].astParas); ++uiIdxPara)
        {
            if (NULL == pstCmds[uiIdx].astParas[uiIdxPara].pcName)
            {
                break;
            }
        }

        pstCmdsTemp[uiIdx].stParasDesc.iNum = uiIdxPara;
    }

    //添加命令。
    CmdM_AddCmds(pstCmdsTemp);

    free(pstCmdsTemp);
}


/***************************************************************************
* 描述: 清除初始化.
***************************************************************************/
void CmdM_Close()
{
    if (s_bHaveInit)
    {
        s_bHaveInit = false;
        Util_Free(s_astCmds);
        s_astCmds = NULL;
    }

    s_bRunContinue = false;
}

/***************************************************************************
* 描述: 停止接收命令.
***************************************************************************/
void CmdM_Exit()
{
    s_bRunContinue = false;
}

/***************************************************************************
* 描述: 使用变参合成命令后再执行。
***************************************************************************/
int CmdM_ExecVa(bool bQuiet, void* pData, const char* pcCmdFmt, ...)
{
    char acCmdStr[MAX_CMD_LEN + 1] = {0};
    va_list args;

    va_start(args, pcCmdFmt);
    vsprintf(acCmdStr, pcCmdFmt, args);
    va_end(args);

    return CmdM_Exec(acCmdStr, pData, bQuiet);
}

/***************************************************************************
* 描述: 执行命令脚本。
***************************************************************************/
int CmdM_ExecFile(const char* pcCmdFile, bool bQuiet)
{
    int iRet;
    char* pcFileStr = NULL;
    char* pcCmd;
    char* pcCmdNext;
    int iCntLine = 0;

    //注释字符。
#define CMDM_EXEC_CHK_NOTE(str) ('/' == *str && '/' == *(str+1))

    //1. 加载命令文件。
    iRet = Util_ReadFileToBuffer(pcCmdFile, &pcFileStr, NULL);
    UTIL_CALL_FN_LAB(Util_ReadFileToBuffer, iRet, LAB_END);

    if (NULL == pcFileStr)
    {
        return RET_OK;
    }

    //2. 执行文件。

    //2.1 得到命令行。
    pcCmdNext = pcFileStr;

    while ('\0' != *pcCmdNext)
    {
        pcCmd = pcCmdNext;

        //找到命令尾部。
        while ('\0' != *pcCmdNext && '\n' != *pcCmdNext && '\r' != *pcCmdNext)
        { ++pcCmdNext; }

        //添加字符串结束字符到命令尾部，并迭代至下一命令。
        if ('\0' != *pcCmdNext)
        {
            *pcCmdNext++ = '\0';

            while ('\0' != *pcCmdNext && UTIL_IS_EMPTY_SYB(*pcCmdNext))
            {
                if ('\n' == *pcCmdNext)
                { ++iCntLine; }

                ++pcCmdNext;
            }
        }

        //2.2 若非注释，则执行该行命令。
        if (!CMDM_EXEC_CHK_NOTE(pcCmd) && !Util_StrChkEmpty(pcCmd))
        {
            iRet = CmdM_Exec(pcCmd, NULL, bQuiet);
            UTIL_CALL_FN_LAB(CmdM_Exec, iRet, LAB_END);
        }
    }

LAB_END:

    if (RET_OK != iRet)
    { LOG_INFO("执行第%d行命令出现错误。", iCntLine); }

    Util_Free(pcFileStr);
    return iRet;
}


/***************************************************************************
* 定义命令处理函数。
***************************************************************************/
int CmdM_Exec(const char* pcCmd, void* pData, bool bQuiet)
{
    int iRet = RET_OK;
    int iCmdLen = 0;
    const CmdFnSt* pstCmdDesc = NULL;
    ParasSt stParas = {0};
    bool bShowHit;

    if (NULL == pcCmd)
    {
        LOG_ERR("无效的命令:%s", "空命令");
        return RET_ERR;
    }

    /***********************************************************
    * 去除前序空格。
    ***********************************************************/
    while ('\0' != *pcCmd && UTIL_IS_EMPTY_SYB(*pcCmd))
    { ++pcCmd; }

    iCmdLen = strlen(pcCmd);

    if (0 == iCmdLen)
    {
        LOG_ERR("无效的命令:%s", "空命令");
        return RET_ERR;
    }

    /***********************************************************
    * 查找命令。会自动以空白作为分隔。
    ***********************************************************/
    pstCmdDesc = CmdM_CmdGet(pcCmd, iCmdLen);

    if (NULL == pstCmdDesc)
    {
        LOG(FMT_SEG1_E);
        LOG_INFO("命令\"%s\"不存在。", pcCmd);
        LOG(FMT_SEG1_E);

        //显示以这些字符串起始的命令。
        if (!bQuiet)
        {
            LOG("参考:\n");
            pstCmdDesc = CmdM_CmdGetSimilar(pcCmd, iCmdLen, NULL);

            while (NULL != pstCmdDesc)
            {
                LOG("-> %s : %s。\n",
                    pstCmdDesc->pcCmdStr,
                    pstCmdDesc->pcTitle);
                pstCmdDesc = CmdM_CmdGetSimilar(pcCmd, iCmdLen, pstCmdDesc);
            }
        }

        return RET_ERR;
    }

    pcCmd += strlen(pstCmdDesc->pcCmdStr);

    /***********************************************************
    * 若第一个参数为-h，显示帮助信息。
    ***********************************************************/
    while (UTIL_IS_EMPTY_SYB(*pcCmd))
    { ++pcCmd; }

    if (0 == Util_StrNCmpIgrUp(pcCmd, "-h", sizeof("-h")))
    {
        CmdM_ExecVa(false, NULL, "%s %s", CMD_HELP, pstCmdDesc->pcCmdStr);
        return RET_OK;
    }

    /***********************************************************
    * 执行命令。
    ***********************************************************/

    //获取是否显示提示信息的配置，默认不显示。
    bShowHit = Cfg_GetInt(CFG_CMD_HIT, &iRet);
    bShowHit = bShowHit ? (bool)iRet : false;

    //解析参数。
    iRet = Para_Parse(&pstCmdDesc->stParasDesc, pcCmd, &stParas);

    if (pstCmdDesc->stParasDesc.iNum != iRet)
    {
        Para_Cls(&stParas);
        LOG_INFO("解析参数失败。");
        CmdM_ExecVa(false, NULL, "%s %s", CMD_HELP, pstCmdDesc->pcCmdStr);
        return RET_ERR;
    }

    //显示命令详细信息。
    if (!bQuiet)
    {
        LOG(FMT_SEG_CMD_BG);

        if (bShowHit)
        {
            //LOG_FIX_LINE("命  令: %s \n", pstCmdDesc->pcCmdStr);
            Para_Show(pstCmdDesc->pcCmdStr, &stParas);
            LOG_FIX_LINE("描  述: %s。\n", pstCmdDesc->pcTitle);
            LOG(FMT_SEG1_E);
        }
    }

    //执行命令，并清理参数。
    iRet = pstCmdDesc->pfn(&stParas, pData);
    Para_Cls(&stParas);

    //显示结果。
    if (!bQuiet)
    {
        if (bShowHit)
        {
            LOG("\n"FMT_SEG1_E);
            LOG("返回值: 0x%X。\n", iRet);
        }

        LOG(FMT_SEG_CMD_ED);
    }

    return iRet;
}

/***************************************************************************
* 描述: 控制台或终端操作规划函数。
***************************************************************************/
void CmdM_Run(const char* apcCmdArray[], int iCmdNum, bool bMoreCmds, CmdTimeFn fnTime)
{
    char* pcCmdBuf = NULL;
    int index;

    /***********************************************************
    *初始帮助信息。
    ***********************************************************/
    LOG(FMT_SEG3_E);
    LOG("1. 输入\"%s\"列出全部命令。命令忽略大小写。\n", CMD_HELP);
    LOG("2. 输入\"命令名 -h\"查看命令详细描述。\n");
    LOG(FMT_SEG3_E);

    /***********************************************************
    * 处理自动运行
    ***********************************************************/
    if (0 != apcCmdArray && iCmdNum > 0)
    {
        s_bRunContinue = true;

        for (index = 0; index < iCmdNum && s_bRunContinue; index++)
        {
            CMDM_EXEC(apcCmdArray[index]);
        }
    }

    /***********************************************************
    * 处理后续手动命令
    ***********************************************************/
    if (bMoreCmds)
    {
        s_bRunContinue = true;
        pcCmdBuf = (char*)malloc(MAX_CMD_LEN + 1);

        while (s_bRunContinue)
        {
            if (NULL == fnTime)
            { LOG(FMT_SEG3_E); }
            else
            { LOG(FMT_SEG_TIME(fnTime)); }

            while (true)
            {
                LOG("请输入命令:");
                memset(pcCmdBuf, 0, MAX_CMD_LEN);
                Util_FileGets(pcCmdBuf, MAX_CMD_LEN, stdin);
                pcCmdBuf[MAX_CMD_LEN] = '\0';

                index = 0;

                while (UTIL_IS_EMPTY_SYB(pcCmdBuf[index]))
                { ++index; }

                if ('\0' == pcCmdBuf[index])
                { continue; }

                break;
            }

            if (NULL == fnTime)
            { LOG(FMT_SEG3_E); }
            else
            { LOG(FMT_SEG_TIME(fnTime)); }

            //查找并执行对应命令。
            CMDM_EXEC(pcCmdBuf + index);
        }

        Util_Free(pcCmdBuf);
    }
}


/***************************************************************************
* 描述: 迭代获取全部命令。输入空元素，返回首个元素。
***************************************************************************/
const CmdFnSt* CmdM_CmdItr(const CmdFnSt* pstLast)
{
    int iItr = 0;
    int iTotal = 0;

    /***********************************************************
    * 得到命令段的数目。
    ***********************************************************/
    while (NULL != s_astCmds && NULL != s_astCmds[iTotal].pfn)
    { ++iTotal; }

    /***********************************************************
    * 得到下一个命令的段及段内索引。
    ***********************************************************/
    if (NULL != pstLast)
    {
        iItr = pstLast - s_astCmds + 1;
    }
    else
    { iItr = 0; }

    if (s_astCmds[iItr].pfn != NULL)
    { return &s_astCmds[iItr]; }
    else
    { return NULL; }
}

/***************************************************************************
* 描述: 查找命令。
***************************************************************************/
const CmdFnSt* CmdM_CmdGet(const char* pcStr, int iLen)
{
    const CmdFnSt* pstRet = NULL;
    int iCmp;
    CmdMWordSt stWord;
    int iRet;

    UTIL_CHK(NULL != pcStr && iLen > 0);

    /***********************************************************
    * 忽略前序空白字符，并以空白字符作为命令与命令选项的分隔符。
    ***********************************************************/
    iRet = Para_GetWord(pcStr, iLen, &stWord);

    if (RET_OK != iRet || 0 == stWord.iWordLen)
    {
        LOG_ERR("无效的空命令。");
        return NULL;
    }

    /***********************************************************
    * 查找命令。
    ***********************************************************/
    pstRet = CmdM_CmdItr(NULL);

    while (NULL != pstRet)
    {
        iCmp = Util_StrNCmpIgrUp(stWord.pcWord,
                                 pstRet->pcCmdStr,
                                 stWord.iWordLen);

        if (0 == iCmp && '\0' == pstRet->pcCmdStr[stWord.iWordLen])
        {
            return pstRet;
        }

        pstRet = CmdM_CmdItr(pstRet);
    }

    return NULL;
}


const CmdFnSt* CmdM_CmdGetSimilar(const char* pcStr, int iLen, const CmdFnSt* pstLast)
{
    const CmdFnSt* pstRet = pstLast;
    int iCmp;
    CmdMWordSt stWord;
    int iRet;

    UTIL_CHK(NULL != pcStr && iLen > 0);

    /***********************************************************
    * 忽略前序空白字符，并以空白字符作为命令与命令选项的分隔符。
    ***********************************************************/
    iRet = Para_GetWord(pcStr, iLen, &stWord);

    if (RET_OK != iRet || 0 == stWord.iWordLen)
    {
        LOG_ERR("无效的空命令。");
        return NULL;
    }

    /***********************************************************
    * 查找命令。
    ***********************************************************/
    pstRet = CmdM_CmdItr(pstRet);

    while (NULL != pstRet)
    {
        iCmp = Util_StrNCmpIgrUp(stWord.pcWord,
                                 pstRet->pcCmdStr,
                                 stWord.iWordLen);

        if (0 == iCmp)
        {
            return pstRet;
        }

        pstRet = CmdM_CmdItr(pstRet);
    }

    return NULL;
}



/***************************************************************************
* --------------------------- 内部函数分界线 ------------------------------
***************************************************************************/
void _CmdM_InitCmds(CmdFnSt* pstCmds)
{
    int itrCmd = 0;
    ParaDescSt* pcParaTmp;
    int iParaNum;
    int idx;

    UTIL_CHK(NULL != pstCmds);

    while (pstCmds[itrCmd].pfn != NULL)
    {
        //有效性验证。
        pcParaTmp = pstCmds[itrCmd].stParasDesc.pstParas;
        iParaNum = pstCmds[itrCmd].stParasDesc.iNum;
        UTIL_CHK((0 == iParaNum) || (NULL != pcParaTmp));

        //初始化描述。
        if (NULL == pstCmds[itrCmd].pcDesc)
        {
            pstCmds[itrCmd].pcDesc = pstCmds[itrCmd].pcTitle;
        }

        //校验不存在同名命令。
        for (idx = 0; idx < itrCmd; ++idx)
        {
            UTIL_CHK(0 != strcmp(pstCmds[itrCmd].pcCmdStr, pstCmds[idx].pcCmdStr)
                     && pstCmds[itrCmd].pfn != pstCmds[idx].pfn);
        }

        ++itrCmd;
    }

}


/***************************************************************************
* 描述: 获取相似命令 -- 以指定字符串起始。
*       若pstLast传入NULL，则获取第一个相似元素，否则获取下一个。
***************************************************************************/
int Para_GetWord(const char* pcStr, int iStrLen, CmdMWordSt* pstWord)
{
    const char* pcItr = pcStr;
    int iWLen = iStrLen;
    int index;
    bool bHaveQuo;

    UTIL_CHK(NULL != pcStr && NULL != pstWord);

    if (0 == iStrLen || '\0' == *pcStr)
    {
        pstWord->pcStr = pcStr;
        pstWord->pcWord = pcStr;
        pstWord->iStrLen = 0;
        pstWord->iWordLen = 0;
        return RET_OK;
    }

    /***********************************************************
    * 得到外围字段起始。包含双引号。
    ***********************************************************/
    while (iWLen > 0 && '\0' != *pcItr && UTIL_IS_EMPTY_SYB(*pcItr))
    {
        --iWLen;
        ++pcItr;
    }

    pstWord->pcStr = pcItr;
    pstWord->pcWord = pcItr;

    //允许双引号作为字段分隔。
    bHaveQuo = false;

    if ('"' == *pcItr)
    {
        ++pcItr;
        --iWLen;
        pstWord->pcWord = pcItr;
        bHaveQuo = true;
    }

    //以空白或引号字符作为字段的分隔符.
    index = 0;

    while (index < iWLen && pcItr[index] != '\0')
    {
        if (bHaveQuo)
        {
            if ('"' == pcItr[index])
            {
                pstWord->iStrLen = index + 1 + 1;
                pstWord->iWordLen = index;
                return RET_OK;
            }
        }
        else
        {
            if (UTIL_IS_EMPTY_SYB(pcItr[index]))
            {
                pstWord->iStrLen = index;
                pstWord->iWordLen = index;
                return RET_OK;
            }
        }

        ++index;
    }

    //到了字符串末尾。
    if (bHaveQuo)
    { pstWord->iStrLen = index + 1; }
    else
    { pstWord->iStrLen = index; }

    pstWord->iWordLen = index;

    return RET_OK;
}
