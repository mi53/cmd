#include "cfg.h"
#include "util.h"
#include "cfg_common.h"

#include <string.h>


/***************************************************************************
* 描述: 配置存储对象。
***************************************************************************/
typedef struct _CfgItemSt
{
    CfgItemDescSt stDesc;
    DataSt stVal;
} CfgItemSt;
typedef struct _CfgSt
{
    int iItemNum;
    int iFreeNum;
    CfgItemSt* pstCfgs;
} CfgSt;

static CfgSt s_stCfgs;
static CfgItemSt* _Cfg_GetItem(const char* pcCfgName);

/***************************************************************************
* 描述: 配置相关操作。
***************************************************************************/
static int _Cfg_Add(CfgItemDescSt* pstCfgItem)
{
    CfgItemSt* pstNew;
    DataSt stData;

    if (NULL == pstCfgItem
        || NULL == pstCfgItem->pcName
        || !Util_ChkType(pstCfgItem->eValType))
    {
        LOG_ERR("企图添加无效配置项。");
        return RET_ERR;
    }

    if (NULL != _Cfg_GetItem(pstCfgItem->pcName))
    {
        LOG_INFO("配置项%s已经存在。", pstCfgItem->pcName);
        return RET_ERR;
    }

    stData.eType = pstCfgItem->eValType;

    if (RET_OK != Util_DataWriteByStr(&stData,
                                      pstCfgItem->pcDfltVal,
                                      strlen(pstCfgItem->pcDfltVal)))
    {
        LOG_ERR("默认值无效。");
        return RET_ERR;
    }

    //如果没有空闲，请求更大空间。
    if (0 == s_stCfgs.iFreeNum)
    {
        s_stCfgs.iFreeNum += (2 * s_stCfgs.iItemNum + 1);
        pstNew = (CfgItemSt*)malloc(sizeof(CfgItemSt) * s_stCfgs.iFreeNum);
        memset(pstNew, 0, sizeof(CfgItemSt) * s_stCfgs.iFreeNum);

        if (s_stCfgs.iItemNum > 0)
        { memcpy(pstNew, s_stCfgs.pstCfgs, sizeof(CfgItemSt) * s_stCfgs.iItemNum); }

        Util_Free(s_stCfgs.pstCfgs);
        s_stCfgs.pstCfgs = pstNew;
        s_stCfgs.iFreeNum -= s_stCfgs.iItemNum;
    }

    //在末尾添加新的结点。
    memcpy(&s_stCfgs.pstCfgs[s_stCfgs.iItemNum].stVal, &stData, sizeof(DataSt));
    s_stCfgs.pstCfgs[s_stCfgs.iItemNum].stDesc.pcName =
        Util_StrCpy(pstCfgItem->pcName);
    s_stCfgs.pstCfgs[s_stCfgs.iItemNum].stDesc.pcDesc =
        Util_StrCpy(pstCfgItem->pcDesc);
    s_stCfgs.pstCfgs[s_stCfgs.iItemNum].stDesc.pcDfltVal =
        Util_StrCpy(pstCfgItem->pcDfltVal);
    s_stCfgs.pstCfgs[s_stCfgs.iItemNum].stDesc.eValType = pstCfgItem->eValType;
    s_stCfgs.pstCfgs[s_stCfgs.iItemNum].stDesc.bConst = pstCfgItem->bConst;

    ++s_stCfgs.iItemNum;
    --s_stCfgs.iFreeNum;

    return RET_OK;
}

int Cfg_AddItems(CfgItemDescSt* pstCfgs, int iNum)
{
    int iRet;
    int iCount = 0;

    while (iCount < iNum)
    {
        iRet = _Cfg_Add(pstCfgs + iCount);

        if (RET_OK != iRet)
        { break; }

        ++iCount;
    }

    iRet = iCount;

    return iRet;
}

bool Cfg_GetInt(const char* pcCfgName, int* piVal)
{
    CfgItemSt* pstItem = NULL;

    if (NULL == pcCfgName)
    { return false; }

    pstItem = _Cfg_GetItem(pcCfgName);

    if (NULL == pstItem)
    { return false; }

    return Util_DataGetInt(&pstItem->stVal, piVal);
}

