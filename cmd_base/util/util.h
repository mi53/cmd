#ifndef _util_h_
#define _util_h_

#include "base.h"
#include <stdio.h>
#include <stdlib.h>


/***************************************************************************
* 描述: Store.
* 存、取、删。
* 取: 迭代、特征(比较、获取）。
* 数据对象: 大小、初始化、清理。
***************************************************************************/

typedef int (*StoreNodeMarkCmpFn)(void* pL, void* pR);
typedef int (*StoreNodeInitFn)(void* pData);
typedef int (*StoreNodeClsFn)(void* pData);
typedef int (*StoreNodeCpyFn)(void* pData);
typedef int (*StoreNodeMarkGetFn)(void* pData);

typedef struct _StoreNodeSt
{
    int iSize;
    bool bSortIncRepeat;

    StoreNodeMarkCmpFn fnMarkCmp;
    StoreNodeMarkGetFn fnMarkGet;

    StoreNodeInitFn fnInit;
    StoreNodeClsFn fnCls;
    StoreNodeCpyFn fnCpy;
} StoreNodeSt;

typedef enum _StoreTypeEn
{
    STORE_ARRAY,
    STORE_LIST,
    STORE_TREE,
    STORE_BUF
} StoreTypeEn;

typedef struct _StoreSt
{
    StoreTypeEn eType;
    void* pStore;
} StoreSt;

#define HStoreItr void*

void Store_Init(StoreSt* pstStore, StoreTypeEn eType, StoreNodeSt* pstNodeDesc);
void Store_Cls(StoreSt* pstStore);

StoreSt* Store_New(StoreTypeEn eType, StoreNodeSt* pstNodeDesc);
void Store_FreePP(StoreSt** ppstStore);

void* Store_Get(StoreSt* pstStore, void* pMark);
void* Store_Add(StoreSt* pstStore, void* pData);
void Store_Del(StoreSt* pstStore, void* pMark);

HStoreItr Store_ItrFrist(StoreSt* pstStore);
HStoreItr Store_ItrNext(HStoreItr hItr);
bool Store_ItrCheckEnd(HStoreItr hItr);

void* Store_ItrData(HStoreItr hItr);

HStoreItr Store_ItrGet(StoreSt* pstStore, void* pMark);


/*****************************************************************************
* 链表操作。
*****************************************************************************/

/* 链表结点必须包含的成员 */
#define TL_LIST_NODE_MEBR(type) type* pstLast; type* pstNext

/* 定义链表类型 */
#define TL_LIST_DIM(type) \
    struct {type* pstHead; type* pstTail; unsigned int uiTotal; }

/* 添加一个结点，直接添加结点指针。若指定pstLast为空，在头部添加 */
#define TL_ListAdd_M(stList, pstNode, pstLastNode) \
    {\
        (pstNode)->pstLast = (pstLastNode);\
        if ((pstNode)->pstLast != NULL)\
        {\
            (pstNode)->pstNext = (pstNode)->pstLast->pstNext;\
            (pstNode)->pstLast->pstNext = (pstNode);\
        }\
        else\
        {\
            (pstNode)->pstNext = (stList).pstHead;\
            (stList).pstHead = (pstNode);\
        }\
        if (pstNode->pstNext != NULL)\
        {\
            (pstNode)->pstNext->pstLast = (pstNode);\
        }\
        else\
        {\
            (stList).pstTail = (pstNode);\
        }\
        ++(stList).uiTotal;\
    }


/* 删除一个结点，不释放结点本身所占用的空间 */
#define TL_ListDel_M(stList, pstNode) \
    {\
        if ((pstNode)->pstLast != NULL)\
        {\
            (pstNode)->pstLast->pstNext = (pstNode)->pstNext;\
        }\
        else\
        {\
            (stList).pstHead = (pstNode)->pstNext;\
        }\
        if ((pstNode)->pstNext != NULL)\
        {\
            (pstNode)->pstNext->pstLast = (pstNode)->pstLast;\
        }\
        else\
        {\
            (stList).pstTail = (pstNode)->pstLast;\
        }\
        --(stList).uiTotal;\
    }


/***************************************************************************
* 描述: 函数。
***************************************************************************/

//转换字符串为对应的类型描述。
TypeEn Util_ChangStrToType(const char* pcType);

//读取文件到缓存中，需要外部释放缓存內存。
int Util_ReadFileToBuffer(const char* pcFile, char** ppcBuf, int* pulReadLen);

//日志打印函数，限制行长度。最好用于英文打印。bEndBS判断是否处理末尾单词回退。
void Util_LogFixLine(bool bEndBS, const char* pcFmt, ...);

//日志打印函数,不限制行长度。
void Util_Log(const char* pcFmt, ...);

