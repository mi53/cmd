/**************************************************************************
	purpose  :	测试用例的实现。
***************************************************************************/

#include "case.h"

/***************************************************************************
* 描述: 测试用例需要添加到此变量中。最后一个元素必须为空。
***************************************************************************/
static CaseSt* s_apstCase[] =
{
    //&g_stCaseEsn,
    //&g_stCaseAct,
    NULL
};

/***************************************************************************
* 描述: 初始化用例。
***************************************************************************/
void Case_Init()
{
    Tst_SetCase(s_apstCase);
}

/***************************************************************************
* 描述: 清理CASE。
***************************************************************************/
void Case_Cls()
{
    Tst_ClsCase();
}
