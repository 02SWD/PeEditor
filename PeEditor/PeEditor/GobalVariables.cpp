// GobalVariables.cpp: implementation of the GobalVariables class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//===========================================================================
//��Ӧ�ó���ľ��
HINSTANCE ghInstance = NULL;

//===========================================================================
//������pe�������ļ���·�� (��ȫ�ֱ�����PE�鿴�����µĳ�����Ӧ��)
PTSTR gSzFileNameOfPeView = NULL;

//=========================== Զ���߳� - �ڴ�ע�� ��ʹ�õ���ȫ�ֱ���(DLLInjectByWriteMemory) ================================================
//���ڿ���Զ���̵߳���ֹ
int gFlag = TRUE;
//��������ע��ɹ������Ϣ
InjectByWriteMemory gInjectByWriteMemory = {0};

//=================== DLL�ڴ�д�� - ��ʹ��LoadLibrary��������������������޸�IAT����� ��ʹ�õ���ȫ�ֱ���(DLLInjectByWriteMemoryOther) ===================
//dllģ����Ŀ������е��׵�ַ
DWORD gDllAddrInTargetProcess = 0;
//dllģ����Ŀ������е�SizeOfImage
DWORD gDllSizeOfImage = 0;

//================ HOOK ======================
//IatHook ֮ǰIAT������ֵ
DWORD gOldFuncAddr = 0;

//=============== Driver =====================
//���ڼ�¼�򿪵ķ�����������������ķ�ʽ����������
SC_HANDLE g_scManager;
