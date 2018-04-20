/***************************************************************************
* 本文件用于包装ALM头文件，并提供几个ALM相关的工具类函数。
***************************************************************************/

#ifndef _lic_inc_h_
#define _lic_inc_h_

#include "base.h"
#include "alm_intf.h"
#include "alm_util.h"


/***************************************************************************
* 配置: 依据版本号选择对应头文件。
***************************************************************************/

//SPC编号，部分代码受版本影响。
#ifdef _LIC_DEBUG
#define _R5C10_SPC_ID 8
#define _R5C10_B_ID 30
//#define UP_BY_R2C05
#define UP_BY_R5C01
#else
#define _R5C10_SPC_ID 8
#define _R5C10_B_ID 30
//#define UP_BY_R2C05
#define UP_BY_R5C01
#endif

#if (defined(UP_BY_R2C05) && defined(UP_BY_R5C01))
#error Can not support update by r2c05 and r501
#endif

#define R5C10_VER_CHECK_MORE(spc, b) ((_R5C10_SPC_ID*1000 + _R5C10_B_ID) >= (spc*1000 + b))

#define R5C10_VER_CHECK_LESS(spc, b) ((_R5C10_SPC_ID*1000 + _R5C10_B_ID) < (spc*1000 + b))

#ifdef R5_C10

#include "alm_intf.h"
#include "alm_intf_base.h"
#include "alm_intf_common.h"
#include "alm_intf_constant.h"
#include "alm_intf_def.h"
#include "alm_intf_lic_mgr.h"
#include "alm_intf_prd_mgr.h"
#include "alm_intf_state_mgr.h"

#else

#include "alm_intf.h"
#include "alm_intf_base.h"
#include "alm_intf_common.h"
#include "alm_intf_constant.h"
#include "alm_intf_def.h"
#include "alm_intf_lic_mgr.h"
#include "alm_intf_prd_mgr.h"
#include "alm_intf_state_mgr.h"

#endif

/***************************************************************************
* 配置: 基于ALM实现一些工具类函数。
***************************************************************************/

/* 定义状态简写 */
#define TL_ALM_STATE_INVALID                    "INVALID"
#define TL_ALM_STATE_VALID                      "VALID"
#define TL_ALM_STATE_GRACE                      "GRACE"
#define TL_ALM_STATE_DEFAULT                    "DFLT"
#define TL_ALM_STATE_DEMO                       "DEMO"
#define TL_ALM_STATE_NORMAL                     "NORAML"
#define TL_ALM_STATE_EMERGENCY                  "EMERGENCY"
#define TL_ALM_STATE_STICK                      "STICK"
#define TL_ALM_STATE_VERIFY_ITEM_ESN            "ESN"
#define	TL_ALM_STATE_VERIFY_ITEM_PRD_NAME       "PrdName"
#define	TL_ALM_STATE_VERIFY_ITEM_PRD_VER        "PrdVer"
#define TL_ALM_STATE_VERIFY_ITEM_DEADLINE_OVER  "DeadLine"
#define TL_ALM_STATE_VERIFY_ITEM_RVK            "Revoke"


/* 将BBOM类型转换为字符串 */
CONST ALM_CHAR* Tl_AlmBbomTypeToStr(ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType);

/* 将License状态转换为字符串 */
CONST ALM_CHAR* Tl_AlmLicState2Str(CONST ALM_STATE_LIC_STRU* pstLicState);

/* 将License文件类型转换为字符串 */
CONST ALM_CHAR* Tl_AlmLicType2Str(ALM_ENUM(ALM_LIC_TYPE_ENUM) eType);

/* 将产品状态转换为字符串 */
CONST ALM_CHAR* Tl_AlmPrdState2Str(ALM_ENUM(ALM_STATE_PRD_ENUM) eState);

/* 将License状态转换为字符串 */
CONST ALM_CHAR* Tl_AlmState2Str(ALM_ENUM(ALM_STATE_LIC_ENUM) eState);

/* 将校验项转换为字符串 */
CONST ALM_CHAR* Tl_AlmStateChkItem2Str(ALM_ENUM(ALM_STATE_CHECK_ITEM_ENUM) eItem);

/* 将原因转换为字符串 */
CONST ALM_CHAR* Tl_AlmRsnToStr(ALM_UINT64 ulRsn, OUT ALM_CHAR* pcRsn,
                               ALM_UINT32 uiRsnSize);

/* 一个工具性质的调试函数，用于将分钟转换为天、时、分字符串 */
CONST ALM_CHAR* Tl_AlmMins2DayStr(ALM_UINT32 uiMins);

/* 将License状态字符串转换为状态 */
ALM_INT32 Tl_AlmStr2State(CONST ALM_CHAR* pcStr,
                          ALM_ENUM(ALM_STATE_LIC_ENUM)* peState);

