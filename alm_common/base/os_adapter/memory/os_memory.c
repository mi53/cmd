/*******************************************************************************
Description    : 内存管理。
History        :
<version> <date>     <Author> <Description>
*******************************************************************************/
#include "os_memory.h"
#include "os.h"
#include <stdlib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

struct
{
    unsigned int uiAlloc;
    unsigned int uiFree;
    unsigned int uiPeak;
    unsigned int uiCntSize;
} s_stAlmMemLog = {0};

//内存附加信息。结构体大小应该为8的整数倍。
typedef struct
{
    unsigned int uiFlag;
    unsigned int uiSize;
} OS_MEM_ADD_ST;

//用于标记内存未被破坏的标记。
#define OS_MEM_FLAG 0xcccccccc

//用于适配ALM的内存分配。
void* OS_MemAlmAlloc(unsigned int uiSize)
{
    return OSMemAlloc(uiSize, true);
}

//用于适配ALM的内存释放。
void OS_MemAlmFree(void* pMem)
{
    OSMemFree(pMem, true);
}


//显示当前内存使用信息。
void OS_MemAlmLog()
{
    LOG_INFO("alloc %u, free %u, current %u bytes, peak %u bytes.",
             s_stAlmMemLog.uiAlloc, s_stAlmMemLog.uiFree,
             s_stAlmMemLog.uiCntSize, s_stAlmMemLog.uiPeak);
}

//清楚内存记录。
void OS_MemAlmLogCls()
{
    memset(&s_stAlmMemLog, 0, sizeof(s_stAlmMemLog));
}


/* 分配内存，建议使用对应的宏进行操作 */
void* OSMemAlloc(unsigned int uiSize, bool bAlm)
{
    void* pMem;

    if (bAlm)
    {
        OS_MEM_ADD_ST* pstBlock;

        pstBlock = (OS_MEM_ADD_ST*)malloc(sizeof(OS_MEM_ADD_ST) + uiSize);

        if (NULL == pstBlock)
        {
            abort();
        }

        pstBlock->uiFlag = OS_MEM_FLAG;
        pstBlock->uiSize = uiSize;

        ++s_stAlmMemLog.uiAlloc;
        s_stAlmMemLog.uiCntSize += uiSize;

        if (s_stAlmMemLog.uiCntSize > s_stAlmMemLog.uiPeak)
        {
            s_stAlmMemLog.uiPeak = s_stAlmMemLog.uiCntSize;
        }

        pMem = pstBlock + 1;
    }
    else
    {
        pMem = malloc(uiSize);
    }


    return pMem;
}


/* 释放已分配的内存 */
void OSMemFree(void* pMem, bool bAlm)
{
    if (pMem != NULL)
    {
        if (bAlm)
        {
            OS_MEM_ADD_ST* pstBlock = (OS_MEM_ADD_ST*)pMem - 1;

            if (OS_MEM_FLAG != pstBlock->uiFlag)
            {
                LOG_ERR("内存已经被破坏");
                abort();
            }

            ++s_stAlmMemLog.uiFree;
            s_stAlmMemLog.uiCntSize -= pstBlock->uiSize;

            free(pstBlock);
        }
        else
        {
            free(pMem);
        }
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
