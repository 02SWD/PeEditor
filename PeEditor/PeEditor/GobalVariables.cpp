// GobalVariables.cpp: implementation of the GobalVariables class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//===========================================================================
//该应用程序的句柄
HINSTANCE ghInstance = NULL;

//===========================================================================
//待进行pe解析的文件的路径 (该全局变量被PE查看功能下的程序所应用)
PTSTR gSzFileNameOfPeView = NULL;

//=========================== 远程线程 - 内存注入 所使用到的全局变量(DLLInjectByWriteMemory) ================================================
//用于控制远程线程的终止
int gFlag = TRUE;
//用于描述注入成功后的信息
InjectByWriteMemory gInjectByWriteMemory = {0};

//=================== DLL内存写入 - 不使用LoadLibrary函数，且在自身进程中修复IAT表表项 所使用到的全局变量(DLLInjectByWriteMemoryOther) ===================
//dll模块在目标进程中的首地址
DWORD gDllAddrInTargetProcess = 0;
//dll模块在目标进程中的SizeOfImage
DWORD gDllSizeOfImage = 0;

//================ HOOK ======================
//IatHook 之前IAT表表项的值
DWORD gOldFuncAddr = 0;

//=============== Driver =====================
//用于记录打开的服务管理器（以正常的方式加载驱动）
SC_HANDLE g_scManager;
