#ifndef _alm_util_h_
#define _alm_util_h_

#include "base.h"
#include "util.h"
#include "os.h"

/***************************************************************************
* 描述: 函数。
***************************************************************************/

//将字符串时间转换为LIC_SYS_T_STRU时间。
int Util_ChangeTime(const char* pcTime, OsDateSt* pstTime);

/***************************************************************************
* 描述: 函数调用及校验宏。
***************************************************************************/

//将ALM错误码转换为对应的描述，不在本工程内实现。
const char* ALM_ErrStr(int iRet);

/* 表达式校验，并做相应处理 */
#define S_TL_CHK_DO(expr, doing, ...) \
    do {\
        if (!(expr)) { LOG_ERR(__VA_ARGS__); doing;} \
    } while (g_TlDoWhile)

/* 表达式校验，不为真返回void，变参为附加描述 */
#define TL_CHK_VOID(expr, ...) S_TL_CHK_DO(expr, return, __VA_ARGS__)

/* 表达式校验，不为真返回iRet，变参为附加描述 */
#define TL_CHK_RET(expr, ret, ...) S_TL_CHK_DO(expr, return (ret), __VA_ARGS__)

/* 表达式校验，不为真跳转到指定标签，变参为附加描述 */
#define TL_CHK_LAB(expr, lab, ...) S_TL_CHK_DO(expr, goto lab, __VA_ARGS__)


/* 校验函数返回值，并做相应的处理 */
#define S_TL_CALL_DO(doing, bAlmFunc) \
    do {\
        if (RET_OK != (iRet)) \
        {\
            if (bAlmFunc)\
            {\
                LOG_ERR("0x%x - %s.", (iRet), ALM_ErrStr(iRet));\
            }\
            doing; \
        } \
    } while (g_TlDoWhile)

/* 校验函数返回值，出错返回void */
#define TL_CALL_VOID(bAlmFunc) S_TL_CALL_DO(return, bAlmFunc)

/* 校验函数返回值，出错返回iRet */
#define TL_CALL_RET(bAlmFunc, iFuncRet) S_TL_CALL_DO(return (iFuncRet), bAlmFunc)

/* 校验函数返回值，出错跳转到指定标签 */
#define TL_CALL_LAB(bAlmFunc, lab) S_TL_CALL_DO(goto lab, bAlmFunc)

/***************************************************************************
* 描述: 其他。
***************************************************************************/
//将宏数字转换为对应的字符串。
#define _TL_MROC_TO_STR(x) #x
#define TL_MROC_TO_STR(x) _TL_MROC_TO_STR(x)

#endif // _alm_util_h_
