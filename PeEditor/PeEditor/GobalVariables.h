// GobalVariables.h: interface for the GobalVariables class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOBALVARIABLES_H__67A148ED_3C81_49A9_8E39_B60411390AF3__INCLUDED_)
#define AFX_GOBALVARIABLES_H__67A148ED_3C81_49A9_8E39_B60411390AF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//===========================================================================
//��Ӧ�ó���ľ��
extern HINSTANCE ghInstance;

//===========================================================================
//������pe�������ļ���·�� (��ȫ�ֱ�����PE�鿴�����µĳ�����Ӧ��)
extern PTSTR gSzFileNameOfPeView;

//=========================== Զ���߳� - �ڴ�ע�� ��ʹ�õ���ȫ�ֱ���(DLLInjectByWriteMemory) ================================================
//���ڿ���Զ���̵߳���ֹ
extern int gFlag;
//��������ע��ɹ������Ϣ
typedef struct{
	DWORD FlagOfAddr;        //��¼��ȫ�ֱ���gFlag����Ŀ������е��׵�ַ
	DWORD ModuleOfAddr;      //��¼��ע��ģ����Ŀ��������׵�ַ
	DWORD ModuleSizeOfImage; //��¼��ע��ģ���SizeOfImage
	DWORD HRemoteThread;     //��¼��Զ���̵߳ľ��
}InjectByWriteMemory;
extern InjectByWriteMemory gInjectByWriteMemory;

//=================== DLL�ڴ�д�� - ��ʹ��LoadLibrary��������������������޸�IAT����� ��ʹ�õ���ȫ�ֱ���(DLLInjectByWriteMemoryOther) ===================
//dllģ����Ŀ������е��׵�ַ
extern DWORD gDllAddrInTargetProcess;
//dllģ����Ŀ������е�SizeOfImage
extern DWORD gDllSizeOfImage;

//=============== HOOK =======================
//IatHook ֮ǰIAT������ֵ
extern DWORD gOldFuncAddr;

//=============== Driver =====================
//���ڼ�¼�򿪵ķ�����������������ķ�ʽ����������
extern SC_HANDLE g_scManager;



#endif // !defined(AFX_GOBALVARIABLES_H__67A148ED_3C81_49A9_8E39_B60411390AF3__INCLUDED_)
