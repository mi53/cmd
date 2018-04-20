/**************************************************************************
	purpose  :	本文件中添加命令。
***************************************************************************/

#ifndef _cmddef_inc_
#define _cmddef_inc_

#include "cfg_r5.h"
#include "lic_inc.h"
#include "cmd_r5.h"
#include "cmdfn.h"

/***************************************************************************
* 声明。命令函数中无需对参数的正确性进行处理。传入参数不正确，命令不会启动。
***************************************************************************/

//除了预订组号，其他组号不要设置为负数。
enum _GroupEn
{
    GROUP1 = 1,
    GROUP2,
    GROUP3,
    GROUP4,
    GROUP5,
    GROUP6,
    GROUP7,
    GROUP8,
    GROUP9,
    GROUP10 = 10,
    GROUP100 = 100,
};

/***************************************************************************
* 添加命令，以命令的索引作为编号。第一项为组编号，显示帮助时可以指定组编号。
* 组编号必须大于等于0.
***************************************************************************/


// 简单工具
CMD_DEF_FUNC_HEAD(CMD_G100_TBL_SHOW);
CMD_DEF_FUNC_HEAD(CMD_G100_MK_BBOM);
CMD_DEF_FUNC_HEAD(CMD_G100_PS_RM);
CMD_DEF_FUNC_HEAD(CMD_G100_Temp);
CMD_DEF_FUNC_HEAD(CMD_G100_RESET);
CMD_DEF_FUNC_HEAD(CMD_G100_PRD_CFG_ADD);

//定义全部命令。
static CmdFnDefParaSt s_astUserCmds[] = 
{    
    /***************************************************************************
    * GROUP100 - 工具类。
    ***************************************************************************/   
    CMD_DEF_START(GROUP100, CMD_G100_TBL_SHOW, "打印表格", NULL)
    CMD_DEF_PARA("pcTblName", TYPE_STR, "NULL", NULL, "需要查看的表格名，若为NULL显示全部")
    CMD_DEF_END()

    CMD_DEF_START(GROUP100, CMD_G100_MK_BBOM, "生成bbom的命令", NULL)
    CMD_DEF_PARA("pcPrdId", TYPE_STR, NULL, CFG_PRD_ID_STR, "使用哪个产品来解析license文件")
    CMD_DEF_PARA("pcLicFile", TYPE_STR, NULL, NULL, "待解析License文件路径")
    CMD_DEF_END()

    CMD_DEF_START(GROUP100, CMD_G100_PS_RM, "清空持久化文件", NULL)
    CMD_DEF_END()

    CMD_DEF_START(GROUP100, CMD_G100_Temp, "执行一些临时事务", NULL)
    CMD_DEF_END()

    CMD_DEF_START(GROUP100, CMD_G100_RESET, "模拟系统复位：不修改最后的保密文件直接ALM_Stop然后再恢复保密文件，如果执行此步奏有写保密文件的日志输出，则这些写的内容已经丢失。", NULL)
    CMD_DEF_END()

    CMD_DEF_START(GROUP100, CMD_G100_PRD_CFG_ADD, "配置产品信息", NULL)
    CMD_DEF_PARA("pcPrdId", TYPE_STR, NULL, CFG_PRD_ID_STR, "产品编号")
    CMD_DEF_PARA("pccfgname", TYPE_STR, NULL, NULL, "配置名称")
    CMD_DEF_PARA("pctype", TYPE_STR, NULL, NULL, "配置类型，只能为:TYPE_INT或TYPE_STR")
    CMD_DEF_PARA("pcdflt", TYPE_STR, NULL, NULL, "配置默认值")
    CMD_DEF_PARA("pcdesc", TYPE_STR, NULL, NULL, "配置描述")
    CMD_DEF_PARA("bConst", TYPE_INT, "0", NULL, "配置是否为常量活")
    CMD_DEF_END()
};


/***************************************************************************
* 描述: 添加函数前需先声明符号, 添加函数。若为变成，参数数据建议写最大值。
***************************************************************************/
static FnDescSt s_astUserFns[] = {
    //FN_ADD(printf, FN_MAX_PARA_NUM, "std:printf。")
    FN_END()
};


/***************************************************************************
* 定义一些工具性的内容符号。
***************************************************************************/

typedef struct
{
    ALM_SLIC_BBOM_STRU* pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
}ALM_SLIC_BBOMS_STRU;

//用于保存多个紧急或Stick Bbom定义。
static ALM_SLIC_BBOMS_STRU s_stEmOrStickBboms = {0};

//添加BBOM到临时对象。
static void _Cmd_EmOrStickAddBbomToTempObj(const ALM_SLIC_BBOM_STRU* pstBbom);

static void _Cmd_EmOrStickBbomsShow();

typedef struct
{
    ALM_PRD_BBOM_STRU* pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
}ALM_PRD_BBOMS_STRU;

//用于保存待注册的多个BBOM。
static ALM_PRD_BBOMS_STRU s_stPrdBboms = {0};

//添加BBOM到临时对象。
static void _Cmd_PrdTempBbomsAdd(const ALM_PRD_BBOM_STRU* pstBbom);

static void _Cmd_PrdTempBbomsShow();

//配置回调，回调也可以通过参数设置
#define CBFN_SMBL_ADD(callbacktype, pcFnName) FN_SMBL_ADD(pcFnName)
#define CBFN_ADD(callbacktype, pcFnName, iParaNum, pcDesc) FN_ADD(pcFnName, iParaNum, pcDesc)

//CBFN_SMBL_ADD(ALM_ADAPTER_NOTIFY_CHANGE_FUNC, Adapter_NotifyChange)
//CBFN_SMBL_ADD(ALM_ADAPTER_ALARM_FUNC, Adapter_Alarm)

//CBFN_SMBL_ADD(ALM_ADAPTER_BASE_MEM_FREE_FUNC, OS_MemAlmFree)

static FnDescSt s_astOtherCallbackFns[] =
{
    //FN_ADD(printf, FN_MAX_PARA_NUM, "std:printf。")
    CBFN_ADD(ALM_ADAPTER_NOTIFY_CHANGE_FUNC, Adapter_NotifyChange, 0, "状态回调。")
    CBFN_ADD(ALM_ADAPTER_ALARM_FUNC, Adapter_Alarm, 0, "告警回调。")
    FN_END()
};

static FnDescSt s_astBaseCallbackFns[] =
{
    //FN_ADD(printf, FN_MAX_PARA_NUM, "std:printf。")
    CBFN_ADD(ALM_ADAPTER_BASE_MEM_FREE_FUNC, OS_MemAlmFree, 0, "内存释放。")
    FN_END()
};

#endif // _cmddef_inc_
