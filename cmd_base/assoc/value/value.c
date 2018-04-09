#include "value.h"

#include <string.h>

static void _Am_ValSegCls(AmValSegSt* pstValSeg);

//bNullIsPos - true: NULL表示最大值。否则表示最小值。
static int _Am_ValDataCmp(DataSt* pstL,
                          bool bNullIsPosL,
                          DataSt* pstR,
                          bool bNullIsPosR);

//返回值: 0: 表示存在交集并得到合并结果；正数: new > seg；负数: new < seg.
//若取new中数值，会取其深度拷贝。
static int _Am_ValSegCombinTo(AmValSegSt* pstNew, AmValSegSt* pstSeg);


//默认为空数据范围。
int Am_ValRangeInit(AmValRangeSt* pstRange)
{
    UTIL_CHK(NULL != pstRange);

    memset(pstRange, 0, sizeof(AmValRangeSt));

    return RET_OK;
}


void Am_ValRangeCls(AmValRangeSt* pstRange)
{
    unsigned int uiItr;

    if (pstRange != NULL)
    {
        for (uiItr = 0; uiItr < pstRange->uiSegNum; ++uiItr)
        {
            _Am_ValSegCls(&pstRange->pstSegs[uiItr]);
        }

        Util_Free(pstRange->pstSegs);

        memset(pstRange, 0, sizeof(AmValRangeSt));
    }
}

int Am_ValRangeAdd(AmValRangeSt* pstRange, const AmValSegSt* pstNewSeg)
{
    unsigned int uiIdx;
    int iCmp;
    int iRet;
    AmValSegSt stTmp = {0};

    UTIL_CHK(pstNewSeg != NULL && pstRange != NULL);

    //在末尾添加.使用拷贝值。
    stTmp.bIncE = pstNewSeg->bIncE;
    stTmp.bIncS = pstNewSeg->bIncS;

    if (NULL != pstNewSeg->pstEnd)
    { stTmp.pstStart = Util_DataCpy(pstNewSeg->pstStart); }

    if (NULL != pstNewSeg->pstEnd)
    { stTmp.pstEnd = Util_DataCpy(pstNewSeg->pstEnd); }

    iRet = Util_ArrayExpandAuto((void**)&pstRange->pstSegs,
                                sizeof(AmValSegSt),
                                pstRange->uiSegNum,
                                &pstRange->uiSegCap);

    if (RET_OK != iRet)
    {
        _Am_ValSegCls(&stTmp);
        UTIL_CALL_FN_RET(Util_ArrayExpandAuto, iRet);
    }

    memcpy(&pstRange->pstSegs[pstRange->uiSegNum],
           &stTmp,
           sizeof(AmValSegSt));
    ++pstRange->uiSegNum;

    //由小到大排序。
    for (uiIdx = pstRange->uiSegNum - 1; uiIdx > 1; --uiIdx)
    {
        iCmp = _Am_ValSegCombinTo(pstRange->pstSegs + uiIdx,
                                  pstRange->pstSegs + uiIdx - 1);

        if (0 == iCmp)
        {
            //存在交集，合并结点。
            _Am_ValSegCls(pstRange->pstSegs + uiIdx);
            --pstRange->uiSegNum;
        }
        else if (iCmp < 0)
        {
            //结点对换。
            memcpy(&stTmp, pstRange->pstSegs + uiIdx, sizeof(AmValSegSt));
            memcpy(pstRange->pstSegs + uiIdx, pstRange->pstSegs + uiIdx - 1, sizeof(AmValSegSt));
            memcpy(pstRange->pstSegs + uiIdx - 1, &stTmp, sizeof(AmValSegSt));
        }
        else
        {
            //符合正序。
            break;
        }
    }

    return RET_OK;
}


