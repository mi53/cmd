#ifndef _def_h_
#define _def_h_


/***************************************************************************
* 描述: 通用返回及bool类型定义。
***************************************************************************/
#define RET_ERR     -1
#define RET_OK      0

#define bool        unsigned char
#define true        1
#define false       0

#ifndef NULL
#define NULL        0
#endif

/***************************************************************************
* 描述: 几个基础的数据类型。
***************************************************************************/
typedef enum _TypeEn
{
    TYPE_INT,
    TYPE_STR,
    TYPE_UNKNOW,
} TypeEn;

typedef struct _DataSt
{
    TypeEn eType;
    void* pData;
} DataSt;

/***************************************************************************
* 描述: 通用输出格式定义。
***************************************************************************/
#define FMT_SEG0 \
    "---------------------------------------------------------"
#define FMT_SEG1 \
    "- - - - - - - - - - - - - - - - - - - - - - - - - - - - -"

#define FMT_SEG2  \
    "========================================================="

#define FMT_SEG3 \
    "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"

#define FMT_SEG4 \
    "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

#define FMT_SEG0_E FMT_SEG0"\n"
#define FMT_SEG1_E FMT_SEG1"\n"

#define FMT_SEG2_E FMT_SEG2"\n"

#define FMT_SEG3_E FMT_SEG3"\n"

#define FMT_SEG4_E FMT_SEG4"\n"

typedef const char* CmdTimeFn();

#define FMT_SEG_TIME(fnTime) \
    "&&&&&&&&&&&&&&&&&& %19s &&&&&&&&&&&&&&&&&&\n", fnTime()

/***************************************************************************
* 描述: 命令提示信息相关格式宏.
***************************************************************************/
#define FMT_SEG_CMD_BG \
    "/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\\n"
#define FMT_SEG_CMD_ED  \
    "\\_______________________________________________________/\n"

#ifndef INOUT
#define INOUT
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#endif // _def_h_