//忽略大小写比较字符串。
int Util_StrCmpIgrUp(const char* pcLeft, const char* pcRight);

//忽略大小写比较字符串前N个字符。
int Util_StrNCmpIgrUp(const char* pcLeft, const char* pcRight, int iCount);

//拷贝字符串。
char* Util_StrCpy(const char* pcSrc);

//拷贝字符串。
char* Util_StrNCpy(const char* pcSrc, int iNum);

//校验字符串是否为空
bool Util_StrChkEmpty(const char* pcStr);

//拷贝字符串。
bool Util_StrNGetLong(const char* pcSrc, int iStrlen, long* plRet);

//将字符串转换为数字。
bool Util_StrGetLong(const char* pcSrc, long* plRet);

//功能与fgets类似，会处理日志及标准输入最后的换行符。
const char* Util_FileGets(char* pcBuf, int iLen, FILE* pFile);

//判断文件是否存在，如果不存在依据指定创建文件。
bool Util_FileChk(const char* pcFileName, bool bNeedCreate);

//检验参数是否为空再释放内存。
void Util_Free(void* pMem);

/***************************************************************************
* 描述: DataSt相关操作。
***************************************************************************/

//数据比较。
int Util_DataCmp(const DataSt* pstLeft, const DataSt* pstRight, int* piRslt);

//数据拷贝。
DataSt* Util_DataCpy(DataSt* pstData);

//写入数据。若iValLen为0，不参考此参数。
int Util_DataWrite(DataSt* pstData, TypeEn eType, const void* pVal, int iValLen);

//写入数据。
int Util_DataWriteByStr(DataSt* pstData, const char* pcVal, int iLen);

//读取数据。
bool Util_DataGetInt(DataSt* pstData, int* piVal);

//读取数据。
const char* Util_DataGetStr(DataSt* pstData);

//写入数据。
bool Util_DataWriteInt(DataSt* pstData, int iVal);

//写入数据。
const char* Util_DataWriteStr(DataSt* pstData, const char* pcVal);

//清理DataSt
void Util_DataCls(DataSt* pstData);

//校验一个TypeEn对象是否有效。
bool Util_ChkType(TypeEn eType);

//在数组尾部扩展数组，返回当前结点数目。额外增加一个空数据结点。
int Util_ArrayExpand(void** ppArray,
                     unsigned int uiUnitSize,
                     unsigned int uiCurNum,
                     unsigned int uiExpand);

//在数组尾部扩展数组，返回当前结点数目。
int Util_ArrayExpandAuto(void** ppArray,
                         unsigned int uiUnitSize,
                         unsigned int uiCurNum,
                         INOUT unsigned int* puiCap);

/***************************************************************************
* 描述: 工具宏。
***************************************************************************/

//用于做do--while用的常量布尔对象。
extern const bool g_TlDoWhile;

#define UTIL_TRACE_FILE "trace.txt"

#define LOG_STR_INFO "[提示] "
#define LOG_STR_ERR  "[错误] "

//校验变参是否为NULL
bool _Util_ChkIsNull(const char* pcFmt, ...);

//日志宏。
#define LOG(...) Util_Log(__VA_ARGS__)

//行尾单词缩进。不适合中文。
#define LOG_FIX_LINE_BS(...) Util_LogFixLine(true, __VA_ARGS__)

//每行以固定宽度换行，可用于中文。用于中文也有可能出现乱码。主要英文字符数目。
#define LOG_FIX_LINE(...) Util_LogFixLine(false, __VA_ARGS__)

//一些关联日志级别的常用日志宏。
#define LOG_INFO_FIX(...) \
    do {\
        if (!_Util_ChkIsNull(__VA_ARGS__))\
        {\
            LOG_FIX_LINE(LOG_STR_INFO);\
            LOG_FIX_LINE(__VA_ARGS__);\
            LOG_FIX_LINE("\n");\
        }\
    } while (g_TlDoWhile)

#define LOG_ERR_FIX(...)  \
    do {\
        Util_CallRsltSet(false);\
        if (!_Util_ChkIsNull(__VA_ARGS__))\
        {\
            LOG_FIX_LINE(LOG_STR_ERR);\
            LOG_FIX_LINE("%s() - %d\n", __FUNCTION__, __LINE__);\
            LOG_FIX_LINE(__VA_ARGS__);\
            LOG_FIX_LINE("\n");\
        }\
    } while (g_TlDoWhile)

#define LOG_INFO_FIX_BS(...)  \
    do {\
        if (!_Util_ChkIsNull(__VA_ARGS__))\
        {\
            LOG_FIX_LINE_BS(LOG_STR_INFO __VA_ARGS__);\
            LOG_FIX_LINE_BS("\n");\
        }\
    } while (g_TlDoWhile)

