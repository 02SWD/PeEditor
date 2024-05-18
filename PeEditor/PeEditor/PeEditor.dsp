# Microsoft Developer Studio Project File - Name="PeEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PeEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PeEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PeEditor.mak" CFG="PeEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PeEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PeEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PeEditor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "PeEditor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PeEditor - Win32 Release"
# Name "PeEditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ButtonPeViewCodes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonBaseRelocView.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonDataDirectoryView.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonExportTableView.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonIatTableView.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonImportTableView.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonOptionPeView.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonPeViewMain.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonSectionView.cpp
# End Source File
# End Group
# Begin Group "UtilsCode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aesUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\ResetWindowTileUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Rva2A2Foa.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# End Group
# Begin Group "GobalVariable"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GobalVariables.cpp
# End Source File
# End Group
# Begin Group "ButtonAboutCodes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonAboutCode.cpp
# End Source File
# End Group
# Begin Group "ButtonEncryptShellCodes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonEncryptShellMain.cpp
# End Source File
# Begin Source File

SOURCE=.\EncryptingShell.cpp
# End Source File
# Begin Source File

SOURCE=.\peOperateAboutShell.cpp
# End Source File
# End Group
# Begin Group "ButtonDLLInjection"

# PROP Default_Filter ""
# Begin Group "DLLInjectByLoadLibrary"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLLInjectByLoadLibrary.cpp
# End Source File
# End Group
# Begin Group "DLLInjectByWriteMemory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLLInjectByWriteMemory.cpp
# End Source File
# End Group
# Begin Group "DLLInjectByWriteMemoryOther"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLLInjectByWriteMemoryOther.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ButtonDLLinjectMain.cpp
# End Source File
# End Group
# Begin Group "ButtonHook"

# PROP Default_Filter ""
# Begin Group "ButtonIatHook"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonIatHook.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ButtonHookMain.cpp
# End Source File
# End Group
# Begin Group "ButtonDriver"

# PROP Default_Filter ""
# Begin Group "ButtonDriverLoadNormal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonDriverNormal.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ButtonDriverMain.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\GraphicalInterface.rc
# End Source File
# Begin Source File

SOURCE=.\PeEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessAndModuleOperation.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "ButtonPeViewCodeHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonBaseRelocView.h
# End Source File
# Begin Source File

SOURCE=.\ButtonDataDirectoryView.h
# End Source File
# Begin Source File

SOURCE=.\ButtonExportTableView.h
# End Source File
# Begin Source File

SOURCE=.\ButtonIatTableView.h
# End Source File
# Begin Source File

SOURCE=.\ButtonImportTableView.h
# End Source File
# Begin Source File

SOURCE=.\ButtonOptionPeView.h
# End Source File
# Begin Source File

SOURCE=.\ButtonPeViewMain.h
# End Source File
# Begin Source File

SOURCE=.\ButtonSectionView.h
# End Source File
# End Group
# Begin Group "UtilsCodeHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aesUtils.h
# End Source File
# Begin Source File

SOURCE=.\ResetWindowTileUtil.h
# End Source File
# Begin Source File

SOURCE=.\Rva2A2Foa.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# End Group
# Begin Group "GobalVariableHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GobalVariables.h
# End Source File
# End Group
# Begin Group "ButtonAboutCodeHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonAboutCode.h
# End Source File
# End Group
# Begin Group "ButtonEncryptShellHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonEncryptShellMain.h
# End Source File
# Begin Source File

SOURCE=.\EncryptingShell.h
# End Source File
# Begin Source File

SOURCE=.\peOperateAboutShell.h
# End Source File
# End Group
# Begin Group "ButtonDLLInjectionHeader"

# PROP Default_Filter ""
# Begin Group "DLLInjectByLoadLibraryHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLLInjectByLoadLibrary.h
# End Source File
# End Group
# Begin Group "DLLInjectByWriteMemoryHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLLInjectByWriteMemory.h
# End Source File
# End Group
# Begin Group "DLLInjectByWriteMemOtherHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLLInjectByWriteMemoryOther.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ButtonDLLinjectMain.h
# End Source File
# End Group
# Begin Group "ButtonHookHeader"

# PROP Default_Filter ""
# Begin Group "ButtonIatHookHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonIatHook.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ButtonHookMain.h
# End Source File
# End Group
# Begin Group "ButtonDriverHeader"

# PROP Default_Filter ""
# Begin Group "ButtonDriverLoadNormalHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonDriverNormal.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ButtonDriverMain.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ProcessAndModuleOperation.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\QianYiShen.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
