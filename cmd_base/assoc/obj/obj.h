#ifndef _obj_h_
#define _obj_h_

#include "base.h"


/***************************************************************************
* 描述: obj tmplate.
***************************************************************************/
typedef struct _AmObjTmplSt
{
    TypeEn eValType;
    char* pcName;

    unsigned int uiSubNum;
    unsigned int uiSubCap;
    struct _AmObjTmplSt* pstSubs;
} AmObjTmplSt;

//允许匿名。
void Am_ObjTmplInit(AmObjTmplSt* pstTmpl, const char* pcName, TypeEn eType);
void Am_ObjTmplCls(AmObjTmplSt* pstTmpl);

//返回索引，不存在返回-1.
int Am_ObjTmplSubGet(AmObjTmplSt* pstTmpl, const char* pcSub);
void Am_ObjTmplSubAdd(AmObjTmplSt* pstTmpl, const char* pcSub, TypeEn eType);


/***************************************************************************
* 描述: obj实例.
***************************************************************************/
typedef struct _AmObjSt
{
    const AmObjTmplSt* pstTmpl;

    DataSt stVal;

    //数目参考tmpl。
    struct _AmObjSt* pstSubs;
} AmObjSt;

void Am_ObjInit(const AmObjTmplSt* pstTmpl, AmObjSt* pstObj);
void Am_ObjDelCls(AmObjSt* pstObj);


//写入数据。 数值变更及附加触发。
bool Am_ObjValWriteInt(AmObjSt* pstObj, int iVal);
const char* Am_ObjValWriteStr(AmObjSt* pstObj, const char* pcVal);

//读取数据。
int Am_ObjValReadInt(AmObjSt* pstObj);
const char* Am_ObjValReadStr(AmObjSt* pstObj);




#endif // _obj_h_
