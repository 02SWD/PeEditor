// GobalVariables.h: interface for the GobalVariables class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOBALVARIABLES_H__67A148ED_3C81_49A9_8E39_B60411390AF3__INCLUDED_)
#define AFX_GOBALVARIABLES_H__67A148ED_3C81_49A9_8E39_B60411390AF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//===========================================================================
//该应用程序的句柄
extern HINSTANCE ghInstance;

//===========================================================================
//待进行pe解析的文件的路径 (该全局变量被PE查看功能下的程序所应用)
extern PTSTR gSzFileNameOfPeView;

//=========================== 远程线程 - 内存注入 所使用到的全局变量(DLLInjectByWriteMemory) ================================================
//用于控制远程线程的终止
extern int gFlag;
//用于描述注入成功后的信息
typedef struct{
	DWORD FlagOfAddr;        //记录了全局变量gFlag，在目标进程中的首地址
	DWORD ModuleOfAddr;      //记录了注入模块在目标进程中首地址
	DWORD ModuleSizeOfImage; //记录了注入模块的SizeOfImage
	DWORD HRemoteThread;     //记录了远程线程的句柄
}InjectByWriteMemory;
extern InjectByWriteMemory gInjectByWriteMemory;

//=================== DLL内存写入 - 不使用LoadLibrary函数，且在自身进程中修复IAT表表项 所使用到的全局变量(DLLInjectByWriteMemoryOther) ===================
//dll模块在目标进程中的首地址
extern DWORD gDllAddrInTargetProcess;
//dll模块在目标进程中的SizeOfImage
extern DWORD gDllSizeOfImage;

//=============== HOOK =======================
//IatHook 之前IAT表表项的值
extern DWORD gOldFuncAddr;

//=============== Driver =====================
//用于记录打开的服务管理器（以正常的方式加载驱动）
extern SC_HANDLE g_scManager;



#endif // !defined(AFX_GOBALVARIABLES_H__67A148ED_3C81_49A9_8E39_B60411390AF3__INCLUDED_)
