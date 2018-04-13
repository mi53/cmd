/*******************************************************************************
Description    : 内存管理
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#ifndef __OS_MEMORY_H__
#define __OS_MEMORY_H__

#include "def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

/*******************************************************************
* 内存操作。
*******************************************************************/

//简单的内存分配
#define OS_MemAlloc_M(uiSize) OSMemAlloc(uiSize, false)

//简单的内存释放
#define OS_MemFree_M(pMem) OSMemFree(pMem, false)


//用于适配ALM的内存分配。
void* OS_MemAlmAlloc(unsigned int uiSize);

//用于适配ALM的内存释放。
void OS_MemAlmFree(void* pMem);

//显示当前内存使用信息。
void OS_MemAlmLog();

//清楚内存记录。
void OS_MemAlmLogCls();

/* 分配内存，建议使用对应的宏进行操作 */
void* OSMemAlloc(unsigned int uiSize, bool bAlm);

/* 释放已分配的内存 */
void OSMemFree(void* pMem, bool bAlm);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* __OS_MEMORY_H__ */
