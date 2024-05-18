// DecryptingShell.cpp: implementation of the DecryptingShell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DecryptingShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
int aesDecryptApp(IN LPVOID cipherText, IN DWORD sizeOfCipherText, OUT LPVOID* plainText, OUT PDWORD sizeOfPlainText);
BOOL EnableDebugPrivilege();

/**
 * �ѿǣ�����������Ҫִ�еĳ���
 * @return 		1 �ѿǲ����гɹ�
 *             -1 ���ѿǳ����ȡʧ��
 *             -2 aes����ʧ��
 *             -3 ���ܳ��������Ĳ���PE�ļ���FileBuffer
 *             -4 ���ܳ��������Ĳ���PE�ļ���FileBuffer
 *             -5 FileBuffer ����Ϊ ImageBuffer ʧ��
 *             -6 ntdll.dll ����ʧ��
 *             -7 ZwUnmapViewOfSection()������ַ��ȡʧ��
 *             -9 �����Ĵ����г����ImageBufferд���ڴ�ʧ��
 *            -10 ImageBase�޸�ʧ��
 *            -11 û����ָ����λ�����뵽�ڴ棬�Ҹ�FileBufferû���ض�λ��
 *            -12 û����ָ����λ�����뵽�ڴ棬��FileBuffer�����ض�λ������������λ�������ڴ�ʱ����Ȼ����ʧ��
 *            -13 �ض�λ���޸�ʧ��
 *             
 */
