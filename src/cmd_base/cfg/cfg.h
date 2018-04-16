#ifndef _cfg_h_
#define _cfg_h_

#include "base.h"

/***************************************************************************
* 描述: 配置项描述。
***************************************************************************/
typedef struct _CfgItemDescSt
{
    char* pcName;
    char* pcDesc;
    TypeEn eValType;
    char* pcDfltVal;
    bool bConst;
} CfgItemDescSt;

/***************************************************************************
* 描述: 配置相关操作。
***************************************************************************/
int Cfg_AddItems(CfgItemDescSt* pstCfgs, int iNum);//返回成功写入的数目。
bool Cfg_GetInt(const char* pcCfgName, int* piVal);
const char* Cfg_GetStr(const char* pcCfgName);
int Cfg_WriteByStr(const char* pcCfgName, const char* pcVal);
void Cfg_Cls();

//恢复某一配置至默认值。
int Cfg_Recover(const char* pcCfgName);

//恢复全部配置至默认值。
int Cfg_RecoverAll();

//显示配置项当前值，若传入NULL，显示全部配置项的值。
void Cfg_Show(const char* pcCfgName);


#endif // _cfg_h_