const char* Cfg_GetStr(const char* pcCfgName)
{
    static char acIntVal[32] = {0};
    CfgItemSt* pstItem = NULL;

    if (NULL == pcCfgName)
    { return NULL; }

    pstItem = _Cfg_GetItem(pcCfgName);

    if (NULL == pstItem)
    { return NULL; }

    if (TYPE_INT == pstItem->stDesc.eValType)
    {
        if (NULL == pstItem->stVal.pData)
        { return NULL; }

        sprintf(acIntVal, "%d", *(int*)pstItem->stVal.pData);

        return acIntVal;
    }
    else
    { return Util_DataGetStr(&pstItem->stVal); }
}

//恢复某一配置至默认值。
int Cfg_Recover(const char* pcCfgName)
{
    CfgItemSt* pstItem = NULL;

    if (NULL == pcCfgName)
    {
        return RET_ERR;
    }

    pstItem = _Cfg_GetItem(pcCfgName);

    if (NULL == pstItem)
    {
        LOG_INFO("不存在配置项:%s.", pcCfgName);
        return RET_ERR;
    }

    return Cfg_WriteByStr(pcCfgName, pstItem->stDesc.pcDfltVal);
}


//恢复全部配置至默认值。
int Cfg_RecoverAll()
{
    DataSt* pstData = NULL;
    CfgItemDescSt* pstDesc = NULL;
    int itr = 0;
    int iRet;

    while (itr < s_stCfgs.iItemNum)
    {
        pstDesc = &s_stCfgs.pstCfgs[itr].stDesc;
        pstData = &s_stCfgs.pstCfgs[itr].stVal;

        Util_DataCls(pstData);
        pstData->eType = pstDesc->eValType;

        iRet = Util_DataWriteByStr(pstData,
                                   pstDesc->pcDfltVal,
                                   strlen(pstDesc->pcDfltVal));

        if (RET_OK != iRet)
        {
            Cfg_Cls();
            return RET_ERR;
        }

        ++itr;
    }


    return RET_OK;
}


int Cfg_WriteByStr(const char* pcCfgName, const char* pcVal)
{
    CfgItemSt* pstItem = NULL;
    int iRet;
    DataSt stData;

    if (NULL == pcCfgName)
    {
        return RET_ERR;
    }

    pstItem = _Cfg_GetItem(pcCfgName);

    if (NULL == pstItem)
    {
        LOG_INFO("不存在配置项:%s.", pcCfgName);
        return RET_ERR;
    }

    if (pstItem->stDesc.bConst)
    {
        LOG_INFO("常量配置项:%s.", pstItem->stDesc.pcName);
        return RET_ERR;
    }

    //写入失败不影响原有的值。
    stData.eType = pstItem->stVal.eType;
    stData.pData = NULL;

    if (NULL != pcVal)
    {
        if (NULL != pcVal)
        {
            iRet = Util_DataWriteByStr(&stData, pcVal, strlen(pcVal));

            if (RET_OK != iRet)
            {
                return RET_ERR;
            }
        }
    }

    Util_DataCls(&pstItem->stVal);
    memcpy(&pstItem->stVal, &stData, sizeof(stData));

    return RET_OK;
}


void Cfg_Cls()
{
    int idx;

    if (s_stCfgs.iItemNum > 0 && s_stCfgs.pstCfgs != NULL)
    {
        for (idx = 0; idx < s_stCfgs.iItemNum; ++idx)
        {
            Util_DataCls(&s_stCfgs.pstCfgs[idx].stVal);
            Util_Free(s_stCfgs.pstCfgs[idx].stDesc.pcDfltVal);
            Util_Free(s_stCfgs.pstCfgs[idx].stDesc.pcDesc);
            Util_Free(s_stCfgs.pstCfgs[idx].stDesc.pcName);
        }

        Util_Free(s_stCfgs.pstCfgs);
    }

    s_stCfgs.iItemNum = 0;
    s_stCfgs.pstCfgs = NULL;
    s_stCfgs.iFreeNum = 0;
}

