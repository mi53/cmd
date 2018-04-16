#include "cmdfn.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>


/***************************************************************************
* 描述: 声明符号。若包含了头文件可以不用在此声明符号。
***************************************************************************/
//FN_SMBL_ADD(Tst_Example);

/***************************************************************************
* 描述: 添加函数前需先声明符号, 添加函数。若为变成，参数数据建议写最大值。
***************************************************************************/
FnDescSt g_astDfltFns[] =
{
    FN_ADD(printf, FN_MAX_PARA_NUM, "std:printf。")
    FN_ADD(malloc, 1, "std:malloc。")
    FN_ADD(Util_Free, 1, "std:Util_Free。")
    FN_END()
};

FnDescSt* g_pstUserFns = NULL;

//用作索引。
typedef enum _FnParaTypeEn
{
    FN_INT = 0,
    FN_LONG,
    FN_CHAR,
    FN_STR,
    FN_PTR,
    FN_BUF
} FnParaTypeEn;

typedef struct _FnParaFmtSt
{
    int iSize;
    const char* pcFmt;
    const char* pcDesc;
} FnParaFmtSt;

static FnParaFmtSt s_astFnParaFmt[] =
{
    {sizeof(int),   "i", "参数为int，可以忽略类型描述"},
    {sizeof(long),  "l", "参数为long"},
    {sizeof(char),  "c", "参数为char"},
    {sizeof(char*), "s", "参数为string，可以忽略类型描述"},
    {sizeof(void*), "p", "参数为ptr"}
};

#define FN_PARA_FMT_VAL_SEG ':'

/***************************************************************************
* 描述: 内部函数声明。
***************************************************************************/

//解析参数类型。
int _ParaRunFn_ParaType(FnParaTypeEn* peFmtType, const char** ppcValStart, const char* pcStr);

/***************************************************************************
* 描述: 函数定义。
***************************************************************************/

//设置函数列表。传入函数描述数组，最后一项应置为空。
int ParaRunFn_SetFuncs(FnDescSt* pstFns)
{
    UTIL_CHK(NULL != pstFns);

    g_pstUserFns = pstFns;

    return RET_OK;
}

