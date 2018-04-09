#include "para.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include "cfg.h"

//失败返回-1，成功返回解析出参数数目。
int Para_Parse(const ParasDescSt* pstDesc, const char* pcStr, ParasSt* pstParas)
{
    const char* pcItrStr = pcStr;
    int iStrLen;
    int index;
    CmdMWordSt stWord;
    int iRet;
    const char* pcVal;

    if ((NULL == pstParas || NULL == pcStr)
        && NULL != pstDesc
        && pstDesc->iNum > 0)
    { return -1; }
    else if (NULL == pstDesc || 0 == pstDesc->iNum)
    {
        memset(pstParas, 0, sizeof(ParasSt));
        return 0;
    }

    iStrLen = strlen(pcItrStr);

    /***********************************************************
    * 遍历获取每一个参数。多出部分截断处理。
    ***********************************************************/
    memset(pstParas, 0, sizeof(ParasSt));
    pstParas->pstParas = (ParaSt*)malloc(pstDesc->iNum * sizeof(ParaSt));
    memset(pstParas->pstParas, 0, pstDesc->iNum * sizeof(ParaSt));

    for (index = 0; index < pstDesc->iNum; ++index)
    {
        iRet = Para_GetWord(pcItrStr, iStrLen, &stWord);

        if (RET_OK != iRet)
        {
            LOG_ERR("获取字段失败。");
            goto ERR_LAB;
        }

        //解析单个参数。
        if (0 != stWord.iWordLen)
        {
            pstParas->pstParas[index].stVal.eType = pstDesc->pstParas[index].eType;

            if (RET_OK != Util_DataWriteByStr(&pstParas->pstParas[index].stVal,
                                              stWord.pcWord,
                                              stWord.iWordLen))
            { goto ERR_LAB; }

            _Para_ChangeNullStr(&pstParas->pstParas[index].stVal);
        }
        else
        { break; }

        //迭代
        iStrLen -= (stWord.pcStr + stWord.iStrLen - pcItrStr);
        pcItrStr = stWord.pcStr + stWord.iStrLen;
        ++(pstParas->iNum);
    }

    /***********************************************************
    * 后续未赋值参数填充默认值。
    ***********************************************************/
    for (; index < pstDesc->iNum; ++index)
    {
        pstParas->pstParas[index].stVal.eType =
            pstDesc->pstParas[index].eType;

        if (NULL != pstDesc->pstParas[index].pcAssioCfg)
        {
            pcVal = Cfg_GetStr(pstDesc->pstParas[index].pcAssioCfg);

            if (NULL == pcVal)
            { pcVal = "NULL"; }
        }
        else if (NULL != pstDesc->pstParas[index].pcDflt)
        {
            pcVal = pstDesc->pstParas[index].pcDflt;
        }
        else
        { break; }

        iRet = Util_DataWriteByStr(&pstParas->pstParas[index].stVal,
                                   pcVal,
                                   strlen(pcVal));
        UTIL_CALL_FN_LAB(Util_DataWriteByStr, iRet, ERR_LAB);

        _Para_ChangeNullStr(&pstParas->pstParas[index].stVal);
        ++(pstParas->iNum);
    }

    return pstParas->iNum;

ERR_LAB:

    Para_Cls(pstParas);
    return -1;
}

/***************************************************************************
* 描述: 参数清理
***************************************************************************/
void Para_Cls(ParasSt* pstParas)
{
    int index;

    if (NULL == pstParas)
    { return; }

    if (NULL == pstParas->pstParas)
    {
        pstParas->iNum = 0;
        return;
    }

    for (index = 0; index < pstParas->iNum; ++index)
    {
        if (NULL != pstParas->pstParas[index].stVal.pData)
        {
            Util_Free(pstParas->pstParas[index].stVal.pData);
            pstParas->pstParas[index].stVal.pData = NULL;
        }
    }

    Util_Free(pstParas->pstParas);
    pstParas->pstParas = NULL;
    pstParas->iNum = 0;
}