#define LOG_ERR_FIX_BS(...)  \
    do {\
        Util_CallRsltSet(false);\
        if (!_Util_ChkIsNull(__VA_ARGS__))\
        {\
            LOG_FIX_LINE(LOG_STR_ERR);\
            LOG_FIX_LINE("%s() - %d\n", __FUNCTION__, __LINE__);\
            LOG_FIX_LINE_BS(__VA_ARGS__);\
            LOG_FIX_LINE("\n");\
        }\
    } while (g_TlDoWhile)

#define LOG_INFO LOG_INFO_FIX
#define LOG_ERR  LOG_ERR_FIX

#define LOG_DESC(desc) /*LOG(FMT_SEG2_E),*/ LOG_FIX_LINE(desc)/*, LOG(FMT_SEG2_E)*/

//判断是否为空白字符。
#define UTIL_IS_EMPTY_SYB(ch) (' ' == ch || '\t' == ch || '\r' == ch || '\n' == ch)
#define UTIL_IS_DIG_SYB(ch) (ch >= '0' && ch <= '9')


//校验宏。
#define UTIL_CHK(x) \
    {\
        if (!(x))\
        {\
            LOG_ERR("["#x"]");\
            abort();\
        }\
    }

//带描述的校验宏。
#define UTIL_CHK_DESC(x, desc) \
    {\
        if (!(x))\
        {\
            LOG_ERR(desc"["#x"]");\
            abort();\
        }\
    }

//函数返回值处理。
#define UTIL_CALL_FN(name, ulRslt) \
    {\
        if (RET_OK != ulRslt)\
        {\
            LOG_ERR_FIX_BS(""#name" failed!!! - 0x%x.", ulRslt);\
        }/*\
        else\
        {\
            LOG_INFO(#name" ok.");\
        }*/\
    }

#define UTIL_CALL_FN_RET0(name, ulRslt) \
    {\
        unsigned long _ulRet_maroc_tmp_ = (ulRslt);\
        UTIL_CALL_FN(name, _ulRet_maroc_tmp_);\
        if (RET_OK != _ulRet_maroc_tmp_)\
        {\
            return ;\
        }\
    }

#define UTIL_CALL_FN_RET(name, ulRslt) \
    {\
        unsigned long _ulRet_maroc_tmp_ = (ulRslt);\
        UTIL_CALL_FN(name, _ulRet_maroc_tmp_);\
        if (RET_OK != _ulRet_maroc_tmp_)\
        {\
            return RET_ERR;\
        }\
    }

#define UTIL_CALL_FN_LAB(name, ulRslt, lab) \
    {\
        unsigned long _ulRet_maroc_tmp_ = (ulRslt);\
        UTIL_CALL_FN(name, _ulRet_maroc_tmp_);\
        if (RET_OK != _ulRet_maroc_tmp_)\
        {\
            goto lab;\
        }\
    }

#define UTIL_FN_RET(iRet) {if(RET_OK != (iRet)) return RET_ERR;}
#define UTIL_FN_RET0(iRet) {if(RET_OK != (iRet)) return ;}
#define UTIL_FN_LAB(iRet, lab) {if(RET_OK != (iRet)) goto lab;}

/***************************************************************************
* 描述: 限定及配额。
***************************************************************************/
#define UTIL_LOG_LINE_LEN sizeof(FMT_SEG1) - 2
#define UTIL_LOG_MAX_LEN (1024 * 10) - 1


/**************************************************************************
* 函数调用。
* 被调用函数失败时应该使用UTIL_RET_ERR返回，或者使用了CHK系列宏。
**************************************************************************/
//true - ok, false - fail.
void Util_CallRsltSet(bool bRslt);
bool Util_CallRsltGet();

//函数失败时返回。
#define UTIL_RET_ERR(...) {\
        Util_CallRsltSet(false);\
        return __VA_ARGS__;\
    }

//函数调用。
#define CALL(run) {\
        Util_CallRsltSet(true);\
        {run;};\
        if (!Util_CallRsltGet()){\
            LOG_ERR("");\
        }\
    }

//函数调用失败，返回指定值。
#define CALL_RET(run, ...) {\
        Util_CallRsltSet(true);\
        {run;};\
        if (!Util_CallRsltGet()){\
            LOG_ERR("");\
            return __VA_ARGS__;\
        }\
    }

//函数调用失败，跳转至指定标签。
#define CALL_LAB(run, lab) {\
        Util_CallRsltSet(true);\
        {run;};\
        if (!Util_CallRsltGet()){\
            LOG_ERR("");\
            goto lab;\
        }\
    }

