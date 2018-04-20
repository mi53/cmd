/**************************************************************************
	purpose  :	测试用例或相关命令可能用到的一些配置。
***************************************************************************/
#ifndef _cfg_r5_h_
#define _cfg_r5_h_

#include "cfg_common.h"
#include "lic_inc.h"
#include "callback.h"


/***************************************************************************
* 配置一: 配置。
***************************************************************************/
#define CFG_KEY                 "CFG_KEY"
#define CFG_LK                  "CFG_LK"
#define CFG_ESN                 "CFG_ESN"
#define CFG_PS                  "CFG_PS"
#define CFG_PRD_ID              "CFG_PRD_ID"
#define CFG_PRD_NAME            "CFG_PRD_NAME"
#define CFG_PRD_VER             "CFG_PRD_VER"
#define CFG_PRD_ID_STR          "CFG_PRD_ID_STR"
#define CFG_PRD_DFLT            "CFG_PRD_DFLT"

#define CFG_PRD_SAFETIME_ENABLE            "CFG_PRD_SAFETIME_ENABLE"
#define CFG_PRD_MINVALUE_ENABLE            "CFG_PRD_MINVALUE_ENABLE"
#define CFG_PRD_TIXTIME_ENABLE             "CFG_PRD_TIXTIME_ENABLE"
#define CFG_PRD_ESNVERMISONCE_ENABLE       "CFG_PRD_ESNVERMISONCE_ENABLE"
#define CFG_PRD_R5C01CMP_ENABLE            "CFG_PRD_R5C01CMP_ENABLE"


//c01持久化文件
#define CFG_PS_R2C05        "CFG_PS_R2C05"
#define CFG_PS_C01_1        "CFG_PS_C01_1"
#define CFG_PS_C01_2        "CFG_PS_C01_2"

//配置BBOM到临时对象相关配置文件。
#define CFG_BBOM_TO_TEMP        "cfg/cfg_bbom.txt"
#define CFG_BBOM_TO_TEMP_STICK  "cfg/cfg_stick_bbom.txt"
#define CFG_BBOM_TO_TEMP_EM     "cfg/cfg_em_bbom.txt"

//state chk配置到临时对象相关配置文件
#define CFG_STATE_CHK_TO_TEMP   "cfg/cfg_state_chk.txt"

//适配系统复位的操作,其实就是把保密文件的路径指向别的目录，让ALM正常Stop，而后恢复原来保密文件的路径
#define CFG_PS_TMP               "CFG_PS_TMP"
#define CFG_PS_C01_1_TMP    "CFG_PS_C01_1_TMP"
#define CFG_PS_C01_2_TMP    "CFG_PS_C01_2_TMP"

#define GET_CFG_STR(s) s
#endif // _cfg_r5_h_