/***************************************************************************
* 描述: 参数校验函数。
***************************************************************************/
bool Para_Chk(const ParasDescSt* pstDesc, ParasSt* pPara)
{
    int index;

    if (NULL == pPara)
    {
        if (0 == pstDesc->iNum)
        { return true; }
        else
        { return false; }
    }

    if (pPara->iNum != pstDesc->iNum)
    { return false; }
    else if (pPara->iNum > 0 && NULL == pPara->pstParas)
    { return false; }

    for (index = 0; index < pPara->iNum ; ++index)
    {
        if (pPara->pstParas[index].stVal.eType
            != pstDesc->pstParas[index].eType)
        { return false; }

        //字符串可以传入NULL
        if (NULL == pPara->pstParas[index].stVal.pData
            && TYPE_STR != pPara->pstParas[index].stVal.eType)
        { return false; }
    }

    return true;
}


/***************************************************************************
* 描述: 从参数中获取一个整数。
***************************************************************************/
int* Para_GetInt(ParasSt* pstParas, int iParaIdx)
{
    int index = iParaIdx - 1;

    if (NULL == pstParas || pstParas->iNum < iParaIdx || iParaIdx < 0)
    {
        return NULL;
    }

    UTIL_CHK_DESC(TYPE_INT == pstParas->pstParas[index].stVal.eType,
                  "参数类型不匹配");

    return (int*)pstParas->pstParas[index].stVal.pData;
}

/***************************************************************************
* 描述: 从参数中获取一个字符串。
***************************************************************************/
char* Para_GetStr(ParasSt* pstParas, int iParaIdx)
{
    int index = iParaIdx - 1;

    if (NULL == pstParas || pstParas->iNum < iParaIdx || iParaIdx < 0)
    {
        return NULL;
    }

    UTIL_CHK_DESC(TYPE_STR == pstParas->pstParas[index].stVal.eType,
                  "参数类型不匹配");

    return (char*)pstParas->pstParas[index].stVal.pData;
}

/***************************************************************************
* 描述: 打印参数。
***************************************************************************/
void Para_Show(const char* pcFuncName, ParasSt* pstParas)
{
    int index;
    char acBuf[1024] = {0};
    int iLen = 0;

    if (NULL != pcFuncName)
    {
        iLen += sprintf(acBuf + iLen, "命  令: %s ", pcFuncName);
    }

    if (NULL == pstParas || NULL == pstParas->pstParas)
    {
        sprintf(acBuf + iLen, "\n");
        LOG_FIX_LINE(acBuf);
        return;
    }

    for (index = 0; index < pstParas->iNum; ++index)
    {
        if (TYPE_INT == pstParas->pstParas[index].stVal.eType)
        {
            if (NULL != pstParas->pstParas[index].stVal.pData)
            { iLen += sprintf(acBuf + iLen, "\"%d\" ", *(int*)pstParas->pstParas[index].stVal.pData); }
            else
            { iLen += sprintf(acBuf + iLen, "\"%s\" ", "NULL"); }
        }
        else if (TYPE_STR == pstParas->pstParas[index].stVal.eType)
        {
            if (NULL != pstParas->pstParas[index].stVal.pData)
            { iLen += sprintf(acBuf + iLen, "\"%s\" ", (char*)pstParas->pstParas[index].stVal.pData); }
            else
            { iLen += sprintf(acBuf + iLen, "\"%s\" ", "NULL"); }
        }
        else
        {
            LOG_ERR("无效的参数类型。");
            return;
        }
    }

    iLen += sprintf(acBuf + iLen, "\n");
    LOG_FIX_LINE(acBuf);
}


/***************************************************************************
* 描述: 对字符串类型NULL参数处理.
***************************************************************************/
static void _Para_ChangeNullStr(DataSt* pstData)
{
    UTIL_CHK(NULL != pstData);

    if (TYPE_STR == pstData->eType
        && NULL != pstData->pData)
    {
        if (0 == Util_StrCmpIgrUp((char*)pstData->pData, "NULL"))
        {
            Util_DataCls(pstData);
        }
    }
}
