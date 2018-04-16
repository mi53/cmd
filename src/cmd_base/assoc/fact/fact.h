#ifndef _fact_h_
#define _fact_h_

#include "obj.h"


/***************************************************************************
* 描述: 。
***************************************************************************/

typedef struct _AmFactSt
{
    char* pcName;
    AmObjSt stObj;
} AmFactSt;

//只支持简单obj对象。
void Am_FactInit(AmFactSt* pstFact, const char* pcName, TypeEn eType);
void Am_FactCls(AmFactSt* pstFact);


/***************************************************************************
* 描述: fact和obj组域。
***************************************************************************/




#endif // _fact_h_
