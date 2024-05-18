// DLLInjectByWriteMemory.cpp: implementation of the DLLInjectByWriteMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DLLInjectByWriteMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
int repairRelocTableOfImageBuffer(LPVOID pImageBuffer, IN DWORD newImageBase);
DWORD WINAPI InjectEntryWriteMemoryProc(LPVOID lpParameter);
BOOL EnableDebugPrivilege();

/**
 * 远程线程注入 - 以内存写入的方式
 * @param  pid  目标进程的pid
 * @param  flag 用于记录注入程序的退出码
 *              -1 内存写入失败
 *              -2 远程线程创建失败
 *               1 注入成功
 * @return      封装有关注入信息的对象
 */
InjectByWriteMemory RemoteDllInjectMemoryWrite(IN DWORD pid, OUT int* flag){
	//用于记录注入信息，以便卸载函数可以根据该信息来卸载注入模块
	InjectByWriteMemory injectByWriteMemory = {0};
	//获取当前进程exe的模块句柄
	HMODULE hModule = GetModuleHandle(NULL);

	//因为所谓的句柄其实就是模块的首地址，所以这里可以直接将句柄转为PIMAGE_DOS_HEADER类型使用
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSecTableHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pPeHeader->SizeOfOptionalHeader);

	//记录本exe模块的基址
	DWORD ImageBaseTemp = pOptionalHeader->ImageBase;
	//记录本exe模块的SizeOfImage
	DWORD SizeOfImageTemp = pOptionalHeader->SizeOfImage;

	//在本进程中申请SizeOfImage大小的内存空间，并进行初始化
	char* pImageBuffer = (char*)malloc(SizeOfImageTemp);
	memset(pImageBuffer, 0, SizeOfImageTemp);
	//将本exe的ImageBuffer复制到新申请的缓冲区中
	memcpy(pImageBuffer, (LPVOID)hModule, SizeOfImageTemp);
	
	//根据pid打开目标进程，获取目标进程的句柄
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	printf("hRemoteProcess1 %x\n", (DWORD)hRemoteProcess);
	//在目标进程中申请SizeOfImage大小的内存空间
	LPVOID pVirtualAddr = VirtualAllocEx(hRemoteProcess, NULL, SizeOfImageTemp, MEM_COMMIT | MEM_RESERVE , PAGE_EXECUTE_READWRITE);
	//按照在目标进程空间中，新申请内存的首地址，对ImageBuffer进行重定位表修复
	int relocFlag = repairRelocTableOfImageBuffer(pImageBuffer, (DWORD)pVirtualAddr);
	//将经过重定位表修复的ImageBuffer，写入到目标进程的内存空间中。若内存写入失败，则释放资源，并直接返回-1
	if(0 == WriteProcessMemory(hRemoteProcess, pVirtualAddr, pImageBuffer, SizeOfImageTemp, NULL)){
		//释放掉申请的内存空间
		free(pImageBuffer);
		*flag = -1;
		return injectByWriteMemory;
	}

	//获取 InjectEntryProc 函数在本进程空间的函数地址
	DWORD oldFuncAddr = (DWORD)InjectEntryWriteMemoryProc;
	//计算 InjectEntryProc 函数在目标进程空间时的函数地址
	DWORD newFuncAddr = oldFuncAddr - (DWORD)hModule + (DWORD)pVirtualAddr;
	
	//在目标进程中创建远程线程，并将InjectEntryProc函数在目标进程空间的函数地址作为线程回调函数的地址，而线程回调函数的参数为：在目标进程空间中新申请内存的首地址
	HANDLE hRemoteThread = ::CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)newFuncAddr, pVirtualAddr, 0, NULL);
	//若远程线程创建失败，则释放资源，直接返回-2
	if (hRemoteThread == NULL){
		//释放掉申请的内存空间
		*flag = -2;
		return injectByWriteMemory;
	}
	//注入程序运行成功，返回1
	*flag = 1;
	//将全局变量gFlagOfAddr，在目标进程的地址传出
	injectByWriteMemory.FlagOfAddr = (DWORD)(&gFlag) - (DWORD)hModule + (DWORD)pVirtualAddr;
	//将模块在目标进程的首地址传出
	injectByWriteMemory.ModuleOfAddr = (DWORD)pVirtualAddr;
	//将模块的sizeOfImage传出
	injectByWriteMemory.ModuleSizeOfImage = SizeOfImageTemp;
	//将远程线程的句柄传出
	injectByWriteMemory.HRemoteThread = (DWORD)hRemoteThread;
	//释放掉申请的内存空间
	free(pImageBuffer);
	//返回注入信息
	return injectByWriteMemory;
}

