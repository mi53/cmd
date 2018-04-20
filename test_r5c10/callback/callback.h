#ifndef _call_back_h_
#define _call_back_h_

#include "def.h"
#include "os.h"
#include "lic_inc.h"


//设置ESN。
ALM_INT32 Adapter_SetEsn(const char* pcEsn);

//得到ESN。
ALM_INT32 Adapter_GetEsn(OUT ALM_ENV_ESN_STRU* pstEsn);

//实现持久性写。
ALM_INT32 Adapter_PsIOWrite(CONST ALM_VOID* pBuf,
                            IN ALM_UINT32 uiLen,
                            IN ALM_UINT32 uiOffSet);

//实现持久化读。
ALM_INT32 Adapter_PsIORead(OUT ALM_VOID* pBuf,
                           INOUT ALM_UINT32* puiLen,
                           IN ALM_UINT32 uiOffSet);

/**
* 描述: 得到R2C05版本持久化文件相关产品数目或相关产品编号。
* piPrdIds - 存储得到的C01持久化存储相关的产品编号的数组。若为空，只输出产品数目。
* puiPrdNum - piPrdIds不为空时，输入piPrdIds的结点数目；否则用于输出产品数目。
*/
ALM_INT32 Adapter_PsR2C05GetPrds(OUT ALM_INT32* piPrdIds,
                                 INOUT ALM_UINT32* puiPrdNum);

/* 读R2C05版本的持久化文件 */
ALM_INT32 Adapter_PsR2C05Read(ALM_INT32 iPrdId,
                              OUT ALM_VOID* pBuf,
                              INOUT ALM_UINT32* puiInNeedOutGetSize,
                              ALM_UINT32 uiOffSet);

/* 写R2C05版本的持久化文件 */
ALM_INT32 Adapter_PsR2C05Write(ALM_INT32 iPrdId,
                               OUT ALM_VOID* pBuf,
                               ALM_UINT32 uiBufSize,
                               ALM_UINT32 uiOffSet);

/**
* 描述: 得到C01版本持久化文件相关产品数目或相关产品编号。
* piPrdIds - 存储得到的C01持久化存储相关的产品编号的数组。若为空，只输出产品数目。
* puiPrdNum - piPrdIds不为空时，输入piPrdIds的结点数目；否则用于输出产品数目。
*/
ALM_INT32 Adapter_PsC01GetPrds(OUT ALM_INT32* piPrdIds,
                               INOUT ALM_UINT32* puiPrdNum);

/* 读R5C01版本的持久化文件 */
ALM_INT32 Adapter_PsC01Read(ALM_INT32 iPrdId,
                            ALM_ENUM(ALM_R5C01_PERSISTENT_TYPE_ENUM) eType,
                            OUT ALM_VOID* pBuf,
                            INOUT ALM_UINT32* puiInNeedOutGetSize,
                            ALM_UINT32 uiOffSet);

/* 写R5C01版本的持久化文件 */
ALM_INT32 Adapter_PsC01Write(ALM_INT32 iPrdId,
                             ALM_ENUM(ALM_R5C01_PERSISTENT_TYPE_ENUM) eType,
                             OUT ALM_VOID* pBuf,
                             ALM_UINT32 uiBufSize,
                             ALM_UINT32 uiOffSet);

/* 打印日志。 */
ALM_INT32 Adapter_Log(ALM_ENUM(ALM_LOG_LEVEL_ENUM) eLevel,
                      CONST ALM_CHAR* pcLogStr);

/* 状态变更通知 */
ALM_INT32 Adapter_NotifyChange(CONST ALM_NOTIFY_CHANGES_STRU* pstChanges);

/* ALM告警回调 */
ALM_INT32 Adapter_Alarm(CONST ALM_ALARM_PRD_CHANGE_STRU* pstArray,
                        ALM_UINT32 uiNum);


/* 将原因附加操作转换为原因字符串 */
static CONST ALM_CHAR* ALMDbgRsnAndOptToRsnStr(
    ALM_ENUM(ALM_STATE_OPERATE_ENUM) eOperate,
    ALM_UINT64 ulRsn,
    OUT ALM_CHAR* pcRsn,
    ALM_UINT32 uiRsnSize);

#endif