int decryptShell(){
	//EnableDebugPrivilege();
	
	char szFile[MAX_PATH] = {0};
	GetModuleFileName(NULL, szFile, MAX_PATH);
	
	//����ʱʹ��: ��ȡ���ѿǵ��ļ�
	//char* szFile = "C:\\Documents and Settings\\QianYiShen\\����\\shell_shell.exe";
	
	FILEINFOR unshelledAppFile;
	unshelledAppFile = readFile(szFile);
	if (unshelledAppFile.p == NULL){
		return -1;
	}
	
	//ȡ�ô��ѿ��ļ��� dosͷ����׼peͷ����ѡpeͷ����һ���ڱ�ͷ�����һ���ڱ�ͷ
	PIMAGE_DOS_HEADER pDosHeaderOfShell = (PIMAGE_DOS_HEADER)unshelledAppFile.p;
	PIMAGE_NT_HEADERS pNtHeaderOfShell = (PIMAGE_NT_HEADERS)((DWORD)pDosHeaderOfShell + pDosHeaderOfShell->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeaderOfShell = (PIMAGE_FILE_HEADER)((DWORD)pNtHeaderOfShell + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeaderOfShell = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeaderOfShell + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeaderOfShell = (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeaderOfShell + pPeHeaderOfShell->SizeOfOptionalHeader);
	PIMAGE_SECTION_HEADER pLastSectionHeaderOfShell = pSectionHeaderOfShell + pPeHeaderOfShell->NumberOfSections - 1;
	
	//��ȡ�����ѿǳ����е����ģ��������ѿǳ������һ���ڵ����ݣ�
	DWORD sizeOfCipherText = pLastSectionHeaderOfShell->Misc.VirtualSize;
	char* cipherTextBuffer = (char*)malloc(sizeOfCipherText);
	memset(cipherTextBuffer, 0, sizeOfCipherText);
	memcpy(cipherTextBuffer, (LPVOID)((DWORD)pDosHeaderOfShell + pLastSectionHeaderOfShell->PointerToRawData), sizeOfCipherText);

	//�����Ľ��н��ܣ����ܺ�����ģ���Ϊ������Ҫ���еĳ����FileBuffer��
	LPVOID plainText = NULL;
	DWORD sizeOfPlainText = 0;
	int aesFlag = aesDecryptApp(cipherTextBuffer, sizeOfCipherText, &plainText, &sizeOfPlainText);
	if (aesFlag != 1){
		free(unshelledAppFile.p);		
		return -2;
	}

	//��ȡ����Ҫ���еĳ���� dosͷ����׼peͷ����ѡpeͷ����һ���ڱ�ͷ�����һ���ڱ�ͷ
	//�ж��Ƿ�����Ч��MZ��־
	if(*((PWORD)plainText) != IMAGE_DOS_SIGNATURE){
		free(unshelledAppFile.p);
		return -3;
	}
	PIMAGE_DOS_HEADER pDosHeaderOfApp = (PIMAGE_DOS_HEADER)plainText;
	//�ж��Ƿ�����Ч��PE��־
	if(*((PDWORD)((DWORD)plainText+pDosHeaderOfApp->e_lfanew)) != IMAGE_NT_SIGNATURE){
		free(unshelledAppFile.p);
		return -4;
	}
	PIMAGE_NT_HEADERS pNtHeaderOfApp = (PIMAGE_NT_HEADERS)((DWORD)pDosHeaderOfApp + pDosHeaderOfApp->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeaderOfApp = (PIMAGE_FILE_HEADER)((DWORD)pNtHeaderOfApp + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeaderOfApp = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeaderOfApp + IMAGE_SIZEOF_FILE_HEADER);

	//�Դ��ѿǳ��������ѹ���ķ�ʽ���ٴ���һ������
	STARTUPINFO ie_si = {0};
	ie_si.cb = sizeof(ie_si);
	PROCESS_INFORMATION ie_pi;
	CreateProcess(NULL, szFile, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, NULL, &ie_si, &ie_pi);
	
	//��ȡ�´����Ľ��̵� context
	CONTEXT context;
	context.ContextFlags = CONTEXT_FULL;
	BOOL ok = ::GetThreadContext(ie_pi.hThread, &context);
	
	//��ntdll.dll�л�ȡpZwUnmapViewOfSection()����������pZwUnmapViewOfSection()�����ĵ�ַ�洢�� pZwUnmapViewOfSection ָ�������
	HMODULE hModuleNt = LoadLibrary("ntdll.dll");
	if (NULL == hModuleNt){
		free(unshelledAppFile.p);
		free(plainText);
		return -6;
	}
	typedef DWORD(WINAPI *_TZwUnmapViewOfSection)(HANDLE, PVOID);
	_TZwUnmapViewOfSection pZwUnmapViewOfSection = (_TZwUnmapViewOfSection)GetProcAddress(hModuleNt, "ZwUnmapViewOfSection");
	if (NULL == pZwUnmapViewOfSection){
		free(unshelledAppFile.p);
		free(plainText);
		return -7;
	}

	//ж�ظý����У�ԭ����exe������ڴ澵��
	pZwUnmapViewOfSection(ie_pi.hProcess, (LPVOID)pOptionalHeaderOfShell->ImageBase);

	//�� �´����Ľ��������ڴ�ռ� 
	LPVOID pImageAppImageBase = VirtualAllocEx(ie_pi.hProcess, (LPVOID)pOptionalHeaderOfApp->ImageBase, pOptionalHeaderOfApp->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	//LPVOID pImageAppImageBase = NULL; //���ڲ�����ָ����ImageBase���벻���ڴ�ʱ���޸��ض�λ��ĳ����Ƿ���Գɹ����У����Գɹ���
	//�ж��Ƿ������ָ����ImageBase���뵽�ڴ�
	if (pImageAppImageBase == NULL){
		//�жϸ� FileBuffer �Ƿ�����ض�λ�����û���ض�λ����ֱ�ӷ��أ��ӿ�ʧ��
		if(pOptionalHeaderOfApp->DataDirectory[5].VirtualAddress == 0){
			free(unshelledAppFile.p);
			free(plainText);
			return -11;
		}
		//������ض�λ�������½����е�����λ�������㹻��С���ڴ�
		pImageAppImageBase = VirtualAllocEx(ie_pi.hProcess, (LPVOID)NULL, pOptionalHeaderOfApp->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		//�ж��Ƿ�����ɹ�������Ȼ����ʧ�ܣ���ֱ�ӷ��أ��ӿ�ʧ��
		if (pImageAppImageBase == NULL){
			free(unshelledAppFile.p);
			free(plainText);
			return -12;
		}
		//��������λ�óɹ����뵽���ڴ棬����ݷ��ص��ڴ��׵�ַ��FileBuffer���ض�λ���޸�FileBuffer
		int repireBool =  repairRelocTable(plainText, (DWORD)pImageAppImageBase);
		//�ж��Ƿ��޸��ɹ�
		if (repireBool != 1){
			free(unshelledAppFile.p);
			free(plainText);
			return -13;
		}
		//printf("repire success!! new ImageBase %x\n", (DWORD)pImageAppImageBase);
	}
	//printf("ImageBase %x\n", (DWORD)pImageAppImageBase);
	
	//�� ����Ҫ���еĳ����FileBuffer ����Ϊ ImageBuffer
	LPVOID imagePlainText = NULL;
	CopyFileBufferToImageBuffer(plainText, &imagePlainText);
	if (imagePlainText == NULL){
		free(unshelledAppFile.p);
		free(plainText);
		return -5;
	}
	
	
	//�� ���������г����ImageBuffer д�뵽 �´����Ľ�����
	if(0 == WriteProcessMemory(ie_pi.hProcess, (LPVOID)(pOptionalHeaderOfApp->ImageBase), imagePlainText, pOptionalHeaderOfApp->SizeOfImage, NULL)){
		free(unshelledAppFile.p);
		free(plainText);
		free(imagePlainText);
		return -9;
	}

	//�޸��´������̵�context
	//�޸��½��̵� ��ڵ�
	context.Eax = pOptionalHeaderOfApp->AddressOfEntryPoint + pOptionalHeaderOfApp->ImageBase;
	//�޸��½��̵� ImageBase
	if(0 == WriteProcessMemory(ie_pi.hProcess, (LPVOID)(context.Ebx + 8), &(pOptionalHeaderOfApp->ImageBase), 4, NULL)){
		free(unshelledAppFile.p);
		free(plainText);
		free(imagePlainText);
		return -10;
	}
	context.ContextFlags = CONTEXT_FULL;
	SetThreadContext(ie_pi.hThread, &context);

	//�ָ�ִ��
	ResumeThread(ie_pi.hThread);
	
	//�ͷ���Դ
	free(unshelledAppFile.p);
	free(plainText);
	free(imagePlainText);
	return 1;

}

/**
 * AES����
 * @param  cipherText       ����
 * @param  sizeOfCipherText ���Ĵ�С
 * @param  plainText        ����
 * @param  sizeOfPlainText  ���Ĵ�С
 * @return                  1 ���ܳɹ�
 *                         -1 ���Ŀռ�����ʧ��
 */
int aesDecryptApp(IN LPVOID cipherText, IN DWORD sizeOfCipherText, OUT LPVOID* plainText, OUT PDWORD sizeOfPlainText){
    //��Կ������ʵ������Լ����壬AES128 ��16�ֽڡ�AES192 ��24�ֽڡ�AES256 ��32�ֽ�
    unsigned char key[32] = {
        0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x41,0x42,0x43,0x44,0x45,0x46,
        0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x41,0x42,0x43,0x44,0x45,0x46
    };
    //��ʼ������, �̶�����16��, ��mode=AES_MODE_CBCʱ�õ�
    unsigned char IV[4*Nb] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x41,0x42,0x43,0x44,0x45,0x46};
	
	unsigned char aesInfoAll[264] = {0x20, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0xFF, 0x19, 0x00, 0xF4, 0xFE, 0x19, 0x00, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0xCC, 0xCC, 0xCC, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xFF, 0x19, 0x00, 0xB9, 0x2B, 0x40, 0x00, 0x01, 0x00, 0x00, 0x00, 0x38, 0x1A, 0x7D, 0x00, 0x90, 0x1A, 0x7D, 0x00, 0xD0, 0x2A, 0x40, 0x00, 0xD0, 0x2A, 0x40, 0x00, 0x00, 0x30, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0xFF, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0x19, 0x00, 0x40, 0x5C, 0x40, 0x00, 0x30, 0x34, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x19, 0x00, 0x59, 0x63, 0x7B, 0x75, 0x00, 0x30, 0x34, 0x00, 0x40, 0x63, 0x7B, 0x75, 0xDC, 0xFF, 0x19, 0x00, 0x26, 0xE6, 0x50, 0xB2, 0x13, 0xD0, 0x67, 0x8A, 0x2A, 0xE0, 0x26, 0xC8, 0x69, 0xA4, 0x63, 0x8E, 0xC8, 0x7B, 0xC8, 0x2D, 0xFD, 0x4D, 0xFF, 0x15, 0xC4, 0x7D, 0xBE, 0x57, 0x87, 0x39, 0xFB, 0x11, 0x4B, 0xF5, 0x37, 0xDD, 0x4B, 0xF5, 0x37, 0xDD, 0x4B, 0xF5, 0x37, 0xDD, 0x3B, 0x0A, 0x2E, 0xDD, 0x82, 0x21, 0x6E, 0xDD, 0x83, 0x21, 0x6E, 0xDD, 0xBB, 0x3B, 0x13, 0xDD, 0x2B, 0x21, 0x6E, 0xDD, 0xFB, 0x0B, 0x2E, 0xDD, 0x2B, 0x21, 0x6E, 0xDD, 0x2B, 0x11, 0x5A, 0xDD, 0x2B, 0x11, 0x5A, 0xDD, 0x2B, 0x11, 0x5A, 0xDD, 0x6F, 0xEE, 0x43, 0xDD, 0x6F, 0xEE, 0x43, 0xDD, 0xA3, 0x11, 0x5A, 0xDD, 0xE3, 0x4D, 0x1A, 0xDD, 0xD3, 0x79, 0x58, 0xDD, 0xD3, 0x79, 0x58, 0xDD, 0x53, 0x86, 0x41, 0xDD};

	//���ü��ܷ�ʽ���ܳ�
	AESInfo_t aesInfo;
	memcpy(&aesInfo, aesInfoAll, 264);
	aesInfo.type = AES256;
	aesInfo.mode = AES_MODE_CBC;
	aesInfo.key = key;
	aesInfo.pIV = IV;

	//Ҫ���ܵ�����
	unsigned char* source = (unsigned char*)cipherText;
	//���ܺ������
    unsigned char* decryptMsg = (unsigned char*)malloc(sizeOfCipherText);	
	if (decryptMsg ==NULL){
		return -1;
	}
	memset(decryptMsg, 0, sizeOfCipherText);
	
	//��ʼ��
	//AESInit(&aesInfo);
	//����
    unsigned int len = AESDecrypt(&aesInfo, decryptMsg, source, sizeOfCipherText);
	
	//�� �������� �� ���ĵĴ�С����
	*plainText = decryptMsg;
	*sizeOfPlainText = len;
	return 1;
}

// ��Ȩ����������ΪDEBUGȨ��
BOOL EnableDebugPrivilege(){
	HANDLE hToken;
	BOOL fOk = FALSE;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
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

