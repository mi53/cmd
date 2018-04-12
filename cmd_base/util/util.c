#include "util.h"
#include "cfg.h"

#include <string.h>
#include <stdarg.h>

//用于做do--while用的常量布尔对象。
const bool g_TlDoWhile = false;

//转换字符串为对应的类型描述。
TypeEn Util_ChangStrToType(const char* pcType)
{
    int iCmp;

    iCmp = Util_StrCmpIgrUp(pcType, "TYPE_INT");

    if (0 == iCmp)
    { return TYPE_INT; }

    iCmp = Util_StrCmpIgrUp(pcType, "TYPE_STR");

    if (0 == iCmp)
    { return TYPE_STR; }

    return TYPE_UNKNOW;
}

//读取文件到缓存中，需要外部释放缓存內存。
int Util_ReadFileToBuffer(const char* pcFile, char** ppcBuf, int* pulReadLen)
{
    FILE* pLk = NULL;
    int iLkLen = 0;
    int iLen;

    //参数检测
    if (NULL == pcFile || NULL == ppcBuf)
    {
        LOG_INFO("Null parameter!");
        return RET_ERR;
    }

    //初始化输出
    *ppcBuf = NULL;

    if (NULL != pulReadLen)
    { *pulReadLen = 0; }

    /************************************************************
    * 1. 打开文件
    ************************************************************/
    pLk = fopen(pcFile, "rb");

    if (NULL == pLk)
    {
        LOG_ERR("file %s is not exist.", pcFile);
        return RET_ERR;
    }


    /************************************************************
    * 2. 获取文件长度并分配内存
    ************************************************************/

    /* 获取文件长度 */
    fseek(pLk, 0L, SEEK_END);
    iLkLen = ftell(pLk);

    if (0 == iLkLen)
    {
        fclose(pLk);
        return RET_OK;
    }

    fseek(pLk, 0L, SEEK_SET);

    /* 分配内存 */
    *ppcBuf = (char*)malloc(iLkLen + 1);

    if (NULL == *ppcBuf)
    {
        LOG_ERR("Malloc fail, malloc size is %d.", iLkLen + 1);
        fclose(pLk);
        Util_Free(*ppcBuf);
        *ppcBuf = NULL;

        return RET_ERR;
    }

    memset(*ppcBuf, 0, iLkLen + 1);

    /************************************************************
    * 3. 读取文件内容
    ************************************************************/
    iLen = fread(*ppcBuf, sizeof(char), iLkLen, pLk);

    if (iLen != iLkLen)
    {
        LOG_ERR("Need read %d bytes, but only read %d bytes.",
                iLkLen,
                iLen);
        fclose(pLk);
        Util_Free(*ppcBuf);
        *ppcBuf = NULL;

        return RET_ERR;
    }

    if (NULL != pulReadLen)
    { *pulReadLen = iLen; }

    /************************************************************
    * 4. 关闭文件
    ************************************************************/
    fclose(pLk);

    return RET_OK;
}

