/***************************************************************************
* 描述: 参数定义及字符串参数解析。
***************************************************************************/
#ifndef _para_h_
#define _para_h_

#include "base.h"

/***************************************************************************
* 描述:
***************************************************************************/

typedef struct _ParaDescSt
{
    TypeEn eType;           //字符串或者数据。
    const char* pcName;     //参数名。
    const char* pcDesc;     //参数描述。
    const char* pcDflt;     //默认值指向一个常量。
    const char* pcAssioCfg; //关联配置项。
} ParaDescSt;

typedef struct _ParaSt
{
    DataSt stVal;
} ParaSt;

//命令参数定义结构体。
typedef struct _ParasDescSt
{
    int iNum;
    ParaDescSt* pstParas;
} ParasDescSt;

typedef struct _ParasSt
{
    int iNum;
    ParaSt* pstParas;
} ParasSt;

typedef int (*ParaRunFn)(ParasSt* pstPara, void* pData);
/***************************************************************************
* 描述: 从参数中获取一个整数。从1开始索引。
***************************************************************************/
int* Para_GetInt(ParasSt* pstParas, int iParaIdx);

//必须确保参数正确，否则崩溃。
#define PARA_GETINT(p, id) (*(Para_GetInt(p, id)))

/***************************************************************************
* 描述: 从参数中获取一个字符串。从1开始索引。
***************************************************************************/
char* Para_GetStr(ParasSt* pstParas, int iParaIdx);

/***************************************************************************
* 描述: 参数转换函数。成功返回解析出参数数目。失败返回-1.
***************************************************************************/
int Para_Parse(const ParasDescSt* pstDesc, const char* pcStr, ParasSt* pstParas);

/***************************************************************************
* 描述: 参数校验函数。
***************************************************************************/
bool Para_Chk(const ParasDescSt* pstDesc, ParasSt* pPara);

/***************************************************************************
* 描述: 清理。
***************************************************************************/
void Para_Cls(ParasSt* pstParas);

/***************************************************************************
* 描述: 打印参数。
***************************************************************************/
void Para_Show(const char* pcFuncName, ParasSt* pstParas);

/***************************************************************************
* 描述: 解析单个字段。
***************************************************************************/
typedef struct _CmdMWordSt
{
    //字段、格式。
    const char* pcStr;
    int iStrLen;

    //字段内核。
    const char* pcWord;
    int iWordLen;
} CmdMWordSt;

int Para_GetWord(const char* pcStr, int iStrLen, CmdMWordSt* pstWord);

//命令参数使用
#define DEF_PARA(eType, pcName, pcDesc, pcDflt, pcAssioCfg) \
    {eType, (const char*)pcName, (const char*)pcDesc, (const char*)pcDflt, (const char*)pcAssioCfg},

/***************************************************************************
* 描述: 对字符串类型NULL参数处理.
***************************************************************************/
static void _Para_ChangeNullStr(DataSt* pstData);

#endif // _para_h_
