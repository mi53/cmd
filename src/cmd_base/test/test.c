#include "test.h"
#include "cmd.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

#include "case_example.h"

/***************************************************************************
* 描述: 存储全部定义好的用例。
***************************************************************************/
static CaseSt** s_apstCases = NULL;

//解析后的用例条件。
typedef struct _CaseCdtPSt
{
    int iCdtNum;
    ParasSt* pstCdtTypes;
} CaseCdtPSt;

static bool _Tst_CdtParse(CaseSt* pstCase);
static void _Tst_CdtParseCls(CaseCdtPSt* pstCdtP);

typedef struct _CdtAndValItrSt
{
    int iCdtItr;
    int iCdtValItr;
} CdtAndValItrSt;

static int _Tst_ScenceRun(CaseSt* pstCase, int* aiCdtIdxs);

//此处索引位外部编号，从1开始编号。
static int _Tst_ChkCdtInSet(unsigned int uiCdt,
                            const char* pcSetDesc,
                            int iStrLen,
                            bool* pbRslt);

static int _Tst_GetExpectRslt(CaseSt* pstCase, int* piCdts, int* piExpect);


//解析一个符号|隔开的段。
static int _Tst_ChkCdtInSeg(unsigned int uiCdt,
                            const char* pcSegStart,
                            const char* pcSegEnd,
                            bool* pbRslt);
/***************************************************************************
* 描述: 设置用例。
***************************************************************************/
int Tst_SetCase(CaseSt** apstCase)
{
    int idx = 0;
    ParasSt stParas = {0};

    if (NULL == apstCase || NULL == apstCase[0])
    {
        return RET_OK;
    }

    //检测有效性。
    while (NULL != apstCase[idx])
    {
        //基函数、基础用例名及描述均不允许为空。
        if (NULL == apstCase[idx]->fnBase
            || NULL == apstCase[idx]->pcBase
            || NULL == apstCase[idx]->pcDesc)
        {
            LOG_INFO("传入的case无效，注意: 有效的用例中基函数、"
                     "基础用例名、基础用例描述不能为空。");
            return RET_ERR;
        }

        //检验条件。
        if ((0 == apstCase[idx]->stCdtDesc.iNum
             && NULL != apstCase[idx]->stCdtDesc.pstParas)
            || (0 != apstCase[idx]->stCdtDesc.iNum
                && NULL == apstCase[idx]->stCdtDesc.pstParas)
            || (0 == apstCase[idx]->stCdts.iNum
                && NULL != apstCase[idx]->stCdts.pstCdts)
            || (0 != apstCase[idx]->stCdts.iNum
                && NULL == apstCase[idx]->stCdts.pstCdts))
        {
            LOG_INFO("条件数目与对应的内容只能同为0或NULL。");
            return RET_ERR;
        }
        else if (0 == apstCase[idx]->stCdtDesc.iNum
                 && 0 != apstCase[idx]->stCdts.iNum)
        {
            LOG_INFO("条件描述为空，而条件域不为空。");
            return RET_ERR;
        }

        //检验场景-结果表。
        if ((0 == apstCase[idx]->stRslt.iNum
             && NULL != apstCase[idx]->stRslt.pstRslts)
            || (0 != apstCase[idx]->stRslt.iNum
                && NULL == apstCase[idx]->stRslt.pstRslts))
        {
            LOG_INFO("场景-结果匹配表中数目与对应的内容只能同为0或NULL。");
            return RET_ERR;
        }


        //解析条件。
        if (!_Tst_CdtParse(apstCase[idx]))
        {
            LOG_INFO("解析条件域失败。");
            return RET_ERR;
        }

        ++idx;
    }

    s_apstCases = apstCase;

    return RET_OK;
}

/***************************************************************************
* 描述: 清理用例。
***************************************************************************/
void Tst_ClsCase()
{
    CaseSt* pstCase;

    if (NULL != s_apstCases)
    {
        int idx = 0;

        pstCase = s_apstCases[idx];

        while (pstCase != NULL)
        {
            _Tst_CdtParseCls((CaseCdtPSt*)pstCase->pCdtsAftP);
            pstCase->pCdtsAftP = NULL;

            pstCase = s_apstCases[++idx];
        }

        s_apstCases = NULL;
    }
}