//函数调用失败，跳转至指定标签。
#define CALL_FAIL(run, fail_run) {\
        Util_CallRsltSet(true);\
        {run;};\
        if (!Util_CallRsltGet()){\
            LOG_ERR("");\
            {fail_run;};\
        }\
    }

//函数调用失败，跳转至指定标签。
#define CALL_FAIL_RET(run, fail_run, ...) {\
        Util_CallRsltSet(true);\
        {run;};\
        if (!Util_CallRsltGet()){\
            LOG_ERR("");\
            {fail_run;};\
            return __VA_ARGS__;\
        }\
    }

//函数调用失败，跳转至指定标签。
#define CALL_FAIL_LAB(run, fail_run, lab) {\
        Util_CallRsltSet(true);\
        {run;};\
        if (!Util_CallRsltGet()){\
            LOG_ERR("");\
            {fail_run;};\
            goto lab;\
        }\
    }


/**************************************************************************
* 验证表达式。
**************************************************************************/

//验证表达式。
#define CHK(expr) {\
        Util_CallRsltSet(true);\
        if (!(expr)){\
            LOG_ERR("");\
            Util_CallRsltSet(false);\
        }\
    }

//验证表达式并返回。
#define CHK_RET(expr, ...) {\
        Util_CallRsltSet(true);\
        if (!(expr)){\
            LOG_ERR("");\
            Util_CallRsltSet(false);\
            return __VA_ARGS__;\
        }\
    }

//验证表达式并跳转到指定标签。
#define CHK_LAB(expr, lab) {\
        Util_CallRsltSet(true);\
        if (!(expr)){\
            LOG_ERR("");\
            Util_CallRsltSet(false);\
            goto lab;\
        }\
    }

#define UTIL_REPORT_FILE    "TestReport.html"

int init_log();

void WriteGropuTableToHtml(char* aucGroupName);
void WriteCdtToHtml(int iCdtIdx, char* aucCdtName, DataSt* ptVal);
void WritePassResultToHtml();
void WriteFailResultToHtml(char* acFileName, int iLineNo);
void WriteToHtml(char* acbuf);
void WriteCountToHtml(int* i);
void TestResut(char* pstatus, int LineNo, char* pFileName);

#define STF_ASSERT_VOID(value) \
    do  { \
        if (!(value) || !STF_ASSERT_LAST_RSLT()) \
        { \
            TestResut("FAILED",__LINE__,__FILE__);\
            return;\
        } \
    } while (0);

/** Asserts that actual == expected.
 *  Reports failure with no other action.
 */
#define STF_ASSERT_EQUAL(actual, expected) \
    do \
    { \
        if ((actual) != (expected)) \
        { \
            TestResut("FAILED",__LINE__,__FILE__);\
            /* System error */ \
        } \
        else if ((actual) == (expected)) \
        { \
            TestResut("PASSED",__LINE__,__FILE__);\
            /* System error */ \
        } \
    }while (0);

/** Asserts that actual == expected.
 *  Reports failure and causes test to abort.
 */
#define STF_ASSERT_EQUAL_FATAL(actual, expected) \
    do \
    { \
        if (actual != expected) \
        { \
            TestResut("FAILED",__LINE__,__FILE__);\
            /* System error */ \
            return ; \
        } \
    }while (0);

/** Asserts that string actual == expected.
 *  Reports failure with no other action.
 */
#define STF_ASSERT_STRING_EQUAL(actual, expected) \
    do \
    { \
        if (strcmp(actual, expected)) \
        { \
            TestResut("FAILED",__LINE__,__FILE__);\
            /* System error */ \
        } \
    }while (0);

/** Asserts that string actual == expected.
 *  Reports failure and causes test to abort.
 */
#define STF_ASSERT_STRING_EQUAL_FATAL(actual, expected) \
    do \
    { \
        if (strcmp(actual, expected)) \
        { \
            TestResut("FAILED",__LINE__,__FILE__);\
            /* System error */ \
            return ; \
        } \
    }while (0);

/** Asserts that pointer value is not NULL.
 *  Reports failure and causes test to abort.
 */
#define STF_ASSERT_PTR_NOT_NULL_FATAL(value) \
    do \
    { \
        if (value == NULL) \
        { \
            TestResut("FAILED",__LINE__,__FILE__);\
            /* System error */ \
            return; \
        } \
    }while (0);

#define STF_ASSERT_EQUAL_LAB(actual, expected, lab) \
    do \
    { \
        if ((actual) != (expected)) \
        { \
            TestResut("FAILED",__LINE__,__FILE__);\
            /* System error */ \
            goto lab; \
        } \
    }while (0);


#endif // _util_h_
