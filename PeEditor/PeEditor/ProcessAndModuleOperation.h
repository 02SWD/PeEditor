// ProcessAndModuleOperation.h: interface for the ProcessAndModuleOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSANDMODULEOPERATION_H__8C3D8B00_C6E5_47BB_A5FC_C439CEA6EFB6__INCLUDED_)
#define AFX_PROCESSANDMODULEOPERATION_H__8C3D8B00_C6E5_47BB_A5FC_C439CEA6EFB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/**
 * 枚举当前操作系统中所有的进程
 * @param  hwndDlg 主对话框的句柄
 * @return         枚举成功返回1，CreateToolhelp32Snapshot调用失败返回-1
 */
int EnumAllProcess(HWND hwndDlg);

/**
 * 根据pid枚举其进程中所有的模块
 * @param  hwndDlg 主对话框句柄
 * @param  wParam  WPARAM
 * @param  lParam  LPARAM
 * @return         枚举成功则返回1，没有获取到行则返回-1，第一个模块信息获取失败则返回-2
 */
int EnumModuleListByPid(HWND hwndDlg, WPARAM wParam, LPARAM lParam);

/**
 * 初始化 进程列表控件（为进程列表控件添加表头）
 * @param  hwndDlg 主对话框句柄
 */
VOID InitProcessList(HWND hwndDlg);

/**
 * 初始化 模块列表控件（为模块列表控件添加表头）
 * @param  hwndDlg 主对话框句柄
 */
VOID InitModuleList(HWND hwndDlg);





#endif // !defined(AFX_PROCESSANDMODULEOPERATION_H__8C3D8B00_C6E5_47BB_A5FC_C439CEA6EFB6__INCLUDED_)