//显示配置项当前值，若传入NULL，显示全部配置项的值。
void Cfg_Show(const char* pcCfgName)
{
    CfgItemSt* pstItem = NULL;
    int idx = 0;
    const char* pcVal = NULL;
    const char* pcType = NULL;
    int iVal;
    bool bShowAll = true;
    char acIntToStr[32];

    if (NULL != pcCfgName)
    {
        pstItem = _Cfg_GetItem(pcCfgName);

        if (NULL == pstItem)
        {
            LOG_INFO("不存在配置项:%s.", pcCfgName);
            return ;
        }

        bShowAll = false;
        idx = pstItem - s_stCfgs.pstCfgs;
    }


    LOG("%-15s %-14s %-13s %s\n",
        "名称", "类型", "当前值", "默认值");
    LOG("%-15s %-14s %-13s %s\n",
        "===============", "==============", "=============", "============");

    if (bShowAll)
    { idx = 0; }

    for (; idx < s_stCfgs.iItemNum; ++idx)
    {
        switch (s_stCfgs.pstCfgs[idx].stDesc.eValType)
        {
            case TYPE_INT:
                Util_DataGetInt(&s_stCfgs.pstCfgs[idx].stVal, &iVal);
                sprintf(acIntToStr, "%d", iVal);
                acIntToStr[sizeof(acIntToStr) - 1] = '\0';
                pcVal = acIntToStr;
                pcType = "TYPE_INT";
                break;

            case TYPE_STR:
                pcVal = Util_DataGetStr(&s_stCfgs.pstCfgs[idx].stVal);
                pcType = "TYPE_STR";
                break;

            default:
                pcType = "TYPE_UNKNOW";
                pcVal = "无效";
                break;
        }

        if (s_stCfgs.pstCfgs[idx].stDesc.bConst)
        {
            LOG("%-15s %-5s %-8s %-13s %s\n",
                s_stCfgs.pstCfgs[idx].stDesc.pcName,
                "CONST",
                pcType,
                pcVal,
                s_stCfgs.pstCfgs[idx].stDesc.pcDfltVal);
        }
        else
        {
            LOG("%-15s %-14s %-13s %s\n",
                s_stCfgs.pstCfgs[idx].stDesc.pcName,
                pcType,
                pcVal,
                s_stCfgs.pstCfgs[idx].stDesc.pcDfltVal);
        }

        if (!bShowAll)
        {
            LOG(FMT_SEG1_E);
            LOG("说明: %s\n", s_stCfgs.pstCfgs[idx].stDesc.pcDesc);
            break;
        }
    }
}

//查找配置项
static CfgItemSt* _Cfg_GetItem(const char* pcCfgName)
{
    int iCmp;
    int idx;
    const char* pcSel = pcCfgName;
    const char* pcItr = NULL;

    UTIL_CHK(NULL != pcSel);

    //如果配置项以cfg_开头，可以忽略头进行比较。
    iCmp = Util_StrNCmpIgrUp(pcSel, "cfg_", sizeof("cfg_") - 1);

    if (0 == iCmp)
    {
        pcSel += (sizeof("cfg_") - 1);
    }

    for (idx = 0; idx < s_stCfgs.iItemNum; ++idx)
    {
        pcItr = s_stCfgs.pstCfgs[idx].stDesc.pcName;

        //如果配置项以cfg_开头，可以忽略头进行比较。
        iCmp = Util_StrNCmpIgrUp(pcItr, "cfg_", sizeof("cfg_") - 1);

        if (0 == iCmp)
        {
            pcItr += (sizeof("cfg_") - 1);
        }

        //配置项忽略大小写。
        iCmp = Util_StrCmpIgrUp(pcSel, pcItr);

        if (0 == iCmp)
        {
            return s_stCfgs.pstCfgs + idx;
        }
    }

    return NULL;
}