/***************************************************************************
* 描述: 测试入口函数。
***************************************************************************/
void Tst_CaseRun(const char* pcBase, const char* pcCdt)
{
    int iRet = RET_OK;

    int idx = 0;
    bool bRunAllCase = !(pcBase);
    bool bRunAllSecense = !(pcCdt);
    int iCmp;

    ParasDescSt* pstCdtDesc;
    CaseCdtSt* pstCdtItr;

    int idxCdt;
    int* aiCdtIdxs;//当前场景。
    CaseCdtPSt* pstCdtsP;

    int iSuccessNum = 0;
    int iFailNum = 0;

    if (NULL == s_apstCases)
    {
        LOG_INFO("no case.");
        return;
    }

    /***********************************************************
    * 遍历查找并执行对应的测试用例。
    ***********************************************************/
    while (NULL != s_apstCases[idx])
    {
        //判断当前基函数是否需要运行。
        if (!bRunAllCase)
        {
            iCmp = Util_StrCmpIgrUp(pcBase, s_apstCases[idx]->pcBase);

            if (0 != iCmp)
            {
                ++idx;
                continue;
            }
        }

        /***********************************************************
        * 没有条件因子的用例，直接执行基函数。
        ***********************************************************/
        if (0 == s_apstCases[idx]->stCdtDesc.iNum)
        {
            iRet = _Tst_ScenceRun(s_apstCases[idx], NULL);

            if (RET_OK != iRet)
            {
                ++iFailNum;
                LOG(FMT_SEG1"- - - - - %d\n", iFailNum);
            }
            else
            { ++iSuccessNum; }

            //判断是否需要继续。
            if (!bRunAllCase)
            { break; }

            ++idx;
            continue;
        }

        /***********************************************************
        * 遍历查找对应条件，并执行对应场景。
        ***********************************************************/
        pstCdtDesc = &(s_apstCases[idx]->stCdtDesc);
        pstCdtItr = s_apstCases[idx]->stCdts.pstCdts;

        //获取并执行对应场景。-- 条件类别为0.
        aiCdtIdxs = (int*)malloc(s_apstCases[idx]->stCdtDesc.iNum * sizeof(int));
        memset(aiCdtIdxs, 0, s_apstCases[idx]->stCdtDesc.iNum * sizeof(int));

        if (!bRunAllSecense)
        {
            for (idxCdt = 0; idxCdt < s_apstCases[idx]->stCdtDesc.iNum; ++idxCdt)
            {
                aiCdtIdxs[idxCdt] = 0;

                //后续未输入的数字，默认为0.
                while (*pcCdt >= '0' && *pcCdt <= '9')
                {
                    aiCdtIdxs[idxCdt] *= 10;
                    aiCdtIdxs[idxCdt] += (*pcCdt - '0');
                    ++pcCdt;
                }

                while (*pcCdt != '\0'
                       && (*pcCdt < '0' || *pcCdt > '9'))
                {
                    ++pcCdt;
                }

                //外部编号从1开始。0，也当做第一项处理。
                if (aiCdtIdxs[idxCdt] > 0)
                { --aiCdtIdxs[idxCdt]; }
            }
        }

        //初始化条件参数。
        pstCdtsP = (CaseCdtPSt*)s_apstCases[idx]->pCdtsAftP;

        //执行场景。
        do
        {

            iRet = _Tst_ScenceRun(s_apstCases[idx], aiCdtIdxs);

            if (RET_OK != iRet)
            {
                ++iFailNum;
                LOG(FMT_SEG1"- - - - - %d\n", iFailNum);
            }
            else
            { ++iSuccessNum; }

            //迭代下一个场景。
            ++aiCdtIdxs[0];

            for (idxCdt = 0; idxCdt < s_apstCases[idx]->stCdtDesc.iNum; ++idxCdt)
            {
                if (aiCdtIdxs[idxCdt] < pstCdtsP->pstCdtTypes[idxCdt].iNum)
                {
                    break;
                }
                else//进位
                {
                    aiCdtIdxs[idxCdt] = 0;

                    if (idxCdt + 1 < s_apstCases[idx]->stCdtDesc.iNum)
                    {
                        ++aiCdtIdxs[idxCdt + 1];
                    }
                }
            }

            if (idxCdt == s_apstCases[idx]->stCdtDesc.iNum)
            {
                break;
            }
        }
        while (bRunAllSecense);

        Util_Free(aiCdtIdxs);

        //判断是否需要继续。
        if (!bRunAllCase)
        { break; }

        ++idx;
    }

    /***********************************************************
    * 显示运行结果。
    ***********************************************************/
    LOG(FMT_SEG4_E);
    LOG("%16s:%05d   |   %s:%05d\n", "成功", iSuccessNum, "失败", iFailNum);
    LOG(FMT_SEG4_E);
}


