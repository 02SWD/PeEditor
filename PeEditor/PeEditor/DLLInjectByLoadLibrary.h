// DLLInjectByLoadLibrary.h: interface for the DLLInjectByLoadLibrary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLINJECTBYLOADLIBRARY_H__EC67B958_3BBD_439B_95E6_9DFA08522FA6__INCLUDED_)
#define AFX_DLLINJECTBYLOADLIBRARY_H__EC67B958_3BBD_439B_95E6_9DFA08522FA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * Զ���߳�ע��Dll
 * @param  pid     Ҫע��Ľ���pid
 * @param  dllPath Ҫע���dll·��
 * @return         -1 Kernel32.dllģ�����ʧ��
 *                 -2 pidָ���Ľ��̾����ȡʧ��
 *                 -3 ָ�����̵��ڴ�ռ�����ʧ��
 *                 -4 dll·��д��ʧ��
 *                 -5 ��ָ���Ľ����д����߳�ʧ��
 *                 �Ǹ� dllע��ɹ����ģ����������dllע��ɹ��󣬸�dll���׵�ַ��
 */
int RemoteThreadInjectionModule(IN DWORD pid, IN LPSTR dllPath);


/**
 * Զ��ж��DLL
 * @param  pid     Ҫж�صĽ���pid
 * @param  dllName Ҫж�ص�dll����
 * @return         -1 ָ���Ľ���û�д�ж�ص�ģ��
 *                 -2 ����pid��ȡ���ʧ��
 *                 -3 ��ָ���Ľ����д����߳�ʧ��
 *                  1 ж�سɹ�
 */
int RemoteThreadUninstallModule(DWORD pid, LPSTR dllName);



#endif // !defined(AFX_DLLINJECTBYLOADLIBRARY_H__EC67B958_3BBD_439B_95E6_9DFA08522FA6__INCLUDED_)