//解析参数。
int ParaRunFn_ParaAdd(FnParaSt* pstPara,  const char* pcFmtStr, FnParaAddRsltSt* pstRslt)
{
    long lRet;

    const char* pcVal = NULL;
    FnParaFmtSt* pstParaFmt = NULL;
    FnParaTypeEn eFmtType = FN_INT;

    int index;

    long lValTmp;
    int iValTmp;
    void* pValTmp;

    int* piValStart = NULL;
    /***********************************************************
    * 参数校验。
    ***********************************************************/
    UTIL_CHK(NULL != pstPara && NULL != pstRslt);

    //此处不屏蔽null。
    if (NULL == pcFmtStr)
    {
        pcFmtStr = "null";
    }

    /***********************************************************
    * 初始化输出。
    ***********************************************************/
    if (pstRslt->iParaIntIdx >= FN_PARA_INT_NUM)
    {
        LOG_INFO("参数过大或过多.");
        return RET_ERR;
    }

    /***********************************************************
    * 获取参数格式信息。
    ***********************************************************/
    lRet = _ParaRunFn_ParaType(&eFmtType, &pcVal, pcFmtStr);

    if (RET_OK != lRet)
    {
        return RET_ERR;
    }

    //根据参数大小获取需要在参数列表中保存的整数个数。
#define  _TMP_FN_PARA_NUM(iSize) \
    ((int)((((iSize) + sizeof(int)-1) & ~(sizeof(int) - 1)) / sizeof(int)))

    /***********************************************************
    * 获取值。
    ***********************************************************/
    switch (eFmtType)
    {
        case FN_INT:
            piValStart = &iValTmp;

            if (!Util_StrGetLong(pcVal, &lValTmp))
            {
                LOG_INFO("参数不是一个数据");
                return RET_ERR;
            }

            iValTmp = (int)lValTmp;
            break;

        case FN_LONG:
            piValStart = (int*)&lValTmp;

            if (!Util_StrGetLong(pcVal, &lValTmp))
            {
                LOG_INFO("参数不是一个数据");
                return RET_ERR;
            }

            break;

        case FN_PTR:
            piValStart = (int*)&pValTmp;

            if (!Util_StrGetLong(pcVal, &lValTmp))
            {
                LOG_INFO("参数不是一个数据");
                return RET_ERR;
            }

            pValTmp = (void*)lValTmp;
            break;

        case FN_CHAR:
            iValTmp = *pcVal;
            piValStart = &iValTmp;

            //校验是否存在无效字符。
            ++pcVal;

            while (*pcVal != '\0')
            {
                if (!UTIL_IS_EMPTY_SYB(*pcVal))
                {
                    LOG_INFO("参数不是一个字符.");
                    return RET_ERR;
                }
            }

            break;

        case FN_STR:
            piValStart = (int*)&pcVal;
            break;

        default:
            LOG_INFO("参数为未知的数据类型。");
            return RET_ERR;
    }

    //统一在此赋值。
    for (index = 0;
         index < _TMP_FN_PARA_NUM(s_astFnParaFmt[eFmtType].iSize);
         ++index)
    {
        memcpy(&pstPara->aiPara[pstRslt->iParaIntIdx],
               piValStart + index,
               sizeof(int));
        ++(pstRslt->iParaIntIdx);
    }

    //更新样式。
    if (_TMP_FN_PARA_NUM(s_astFnParaFmt[eFmtType].iSize) > 1)
    {
        pstRslt->iStype |= (1 << pstRslt->iParaIdx);
    }
    else
    {
        pstRslt->iStype &= ~(1 << pstRslt->iParaIdx);
    }

    ++(pstRslt->iParaIdx);


#undef _TMP_FN_PARA_NUM
    return RET_OK;
}


/***************************************************************************
* 描述: 内部函数实现。
***************************************************************************/

//解析参数类型。
int _ParaRunFn_ParaType(FnParaTypeEn* peFmtType, const char** ppcValStart, const char* pcStr)
{
    int iCmp;
    int index;
    int iFmtLen;
    const char* pcTmp;
    long lValTmp;

    UTIL_CHK(NULL != peFmtType && NULL != ppcValStart && NULL != pcStr);

    //整理字符串
    while (UTIL_IS_EMPTY_SYB(*pcStr))
    { ++pcStr; }

    /***********************************************************
    * 依据类型描述解析参数。
    ***********************************************************/
    for (index = 0; index < sizeof(s_astFnParaFmt) / sizeof(FnParaFmtSt); ++index)
    {
        pcTmp = pcStr;
        iFmtLen = strlen(s_astFnParaFmt[index].pcFmt);
        iCmp = Util_StrNCmpIgrUp(s_astFnParaFmt[index].pcFmt, pcTmp, iFmtLen);

        if (0 == iCmp)
        {
            //寻找分隔符。
            pcTmp += iFmtLen;

            while (UTIL_IS_EMPTY_SYB(*pcTmp))
            { ++pcTmp; }

            if (FN_PARA_FMT_VAL_SEG == *pcTmp)
            {
                ++pcTmp;

                //得到输出。
                *ppcValStart = pcTmp;
                *peFmtType = (FnParaTypeEn)index;
                return RET_OK;
            }
        }
    }

    /***********************************************************
    * 自动解析字符串及整数。只有s和i支持忽略类型描述。
    ***********************************************************/
    if (Util_StrGetLong(pcStr, &lValTmp))
    {
        *ppcValStart = pcStr;
        *peFmtType = FN_INT;
    }
    else
    {
        *ppcValStart = pcStr;
        *peFmtType = FN_STR;
    }


    return RET_OK;
}
