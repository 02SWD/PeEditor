// DLLInjectByWriteMemory.cpp: implementation of the DLLInjectByWriteMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DLLInjectByWriteMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
int repairRelocTableOfImageBuffer(LPVOID pImageBuffer, IN DWORD newImageBase);
DWORD WINAPI InjectEntryWriteMemoryProc(LPVOID lpParameter);
BOOL EnableDebugPrivilege();

/**
 * Զ���߳�ע�� - ���ڴ�д��ķ�ʽ
 * @param  pid  Ŀ����̵�pid
 * @param  flag ���ڼ�¼ע�������˳���
 *              -1 �ڴ�д��ʧ��
 *              -2 Զ���̴߳���ʧ��
 *               1 ע��ɹ�
 * @return      ��װ�й�ע����Ϣ�Ķ���
 */
InjectByWriteMemory RemoteDllInjectMemoryWrite(IN DWORD pid, OUT int* flag){
	//���ڼ�¼ע����Ϣ���Ա�ж�غ������Ը��ݸ���Ϣ��ж��ע��ģ��
	InjectByWriteMemory injectByWriteMemory = {0};
	//��ȡ��ǰ����exe��ģ����
	HMODULE hModule = GetModuleHandle(NULL);

	//��Ϊ��ν�ľ����ʵ����ģ����׵�ַ�������������ֱ�ӽ����תΪPIMAGE_DOS_HEADER����ʹ��
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSecTableHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeader + pPeHeader->SizeOfOptionalHeader);

	//��¼��exeģ��Ļ�ַ
	DWORD ImageBaseTemp = pOptionalHeader->ImageBase;
	//��¼��exeģ���SizeOfImage
	DWORD SizeOfImageTemp = pOptionalHeader->SizeOfImage;

	//�ڱ�����������SizeOfImage��С���ڴ�ռ䣬�����г�ʼ��
	char* pImageBuffer = (char*)malloc(SizeOfImageTemp);
	memset(pImageBuffer, 0, SizeOfImageTemp);
	//����exe��ImageBuffer���Ƶ�������Ļ�������
	memcpy(pImageBuffer, (LPVOID)hModule, SizeOfImageTemp);
	
	//����pid��Ŀ����̣���ȡĿ����̵ľ��
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	printf("hRemoteProcess1 %x\n", (DWORD)hRemoteProcess);
	//��Ŀ�����������SizeOfImage��С���ڴ�ռ�
	LPVOID pVirtualAddr = VirtualAllocEx(hRemoteProcess, NULL, SizeOfImageTemp, MEM_COMMIT | MEM_RESERVE , PAGE_EXECUTE_READWRITE);
	//������Ŀ����̿ռ��У��������ڴ���׵�ַ����ImageBuffer�����ض�λ���޸�
	int relocFlag = repairRelocTableOfImageBuffer(pImageBuffer, (DWORD)pVirtualAddr);
	//�������ض�λ���޸���ImageBuffer��д�뵽Ŀ����̵��ڴ�ռ��С����ڴ�д��ʧ�ܣ����ͷ���Դ����ֱ�ӷ���-1
	if(0 == WriteProcessMemory(hRemoteProcess, pVirtualAddr, pImageBuffer, SizeOfImageTemp, NULL)){
		//�ͷŵ�������ڴ�ռ�
		free(pImageBuffer);
		*flag = -1;
		return injectByWriteMemory;
	}

	//��ȡ InjectEntryProc �����ڱ����̿ռ�ĺ�����ַ
	DWORD oldFuncAddr = (DWORD)InjectEntryWriteMemoryProc;
	//���� InjectEntryProc ������Ŀ����̿ռ�ʱ�ĺ�����ַ
	DWORD newFuncAddr = oldFuncAddr - (DWORD)hModule + (DWORD)pVirtualAddr;
	
	//��Ŀ������д���Զ���̣߳�����InjectEntryProc������Ŀ����̿ռ�ĺ�����ַ��Ϊ�̻߳ص������ĵ�ַ�����̻߳ص������Ĳ���Ϊ����Ŀ����̿ռ����������ڴ���׵�ַ
	HANDLE hRemoteThread = ::CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)newFuncAddr, pVirtualAddr, 0, NULL);
	//��Զ���̴߳���ʧ�ܣ����ͷ���Դ��ֱ�ӷ���-2
	if (hRemoteThread == NULL){
		//�ͷŵ�������ڴ�ռ�
		*flag = -2;
		return injectByWriteMemory;
	}
	//ע��������гɹ�������1
	*flag = 1;
	//��ȫ�ֱ���gFlagOfAddr����Ŀ����̵ĵ�ַ����
	injectByWriteMemory.FlagOfAddr = (DWORD)(&gFlag) - (DWORD)hModule + (DWORD)pVirtualAddr;
	//��ģ����Ŀ����̵��׵�ַ����
	injectByWriteMemory.ModuleOfAddr = (DWORD)pVirtualAddr;
	//��ģ���sizeOfImage����
	injectByWriteMemory.ModuleSizeOfImage = SizeOfImageTemp;
	//��Զ���̵߳ľ������
	injectByWriteMemory.HRemoteThread = (DWORD)hRemoteThread;
	//�ͷŵ�������ڴ�ռ�
	free(pImageBuffer);
	//����ע����Ϣ
	return injectByWriteMemory;
}

