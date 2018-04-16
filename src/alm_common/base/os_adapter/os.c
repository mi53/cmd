#include "os.h"


/***************************************************************************
* 描述: OS启动。lock -> timer -> simula date
***************************************************************************/
int OS_Start()
{
    OS_MemAlmLogCls();
    OS_LockStart();
    OS_TimerStart();
    OS_DateSimulaStart();

    return RET_OK;
}

/***************************************************************************
* 描述: OS关闭。simula date -> timer -> lock
***************************************************************************/
int OS_Close()
{
    //OS_LockLog();

    OS_DateSimulaClose();
    OS_TimerClose();
    OS_LockClose();

    OS_MemAlmLogCls();

    return RET_OK;
}