//日志打印函数，限制行长度。最好用于英文打印
void Util_LogFixLine(bool bEndBS, const char* pcFmt, ...)
{
    char* pcOut = NULL;
    char acLineBuf[UTIL_LOG_LINE_LEN + 2];//保留换行及截止符
    va_list pcArgPtr;
    char* pcLineStart;
    char* pcLineEnd;
    int index;
    bool bNeedEnter;//上一行是否需要换行

    if (NULL == pcFmt)
    {
        return;
    }

    pcOut = (char*)malloc(UTIL_LOG_MAX_LEN + 1);

    /************************************************************
    * 1. 获取打印内容
    ************************************************************/
    va_start(pcArgPtr, pcFmt);
    (void)vsnprintf(pcOut, UTIL_LOG_MAX_LEN, pcFmt, pcArgPtr);
    va_end(pcArgPtr);

    /***********************************************************
    * 2. 整理打印内容并打印。
    ***********************************************************/
    pcLineStart = pcOut, index = 0, bNeedEnter = true;

    while (*pcLineStart != '\0')
    {
        /***********************************************************
        * 获取打印行
        ***********************************************************/

        /* 非首行且上一行需要换行先填充两个空格。 */
        if (pcLineStart != pcOut && bNeedEnter)
        {
            acLineBuf[0] = ' ';
            acLineBuf[1] = ' ';
            index = 2;

            /* 去掉自动换行行首多余空格 */
            while (' ' == *pcLineStart || '\t' == *pcLineStart)
            { ++pcLineStart; }
        }
        else
        { index = 0; }

        pcLineEnd = pcLineStart, bNeedEnter = true;

        while (index < UTIL_LOG_LINE_LEN && *pcLineEnd != '\0'
               && *pcLineEnd != '\n')
        {
            /* table字符做四个空格处理，若在自动换行行首则忽略 */
            if ('\t' == *pcLineEnd)
            {
                if (index < UTIL_LOG_LINE_LEN - 4)
                {
                    acLineBuf[index++] = ' ';
                    acLineBuf[index++] = ' ';
                    acLineBuf[index++] = ' ';
                    acLineBuf[index++] = ' ';
                }
                else
                {
                    ++pcLineEnd;
                    break;
                }
            }
            else
            { acLineBuf[index++] = *pcLineEnd; }

            ++pcLineEnd;
        }

        acLineBuf[index] = '\0';

        /* 最后一个未完成的字退回处理 */
        if (UTIL_LOG_LINE_LEN == index
            && *pcLineEnd != ' '
            && *pcLineEnd != '\0'
            && *pcLineEnd != '\n'
            && bEndBS)
        {
            char* pcTmpEnd;

            /* 保留END */
            pcLineStart = pcLineEnd;

            /* 退回到最后一个空格出 */
            while (index > 0 && acLineBuf[--index] != ' ')
            { --pcLineEnd; }

            pcTmpEnd = acLineBuf + (++index);

            /* 判断前序是否存在有效字符，若无无须换行 */
            while (index > 0 && acLineBuf[--index] == ' ');

            /* 若本行就为一个单词，无须退回且不用换行。 */
            if (index <= 0)
            {
                bNeedEnter = false;
                pcLineEnd = pcLineStart;
            }
            else
            {
                bNeedEnter = true;
                *pcTmpEnd = '\0';
                pcLineEnd;
            }
        }
        else if ('\n' == *pcLineEnd)
        { ++pcLineEnd; }
        else if ('\0' == *pcLineEnd)
        {
            bNeedEnter = false;
        }


        /***********************************************************
        * 打印一行。
        ***********************************************************/
        {
            if (bNeedEnter)
            { Util_Log("%s\n", acLineBuf); }
            else
            { Util_Log("%s", acLineBuf); }
        }

        pcLineStart = pcLineEnd;
    }

    Util_Free(pcOut);
}


