#include "obj.h"
#include "util.h"

#include <string.h>

//允许匿名。
void Am_ObjTmplInit(AmObjTmplSt* pstTmpl, const char* pcName, TypeEn eType)
{

    CHK_RET(pstTmpl != NULL);

    memset(pstTmpl, 0, sizeof(AmObjTmplSt));

    if (pcName != NULL)
    {
        pstTmpl->pcName = Util_StrCpy(pcName);
    }

    pstTmpl->eValType = eType;
}


void Am_ObjTmplCls(AmObjTmplSt* pstTmpl)
{
    unsigned int uiItr;

    if (pstTmpl != NULL)
    {
        Util_Free(pstTmpl->pcName);

        for (uiItr = 0; uiItr < pstTmpl->uiSubNum; ++uiItr)
        {
            Am_ObjTmplCls(pstTmpl->pstSubs + uiItr);
        }

        memset(pstTmpl, 0, sizeof(AmObjTmplSt));
    }
}

//返回索引，不存在返回-1.
int Am_ObjTmplSubGet(AmObjTmplSt* pstTmpl, const char* pcSub)
{
    unsigned int uiItr;
    int iCmp;

    CHK_RET(pstTmpl != NULL && pcSub != NULL, -1);

    for (uiItr = 0; uiItr < pstTmpl->uiSubNum; ++uiItr)
    {
        iCmp = strcmp(pcSub, pstTmpl->pstSubs[uiItr].pcName);

        if (0 == iCmp)
        {
            return uiItr;
        }
    }

    return -1;
}


void Am_ObjTmplSubAdd(AmObjTmplSt* pstTmpl, const char* pcSub, TypeEn eType)
{
    int idx;
    AmObjTmplSt stSub;

    CHK_RET(pstTmpl != NULL && pcSub != NULL);

    CALL_RET(idx = Am_ObjTmplSubGet(pstTmpl, pcSub));
    CHK_RET(idx >= 0);

    CALL_RET(Am_ObjTmplInit(&stSub, pcSub, eType));

    CALL_RET(Util_ArrayExpandAuto((void**)&pstTmpl->pstSubs,
                                  sizeof(AmObjTmplSt),
                                  pstTmpl->uiSubNum,
                                  &pstTmpl->uiSubCap));

    memcpy(pstTmpl->pstSubs + pstTmpl->uiSubNum, &stSub, sizeof(AmObjTmplSt));
    ++pstTmpl->uiSubNum;
}