static void _Tst_CdtParseCls(CaseCdtPSt* pstCdtP)
{
    int idxCdt;

    if (NULL != pstCdtP)
    {
        for (idxCdt = 0; idxCdt < pstCdtP->iCdtNum; ++idxCdt)
        {
            Para_Cls(pstCdtP->pstCdtTypes + idxCdt);
        }

        memset(pstCdtP->pstCdtTypes, 0, sizeof(ParasSt*) * pstCdtP->iCdtNum);
        Util_Free(pstCdtP->pstCdtTypes);
        Util_Free(pstCdtP);
    }
}

static bool _Tst_CdtParse(CaseSt* pstCase)
{
    CaseCdtPSt* pstOut;
    int iCmp;
    CdtAndValItrSt* astCdtValItr;
    int idxCdt;
    int idxDesc;
    int idxDescStart;
    ParasDescSt stSingleCdt;
    ParasSt stCdtVal;

    int iRet;

    UTIL_CHK(NULL != pstCase);

    pstCase->pCdtsAftP = NULL;

    //校验条件类别描述。
    if (0 != pstCase->stCdtDesc.iNum)
    {
        if (NULL == pstCase->stCdtDesc.pstParas)
        {
            LOG_ERR("条件类别不为0，却不存在确切的条件。");
            return false;
        }
    }
    else
    { return true; }

    //创建解析对象。
    pstOut = (CaseCdtPSt*)malloc(sizeof(CaseCdtPSt));
    pstOut->iCdtNum = pstCase->stCdtDesc.iNum;
    pstOut->pstCdtTypes = (ParasSt*)malloc(sizeof(ParasSt) * pstOut->iCdtNum);
    memset(pstOut->pstCdtTypes, 0, sizeof(ParasSt) * pstOut->iCdtNum);

    //获取全部条件因子的条件数目。
    for (idxCdt = 0; idxCdt < pstCase->stCdts.iNum; ++idxCdt)
    {
        //获取对应的条件描述。一般而言相同条件存储在邻近位。
        iCmp = -1;

        for (idxDesc = 0; idxDesc < pstCase->stCdtDesc.iNum; ++idxDesc)
        {
            iCmp = strcmp(pstCase->stCdtDesc.pstParas[idxDesc].pcName,
                          pstCase->stCdts.pstCdts[idxCdt].pcCdtName);

            if (0 == iCmp)
            {
                idxDescStart = idxDesc;
                ++(pstOut->pstCdtTypes[idxDesc].iNum);
                break;
            }
        }

        if (idxDesc == pstCase->stCdtDesc.iNum)
        {
            LOG_ERR("用例:%s, 中存在无效条件%s。",
                    pstCase->pcBase,
                    pstCase->stCdts.pstCdts[idxCdt].pcCdtName);
            goto LAB_ERR;
        }
    }

    //创建条件值存储对象。
    for (idxDesc = 0; idxDesc < pstCase->stCdtDesc.iNum; ++idxDesc)
    {
        if (0 != pstOut->pstCdtTypes[idxDesc].iNum)
        {
            pstOut->pstCdtTypes[idxDesc].pstParas = (ParaSt*)malloc(
                    sizeof(ParaSt) * pstOut->pstCdtTypes[idxDesc].iNum);
            memset(pstOut->pstCdtTypes[idxDesc].pstParas,
                   0,
                   sizeof(ParaSt) * pstOut->pstCdtTypes[idxDesc].iNum);
        }
        else
        {
            LOG_ERR("用例:%s, 中条件因子:%s，不存在任何条件值。",
                    pstCase->pcBase,
                    pstCase->stCdts.pstCdts[idxCdt].pcCdtName);
            goto LAB_ERR;
        }
    }

    //填充值。
    astCdtValItr = (CdtAndValItrSt*)malloc(
                       sizeof(CdtAndValItrSt) * pstCase->stCdtDesc.iNum);
    memset(astCdtValItr, 0, sizeof(CdtAndValItrSt) * pstCase->stCdtDesc.iNum);

    idxDesc = 0;
    idxDescStart = 0;

    for (idxCdt = 0; idxCdt < pstCase->stCdts.iNum; ++idxCdt)
    {
        //获取对应的条件描述。一般而言相同条件存储在邻近位。
        idxDesc = idxDescStart;
        iCmp = -1;

        while (idxDesc >= 0)
        {
            iCmp = strcmp(pstCase->stCdtDesc.pstParas[idxDesc].pcName,
                          pstCase->stCdts.pstCdts[idxCdt].pcCdtName);

            if (0 == iCmp)
            {
                idxDescStart = idxDesc;
                break;
            }

            //先遍历本身及后序，再遍历前序。
            if (idxDescStart > 0)
            {
                if (idxDesc >= idxDescStart)
                {
                    if (idxDesc < pstCase->stCdtDesc.iNum - 1)
                    { ++idxDesc; }
                    else
                    { idxDesc = idxDescStart; }
                }
                else
                {
                    --idxDesc;
                }
            }
            else
            {
                if (idxDesc < pstCase->stCdtDesc.iNum - 1)
                { ++idxDesc; }
                else
                { idxDesc = -1; }
            }
        }

        UTIL_CHK(0 == iCmp);//在前段解析条件数目已保证必定匹配成功。

        stSingleCdt.iNum = 1;
        stSingleCdt.pstParas = pstCase->stCdtDesc.pstParas + idxDesc;

        //解析条件值。
        iRet = Para_Parse(&stSingleCdt,
                          pstCase->stCdts.pstCdts[idxCdt].pcCdtVal,
                          &stCdtVal);

        if (stSingleCdt.iNum != iRet)
        {
            LOG_ERR("解析用例:%s，条因子:%s的条件值:%s 失败。",
                    pstCase->pcBase,
                    stSingleCdt.pstParas->pcName,
                    pstCase->stCdts.pstCdts[idxCdt].pcCdtVal);
            goto LAB_ERR;
        }

        //赋值。写到尾部。
        Util_DataWrite(&(pstOut->pstCdtTypes[idxDesc].pstParas[astCdtValItr[idxDesc].iCdtValItr].stVal),
                       stCdtVal.pstParas->stVal.eType,
                       stCdtVal.pstParas->stVal.pData,
                       0);
        Para_Cls(&stCdtVal);

        ++astCdtValItr[idxDesc].iCdtValItr;
    }

    pstCase->pCdtsAftP = pstOut;
    Util_Free(astCdtValItr);
    return true;

LAB_ERR:
    _Tst_CdtParseCls(pstOut);
    Util_Free(astCdtValItr);
    return false;
}

