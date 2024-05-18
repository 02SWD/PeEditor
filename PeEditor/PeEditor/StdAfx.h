// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include <iostream>
#include <TlHelp32.h>
#include <Psapi.h>
#include <commdlg.h>

#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")

#include "GobalVariables.h"
#include "resource.h"
#include "Utils.h"
#include "ResetWindowTileUtil.h"
#include "Rva2A2Foa.h"

#include "ProcessAndModuleOperation.h"
#include "ButtonAboutCode.h"
#include "ButtonPeViewMain.h"
#include "ButtonOptionPeView.h"
#include "ButtonSectionView.h"
#include "ButtonDataDirectoryView.h"
#include "ButtonExportTableView.h"
#include "ButtonImportTableView.h"
#include "ButtonBaseRelocView.h"
#include "ButtonIatTableView.h"

#include "EncryptingShell.h"
#include "aesUtils.h"
#include "peOperateAboutShell.h"
#include "ButtonEncryptShellMain.h"

#include "ButtonDLLinjectMain.h"
#include "DLLInjectByLoadLibrary.h"
#include "DLLInjectByWriteMemory.h"
#include "DLLInjectByWriteMemoryOther.h"

#include "ButtonHookMain.h"
#include "ButtonIatHook.h"

#include "ButtonDriverMain.h"
#include "ButtonDriverNormal.h"



void __cdecl OutputDebugStringF(const char *format, ...); 							
#ifdef _DEBUG //≈–∂œ «∑Ò «debug∞Ê±æ					
#define DbgPrintf   OutputDebugStringF  					
#else  					
#define DbgPrintf  					
#endif 					


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
