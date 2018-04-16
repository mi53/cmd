#include "def.h"

typedef struct _OsDateSt
{
    /*if set to OS time the scope is 1970 ~ 2038, or
     the scope is 1970 ~ 2100
     */
    unsigned short uwYear;

    /* scope is 1 - 12 */
    unsigned char ucMonth;

    /* scope is 1 - 31 */
    unsigned char ucDate;

    /* scope is 0 - 23 */
    unsigned char ucHour;

    /* scope is 0 - 59 */
    unsigned char ucMinute;

    /* scope is 0 - 59 */
    unsigned char ucSecond;

    /* scope is 0 - 6  */
    unsigned char ucWeek;
} OsDateSt;

/* 配置模拟时间的初始时间，若为NULL，使用系统时间 */
#define OS_DATA_SIMULA_INIT_TIME "1999-1-1 12:0:0"

/***************************************************************************
* 描述: 需要适配的函数.
***************************************************************************/

//获取系统时间。
int OS_DateGet(OsDateSt* pstDate);


/***************************************************************************
* 描述: 其他工具类函数。
***************************************************************************/

//获取系统时间，并以字符串形式返回。
const char* OS_DateStr();

//启动日期模拟器。需要先初始化内存及互斥量模块。
void OS_DateSimulaStart();

//关闭日期模拟器。
void OS_DateSimulaClose();

//获取当前的模拟时间。
int OS_DateSimulaGet(OsDateSt* pstDate);

//以字符串形式返回当前模拟时间。
const char* OS_TimeSimulaStr();

//以字符串形式返回当前模拟日期。
const char* OS_DateSimulaStr();

//增加指定秒数到模拟时间。
const char* OS_DateSimulaAddSecs(int iSecs);

//模拟时间往前调整。
const char* OS_DateSimulaReduceSecs(int iSecs);

//设置模拟时间。
int OS_DateSimulaSet(OsDateSt* pstNewTime);

//设置模拟时间。
int OS_DateSimulaSetByStr(const char* pcData);

/***************************************************************************
* 描述: 内部函数。
***************************************************************************/

//将时间转变为整数。
static int _OS_DateToInt(const OsDateSt* pstDate, OUT long* plInt);

//将整数转变为时间。
static int _OS_DateGetByInt(OUT OsDateSt* pstDate, long ulInt);

//定时器自动累加时间。
static void _OS_DateSimulaAuto(void* pPara);
