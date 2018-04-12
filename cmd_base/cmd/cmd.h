#ifndef _cmd_h_
#define _cmd_h_


#include "base.h"
#include "para.h"

/***************************************************************************
* 描述: 架设命令工作环境，简写CmdM。
***************************************************************************/

#define CMD_USER_PARA_MAX 15

/***************************************************************************
* 描述: 定义命令定义信息存储结构。
***************************************************************************/

//命令定义。
typedef struct _CmdFnSt
{
    int iGroup;
    const char* pcCmdStr;
    const char* pcTitle;//简要说明。
    const char* pcDesc; //详细描述。若为NULL，使用pcTitle。
    ParaRunFn pfn;
    ParasDescSt stParasDesc;
} CmdFnSt;

//命令定义。
typedef struct _CmdFnDefParaSt
{
    int iGroup;
    const char* pcCmdStr;
    const char* pcTitle;//简要说明。
    const char* pcDesc; //详细描述。若为NULL，使用pcTitle。
    ParaRunFn pfn;
    ParaDescSt astParas[CMD_USER_PARA_MAX + 1];
} CmdFnDefParaSt;

//几个便于自定义命令的工具宏。
#define _CMD_DEF_FUNC(cmd) CmdUser_##cmd
#define _CMD_DEF_TO_STR(cmd) #cmd

//将宏转换为字符串。
#define CMD_DEF_TO_STR(cmd) _CMD_DEF_TO_STR(cmd)

//命令定义中使用，命令起始。
#define CMD_DEF_START(group, cmd, title, desc) \
    {group, CMD_DEF_TO_STR(cmd), title, desc, _CMD_DEF_FUNC(cmd), {

//命令定义中使用，定义命令涉及的参数。
#define CMD_DEF_PARA(name, type, dflt, cfgItem, desc) \
    {type, name, desc, dflt, cfgItem},

//命令定义中使用，命令定义结束。
#define CMD_DEF_END() {TYPE_INT}}},

//定义命令关联函数的函数头。
#define CMD_DEF_FUNC_HEAD(cmd) \
    static int _CMD_DEF_FUNC(cmd)(ParasSt* pstPara, void* pData)

//用于获取命令关联函数中的参数
#define CMD_DEF_FUNC_PARA(IntOrStr, idx) Para_Get##IntOrStr(pstPara, idx)

//用于获取命令关联函数中附加的pData对象。
#define CMD_DEF_FUNC_DATA() pData


//将带参命令添加到全部命令存储的全局变量需要用到的工具宏。
#define PARAS(x)  {sizeof(x)/sizeof(ParaDescSt), x}

#define CMD_INVALID_ID -1 //命令按索引编号，负值不可能出现。

#define GROUP_DFLT 0  //预订命令组编号。
#define GROUP_HIDE -1 //负数组命令不显示于HELP全显示中。
/***************************************************************************
* 描述: 初始化.
***************************************************************************/
void CmdM_Start();

/***************************************************************************
* 描述: 清除初始化.
***************************************************************************/
void CmdM_Close();

/***************************************************************************
* 描述: 停止接收命令.
***************************************************************************/
void CmdM_Exit();

/***************************************************************************
* 描述: 执行单个命令。需要先调用CmdM_Start。
***************************************************************************/
int CmdM_Exec(const char* pcCmd, void* pData, bool bQuiet);
#define CMDM_EXEC(pcCmd) CmdM_Exec(pcCmd, NULL, false)

/***************************************************************************
* 描述: 使用变参合成命令后再执行。
***************************************************************************/
int CmdM_ExecVa(bool bQuiet, void* pData, const char* pcCmdFmt, ...);

/***************************************************************************
* 描述: 执行命令脚本。
***************************************************************************/
int CmdM_ExecFile(const char* pcCmdFile, bool bQuiet);

/***************************************************************************
* 描述: 命令处理函数。需要先调用CmdM_Start。
***************************************************************************/
void CmdM_Run(const char* apcCmdArray[], int iCmdNum, bool bMoreCmds, CmdTimeFn fnTime);

/***************************************************************************
* 描述: 使用CmdFnSt对象设置用户命令。
***************************************************************************/
void CmdM_AddCmds(CmdFnSt* pstCmds);

/***************************************************************************
* 描述: 使用CmdFnDefParaSt对象设置用户命令。
***************************************************************************/
void CmdM_AddCmdsByDefPara(CmdFnDefParaSt* pstCmds, unsigned int uiNum);

/***************************************************************************
* 描述: 查找命令。
***************************************************************************/
const CmdFnSt* CmdM_CmdGet(const char* pcStr, int iLen);

/***************************************************************************
* 描述: 获取相似命令 -- 以指定字符串起始。
*       若pstLast传入NULL，则获取第一个相似元素，否则获取下一个。
***************************************************************************/
const CmdFnSt* CmdM_CmdGetSimilar(const char* pcStr, int iLen, const CmdFnSt* pstLast);

/***************************************************************************
* 描述: 迭代获取全部命令。输入空元素，返回首个元素。
***************************************************************************/
const CmdFnSt* CmdM_CmdItr(const CmdFnSt* pstLast);


#endif // _cmd_h_