static int _Tst_ScenceRun(CaseSt* pstCase, int* aiCdtIdxs)
{
    int iRet;
    ParasSt stCntScence = {0};
    int iScenceRslt;
    int idxCdt;
    CaseCdtPSt* pstCdtsP;
    int iRsltExpect;

    UTIL_CHK(NULL != pstCase);

    //显示用例信息
    LOG(FMT_SEG4_E);
    LOG_FIX_LINE("说  明: %s.\n", pstCase->pcDesc);
    LOG("用  例: %s ", pstCase->pcBase);

    for (idxCdt = 0; idxCdt < pstCase->stCdtDesc.iNum; ++idxCdt)
    { LOG("%d ", aiCdtIdxs[idxCdt] + 1); }

    LOG("\n");
    LOG(FMT_SEG4_E);

    //场景转换: 将编号转为为具体条件。
    if (pstCase->stCdtDesc.iNum != 0 && NULL != aiCdtIdxs)
    {
        pstCdtsP = (CaseCdtPSt*)pstCase->pCdtsAftP;
        stCntScence.iNum = pstCase->stCdtDesc.iNum;
        stCntScence.pstParas = (ParaSt*)malloc(sizeof(ParaSt) * stCntScence.iNum);
        memset(stCntScence.pstParas, 0, sizeof(ParaSt) * stCntScence.iNum);

        //组装条件成对应的场景。
        for (idxCdt = 0; idxCdt < pstCase->stCdtDesc.iNum; ++idxCdt)
        {
            if (aiCdtIdxs[idxCdt] < pstCdtsP->pstCdtTypes[idxCdt].iNum)
            {
                memcpy(stCntScence.pstParas + idxCdt,
                       pstCdtsP->pstCdtTypes[idxCdt].pstParas + aiCdtIdxs[idxCdt],
                       sizeof(ParaSt));
            }
            else
            {
                LOG("用  例: %s ", pstCase->pcBase);

                for (idxCdt = 0; idxCdt < pstCase->stCdtDesc.iNum; ++idxCdt)
                { LOG("%d ", aiCdtIdxs[idxCdt] + 1); }

                LOG("无效\n");

                Util_Free(stCntScence.pstParas);
                return RET_ERR;
            }
        }
    }

    //输出条件名称和值
    for (idxCdt = 0; idxCdt < stCntScence.iNum; ++idxCdt)
    {
        LOG("条件[%s]: ", pstCase->stCdtDesc.pstParas[idxCdt].pcName);

        switch (stCntScence.pstParas[idxCdt].stVal.eType)
        {
            case TYPE_INT:
                LOG("%d \n", stCntScence.pstParas[idxCdt].stVal.pData);
                break;

            case TYPE_STR:
                LOG("%s \n", stCntScence.pstParas[idxCdt].stVal.pData);
                break;

            default:
                LOG_ERR("data type invalid.");
                return RET_ERR;
        }

    }

    //执行并验证结果。
    iRet = pstCase->fnBase(&stCntScence, &iScenceRslt);

    if (RET_OK == iRet)
    {
        iRet = _Tst_GetExpectRslt(pstCase,
                                  aiCdtIdxs,
                                  &iRsltExpect);

        if (RET_OK == iRet && iScenceRslt != iRsltExpect)
        { iRet = RET_ERR; }
    }

    //打印结果。
    LOG("\n"FMT_SEG0_E);

    if (RET_OK != iRet)
    { LOG("结  果: 失败。\n"); }
    else
    { LOG("结  果: 成功。\n"); }

    LOG(FMT_SEG0_E);

    Util_Free(stCntScence.pstParas);

    return iRet;
}