/**
 * ж��ģ��
 * @param  pid                 Ŀ����̵�pid
 * @param  injectByWriteMemory ��װ��ע��ģ����Ϣ�Ķ����Ա�ж�غ������ݸ���Ϣж��ģ��
 * @return                     -1 �ڴ����Ը���ʧ��
 *                             -2 Զ���߳���ֹʧ��
 *                             -3 ж��ʧ��
 *                              1 ж�سɹ�
 */
int RemoteUninstallModule(IN DWORD pid, IN InjectByWriteMemory injectByWriteMemory){
	//��Ȩ
	EnableDebugPrivilege();
	//����pid�򿪽���
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	//�޸� ���ڿ���Զ���߳̽�����ȫ�ֱ��� ���ڿռ��Ȩ��
	PDWORD oldProtect1 = (PDWORD)malloc(5);
	int virtualProtectFlag = VirtualProtectEx(hRemoteProcess, (LPVOID)injectByWriteMemory.FlagOfAddr, 4, PAGE_READWRITE, oldProtect1);
	if (virtualProtectFlag == NULL){
		CloseHandle(hRemoteProcess);
		return -1;
	}	
	//�� ���ڿ���Զ���̵߳�ȫ�ֱ��� �޸�ΪFALSE�����ڽ���Զ���̵߳�ִ�У� 
	int flag = FALSE;
	if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)injectByWriteMemory.FlagOfAddr, &flag, 4, NULL)){
		return -2;
	}
	//�ȴ���ֱ��Զ���߳���ֹ
	WaitForSingleObject((HANDLE)injectByWriteMemory.HRemoteThread, INFINITE);
	//�ر�Զ���̵߳ľ��
	CloseHandle((HANDLE)injectByWriteMemory.HRemoteThread);
	
	BOOL VirtualFreeExFlag = VirtualFreeEx(hRemoteProcess, (LPVOID)injectByWriteMemory.ModuleOfAddr, injectByWriteMemory.ModuleSizeOfImage, MEM_DECOMMIT);
	if (0 == VirtualFreeExFlag){
		CloseHandle(hRemoteProcess);
		return -4;
	}
	
	//�ر�Զ�̽��̵ľ��
	CloseHandle(hRemoteProcess);
	return 1;
}


/**
 * Զ���̵߳Ļص�����
 * @param  lpParameter �̻߳ص������Ĳ����������ʾ��Ŀ����������������ڴ���׵�ַ��
 * @return             [description]
 */
DWORD WINAPI InjectEntryWriteMemoryProc(LPVOID lpParameter){
	//��λ dosͷ��ntͷ����׼peͷ����ѡpeͷ
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpParameter;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)lpParameter + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);
	
	//��λ��һ��������λ��
	DWORD importTableRva = pOptionalHeader->DataDirectory[1].VirtualAddress;
	PIMAGE_IMPORT_DESCRIPTOR importTableAddr = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)lpParameter + importTableRva);
	
    //�����ṹ��ֹһ������������ʹ��whileѭ���������ı�־Ϊsizeof(IMAGE_IMPORT_DESCRIPTOR)��0��Ϊ�˷��㣬������ֱ��ͨ������һ�����Ƿ�Ϊ0�����ж��Ƿ����
	while (importTableAddr->OriginalFirstThunk !=0 && importTableAddr->FirstThunk !=0){
		//�� IatTableAddr ָ��ָ��iat����׵�ַ
		int* IatTableAddr = (int*)((DWORD)lpParameter + importTableAddr->FirstThunk);
		//�� IntTableAddr ָ��ָ��int����׵�ַ
		int* IntTableAddr = (int*)((DWORD)lpParameter + importTableAddr->OriginalFirstThunk);
		//�� dllNameAddr ָ��ָ��õ����������ַ�����ַ
		char* dllNameAddr = (char*)((DWORD)lpParameter + importTableAddr->Name);
		
		//Ϊָ��pid�Ľ����������
		HMODULE hModule = NULL; 
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
		//�ýṹ��������ڼ�¼ģ�����Ϣ
		MODULEENTRY32 mem32 = {0};
		mem32.dwSize = sizeof(MODULEENTRY32);
		//��ȡ�ý����е�һ��ģ�����Ϣ
		BOOL isNext = Module32First(hSnap, &mem32);
		BOOL flag = FALSE;
		//�����ý�����ʣ�µ�ģ����Ϣ
		while (isNext){
			//�жϵ�ǰģ��������Ƿ��Ǵ�ж�ص�ģ�����ƣ�����ǣ���flag��Ϊtrue��������ģ��ľ��������Ȼ������ѭ��
			if (strcmp(mem32.szModule, dllNameAddr) == 0){
				flag = TRUE;
				hModule = mem32.hModule;
				break;
			}
			//��ȡ��һ��ģ�����Ϣ
			isNext = Module32Next(hSnap, &mem32);
		}
		//���������û���Ѽ��غõĸ�ģ�飬��ô���Ǿ��Լ�����
		if (flag == FALSE){
			CloseHandle(hSnap);
			hModule = LoadLibrary(dllNameAddr);
			if(hModule == NULL){
				return -1;
			}
		}
		//ѭ������INT��������INT���޸�IAT��
		while (*IntTableAddr != 0){
			//�жϸú���������ŵ��룬�����Ժ���������
			if ((*IntTableAddr & 0x80000000) == 0x80000000){
				//����ŵ���Ĵ������
				DWORD exportFuncAddr = (DWORD)GetProcAddress(hModule, (char*)(*IntTableAddr & 0x7fffffff));
				*IatTableAddr = exportFuncAddr;
			}else{
				//�Ժ���������Ĵ������
				PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)lpParameter + *IntTableAddr);
				DWORD exportFuncAddr = (DWORD)GetProcAddress(hModule, (char*)funcName->Name);
				*IatTableAddr = exportFuncAddr;
			}
			//IatTableAddrָ��ָ����һ��iat�����
			IatTableAddr++;
			//IntTableAddrָ��ָ����һ��int�����
			IntTableAddr++;
		}
		//��importTableAddrָ��ָ����һ�������
		importTableAddr++;

	}

	//����ִ������������룬��gFlagΪFALSEʱ��ѭ���Ż������Զ���̲߳Ż����ִ��
	while (TRUE){
		if(gFlag == TRUE){
			MessageBoxA(0,TEXT("writeMemory inject success��"),0,0);
			Sleep(7000);
		}else{
			break;
		}
	}
	return 0;
}

