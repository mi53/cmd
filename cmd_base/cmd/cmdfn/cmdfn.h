#ifndef _cmdfn_h_
#define _cmdfn_h_

#include "def.h"

/***************************************************************************
* 描述: 工具宏。
***************************************************************************/
#define FN_SMBL_ADD(pcFnName)   long pcFnName();
#define FN_ADD(pcFnName, iParaNum, pcDesc) {#pcFnName, (FnType)pcFnName, iParaNum, pcDesc},
#define FN_END()    {NULL, NULL}

//参数从右至左解析
#define FN_CALL(pFn, piVal, iStype) pFn(\
                                        (--piVal) && (iStype & (1 << 0)) && (--piVal) ? *(long*)piVal : *piVal,\
                                        (--piVal) && (iStype & (1 << 1)) && (--piVal) ? *(long*)piVal : *piVal, \
                                        (--piVal) && (iStype & (1 << 2)) && (--piVal) ? *(long*)piVal : *piVal, \
                                        (--piVal) && (iStype & (1 << 3)) && (--piVal) ? *(long*)piVal : *piVal, \
                                        (--piVal) && (iStype & (1 << 4)) && (--piVal) ? *(long*)piVal : *piVal)

/***************************************************************************
* 描述: 函数描述。
***************************************************************************/
#define FN_MAX_PARA_MUL  2 // 最大参数是int的多少倍大小。
#define FN_MAX_PARA_NUM  5 //FN_CALL与此相关，需要同步修改。
#define FN_PARA_INT_NUM  FN_MAX_PARA_NUM * FN_MAX_PARA_MUL
typedef struct _FnParaSt
{
    int aiPara[FN_PARA_INT_NUM];
} FnParaSt;

typedef FN_SMBL_ADD((*FnType));
//typedef long (*FnType)();

typedef struct _FnDescSt
{
    const char* pcFnName;
    FnType fnFunc;
    int iParaNum;
    const char* pcDesc;
} FnDescSt;

typedef struct _FnParaAddRsltSt
{
    int iParaIdx;
    int iParaIntIdx;
    int iStype;
} FnParaAddRsltSt;
/***************************************************************************
* 描述: 存储函数命令信息。
***************************************************************************/
extern FnDescSt g_astDfltFns[];
extern FnDescSt* g_pstUserFns;

//解析参数。
int ParaRunFn_ParaAdd(FnParaSt* pstPara,  const char* pcFmtStr, FnParaAddRsltSt* pstRslt);

//设置函数列表。传入函数描述数组，最后一项应置为空。
int ParaRunFn_SetFuncs(FnDescSt* pstFns);

#endif // _cmdfn_h_