//结果集定义:
//1. 条件因子使用定义时的序号做代指。从1开始编号，编号0做编号1处理。
//2. 以分号隔开单个条件因子的定义域。条件因子取交集作为总体因子集。
//3. 支持! [] & |.
//   ! : 取非，作用域为一个竖线隔开的数据段。
//   []: 数据范围，如:[1-3]表示数据1/2/3.也支持单个数据，如:[3]。
//       单个数据可以不适用中括号。
//   & : 竖线隔开的数据段内数据累加操作。主要为了支持!同时否定多个
//       数据域。
//       例1: 1&2&3 表示只要编号为1或2或3均可。此时与竖线意义相同。
//       例2: !1&2&3 表示编号不为1/2/3中任何一个才匹配成功。
//   | : 一个数据段。只需要成功匹配一个数据段，便匹配成功。
//4. 结果也使用整数描述。建议定义结果集时使用某一枚举统一表示。
//
//本处只解析单个条件是否满足要求。从条件名+冒号后续的内容开始解析。
//此处索引位外部编号，从1开始编号。
static int _Tst_ChkCdtInSet(unsigned int uiCdt,
                            const char* pcSetDesc,
                            int iStrLen,
                            bool* pbRslt)
{
    int itr = 0;
    int iSubStart;
    int iSubEnd;
    int iRet;

    UTIL_CHK(NULL != pbRslt);
    *pbRslt = false;

    //编号从1开始，0做1处理。
    if (0 == uiCdt)
    { uiCdt = 1; }

    //如果字符串为空，按照默认适配成功处理。
    if (0 == iStrLen || NULL == pcSetDesc)
    { return RET_OK; }

    //进行匹配校验。
    while (itr < iStrLen)
    {
        //1. 得到一个用竖线隔开的条件子段。多个竖线做一个竖线处理。
        while (itr < iStrLen
               && (UTIL_IS_EMPTY_SYB(pcSetDesc[itr])
                   || '|' == pcSetDesc[itr]))
        { ++itr; }

        iSubStart = itr;

        while ('|' != pcSetDesc[itr] && itr < iStrLen)
        { ++itr; }

        iSubEnd = itr - 1;

        if (iSubEnd < iSubStart)
        {
            LOG_INFO_FIX("条件集中存在空的条件描述段:\"%s\" - \"%s\".",
                         pcSetDesc);
            *pbRslt = true;
            return RET_OK;
        }

        //2. 校验该子段是否能成功匹配。
        iRet = _Tst_ChkCdtInSeg(uiCdt,
                                pcSetDesc + iSubStart,
                                pcSetDesc + iSubEnd,
                                pbRslt);

        if (RET_OK != iRet)
        { return RET_ERR; }

        if (*pbRslt)
        { return RET_OK; }

        //3. 迭代下一个子段。
    }

    return RET_OK;
}


