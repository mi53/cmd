#include "lic_inc.h"
#include "util.h"

#include <string.h>

#define ALM_TIME_ONE_MIN_SECS 60
#define ALM_TIME_ONE_HOUR_SECS (60 * 60)
#define ALM_TIME_ONE_DAY_SECS (24 * 60 * 60)
#define ALM_TIME_ONE_DAY_MINS (24 * 60)
#define ALM_TIME_ONE_HOUR_MINS (60)
#define ALM_TIME_ONE_DAY_HOURS (24)


/* 将BBOM类型转换为字符串 */
CONST ALM_CHAR* Tl_AlmBbomTypeToStr(ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType)
{
    switch (eType)
    {
        case ALM_LIC_BBOM_RESOURCE:
            return "RES";

        case ALM_LIC_BBOM_FUNCTION:
            return "FUN";

        default:
            return "UNKONW";
    }
}


/* 将License状态转换为字符串 */
CONST ALM_CHAR* Tl_AlmLicState2Str(CONST ALM_STATE_LIC_STRU* pstLicState)
{
    CONST ALM_CHAR* pcRet = "unknow";

    if (pstLicState->bUsing)
    {
        pcRet = Tl_AlmState2Str(pstLicState->stBase.eState);
    }
    else
    {
        pcRet = "NotUsing";
    }

    return pcRet;
}


/* 将License文件类型转换为字符串 */
CONST ALM_CHAR* Tl_AlmLicType2Str(ALM_ENUM(ALM_LIC_TYPE_ENUM) eType)
{
    switch (eType)
    {
        case ALM_LIC_TYPE_DFLT:
            return "DFLT";

        case ALM_LIC_TYPE_DEMO:
            return "DEMO";

        case ALM_LIC_TYPE_COMM:
            return "COMM";

        case ALM_LIC_TYPE_STICK:
            return "STICK";

        case ALM_LIC_TYPE_EMERGENCY:
            return "EMGS";

        default:
            return "ERROR";
    }
}


/* 将产品状态转换为字符串 */
CONST ALM_CHAR* Tl_AlmPrdState2Str(ALM_ENUM(ALM_STATE_PRD_ENUM) eState)
{
    CONST ALM_CHAR* pcState;

    switch (eState)
    {
        case ALM_STATE_PRD_INVALID:
            pcState = TL_ALM_STATE_INVALID;
            break;

        case ALM_STATE_PRD_DEFAULT:
            pcState = TL_ALM_STATE_DEFAULT;
            break;

        case ALM_STATE_PRD_GRACE_PERIOD:
            pcState = TL_ALM_STATE_GRACE;
            break;

        case ALM_STATE_PRD_DEMO:
            pcState = TL_ALM_STATE_DEMO;
            break;

        case ALM_STATE_PRD_NORMAL:
            pcState = TL_ALM_STATE_NORMAL;
            break;

        case ALM_STATE_PRD_EMERGENCY:
            pcState = TL_ALM_STATE_EMERGENCY;
            break;

        case ALM_STATE_PRD_STICK:
            pcState = TL_ALM_STATE_STICK;
            break;

        default:
            pcState = "UNKNOW";
            break;
    }

    return pcState;
}

/* 将License状态转换为字符串 */
CONST ALM_CHAR* Tl_AlmState2Str(ALM_ENUM(ALM_STATE_LIC_ENUM) eState)
{
    CONST ALM_CHAR* pcState;

    switch (eState)
    {
        case ALM_STATE_LIC_INVALID:
            pcState = TL_ALM_STATE_INVALID;
            break;

        case ALM_STATE_LIC_GRACE_PERIOD:
            pcState = TL_ALM_STATE_GRACE;
            break;

        case ALM_STATE_LIC_VALID:
            pcState = TL_ALM_STATE_VALID;
            break;

        default:
            pcState = "UNKNOW";
            break;
    }

    return pcState;
}

CONST ALM_CHAR* Tl_AlmStateChkItem2Str(ALM_ENUM(ALM_STATE_CHECK_ITEM_ENUM) eItem)
{
    CONST ALM_CHAR* pcItem;

    switch (eItem)
    {
        case ALM_STATE_VERIFY_ITEM_ESN:
            pcItem = TL_ALM_STATE_VERIFY_ITEM_ESN;
            break;

        case ALM_STATE_VERIFY_ITEM_PRD_NAME:
            pcItem = TL_ALM_STATE_VERIFY_ITEM_PRD_NAME;
            break;

        case ALM_STATE_VERIFY_ITEM_PRD_VER:
            pcItem = TL_ALM_STATE_VERIFY_ITEM_PRD_VER;
            break;

        case ALM_STATE_VERIFY_ITEM_DEADLINE_OVER:
            pcItem = TL_ALM_STATE_VERIFY_ITEM_DEADLINE_OVER;
            break;

        case ALM_STATE_VERIFY_ITEM_RVK:
            pcItem = TL_ALM_STATE_VERIFY_ITEM_RVK;
            break;

        default:
            pcItem = "UNKNOW";
            break;
    }

    return pcItem;
}

