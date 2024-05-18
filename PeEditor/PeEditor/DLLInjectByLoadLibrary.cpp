// DLLInjectByLoadLibrary.cpp: implementation of the DLLInjectByLoadLibrary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DLLInjectByLoadLibrary.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


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
int RemoteThreadInjectionModule(IN DWORD pid, IN LPSTR dllPath){
	//加载Kernel32.dll模块
	HMODULE hModule =  GetModuleHandle("Kernel32.dll");
	if(hModule == NULL){
		return -1;
	}
	//从 Kernel32.dll 模块中获取 LoadLibraryA函数 的函数地址
	LPVOID funcAddr = GetProcAddress(hModule, "LoadLibraryA");

	//根据pid获取对应进程的句柄
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hRemoteProcess == NULL){
		return -2;
	}
	//向执行的进程空间中申请空间
	LPVOID pVirtualAddr = VirtualAllocEx(hRemoteProcess, NULL, strlen(dllPath)+1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(pVirtualAddr == NULL){
		CloseHandle(hRemoteProcess);
		return -3;
	}
	//将DLL的路径写入指定进程的内存空间中中
	if(0 == WriteProcessMemory(hRemoteProcess, pVirtualAddr, dllPath, strlen(dllPath)+1, NULL)){
		VirtualFreeEx(hRemoteProcess, pVirtualAddr, 0, MEM_RELEASE);
		CloseHandle(hRemoteProcess);
		return -4;
	}
	//在指定的进程中创建线程
	HANDLE hRemoteThread = ::CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)funcAddr, pVirtualAddr, 0, NULL);
	if(hRemoteThread == NULL){
		VirtualFreeEx(hRemoteProcess, pVirtualAddr, 0, MEM_RELEASE);
		CloseHandle(hRemoteProcess);
		return -5;
	}
	//等待，直至线程执行完毕
	WaitForSingleObject(hRemoteThread, INFINITE);
	//获取指定进程在加载完毕dll模块后的句柄
	DWORD handleOfDll = 0;
	GetExitCodeThread(hRemoteThread,&handleOfDll);
	
	//释放资源
	VirtualFreeEx(hRemoteProcess, pVirtualAddr, 0, MEM_RELEASE);
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);
	//返回句柄
	return handleOfDll;
}

/**
 * 远程卸载DLL
 * @param  pid     要卸载的进程pid
 * @param  dllName 要卸载的dll名称
 * @return         -1 指定的进程没有待卸载的模块
 *                 -2 根据pid获取句柄失败
 *                 -3 在指定的进程中创建线程失败
 *                  1 卸载成功
 */
int RemoteThreadUninstallModule(DWORD pid, LPSTR dllName){
	//为指定pid的进程拍摄快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 mem32 = {0};
	mem32.dwSize = sizeof(MODULEENTRY32);
	//获取该进程中第一个模块的信息
	BOOL isNext = Module32First(hSnap, &mem32);
	BOOL flag = FALSE;
	//遍历该进程中剩下的模块信息
	while (isNext){
		//判断当前模块的名称是否是待卸载的模块名称，如果是，则将flag改为true，并跳出循环
		if (stricmp(mem32.szModule, dllName) == 0){
			flag = TRUE;
			break;
		}
		//获取下一个模块的信息
		isNext = Module32Next(hSnap, &mem32);
	}
	//如果flag为flase，说明该进程中并没有你想要卸载的模块
	if (flag == FALSE){
		CloseHandle(hSnap);
		return -1;
	}
	
	//根据pid获取指定的进程句柄
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(hRemoteProcess == NULL){
		CloseHandle(hSnap);
		return -2;
	}
	//加载Kernel32.dll模块
	HMODULE hModule =  GetModuleHandle("Kernel32.dll");
	//通过Kernel32.dll模块查找FreeLibrary函数的函数地址
	LPVOID funcAddr = GetProcAddress(hModule, "FreeLibrary");
	//在指定的进程中创建线程
	HANDLE hRemoteThread = ::CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)funcAddr, mem32.hModule, 0, NULL);
	if(hRemoteThread == NULL){
		CloseHandle(hSnap);
		CloseHandle(hRemoteProcess);
		return -3;
	}
	//等待，直至线程执行完毕
	WaitForSingleObject(hRemoteThread, INFINITE);

	//释放资源
	CloseHandle(hSnap);
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);

	return 1;

}





