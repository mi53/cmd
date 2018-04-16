#ifndef _value_h_
#define _value_h_

#include "util.h"


/***************************************************************************
* 描述: 值、值数据范围 -- 因子数据范围。 简单数据范围。
***************************************************************************/

//区间。
typedef struct _AmValSegSt
{
    //若为NULL表示数值最小边界。
    DataSt* pstStart;

    //若为NULL表示数值最大边界。end与start相同时指向start。
    DataSt* pstEnd;

    //左区间闭包性。
    bool bIncS;

    //右区间闭包性。
    bool bIncE;

} AmValSegSt;

typedef struct _AmValSegAndAssSt
{
    AmValSegSt* pstSeg;
    bool bInc;//关系.
} AmValSegAndAssSt;


//值数据范围。
typedef struct _AmValRangeSt
{
    AmValSegSt* pstSegs; //均为包含数据范围
    unsigned int uiSegNum;
    unsigned int uiSegCap;
} AmValRangeSt;

//默认为空数据范围。
int Am_ValRangeInit(AmValRangeSt* pstRange);
void Am_ValRangeCls(AmValRangeSt* pstRange);

//添加newseg的拷贝。
int Am_ValRangeAdd(AmValRangeSt* pstRange, const AmValSegSt* pstNewSeg);

//判断单个数据是否存在于数据数据范围内。
bool Am_ValRangeIncChk(AmValRangeSt* pstRange, DataSt* pstData);

/***************************************************************************
* 描述: 逻辑组合数据范围。逻辑处理。
***************************************************************************/



#endif // _value_h_
