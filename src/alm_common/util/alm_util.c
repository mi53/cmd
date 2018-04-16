#include "alm_util.h"
#include "cfg.h"

#include <string.h>


//将字符串时间转换为OsDateSt时间。
int Util_ChangeTime(const char* pcTime, OsDateSt* pstTime)
{
    int iTmp;
    int iTypeIndex;
    int index;

    UTIL_CHK(NULL != pcTime && NULL != pstTime);

    /***********************************************************
    * 填充当前时间，使得未修改部分保留现有值。
    ***********************************************************/
    OS_DateSimulaGet(pstTime);

    /***********************************************************
    * 获取时间
    ***********************************************************/
    iTypeIndex = 0, iTmp = 0;

    for (index = 0;
         index < (int)(strlen(pcTime) + 1);
         ++index)
    {
        //获取单个数据段。
        if (pcTime[index] >= '0' && pcTime[index] <= '9')
        {
            iTmp *= 10;
            iTmp += (pcTime[index] - '0');
        }
        //已发现数据段。
        else if (pcTime[index] == '/'
                 || pcTime[index] == '-'
                 || pcTime[index] == ':'
                 || pcTime[index] == ' '
                 || pcTime[index] == '\0')
        {
            switch (iTypeIndex)
            {
                case 0:
                    pstTime->uwYear = (unsigned short)iTmp;
                    break;

                case 1:
                    pstTime->ucMonth = (unsigned char)iTmp;
                    break;

                case 2:
                    pstTime->ucDate = (unsigned char)iTmp;
                    break;

                case 3:
                    pstTime->ucHour = (unsigned char)iTmp;
                    break;

                case 4:
                    pstTime->ucMinute = (unsigned char)iTmp;
                    break;

                case 5:
                    pstTime->ucSecond = (unsigned char)iTmp;
                    break;

                default:
                    return RET_ERR;
            }

            iTmp = 0;
            iTypeIndex++;
        }
        //存在无效字符。
        else
        {
            return RET_ERR;
        }

        if (pcTime[index] == '\0')
        { break; }
    }

    return RET_OK;
}