//判断单个数据是否存在于数据数据范围内。
bool Am_ValRangeIncChk(AmValRangeSt* pstRange, DataSt* pstData)
{
    int iCmp;
    unsigned int uiItr;

    UTIL_CHK(pstRange != NULL && pstData != NULL && NULL != pstData->pData);

    for (uiItr = 0; uiItr < pstRange->uiSegNum; ++uiItr)
    {
        //交集判断
        iCmp = _Am_ValDataCmp(pstData, true, pstRange->pstSegs[uiItr].pstEnd, true);

        if (iCmp > 0
            || (0 == iCmp && !pstRange->pstSegs[uiItr].bIncE))
        {
            continue;
        }

        iCmp = _Am_ValDataCmp(pstData, true, pstRange->pstSegs[uiItr].pstStart, false);

        if (iCmp < 0
            || (0 == iCmp && !pstRange->pstSegs[uiItr].bIncS))
        {
            return false;
        }

        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////

static void _Am_ValSegCls(AmValSegSt* pstValSeg)
{
    if (pstValSeg != NULL)
    {
        if (pstValSeg->pstEnd != NULL
            && pstValSeg->pstEnd != pstValSeg->pstStart)
        {
            Util_DataCls(pstValSeg->pstEnd);
            Util_Free(pstValSeg->pstEnd);
        }

        if (pstValSeg->pstStart != NULL)
        {
            Util_DataCls(pstValSeg->pstStart);
            Util_Free(pstValSeg->pstStart);
        }

        pstValSeg->pstStart = NULL;
        pstValSeg->pstEnd = NULL;
    }
}

//bNullIsPos - true: NULL表示最大值。否则表示最小值。
static int _Am_ValDataCmp(DataSt* pstL,
                          bool bNullIsPosL,
                          DataSt* pstR,
                          bool bNullIsPosR)
{
    int iRet;
    int iRslt;

    if (NULL == pstL)
    {
        if (bNullIsPosL)
        {
            if (NULL == pstR && bNullIsPosR)
            { return 0; }

            return 1;
        }
        else
        {
            if (NULL == pstR && !bNullIsPosR)
            { return 0; }

            return -1;
        }
    }

    if (NULL == pstR)
    {
        if (bNullIsPosR)
        { return -1; }
        else
        { return 1; }
    }

    //都不为NULL。
    iRet = Util_DataCmp(pstL, pstR, &iRslt);
    UTIL_CHK(RET_OK == iRet);

    return iRslt;
}

//返回值: 0: 表示存在交集并得到合并结果；正数: new > seg；负数: new < seg.
//若取new中数值，会取其深度拷贝。
static int _Am_ValSegCombinTo(AmValSegSt* pstNew, AmValSegSt* pstSeg)
{
    int iCmp;

    UTIL_CHK(pstNew != NULL && pstSeg != NULL);

    //交集判断
    iCmp = _Am_ValDataCmp(pstNew->pstStart, false, pstSeg->pstEnd, true);

    if (iCmp > 0
        || (0 == iCmp && !pstNew->bIncS && !pstSeg->bIncE))
    {
        return iCmp;
    }

    iCmp = _Am_ValDataCmp(pstNew->pstEnd, true, pstSeg->pstStart, false);

    if (iCmp < 0
        || (0 == iCmp && !pstNew->bIncS && !pstSeg->bIncS))
    {
        return iCmp;
    }

    //取得最小值。
    iCmp = _Am_ValDataCmp(pstNew->pstStart, false, pstSeg->pstStart, false);

    if (0 == iCmp && pstNew->bIncS)
    {
        pstSeg->bIncS = true;
    }
    else if (iCmp < 0)
    {
        Util_DataCls(pstSeg->pstStart);
        pstSeg->bIncS = pstNew->bIncS;

        if (pstNew->pstStart != NULL)
        { pstSeg->pstStart = Util_DataCpy(pstNew->pstStart); }
        else
        { pstSeg->pstStart = NULL; }
    }

    //取得最大值。
    iCmp = _Am_ValDataCmp(pstNew->pstEnd, true, pstSeg->pstEnd, true);

    if (0 == iCmp && pstNew->bIncE)
    {
        pstSeg->bIncE = true;
    }
    else if (iCmp > 0)
    {
        Util_DataCls(pstSeg->pstEnd);
        pstSeg->bIncE = pstNew->bIncE;

        if (pstNew->pstEnd != NULL)
        { pstSeg->pstEnd = Util_DataCpy(pstNew->pstEnd); }
        else
        { pstSeg->pstEnd = NULL; }
    }

    return 0;
}
