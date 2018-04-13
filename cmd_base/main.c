#include "cmd.h"
#include "util.h"

/***************************************************************************
* 入口
***************************************************************************/
int main()
{
    const char* apcCmds[] = {"Help -c help"};

    CmdM_Start();

    CmdM_Run(apcCmds, 0/*sizeof(apcCmds)/sizeof(char*)*/, true);

    CmdM_Close();

    return 0;
}