//校验变参是否为NULL
bool _Util_ChkIsNull(const char* pcFmt, ...)
{
    if (NULL == pcFmt)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//日志打印函数,不限制行长度。
void Util_Log(const char* pcFmt, ...)
{
    char* pcOut = NULL;
    va_list pcArgPtr;

    if (NULL == pcFmt)
    {
        return;
    }

    pcOut = (char*)malloc(UTIL_LOG_MAX_LEN + 1);

    /************************************************************
    * 1. 获取打印内容
    ************************************************************/
    va_start(pcArgPtr, pcFmt);

    vsnprintf(pcOut, UTIL_LOG_MAX_LEN, pcFmt, pcArgPtr);
    pcOut[UTIL_LOG_MAX_LEN] = '\0';

    va_end(pcArgPtr);

    /***********************************************************
    * 2. 打印。
    ***********************************************************/
    printf("%s", pcOut);

    /***********************************************************
    * 3. 备注到日志文件。
    ***********************************************************/
    if (NULL != UTIL_TRACE_FILE)
    {
        FILE* pLog = fopen(UTIL_TRACE_FILE, "a+");

        if (NULL != pLog)
        {
            fprintf(pLog, "%s", pcOut);
            fclose(pLog);
        }
    }

    Util_Free(pcOut);
}

//忽略大小写比较字符串。
int Util_StrCmpIgrUp(const char* pcLeft, const char* pcRight)
{
    int index = 0;
    int iUpOffset = 'A' - 'a';

#define UTIL_STR_CMP_MAX_LEN 1024*1024*100

#define _TST_IS_SYB(x) \
    ((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z'))

#define _TST_CH_EQ_IGRUP(ch1, ch2) \
    (ch1 == ch2) || (_TST_IS_SYB(ch1) && _TST_IS_SYB (ch2) \
                     && (ch1 - ch2 == iUpOffset || ch2 - ch1 == iUpOffset))

    if (pcLeft == pcRight && NULL != pcLeft)
    {
        return 0;
    }
    else if (NULL == pcLeft || NULL == pcRight)
    {
        LOG_ERR("传入空指针进行字符串比较。");
        return -1;
    }

    while (index < UTIL_STR_CMP_MAX_LEN)
    {
        if (_TST_CH_EQ_IGRUP(pcLeft[index], pcRight[index]))
        {
            if ('\0' == pcLeft[index])
            { return 0; }

            ++index;
        }
        else
        {
            return pcLeft[index] - pcRight[index];
        }
    }

    LOG_ERR("超长字符串。");
    return -1;
}

//忽略大小写比较字符串前N个字符。
int Util_StrNCmpIgrUp(const char* pcLeft, const char* pcRight, int iCount)
{
    char* pcBufLeft;
    char* pcBufRight;
    int iCmp;

    if (NULL == pcLeft || NULL == pcRight || iCount <= 0)
    {
        LOG_ERR("字符串比较传入无效参数。");
        return -1;
    }

    pcBufLeft = (char*)malloc(iCount + 1);
    UTIL_CHK(NULL != pcBufLeft);
    pcBufRight = (char*)malloc(iCount + 1);
    UTIL_CHK(NULL != pcBufRight);

    memset(pcBufLeft, 0, iCount + 1);
    memset(pcBufRight, 0, iCount + 1);

    strncpy(pcBufLeft, pcLeft, iCount);
    strncpy(pcBufRight, pcRight, iCount);

    iCmp = Util_StrCmpIgrUp(pcBufLeft, pcBufRight);

    Util_Free(pcBufLeft);
    Util_Free(pcBufRight);

    return iCmp;
}

//拷贝字符串。
char* Util_StrCpy(const char* pcSrc)
{
    char* pcRet;
    int iLen;

    if (NULL == pcSrc)
    {
        return NULL;
    }

    /***********************************************************
    * 获取字符串长度。不考虑结尾符号
    ***********************************************************/
    iLen = -1;

    while ('\0' != pcSrc[++iLen]);

    /***********************************************************
    * 拷贝
    ***********************************************************/
    pcRet = (char*)malloc(iLen + 1);
    UTIL_CHK_DESC(pcRet != NULL, "内存请求失败.");
    memcpy(pcRet, pcSrc, iLen);
    pcRet[iLen] = '\0';

    return pcRet;
}

//拷贝字符串。
char* Util_StrNCpy(const char* pcSrc, int iNum)
{
    int index;
    char* pcRet;

    if (NULL == pcSrc)
    {
        return NULL;
    }

    /***********************************************************
    * 获取字符串长度。
    ***********************************************************/
    for (index = 0; index < iNum; ++index)
    {
        if ('\0' == pcSrc[index])
        {
            break;
        }
    }

    //计算长度，不考虑结尾符号。
    if (index != iNum)
    {
        iNum = index;
    }

    /***********************************************************
    * 拷贝
    ***********************************************************/
    pcRet = (char*)malloc(iNum + 1);
    UTIL_CHK_DESC(pcRet != NULL, "内存请求失败.");
    memcpy(pcRet, pcSrc, iNum);
    pcRet[iNum] = '\0';

    return pcRet;
}


//校验字符串是否为空
bool Util_StrChkEmpty(const char* pcStr)
{
    bool bRet = true;

    if (NULL == pcStr)
    {
        return true;
    }

    while ('\0' != *pcStr)
    {
        if ('\n' != *pcStr && ' ' != *pcStr && '\r' != *pcStr && '\t' != *pcStr)
        {
            bRet = false;
            break;
        }

        ++pcStr;
    }

    return bRet;
}

bool Util_StrNGetLong(const char* pcSrc, int iStrlen, long* plRet)
{
    char* pcTmp = NULL;
    bool bRet;

    if (iStrlen > 0)
    { pcTmp = Util_StrNCpy(pcSrc, iStrlen); }

    if (NULL == pcTmp)
    { return false; }

    bRet = Util_StrGetLong(pcTmp, plRet);
    Util_Free(pcTmp);

    return bRet;
}

//将字符串转换为数字。
bool Util_StrGetLong(const char* pcSrc, long* plRet)
{
    int index = 0;
    bool bIsNeg = false;

    UTIL_CHK(NULL != pcSrc && NULL != plRet);

    *plRet = 0;

    while (pcSrc[index] != '\0')
    {
        if (pcSrc[index] >= '0' && pcSrc[index] <= '9')
        {
            *plRet *= 10;
            *plRet += (pcSrc[index] - '0');
        }
        else if (0 == index && '-' == pcSrc[index])
        {
            bIsNeg = true;
        }
        else if (' ' == pcSrc[index] || '\t' == pcSrc[index])
        {
            ++index;
            continue;
        }
        else
        {
            return false;
        }


        ++index;
    }

    if (bIsNeg)
    {
        *plRet *= -1;
    }

    return true;
}

//功能与fgets类似，会处理日志及标准输入最后的换行符。
const char* Util_FileGets(char* pcBuf, int iLen, FILE* pFile)
{
    int iStrLen;

    UTIL_CHK_DESC(pcBuf != NULL && iLen > 0 && pFile != NULL,
                  "Util_FileGets传入无效参数");

    *pcBuf = '\0';

    fflush(pFile);
    fgets(pcBuf, iLen, pFile);
    fflush(pFile);
    pcBuf[iLen - 1] = '\0';

    /* 去除末尾的换行符 */
    if (stdin == pFile)
    {
        iStrLen = strlen(pcBuf);

        if (iStrLen >= iLen)
        { iStrLen = iLen - 1; }

        if (iStrLen > 0 && '\n' == pcBuf[iStrLen - 1])
        { pcBuf[iStrLen - 1] = '\0'; }
    }

    if (NULL != UTIL_TRACE_FILE)
    {
        FILE* pLog = fopen(UTIL_TRACE_FILE, "a+");

        if (NULL != pLog)
        {
            fprintf(pLog, "%s\n", pcBuf);
            fclose(pLog);
        }
    }

    return pcBuf;
}


//判断文件是否存在，如果不存在依据指定创建文件。
bool Util_FileChk(const char* pcFileName, bool bNeedCreate)
{
    FILE* pFile = NULL;

    if (NULL == pcFileName)
    { return false; }

    pFile = fopen(pcFileName, "rb");

    if (NULL == pFile && bNeedCreate)
    {
        //保证不截断现有文件。
        pFile = fopen(pcFileName, "ab+");
    }

    if (NULL != pFile)
    {
        fclose(pFile);
        return true;
    }

    return false;
}


//检验参数是否为空再释放内存。
void Util_Free(void* pMem)
{
    if (NULL != pMem)
    {
        free(pMem);
        pMem = NULL;
    }
}


//数据比较。
int Util_DataCmp(const DataSt* pstLeft, const DataSt* pstRight, int* piRslt)
{
    if (NULL == pstLeft || NULL == pstRight || NULL == piRslt
        || NULL == pstLeft->pData || NULL == pstRight->pData)
    {
        LOG_ERR("para null");
        return RET_ERR;
    }

    //暂时只支持TYPE_INT, TYPE_STR,
    if (pstLeft->eType != pstRight->eType)
    {
        return RET_ERR;
    }

    switch (pstLeft->eType)
    {
        case TYPE_INT:
            *piRslt = *(int*)pstLeft->pData - *(int*)pstRight->pData;
            break;

        case TYPE_STR:
            *piRslt = strcmp((char*)pstLeft->pData, (char*)pstRight->pData);
            break;

        default:
            LOG_ERR("data type invalid.");
            return RET_ERR;
    }


    return RET_OK;
}

//数据拷贝。
DataSt* Util_DataCpy(DataSt* pstData)
{
    DataSt* pstRet = NULL;

    UTIL_CHK(pstData != NULL);

    pstRet = (DataSt*)malloc(sizeof(DataSt));

    Util_DataWrite(pstRet, pstData->eType, pstData->pData, 0);

    return pstRet;
}

//写入数据。若iValLen为0，不参考此参数。
int Util_DataWrite(DataSt* pstData, TypeEn eType, const void* pVal, int iValLen)
{
    UTIL_CHK(NULL != pstData && NULL != pVal);

    pstData->eType = eType;

    switch (eType)
    {
        case TYPE_INT:
            if (iValLen != 0 && iValLen != sizeof(int))
            {
                return RET_ERR;
            }

            pstData->pData = malloc(sizeof(int));
            UTIL_CHK(NULL != pstData->pData);
            *(int*)pstData->pData = *(int*)pVal;

            break;

        case TYPE_STR:
            if (0 == iValLen)
            {
                iValLen = strlen((char*)pVal);
            }

            pstData->pData = malloc(iValLen + 1);
            UTIL_CHK(NULL != pstData->pData);
            memcpy(pstData->pData, pVal, iValLen);
            ((char*)pstData->pData)[iValLen] = '\0';
            break;

        default:
            return RET_ERR;
    }

    return RET_OK;
}

//写入数据。
int Util_DataWriteByStr(DataSt* pstData, const char* pcVal, int iLen)
{
    char* pcTmp = NULL;
    int iRet;
    long lVal;
    int iVal;
    const void* pValStart;
    int iValLen;

    UTIL_CHK(NULL != pstData && NULL != pcVal && 0 < iLen);

    pcTmp = (char*)malloc(iLen + 1);
    strncpy(pcTmp, pcVal, iLen);
    pcTmp[iLen] = '\0';

    switch (pstData->eType )
    {
        case TYPE_INT:
            if (!Util_StrGetLong(pcTmp, &lVal))
            {
                Util_Free(pcTmp);
                LOG_INFO("数据类型不符合要求。");
                return RET_ERR;
            }

            iVal = (int)lVal;

            pValStart = &iVal;
            iValLen = sizeof(int);
            break;

        case TYPE_STR:
            pValStart = pcTmp;
            iValLen = strlen(pcTmp);
            break;

        default:
            LOG_INFO("未知的数据类型。");
            Util_Free(pcTmp);
            return RET_ERR;
    }

    iRet = Util_DataWrite(pstData, pstData->eType, pValStart, iValLen);

    Util_Free(pcTmp);
    return iRet;
}


//读取数据。
bool Util_DataGetInt(DataSt* pstData, int* piVal)
{
    if (NULL == pstData || NULL == piVal || NULL == pstData->pData)
    {
        LOG_ERR("空指针。");
        return false;
    }

    if (TYPE_INT != pstData->eType)
    {
        LOG_ERR("从非整数类型的数据中读取一个整数。");
        return false;
    }

    *piVal = *(int*)pstData->pData;

    return true;
}

//读取数据。
const char* Util_DataGetStr(DataSt* pstData)
{
    if (NULL == pstData)
    {
        LOG_ERR("空指针。");
        return NULL;
    }

    if (TYPE_STR != pstData->eType)
    {
        LOG_ERR("从非整数类型的数据中读取一个整数。");
        return NULL;
    }

    return (const char*)pstData->pData;
}

//写入数据。
bool Util_DataWriteInt(DataSt* pstData, int iVal)
{
    int iRet;
    iRet = Util_DataWrite(pstData, TYPE_INT, &iVal, sizeof(int));

    if (RET_OK != iRet)
    {
        return false;
    }

    return true;
}

//写入数据。
const char* Util_DataWriteStr(DataSt* pstData, const char* pcVal);

//清理DataSt
void Util_DataCls(DataSt* pstData)
{
    if (NULL != pstData)
    {
        Util_Free(pstData->pData);
        pstData->pData = NULL;
    }
}

//校验一个TypeEn对象是否有效。
bool Util_ChkType(TypeEn eType)
{
    switch (eType)
    {
        case TYPE_INT:
        case TYPE_STR:
            return true;
    }

    return false;
}

//在数组尾部扩展数组，返回当前结点数目。额外增加一个空数据结点。
int Util_ArrayExpand(void** ppArray,
                     unsigned int uiUnitSize,
                     unsigned int uiCurNum,
                     unsigned int uiExpand)
{
    void* pTmp = NULL;
    unsigned int uiSize = 0;

    if (NULL == ppArray || 0 == uiExpand || 0 == uiUnitSize)
    {
        LOG_ERR("ppArray is null.");
        return uiCurNum;
    }

    /* 分配新空间，额外扩展一个空白结点 */
    uiSize = uiUnitSize * (uiCurNum + uiExpand + 1);
    pTmp = malloc(uiSize);

    if (NULL == pTmp)
    {
        return uiCurNum;
    }

    (void)memset(pTmp, 0, uiSize);

    /* 更新数据 */
    if (uiCurNum > 0)
    {
        (void)memcpy(pTmp, *ppArray, uiUnitSize * uiCurNum);
    }

    Util_Free(*ppArray);
    *ppArray = pTmp;

    return uiCurNum + uiExpand;
}



//在数组尾部扩展数组，返回当前结点数目。
int Util_ArrayExpandAuto(void** ppArray,
                         unsigned int uiUnitSize,
                         unsigned int uiCurNum,
                         INOUT unsigned int* puiCap)
{
    unsigned int uiExpand = 0;
    int iRet;

#define UTIL_ARRAY_AUTO_EXPAND_NUM 10

    if (NULL == ppArray || NULL == puiCap)
    {
        LOG_ERR("null para.");
        return uiCurNum;
    }

    if (*puiCap > uiCurNum)
    {
        return uiCurNum + 1;
    }

    uiExpand = *puiCap - uiCurNum;

    if (uiExpand < UTIL_ARRAY_AUTO_EXPAND_NUM)
    {
        uiExpand = UTIL_ARRAY_AUTO_EXPAND_NUM;
    }

#undef UTIL_ARRAY_AUTO_EXPAND_NUM

    iRet = Util_ArrayExpand(ppArray, uiUnitSize, uiCurNum, uiExpand);

    *puiCap += uiExpand;

    return iRet;
}


//表示函数运行是否出现错误，ok - true;
static bool s_bCallRslt = true;

//true - ok, false - fail.
void Util_CallRsltSet(bool bRslt)
{
    s_bCallRslt = bRslt;
}

bool Util_CallRsltGet()
{
    return s_bCallRslt;
}