static int _Tst_ChkCdtInSeg(unsigned int uiCdt,
                            const char* pcSegStart,
                            const char* pcSegEnd,
                            bool* pbRslt)
{
    const char* pcItr = pcSegStart;

    //整个段是否需要取非。
    bool bReverse = false;

    //数据域。
    unsigned long ulFieldStart;
    unsigned long ulFieldEnd;

    //一个临时解析段。
    const char* pcStart;
    const char* pcEnd;

    //依据&字符分隔出的数据域。
    const char* pcSubStart;
    const char* pcSubEnd;

    UTIL_CHK(NULL != pcSegStart && NULL != pcSegEnd && pcSegEnd >= pcSegStart
             && NULL != pbRslt);
    *pbRslt = false;

    //感叹号只能出现于段第一个字符。
    while (pcItr <= pcSegEnd && ('!' == *pcItr || UTIL_IS_EMPTY_SYB(*pcItr)))
    {
        if ('!' == *pcItr)
        { bReverse = !bReverse; }

        ++pcItr;
    }

    if (pcItr > pcSegEnd)
    {
        LOG_INFO_FIX("条件集中存在只包含取非字符的描述段.");
        return RET_OK;
    }

    while (pcItr <= pcSegEnd)
    {
        //根据&符号截取出子段。
        while (pcItr <= pcSegEnd && '&' == *pcItr)
        { ++pcItr; }

        pcSubStart = pcItr;

        while (pcItr <= pcSegEnd && '&' != *pcItr)
        { ++pcItr; }

        pcSubEnd = pcItr - 1;

        if (pcSubStart > pcSegEnd)
        { break; }

        //解析数据域。
        pcItr = pcSubStart;

        if ('[' == *pcItr)
        {
            ++pcItr;

            //中括号内可以是单个数据，也可以是一个数据段。无需支持负数。
            pcStart = pcItr;

            while (pcItr <= pcSubEnd && *pcItr != '-' && *pcItr != ']')
            { ++pcItr; }

            if (pcItr > pcSubEnd)
            {
                LOG_INFO_FIX("无效条件集，数据域不存在结束符\']\':\"%s\".",
                             pcSubStart);
                return RET_ERR;
            }

            pcEnd = pcItr - 1;

            if (!Util_StrNGetLong(pcStart, pcEnd - pcStart + 1, (long*)&ulFieldStart))
            {
                LOG_INFO_FIX("无效条件集，期待一个整数:\"%s\" - \"%s\".",
                             pcSubStart,
                             pcStart);
                return RET_ERR;
            }

            ulFieldEnd = ulFieldStart;

            if ('-' == *pcItr)
            {
                ++pcItr;
                pcStart = pcItr;

                while (pcItr <= pcSubEnd && *pcItr != ']')
                { ++pcItr; }

                if (pcItr > pcSubEnd)
                {
                    LOG_INFO_FIX("无效条件集，数据域不存在结束符\']\':\"%s\" - \"%s\".",
                                 pcSegStart,
                                 pcStart);
                }

                pcEnd = pcItr - 1;

                if (!Util_StrNGetLong(pcStart, pcEnd - pcStart + 1, (long*)&ulFieldEnd))
                {
                    LOG_INFO_FIX("无效条件集，期待一个整数:\"%s\" - \"%s\".",
                                 pcSegStart,
                                 pcStart);
                    return RET_ERR;
                }
            }
        }
        else
        {
            if (!Util_StrNGetLong(pcSubStart, pcSubEnd - pcSubStart + 1, (long*)&ulFieldStart))
            {
                LOG_INFO_FIX("无效条件集，期待一个整数:\"%s\" - \"%s\".",
                             pcSegStart,
                             pcSubStart);
                return RET_ERR;
            }

            ulFieldEnd = ulFieldStart;
        }

        //域判断
        if (uiCdt >= ulFieldStart && uiCdt <= ulFieldEnd)
        {
            if (bReverse)
            { return RET_OK; }
            else
            {
                *pbRslt = true;
                return RET_OK;
            }
        }

        pcItr = pcSubEnd + 1;
    }

    if (bReverse)
    { *pbRslt = true; }

    return RET_OK;
}

