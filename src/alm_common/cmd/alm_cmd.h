/**************************************************************************
	filename : 	alm_commn\cmd\cmd\alm_cmd.h
	file path:	alm_commn\cmd\cmd
	file ext :	h

	purpose  :	默认命令。
***************************************************************************/
#ifndef _alm_cmd_h_
#define _alm_cmd_h_


#include "cmd.h"

/***************************************************************************
* 描述: 初始化ALM公共命令。
***************************************************************************/
void Cmd_AlmCmdInit();
void Cmd_AlmCmdCls();

/***************************************************************************
* 描述: 模拟时间相关命令。
***************************************************************************/
int Cmd_Time(ParasSt* pstPara, void* pData);
int Cmd_TimeSet(ParasSt* pstPara, void* pData);
int Cmd_TimeAdd(ParasSt* pstPara, void* pData);

/***************************************************************************
* 描述: lic lock相关命令。
***************************************************************************/
int Cmd_LicLockShow(ParasSt* pstPara, void* pData);
int Cmd_LicLockCls(ParasSt* pstPara, void* pData);
int Cmd_LicLockTest(ParasSt* pstPara, void* pData);


/***************************************************************************
* 描述: 内存检测。
***************************************************************************/
int Cmd_LicMemLog(ParasSt* pstPara, void* pData);
int Cmd_LicMemLogCls(ParasSt* pstPara, void* pData);

#endif // _alm_cmd_h_