/* 将原因转换为字符串 */
CONST ALM_CHAR* Tl_AlmRsnToStr(ALM_UINT64 ulRsn, OUT ALM_CHAR* pcRsn,
                               ALM_UINT32 uiRsnSize)
{
    ALM_ENUM(ALM_STATE_RSN_ENUM) aeRsns[ALM_STATE_RSN_NUM];
    ALM_UINT32 uiRsnNum;
    ALM_INT32 iRet;
    ALM_UINT32 uiIdx;

    if (NULL == pcRsn || 0 == uiRsnSize)
    {
        return NULL;
    }

    pcRsn[0] = '\0';

    iRet = ALM_StateMgrSplitRsn(ulRsn, aeRsns, &uiRsnNum);
    LIC_CALL_FN_LAB(ALM_StateMgrSplitRsn, iRet, ERR);

    for (uiIdx = 0; uiIdx < uiRsnNum; ++uiIdx)
    {
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, " ");

        switch (aeRsns[uiIdx])
        {
            case ALM_STATE_RSN_LIC_PRD_NAME_MIS:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "name_mis");
                break;

            case ALM_STATE_RSN_LIC_PRD_VER_MIS:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "ver_mis");
                break;

            case ALM_STATE_RSN_LIC_ESN_MIS:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "esn_mis");
                break;

            case ALM_STATE_RSN_LIC_VALID_OVER:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "valid_over");
                break;

            case ALM_STATE_RSN_LIC_GRACE_OVER:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "grace_over");
                break;

            case ALM_STATE_RSN_LIC_RVK:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "rvked");
                break;

            case ALM_STATE_RSN_LIC_PRD_VER_REMIS:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "ver_remis");
                break;

            case ALM_STATE_RSN_LIC_ESN_REMIS:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "esn_remis");
                break;

            case ALM_STATE_RSN_LIC_ESN_ANY_USE_DAY_LESSER:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "esn_any_less");
                break;

            case ALM_STATE_RSN_LIC_ESN_ANY_USE_DAY_OVER:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "esn_any_over");
                break;

            default:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "unknow");
                break;
        }
    }
ERR:
    return pcRsn;
}


/* 一个工具性质的调试函数，用于将分钟转换为天、时、分字符串 */
CONST ALM_CHAR* Tl_AlmMins2DayStr(ALM_UINT32 uiMins)
{
#define MIN_TO_DAY_MAX_LEN 15
#define MIN_TO_DAY_MUL_NUM 10

    static ALM_CHAR aacBuf[MIN_TO_DAY_MUL_NUM][MIN_TO_DAY_MAX_LEN + 1];
    static int iCnt = 0;

    ++iCnt;
    iCnt %= MIN_TO_DAY_MUL_NUM;

    (ALM_VOID)ALM_sprintf_s(aacBuf[iCnt], sizeof(aacBuf[iCnt]),
                            "%dd,%02dh,%02dm",
                            uiMins / ALM_TIME_ONE_DAY_MINS,
                            (uiMins / ALM_TIME_ONE_HOUR_MINS ) % ALM_TIME_ONE_DAY_HOURS,
                            uiMins % ALM_TIME_ONE_HOUR_MINS);
    aacBuf[iCnt][MIN_TO_DAY_MAX_LEN] = '\0';

    return aacBuf[iCnt];
}

/* 将License状态字符串转换为状态 */
ALM_INT32 Tl_AlmStr2State(CONST ALM_CHAR* pcStr,
                          ALM_ENUM(ALM_STATE_LIC_ENUM)* peState)
{
    TL_CHK_RET(pcStr != NULL && peState != NULL, ALM_ERR_NULL_PTR, (NULL));

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_INVALID))
    {
        *peState = ALM_STATE_LIC_INVALID;
        return ALM_OK;
    }

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_GRACE))
    {
        *peState = ALM_STATE_LIC_GRACE_PERIOD;
        return ALM_OK;
    }

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_VALID))
    {
        *peState = ALM_STATE_LIC_VALID;
        return ALM_OK;
    }

    LOG_ERR("Unknow state: %s", pcStr);
    return ALM_ERR_UNKNOW;
}

