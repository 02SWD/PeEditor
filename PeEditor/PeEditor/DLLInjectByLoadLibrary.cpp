// DLLInjectByLoadLibrary.cpp: implementation of the DLLInjectByLoadLibrary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DLLInjectByLoadLibrary.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


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
int RemoteThreadInjectionModule(IN DWORD pid, IN LPSTR dllPath){
	//����Kernel32.dllģ��
	HMODULE hModule =  GetModuleHandle("Kernel32.dll");
	if(hModule == NULL){
		return -1;
	}
	//�� Kernel32.dll ģ���л�ȡ LoadLibraryA���� �ĺ�����ַ
	LPVOID funcAddr = GetProcAddress(hModule, "LoadLibraryA");

	//����pid��ȡ��Ӧ���̵ľ��
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hRemoteProcess == NULL){
		return -2;
	}
	//��ִ�еĽ��̿ռ�������ռ�
	LPVOID pVirtualAddr = VirtualAllocEx(hRemoteProcess, NULL, strlen(dllPath)+1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(pVirtualAddr == NULL){
		CloseHandle(hRemoteProcess);
		return -3;
	}
	//��DLL��·��д��ָ�����̵��ڴ�ռ�����
	if(0 == WriteProcessMemory(hRemoteProcess, pVirtualAddr, dllPath, strlen(dllPath)+1, NULL)){
		VirtualFreeEx(hRemoteProcess, pVirtualAddr, 0, MEM_RELEASE);
		CloseHandle(hRemoteProcess);
		return -4;
	}
	//��ָ���Ľ����д����߳�
	HANDLE hRemoteThread = ::CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)funcAddr, pVirtualAddr, 0, NULL);
	if(hRemoteThread == NULL){
		VirtualFreeEx(hRemoteProcess, pVirtualAddr, 0, MEM_RELEASE);
		CloseHandle(hRemoteProcess);
		return -5;
	}
	//�ȴ���ֱ���߳�ִ�����
	WaitForSingleObject(hRemoteThread, INFINITE);
	//��ȡָ�������ڼ������dllģ���ľ��
	DWORD handleOfDll = 0;
	GetExitCodeThread(hRemoteThread,&handleOfDll);
	
	//�ͷ���Դ
	VirtualFreeEx(hRemoteProcess, pVirtualAddr, 0, MEM_RELEASE);
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);
	//���ؾ��
	return handleOfDll;
}

/**
 * Զ��ж��DLL
 * @param  pid     Ҫж�صĽ���pid
 * @param  dllName Ҫж�ص�dll����
 * @return         -1 ָ���Ľ���û�д�ж�ص�ģ��
 *                 -2 ����pid��ȡ���ʧ��
 *                 -3 ��ָ���Ľ����д����߳�ʧ��
 *                  1 ж�سɹ�
 */
int RemoteThreadUninstallModule(DWORD pid, LPSTR dllName){
	//Ϊָ��pid�Ľ����������
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 mem32 = {0};
	mem32.dwSize = sizeof(MODULEENTRY32);
	//��ȡ�ý����е�һ��ģ�����Ϣ
	BOOL isNext = Module32First(hSnap, &mem32);
	BOOL flag = FALSE;
	//�����ý�����ʣ�µ�ģ����Ϣ
	while (isNext){
		//�жϵ�ǰģ��������Ƿ��Ǵ�ж�ص�ģ�����ƣ�����ǣ���flag��Ϊtrue��������ѭ��
		if (stricmp(mem32.szModule, dllName) == 0){
			flag = TRUE;
			break;
		}
		//��ȡ��һ��ģ�����Ϣ
		isNext = Module32Next(hSnap, &mem32);
	}
	//���flagΪflase��˵���ý����в�û������Ҫж�ص�ģ��
	if (flag == FALSE){
		CloseHandle(hSnap);
		return -1;
	}
	
	//����pid��ȡָ���Ľ��̾��
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(hRemoteProcess == NULL){
		CloseHandle(hSnap);
		return -2;
	}
	//����Kernel32.dllģ��
	HMODULE hModule =  GetModuleHandle("Kernel32.dll");
	//ͨ��Kernel32.dllģ�����FreeLibrary�����ĺ�����ַ
	LPVOID funcAddr = GetProcAddress(hModule, "FreeLibrary");
	//��ָ���Ľ����д����߳�
	HANDLE hRemoteThread = ::CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)funcAddr, mem32.hModule, 0, NULL);
	if(hRemoteThread == NULL){
		CloseHandle(hSnap);
		CloseHandle(hRemoteProcess);
		return -3;
	}
	//�ȴ���ֱ���߳�ִ�����
	WaitForSingleObject(hRemoteThread, INFINITE);

	//�ͷ���Դ
	CloseHandle(hSnap);
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);

	return 1;

}