/**
 * 卸载模块
 * @param  pid                 目标进程的pid
 * @param  injectByWriteMemory 封装有注入模块信息的对象，以便卸载函数根据该信息卸载模块
 * @return                     -1 内存属性更改失败
 *                             -2 远程线程终止失败
 *                             -3 卸载失败
 *                              1 卸载成功
 */
int RemoteUninstallModule(IN DWORD pid, IN InjectByWriteMemory injectByWriteMemory){
	//提权
	EnableDebugPrivilege();
	//根据pid打开进程
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	//修改 用于控制远程线程结束的全局变量 所在空间的权限
	PDWORD oldProtect1 = (PDWORD)malloc(5);
	int virtualProtectFlag = VirtualProtectEx(hRemoteProcess, (LPVOID)injectByWriteMemory.FlagOfAddr, 4, PAGE_READWRITE, oldProtect1);
	if (virtualProtectFlag == NULL){
		CloseHandle(hRemoteProcess);
		return -1;
	}	
	//将 用于控制远程线程的全局变量 修改为FALSE（用于结束远程线程的执行） 
	int flag = FALSE;
	if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)injectByWriteMemory.FlagOfAddr, &flag, 4, NULL)){
		return -2;
	}
	//等待，直至远程线程终止
	WaitForSingleObject((HANDLE)injectByWriteMemory.HRemoteThread, INFINITE);
	//关闭远程线程的句柄
	CloseHandle((HANDLE)injectByWriteMemory.HRemoteThread);
	
	BOOL VirtualFreeExFlag = VirtualFreeEx(hRemoteProcess, (LPVOID)injectByWriteMemory.ModuleOfAddr, injectByWriteMemory.ModuleSizeOfImage, MEM_DECOMMIT);
	if (0 == VirtualFreeExFlag){
		CloseHandle(hRemoteProcess);
		return -4;
	}
	
	//关闭远程进程的句柄
	CloseHandle(hRemoteProcess);
	return 1;
}


/**
 * 远程线程的回调函数
 * @param  lpParameter 线程回调函数的参数（这里表示在目标进程中所新申请内存的首地址）
 * @return             [description]
 */
DWORD WINAPI InjectEntryWriteMemoryProc(LPVOID lpParameter){
	//定位 dos头、nt头、标准pe头、可选pe头
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpParameter;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)lpParameter + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);
	
	//定位第一个导入表的位置
	DWORD importTableRva = pOptionalHeader->DataDirectory[1].VirtualAddress;
	PIMAGE_IMPORT_DESCRIPTOR importTableAddr = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)lpParameter + importTableRva);
	
    //导入表结构不止一个，所以这里使用while循环，结束的标志为sizeof(IMAGE_IMPORT_DESCRIPTOR)个0，为了方便，这里我直接通过其中一部分是否为0，来判断是否结束
	while (importTableAddr->OriginalFirstThunk !=0 && importTableAddr->FirstThunk !=0){
		//令 IatTableAddr 指针指向iat表的首地址
		int* IatTableAddr = (int*)((DWORD)lpParameter + importTableAddr->FirstThunk);
		//令 IntTableAddr 指针指向int表的首地址
		int* IntTableAddr = (int*)((DWORD)lpParameter + importTableAddr->OriginalFirstThunk);
		//令 dllNameAddr 指针指向该导入表的名称字符串地址
		char* dllNameAddr = (char*)((DWORD)lpParameter + importTableAddr->Name);
		
		//为指定pid的进程拍摄快照
		HMODULE hModule = NULL; 
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
		//该结构体变量用于记录模块的信息
		MODULEENTRY32 mem32 = {0};
		mem32.dwSize = sizeof(MODULEENTRY32);
		//获取该进程中第一个模块的信息
		BOOL isNext = Module32First(hSnap, &mem32);
		BOOL flag = FALSE;
		//遍历该进程中剩下的模块信息
		while (isNext){
			//判断当前模块的名称是否是待卸载的模块名称，如果是，则将flag改为true，并将该模块的句柄传出，然后跳出循环
			if (strcmp(mem32.szModule, dllNameAddr) == 0){
				flag = TRUE;
				hModule = mem32.hModule;
				break;
			}
			//获取下一个模块的信息
			isNext = Module32Next(hSnap, &mem32);
		}
		//如果进程中没有已加载好的该模块，那么我们就自己加载
		if (flag == FALSE){
			CloseHandle(hSnap);
			hModule = LoadLibrary(dllNameAddr);
			if(hModule == NULL){
				return -1;
			}
		}
		//循环遍历INT表，并根据INT表修改IAT表
		while (*IntTableAddr != 0){
			//判断该函数是以序号导入，还是以函数名导入
			if ((*IntTableAddr & 0x80000000) == 0x80000000){
				//以序号导入的处理代码
				DWORD exportFuncAddr = (DWORD)GetProcAddress(hModule, (char*)(*IntTableAddr & 0x7fffffff));
				*IatTableAddr = exportFuncAddr;
			}else{
				//以函数名导入的处理代码
				PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)lpParameter + *IntTableAddr);
				DWORD exportFuncAddr = (DWORD)GetProcAddress(hModule, (char*)funcName->Name);
				*IatTableAddr = exportFuncAddr;
			}
			//IatTableAddr指针指向下一个iat表表项
			IatTableAddr++;
			//IntTableAddr指针指向下一个int表表项
			IntTableAddr++;
		}
		//令importTableAddr指针指向下一个导入表
		importTableAddr++;

	}

	//以下执行其他恶意代码，当gFlag为FALSE时，循环才会结束，远程线程才会结束执行
	while (TRUE){
		if(gFlag == TRUE){
			MessageBoxA(0,TEXT("writeMemory inject success！"),0,0);
			Sleep(7000);
		}else{
			break;
		}
	}
	return 0;
}

