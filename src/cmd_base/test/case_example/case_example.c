#include "case_example.h"
#include "util.h"


/***************************************************************************
* 描述: 样例用例关联的变量。这里仅仅用于模拟一个全景用例。
***************************************************************************/
static int s_iExample = 0;

/***************************************************************************
* 用例: 样例。
* 描述: 样例中，会将设置变量s_iExample的值。
* 场景: 样例。
* 条件: 样例。
* 结果: 成功 - 返回RET_OK且s_iExample的值为2。
***************************************************************************/

//定义条件因子及值域。ParaDescSt后两个成员在此无意义。
static ParaDescSt s_astCdtDesc[] =
{
    {TYPE_INT, "example_val", "样例模拟变量的条件描述"}
};
static CaseCdtSt s_astCdts[] =
{
    {"example_val", "1"},
    {"example_val", "2"},
    {"example_val", "3"}
};

//场景-结果匹配表。
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
static CaseRsltItemSt s_astCdtRslt[] =
{
    {"example_val:1", RET_OK},
    {"example_val:[2-3]", RET_ERR},
};

//汇总用例说明。
CaseSt g_stCaseExample =
{
    "example",
    "exampleESN",
    Case_Example,
    TST_CDT_DESC(s_astCdtDesc),//也可以为{0}.
    TST_CDT_ADD(s_astCdts),//也可以为{0}.如果条件描述为空，这里只能为空。
    TST_RSLT(1, true, s_astCdtRslt)//也可以为{0}.
};

/***************************************************************************
* 描述: 实现样例用例基函数。
***************************************************************************/
static int Case_Example(ParasSt* pstCdts, int* piRslt)
{
    int* piVal = Para_GetInt(pstCdts, 1);

    s_iExample = *piVal;

    //设置执行结果。
    *piRslt = 0;

    return RET_OK;
}

/***************************************************************************
* 描述: 使用一个全局变量存储全部用例。
***************************************************************************/
static CaseSt* s_apstCaseExample[] =
{
    &g_stCaseExample,
    NULL
};

/***************************************************************************
* 描述: 初始化样例.
***************************************************************************/
void Case_ExampleInit()
{
    Tst_SetCase(s_apstCaseExample);
}