/**
 * �����û���������ImageBuffer�޸��ض�λ��ע�⣺���ﴫ�������� ImageBuffer�������� FileBuffer��
 * @param  pImageBuffer    �û�������޸���pImageBuffer��ֱ���޸�ԭʼ��pImageBuffer���ò�����Ϊ�������ҲΪ���������
 * @param  newImageBase   �û������ImageBase
 * @return                 1 �޸��ɹ�
 *                        -1 ��ImageBase���ض�λ��
 */
int repairRelocTableOfImageBuffer(LPVOID pImageBuffer, IN DWORD newImageBase){
	//��ȡ DOSͷ��NTͷ����׼PEͷ����ѡPEͷ�����һ���ڱ�
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

	//��ȡFileBuffer���ض�λ���RVA
	DWORD reLocationRva = pOptionalHeader->DataDirectory[5].VirtualAddress;
	//�жϸ�File�Ƿ�����ض�λ����û�У��򷵻�-1
	if (reLocationRva == 0){
		return -1;
	}
    //reLocationָ��ָ���һ���ض�λ����׵�ַ��ͨ��pNewFileBufferTempָ����ض�λ���FOA������ض�λ�����ڴ��е��׵�ַ������PIMAGE_BASE_RELOCATION���͵�ָ��ָ������
	PIMAGE_BASE_RELOCATION reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)pImageBuffer + reLocationRva);
    //��VirtualAddress��SizeOfBlock��ֵΪ��ʱ����������е��ض�λ�����������Ӧ����ѭ��
	while (!(reLocation->VirtualAddress == 0 && reLocation->SizeOfBlock == 0)){
		//�õ��ض�λ���virtualAddress����ֵ
		DWORD virtualAddress = reLocation->VirtualAddress;
		//�õ��ض�λ���SizeOfBlock����ֵ
		DWORD sizeOfBlock = reLocation->SizeOfBlock;
		//��itemsָ��ָ���������׵�ַ
		short* pItem = (short*)((DWORD)reLocation + 8);
		//���������ĸ���
		DWORD numOfItems = (sizeOfBlock - 8)/2;
		//���������ݿ������еľ����������4λΪ3�ľ����� ������λ�õ����ݽ����޸�
		for (DWORD i=0; i<numOfItems; i++){
			//�жϸþ�����ĸ�4λ��ֵ�Ƿ�Ϊ3����Ϊ3��������޸�
			if ((pItem[i]&0xF000)>>12 == 3){
				//����þ�����������λ�õ�RVA
				DWORD realItemRva = (pItem[i]&0x0FFF) + virtualAddress;
				//�� realItemAddr ָ��ָ��þ������������λ�õ�ַ
				int* realItemAddr = (int*)((DWORD)pImageBuffer + realItemRva);
				//�����޸�
				*realItemAddr = *realItemAddr - (pOptionalHeader->ImageBase) + newImageBase;
			}
		}
		//֮����Ҫ�����ƶ�reLocationָ�룬����ΪPIMAGE_BASE_RELOCATION�ṹ���沢�����ǽ�����PIMAGE_BASE_RELOCATION�ṹ�ģ����ҵľ������С�ǲ�ȷ���ġ�
		reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)reLocation + reLocation->SizeOfBlock);
	}
	//�޸�FileBuffer��ImageBase
	pOptionalHeader->ImageBase = newImageBase;
	return 1;
}

//��Ȩ
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