/* 将产品状态字符串转换为状态 */
ALM_INT32 Tl_AlmStr2PrdState(CONST ALM_CHAR* pcStr,
                             ALM_ENUM(ALM_STATE_PRD_ENUM)* peState)
{
    TL_CHK_RET(pcStr != NULL && peState != NULL, ALM_ERR_NULL_PTR, (NULL));

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_INVALID))
    {
        *peState = ALM_STATE_PRD_INVALID;
        return ALM_OK;
    }

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_DEFAULT))
    {
        *peState = ALM_STATE_PRD_DEFAULT;
        return ALM_OK;
    }

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_GRACE))
    {
        *peState = ALM_STATE_PRD_GRACE_PERIOD;
        return ALM_OK;
    }

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_DEMO))
    {
        *peState = ALM_STATE_PRD_DEMO;
        return ALM_OK;
    }

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_NORMAL))
    {
        *peState = ALM_STATE_PRD_NORMAL;
        return ALM_OK;
    }

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_EMERGENCY) || 0 == Util_StrCmpIgrUp(pcStr, "EM"))
    {
        *peState = ALM_STATE_PRD_EMERGENCY;
        return ALM_OK;
    }

    if (0 == Util_StrCmpIgrUp(pcStr, TL_ALM_STATE_STICK))
    {
        *peState = ALM_STATE_PRD_STICK;
        return ALM_OK;
    }

    LOG_ERR("Unknow state: %s", pcStr);
    return ALM_ERR_UNKNOW;
}


/*将告警类型转为字符串*/
CONST ALM_CHAR* Tl_AlmAlarmType2Str(ALM_UINT32 uiAlarmType)
{
    switch (uiAlarmType)
    {
        case ALM_ALARM_TYPE_FAULT:
            return "FAULT";

        case ALM_ALARM_TYPE_RECOVERY:
            return "RECOVERY";

        case ALM_ALARM_TYPE_INFO:
            return "INFO";

        default:
            return "ERROR";
    }
}

/*将告警名称转为字符串*/
CONST ALM_CHAR* Tl_AlmAlarmName2Str(ALM_UINT32 uiAlarmName)
{
    switch (uiAlarmName)
    {
        case ALM_ALARM_NAME_GRACE_PERIOD:
            return "GRACE";

        case ALM_ALARM_NAME_DEFAULT:
            return "DEFAULT";

        case ALM_ALARM_NAME_FEATURE_GRACE_PERIOD:
            return "FEATURE_GRACE";

        case ALM_ALARM_NAME_FEATURE_DEFAULT:
            return "FEATURE_DEFAULT";

        case ALM_ALARM_NAME_EMERGENCY_START:
            return "EM_START";

        case ALM_ALARM_NAME_EMERGENCY_STOP:
            return "EM_STOP";

        case ALM_ALARM_NAME_STICK_START:
            return "STICK_START";

        case ALM_ALARM_NAME_STICK_STOP:
            return "STICK_STOP";

        default:
            return "ERROR";
    }
}

/* 将告警原因转换为字符串 */
CONST ALM_CHAR* Tl_AlmAlarmRsnToStr(ALM_UINT64 ulRsn, OUT ALM_CHAR* pcRsn,
                                    ALM_UINT32 uiRsnSize)
{
    ALM_ENUM(ALM_ALARM_REASON_ENUM) aeRsns[ALM_ALARM_REASON_NUM];
    ALM_UINT32 uiRsnNum;
    ALM_INT32 iRet;
    ALM_UINT32 uiIdx;

    if (NULL == pcRsn || 0 == uiRsnSize)
    {
        return NULL;
    }

    pcRsn[0] = '\0';

    iRet = ALM_AlarmSplitRsn(ulRsn, aeRsns, &uiRsnNum);
    LIC_CALL_FN_LAB(ALM_StateMgrSplitRsn, iRet, ERR);

    for (uiIdx = 0; uiIdx < uiRsnNum; ++uiIdx)
    {
        (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, " ");

        switch (aeRsns[uiIdx])
        {

            case ALM_ALARM_RSN_LIC_PRD_VER_MIS:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "ver_mis");
                break;

            case ALM_ALARM_RSN_LIC_ESN_MIS:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "esn_mis");
                break;

            case ALM_ALARM_RSN_LIC_DEADLINE_OVER:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "valid_over");
                break;

            case ALM_ALARM_RSN_LIC_GRACE_OVER:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "grace_over");
                break;

            case ALM_ALARM_RSN_LIC_RVK:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "rvk");
                break;

            case ALM_ALARM_RSN_EMERGENCY_START:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "start_em");
                break;

            case ALM_ALARM_RSN_EMERGENCY_STOP:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "stop_em");
                break;

            case ALM_ALARM_RSN_STICK_START:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "start_stick");
                break;

            case ALM_ALARM_RSN_STICK_STOP:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "stop_stick");
                break;

            case ALM_ALARM_RSN_NO_DEFAULT_LK:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "no_lk");
                break;

            case ALM_ALARM_RSN_INIT_FAILED_LK:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "init_lk_failed");
                break;

            default:
                (ALM_VOID)ALM_strcat_s(pcRsn, uiRsnSize, "unknow");
                break;
        }
    }
ERR:
    return pcRsn;
}