static int _Tst_GetExpectRslt(CaseSt* pstCase, int* piCdts, int* piExpect)
{
    int idxCdt;
    int idxRslt;
    const char* pcCdtStart;
    int iCdtLen;
    const char* pcItr;
    const char* pcCdtName;
    int iCdtNameLen;
    int iTmp;
    bool bMatchOk;

    UTIL_CHK(NULL != pstCase && NULL != piExpect);

    *piExpect = pstCase->stRslt.iDfltRslt;

    //若未有指定结果集，按默认值处理。
    if (0 == pstCase->stRslt.iNum || 0 == pstCase->stCdtDesc.iNum)
    { return RET_OK; }

    if (NULL == piCdts)
    {
        LOG_ERR("未指定场景条件。");
        return RET_ERR;
    }

    //遍历匹配全部条件集。
    for (idxRslt = 0; idxRslt < pstCase->stRslt.iNum; ++idxRslt)
    {
        pcItr = pstCase->stRslt.pstRslts[idxRslt].pcCdtSet;
        bMatchOk = true;

        //验证单个条件集。对以未指定的条件因子按照匹配处理。
        while (*pcItr != '\0')
        {
            //得到并校验单个条件段。条件段以分号隔开。
            pcCdtStart = pcItr;

            while (';' != *pcItr && '\0' != *pcItr)
            { ++pcItr; }

            iCdtLen = pcItr - pcCdtStart;

            if (';' == *pcItr)
            { ++pcItr; }

            //找到对应的条件索引。
            //先得到条件名。
            pcCdtName = pcCdtStart;
            iCdtNameLen = iCdtLen;

            while (UTIL_IS_EMPTY_SYB(*pcCdtName))
            {
                ++pcCdtName;
                --iCdtNameLen;
            }

            iTmp = 0;

            while (pcCdtName[iTmp] != ':' && iTmp <= iCdtNameLen)
            { ++iTmp; }

            if (iTmp >= iCdtNameLen)
            {
                LOG_INFO_FIX("条件集子段缺乏条件头:\"%s\" - \"%s\"",
                             pcCdtStart,
                             pcCdtName);
                return RET_ERR;
            }

            iCdtLen -= (pcCdtName + iTmp + 1 - pcCdtStart);
            pcCdtStart = pcCdtName + iTmp + 1;

            --iTmp;

            while (UTIL_IS_EMPTY_SYB(pcCdtName[iTmp]))
            { --iTmp; }

            iCdtNameLen = iTmp + 1;

            //找到条件索引。
            idxCdt = 0;

            while (idxCdt < pstCase->stCdtDesc.iNum)
            {
                if (0 == Util_StrNCmpIgrUp(pcCdtName,
                                           pstCase->stCdtDesc.pstParas[idxCdt].pcName,
                                           iCdtNameLen)
                    && '\0' == pstCase->stCdtDesc.pstParas[idxCdt].pcName[iCdtNameLen])
                { break; }

                ++idxCdt;
            }

            if (idxCdt == pstCase->stCdtDesc.iNum)
            {
                LOG_INFO_FIX("字符串\"%s\"中存在无效条件因子。", pcCdtName);
                return RET_ERR;
            }

            //匹配单个条件段。外部描述从1开始编号，序号需要加1.
            if (RET_OK != _Tst_ChkCdtInSet(piCdts[idxCdt] + 1,
                                           pcCdtStart,
                                           iCdtLen,
                                           &bMatchOk))
            {
                bMatchOk = false;
                break;
            }

            if (!bMatchOk)
            { break; }
        }

        //返回匹配结果。
        if (bMatchOk)
        {
            *piExpect = pstCase->stRslt.pstRslts[idxRslt].iRslt;
            return RET_OK;
        }
    }

    //使用默认结果。
    if (pstCase->stRslt.bUseDflt)
    {
        *piExpect = pstCase->stRslt.iDfltRslt;
        return RET_OK;
    }
    else
    { LOG_INFO_FIX("条件、结果转换表中未定义该场景，且不允许使用默认结果。"); }

    return RET_ERR;
}
