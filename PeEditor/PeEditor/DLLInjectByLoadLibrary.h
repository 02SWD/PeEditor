// DLLInjectByLoadLibrary.h: interface for the DLLInjectByLoadLibrary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLINJECTBYLOADLIBRARY_H__EC67B958_3BBD_439B_95E6_9DFA08522FA6__INCLUDED_)
#define AFX_DLLINJECTBYLOADLIBRARY_H__EC67B958_3BBD_439B_95E6_9DFA08522FA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * 远程线程注入Dll
 * @param  pid     要注入的进程pid
 * @param  dllPath 要注入的dll路径
 * @return         -1 Kernel32.dll模块加载失败
 *                 -2 pid指定的进程句柄获取失败
 *                 -3 指定进程的内存空间申请失败
 *                 -4 dll路径写入失败
 *                 -5 在指定的进程中创建线程失败
 *                 非负 dll注入成功后的模块句柄（即：dll注入成功后，该dll的首地址）
 */
int RemoteThreadInjectionModule(IN DWORD pid, IN LPSTR dllPath);


/**
 * 远程卸载DLL
 * @param  pid     要卸载的进程pid
 * @param  dllName 要卸载的dll名称
 * @return         -1 指定的进程没有待卸载的模块
 *                 -2 根据pid获取句柄失败
 *                 -3 在指定的进程中创建线程失败
 *                  1 卸载成功
 */
int RemoteThreadUninstallModule(DWORD pid, LPSTR dllName);



#endif // !defined(AFX_DLLINJECTBYLOADLIBRARY_H__EC67B958_3BBD_439B_95E6_9DFA08522FA6__INCLUDED_)