/* 将产品状态字符串转换为状态 */
ALM_INT32 Tl_AlmStr2PrdState(CONST ALM_CHAR* pcStr,
                             ALM_ENUM(ALM_STATE_PRD_ENUM)* peState);

/* 将告警原因转换为字符串 */
CONST ALM_CHAR* Tl_AlmAlarmRsnToStr(ALM_UINT64 ulRsn, OUT ALM_CHAR* pcRsn,
                                    ALM_UINT32 uiRsnSize);

/*将告警名称转为字符串*/
CONST ALM_CHAR* Tl_AlmAlarmName2Str(ALM_UINT32 uiAlarmName);

/*将告警类型转为字符串*/
CONST ALM_CHAR* Tl_AlmAlarmType2Str(ALM_UINT32 uiAlarmType);

#define ALM_strcat_s(pcDst, uiSize, pcSrc) strcat(pcDst, pcSrc)
#define ALM_sprintf_s(pcDst, uiSize, pcFmt, ...) sprintf(pcDst, pcFmt, __VA_ARGS__)
/***************************************************************************
* ALM库中提供的调试接口。
***************************************************************************/
typedef ALM_VOID (*ALM_DBG_PRINT_FUNC)(const char* pcFmt, ...);

/* 设置调测打印函数，默认使用printf */
ALM_VOID ALM_DbgSetPrint(ALM_DBG_PRINT_FUNC fnPrint);

/* 模拟运行指定分钟，并按照定时间隔周期触发定时校验 */
/* 本函数不会更新当前时间 */
ALM_VOID ALM_DbgRunTimer(ALM_UINT32 uiRunMins);

/* 打印指定表格内容 */
ALM_VOID ALM_DbgShowTable(CONST ALM_CHAR* pcTbl);

//打印当前已激活的License文件的内容。
ALM_VOID ALM_DbgShowLicStore(IN ALM_INT32 iPrdId);

//直接解析License文件
ALM_INT32 ALM_LicParser(CONST ALM_CHAR* pcFileBuf,
                        ALM_UINT32 uiFileLen,
                        CONST ALM_CHAR* pcKey,
                        ALM_UINT32 uiKeyLen,
                        OUT ALM_LIC_STORE_STRU* pstLicStore);

#define GET_PRD_CFG(cfg, id, cfgname) \
    memset(cfg, 0, sizeof(cfg));\
    strcpy(cfg, GET_CFG_STR(cfgname));\
    strcpy(cfg + strlen(GET_CFG_STR(cfgname)), "_");\
    strcpy(cfg + strlen(GET_CFG_STR(cfgname)) + 1, id);\
     
#define ALARM_CHECK_BIT(var, pos) !(!((var) & (pos)))
#define ALARM_SET_BIT(var, pos) ((var) |= (pos))
#define ALARM_CLEAR_BIT(var, pos) ((var) &= ~(pos))
#define ALARM_RsnAdd_M(ulRsn, eRsnItem) \
    ((ulRsn) |= (((ALM_UINT64)1) << eRsnItem))

/***************************************************************************
* 描述: ALM函数日志宏。
***************************************************************************/
#define LIC_CALL_FN(name, ulRslt) \
    {\
        if (ALM_OK != ulRslt)\
        {\
            LOG_ERR_FIX_BS(""#name" failed!!! - 0x%x - %s.",\
                           ulRslt,\
                           ALM_ErrStr(ulRslt));\
        }/*\
        else\
        {\
            LOG_INFO(#name" ok.");\
        }*/\
    }

#define LIC_CALL_FN_RET0(name, ulRslt) \
    {\
        unsigned long _ulRet_maroc_tmp_ = (ulRslt);\
        LIC_CALL_FN(name, _ulRet_maroc_tmp_);\
        if (RET_OK != _ulRet_maroc_tmp_)\
        {\
            return ;\
        }\
    }

#define LIC_CALL_FN_RET(name, ulRslt) \
    {\
        unsigned long _ulRet_maroc_tmp_ = (ulRslt);\
        LIC_CALL_FN(name, _ulRet_maroc_tmp_);\
        if (RET_OK != _ulRet_maroc_tmp_)\
        {\
            return RET_ERR;\
        }\
    }

#define LIC_CALL_FN_LAB(name, ulRslt, lab) \
    {\
        unsigned long _ulRet_maroc_tmp_ = (ulRslt);\
        LIC_CALL_FN(name, _ulRet_maroc_tmp_);\
        if (RET_OK != _ulRet_maroc_tmp_)\
        {\
            goto lab;\
        }\
    }

#define LIC_CALL_FN_REAL_RET(name, ulRslt) \
    {\
        unsigned long _ulRet_maroc_tmp_ = (ulRslt);\
        LIC_CALL_FN(name, _ulRet_maroc_tmp_);\
        if (RET_OK != _ulRet_maroc_tmp_)\
        {\
            return ulRslt;\
        }\
    }

#endif // _lic_inc_h_