/**
 * 根据用户传进来的ImageBuffer修复重定位表（注意：这里传进来的是 ImageBuffer，而不是 FileBuffer）
 * @param  pImageBuffer    用户传入待修复的pImageBuffer（直接修改原始的pImageBuffer，该参数既为输入参数也为输出参数）
 * @param  newImageBase   用户输入的ImageBase
 * @return                 1 修复成功
 *                        -1 该ImageBase无重定位表
 */
int repairRelocTableOfImageBuffer(LPVOID pImageBuffer, IN DWORD newImageBase){
	//获取 DOS头、NT头、标准PE头、可选PE头、最后一个节表
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

	//获取FileBuffer的重定位表的RVA
	DWORD reLocationRva = pOptionalHeader->DataDirectory[5].VirtualAddress;
	//判断该File是否具有重定位表，若没有，则返回-1
	if (reLocationRva == 0){
		return -1;
	}
    //reLocation指针指向第一个重定位表的首地址（通过pNewFileBufferTemp指针和重定位表的FOA计算出重定位表在内存中的首地址，并令PIMAGE_BASE_RELOCATION类型的指针指向它）
	PIMAGE_BASE_RELOCATION reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)pImageBuffer + reLocationRva);
    //当VirtualAddress与SizeOfBlock的值为零时，则代表所有的重定位表遍历结束，应跳出循环
	while (!(reLocation->VirtualAddress == 0 && reLocation->SizeOfBlock == 0)){
		//得到重定位表的virtualAddress属性值
		DWORD virtualAddress = reLocation->VirtualAddress;
		//得到重定位表的SizeOfBlock属性值
		DWORD sizeOfBlock = reLocation->SizeOfBlock;
		//将items指针指向具体项的首地址
		short* pItem = (short*)((DWORD)reLocation + 8);
		//计算具体项的个数
		DWORD numOfItems = (sizeOfBlock - 8)/2;
		//遍历该数据块中所有的具体项，并将高4位为3的具体项 所代表位置的数据进行修复
		for (DWORD i=0; i<numOfItems; i++){
			//判断该具体项的高4位的值是否为3，若为3，则进行修复
			if ((pItem[i]&0xF000)>>12 == 3){
				//计算该具体项所代表位置的RVA
				DWORD realItemRva = (pItem[i]&0x0FFF) + virtualAddress;
				//令 realItemAddr 指针指向该具体项所代表的位置地址
				int* realItemAddr = (int*)((DWORD)pImageBuffer + realItemRva);
				//进行修复
				*realItemAddr = *realItemAddr - (pOptionalHeader->ImageBase) + newImageBase;
			}
		}
		//之所以要这样移动reLocation指针，是因为PIMAGE_BASE_RELOCATION结构后面并不还是紧跟着PIMAGE_BASE_RELOCATION结构的，并且的具体项大小是不确定的。
		reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)reLocation + reLocation->SizeOfBlock);
	}
	//修改FileBuffer的ImageBase
	pOptionalHeader->ImageBase = newImageBase;
	return 1;
}

//提权
BOOL EnableDebugPrivilege(){
	HANDLE hToken;
	BOOL fOk = FALSE;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)){
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
 
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
 
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}







