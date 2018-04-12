#ifndef _test_h_
#define _test_h_

#include "base.h"
#include "cmd.h"


/***************************************************************************
* 描述: 用例描述。
***************************************************************************/
typedef bool (*CaseRsltChkFn)(void* pPara);
typedef int (*CaseRunFn)(ParasSt* pstCdtsVal, int* iRslt);

//条件描述。
typedef struct _CaseCdtSt
{
    const char* pcCdtName;
    const char* pcCdtVal;
} CaseCdtSt;

typedef struct _CaseCdtsSt
{
    int iNum;
    CaseCdtSt* pstCdts;
} CaseCdtsSt;

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
typedef struct _CaseRsltItemSt
{
    const char* pcCdtSet;
    int iRslt;
} CaseRsltItemSt;

typedef struct _CaseRsltSt
{
    int iDfltRslt;
    bool bUseDflt;
    int iNum;
    CaseRsltItemSt* pstRslts;
} CaseRsltSt;

//用例总体信息。
typedef struct _CaseSt
{
    const char* pcBase;
    const char* pcDesc;
    CaseRunFn fnBase;

    ParasDescSt stCdtDesc;//条件项描述。
    CaseCdtsSt stCdts; //具体的条件域。
    CaseRsltSt stRslt; //场景与预期结果匹配表。

    void* pCdtsAftP;//解析后的条件，配置用例时无需关注，将自动生成。
} CaseSt;



/***************************************************************************
* 描述: 测试入口。
***************************************************************************/
void Tst_CaseRun(const char* pcBase, const char* pcCdt);

/***************************************************************************
* 描述: 设置用例。
***************************************************************************/
int Tst_SetCase(CaseSt** apstCase);

/***************************************************************************
* 描述: 清理用例。
***************************************************************************/
void Tst_ClsCase();

/***************************************************************************
* 描述: 一些工具宏。
***************************************************************************/
#define TST_CALL_RET(x) {if ((x) != RET_OK) return RET_ERR;}
#define TST_CALL_LAB(x, lab) {if ((x) != RET_OK) goto lab;}
#define TST_CASE_ADD(name, desc) {#name, desc, name}
#define TST_CDT_DESC(cdt_desc) {sizeof(cdt_desc)/sizeof(ParaDescSt), cdt_desc}
#define TST_CDT_ITEM(pcCdtName, pcCdtVal) {pcCdtName, (const char* )pcCdtVal},
#define TST_CDT_ADD(cdt) {sizeof(cdt)/sizeof(CaseCdtSt), cdt}
#define TST_RSLT_ITEM(pcCdtSet, iRslt) {(const char*)pcCdtSet, iRslt},
#define TST_RSLT(dflt, bUseDflt, rslts) {dflt, bUseDflt, sizeof(rslts)/sizeof(CaseRsltItemSt), rslts}
/***************************************************************************
* 描述: 校验单个场景的执行结果。
***************************************************************************/
static bool _Tst_CaseRsltChk(CaseSt* pstCase, CaseCdtSt* pstCdt);

#endif // _test_h_
