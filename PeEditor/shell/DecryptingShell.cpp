// DecryptingShell.cpp: implementation of the DecryptingShell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DecryptingShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
int aesDecryptApp(IN LPVOID cipherText, IN DWORD sizeOfCipherText, OUT LPVOID* plainText, OUT PDWORD sizeOfPlainText);
BOOL EnableDebugPrivilege();

/**
 * 脱壳，并运行真正要执行的程序
 * @return 		1 脱壳并运行成功
 *             -1 待脱壳程序读取失败
 *             -2 aes解密失败
 *             -3 解密出来的明文不是PE文件的FileBuffer
 *             -4 解密出来的明文不是PE文件的FileBuffer
 *             -5 FileBuffer 拉伸为 ImageBuffer 失败
 *             -6 ntdll.dll 加载失败
 *             -7 ZwUnmapViewOfSection()函数地址获取失败
 *             -9 真正的待运行程序的ImageBuffer写入内存失败
 *            -10 ImageBase修改失败
 *            -11 没有在指定的位置申请到内存，且该FileBuffer没有重定位表
 *            -12 没有在指定的位置申请到内存，该FileBuffer存在重定位表，但是在任意位置申请内存时，仍然申请失败
 *            -13 重定位表修复失败
 *             
 */
int decryptShell(){
	//EnableDebugPrivilege();
	
	char szFile[MAX_PATH] = {0};
	GetModuleFileName(NULL, szFile, MAX_PATH);
	
	//测试时使用: 读取待脱壳的文件
	//char* szFile = "C:\\Documents and Settings\\QianYiShen\\桌面\\shell_shell.exe";
	
	FILEINFOR unshelledAppFile;
	unshelledAppFile = readFile(szFile);
	if (unshelledAppFile.p == NULL){
		return -1;
	}
	
	//取得待脱壳文件的 dos头、标准pe头、可选pe头、第一个节表头、最后一个节表头
	PIMAGE_DOS_HEADER pDosHeaderOfShell = (PIMAGE_DOS_HEADER)unshelledAppFile.p;
	PIMAGE_NT_HEADERS pNtHeaderOfShell = (PIMAGE_NT_HEADERS)((DWORD)pDosHeaderOfShell + pDosHeaderOfShell->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeaderOfShell = (PIMAGE_FILE_HEADER)((DWORD)pNtHeaderOfShell + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeaderOfShell = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeaderOfShell + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeaderOfShell = (PIMAGE_SECTION_HEADER)((DWORD)pOptionalHeaderOfShell + pPeHeaderOfShell->SizeOfOptionalHeader);
	PIMAGE_SECTION_HEADER pLastSectionHeaderOfShell = pSectionHeaderOfShell + pPeHeaderOfShell->NumberOfSections - 1;
	
	//提取出待脱壳程序中的密文（即：待脱壳程序最后一个节的数据）
	DWORD sizeOfCipherText = pLastSectionHeaderOfShell->Misc.VirtualSize;
	char* cipherTextBuffer = (char*)malloc(sizeOfCipherText);
	memset(cipherTextBuffer, 0, sizeOfCipherText);
	memcpy(cipherTextBuffer, (LPVOID)((DWORD)pDosHeaderOfShell + pLastSectionHeaderOfShell->PointerToRawData), sizeOfCipherText);

	//将密文进行解密（解密后的明文，即为：真正要运行的程序的FileBuffer）
	LPVOID plainText = NULL;
	DWORD sizeOfPlainText = 0;
	int aesFlag = aesDecryptApp(cipherTextBuffer, sizeOfCipherText, &plainText, &sizeOfPlainText);
	if (aesFlag != 1){
		free(unshelledAppFile.p);		
		return -2;
	}

	//获取真正要运行的程序的 dos头、标准pe头、可选pe头、第一个节表头、最后一个节表头
	//判断是否是有效的MZ标志
	if(*((PWORD)plainText) != IMAGE_DOS_SIGNATURE){
		free(unshelledAppFile.p);
		return -3;
	}
	PIMAGE_DOS_HEADER pDosHeaderOfApp = (PIMAGE_DOS_HEADER)plainText;
	//判断是否是有效的PE标志
	if(*((PDWORD)((DWORD)plainText+pDosHeaderOfApp->e_lfanew)) != IMAGE_NT_SIGNATURE){
		free(unshelledAppFile.p);
		return -4;
	}
	PIMAGE_NT_HEADERS pNtHeaderOfApp = (PIMAGE_NT_HEADERS)((DWORD)pDosHeaderOfApp + pDosHeaderOfApp->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeaderOfApp = (PIMAGE_FILE_HEADER)((DWORD)pNtHeaderOfApp + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeaderOfApp = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeaderOfApp + IMAGE_SIZEOF_FILE_HEADER);

	//以待脱壳程序自身，已挂起的方式，再创建一个进程
	STARTUPINFO ie_si = {0};
	ie_si.cb = sizeof(ie_si);
	PROCESS_INFORMATION ie_pi;
	CreateProcess(NULL, szFile, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, NULL, &ie_si, &ie_pi);
	
	//获取新创建的进程的 context
	CONTEXT context;
	context.ContextFlags = CONTEXT_FULL;
	BOOL ok = ::GetThreadContext(ie_pi.hThread, &context);
	
	//从ntdll.dll中获取pZwUnmapViewOfSection()函数，并将pZwUnmapViewOfSection()函数的地址存储到 pZwUnmapViewOfSection 指针变量中
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

	//卸载该进程中，原本的exe程序的内存镜像
	pZwUnmapViewOfSection(ie_pi.hProcess, (LPVOID)pOptionalHeaderOfShell->ImageBase);

	//向 新创建的进程申请内存空间 
	LPVOID pImageAppImageBase = VirtualAllocEx(ie_pi.hProcess, (LPVOID)pOptionalHeaderOfApp->ImageBase, pOptionalHeaderOfApp->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	//LPVOID pImageAppImageBase = NULL; //用于测试在指定的ImageBase申请不到内存时，修复重定位表的程序是否可以成功运行（测试成功）
	//判断是否可以在指定的ImageBase申请到内存
	if (pImageAppImageBase == NULL){
		//判断该 FileBuffer 是否具有重定位表，如果没有重定位表，则直接返回，加壳失败
		if(pOptionalHeaderOfApp->DataDirectory[5].VirtualAddress == 0){
			free(unshelledAppFile.p);
			free(plainText);
			return -11;
		}
		//如果有重定位表，则在新进程中的任意位置申请足够大小的内存
		pImageAppImageBase = VirtualAllocEx(ie_pi.hProcess, (LPVOID)NULL, pOptionalHeaderOfApp->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		//判断是否申请成功，若仍然申请失败，则直接返回，加壳失败
		if (pImageAppImageBase == NULL){
			free(unshelledAppFile.p);
			free(plainText);
			return -12;
		}
		//若在任意位置成功申请到了内存，则根据返回的内存首地址和FileBuffer的重定位表修复FileBuffer
		int repireBool =  repairRelocTable(plainText, (DWORD)pImageAppImageBase);
		//判断是否修复成功
		if (repireBool != 1){
			free(unshelledAppFile.p);
			free(plainText);
			return -13;
		}
		//printf("repire success!! new ImageBase %x\n", (DWORD)pImageAppImageBase);
	}
	//printf("ImageBase %x\n", (DWORD)pImageAppImageBase);
	
	//将 真正要运行的程序的FileBuffer 拉伸为 ImageBuffer
	LPVOID imagePlainText = NULL;
	CopyFileBufferToImageBuffer(plainText, &imagePlainText);
	if (imagePlainText == NULL){
		free(unshelledAppFile.p);
		free(plainText);
		return -5;
	}
	
	
	//将 真正待运行程序的ImageBuffer 写入到 新创建的进程中
	if(0 == WriteProcessMemory(ie_pi.hProcess, (LPVOID)(pOptionalHeaderOfApp->ImageBase), imagePlainText, pOptionalHeaderOfApp->SizeOfImage, NULL)){
		free(unshelledAppFile.p);
		free(plainText);
		free(imagePlainText);
		return -9;
	}

	//修改新创建进程的context
	//修改新进程的 入口点
	context.Eax = pOptionalHeaderOfApp->AddressOfEntryPoint + pOptionalHeaderOfApp->ImageBase;
	//修改新进程的 ImageBase
	if(0 == WriteProcessMemory(ie_pi.hProcess, (LPVOID)(context.Ebx + 8), &(pOptionalHeaderOfApp->ImageBase), 4, NULL)){
		free(unshelledAppFile.p);
		free(plainText);
		free(imagePlainText);
		return -10;
	}
	context.ContextFlags = CONTEXT_FULL;
	SetThreadContext(ie_pi.hThread, &context);

	//恢复执行
	ResumeThread(ie_pi.hThread);
	
	//释放资源
	free(unshelledAppFile.p);
	free(plainText);
	free(imagePlainText);
	return 1;

}

/**
 * AES解密
 * @param  cipherText       密文
 * @param  sizeOfCipherText 密文大小
 * @param  plainText        明文
 * @param  sizeOfPlainText  明文大小
 * @return                  1 解密成功
 *                         -1 明文空间申请失败
 */
int aesDecryptApp(IN LPVOID cipherText, IN DWORD sizeOfCipherText, OUT LPVOID* plainText, OUT PDWORD sizeOfPlainText){
    //秘钥，根据实际情况自己定义，AES128 用16字节、AES192 用24字节、AES256 用32字节
    unsigned char key[32] = {
        0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x41,0x42,0x43,0x44,0x45,0x46,
        0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x41,0x42,0x43,0x44,0x45,0x46
    };
    //初始化向量, 固定长度16个, 当mode=AES_MODE_CBC时用到
    unsigned char IV[4*Nb] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x41,0x42,0x43,0x44,0x45,0x46};
	
	unsigned char aesInfoAll[264] = {0x20, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0xFF, 0x19, 0x00, 0xF4, 0xFE, 0x19, 0x00, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0xCC, 0xCC, 0xCC, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xFF, 0x19, 0x00, 0xB9, 0x2B, 0x40, 0x00, 0x01, 0x00, 0x00, 0x00, 0x38, 0x1A, 0x7D, 0x00, 0x90, 0x1A, 0x7D, 0x00, 0xD0, 0x2A, 0x40, 0x00, 0xD0, 0x2A, 0x40, 0x00, 0x00, 0x30, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0xFF, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0x19, 0x00, 0x40, 0x5C, 0x40, 0x00, 0x30, 0x34, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x19, 0x00, 0x59, 0x63, 0x7B, 0x75, 0x00, 0x30, 0x34, 0x00, 0x40, 0x63, 0x7B, 0x75, 0xDC, 0xFF, 0x19, 0x00, 0x26, 0xE6, 0x50, 0xB2, 0x13, 0xD0, 0x67, 0x8A, 0x2A, 0xE0, 0x26, 0xC8, 0x69, 0xA4, 0x63, 0x8E, 0xC8, 0x7B, 0xC8, 0x2D, 0xFD, 0x4D, 0xFF, 0x15, 0xC4, 0x7D, 0xBE, 0x57, 0x87, 0x39, 0xFB, 0x11, 0x4B, 0xF5, 0x37, 0xDD, 0x4B, 0xF5, 0x37, 0xDD, 0x4B, 0xF5, 0x37, 0xDD, 0x3B, 0x0A, 0x2E, 0xDD, 0x82, 0x21, 0x6E, 0xDD, 0x83, 0x21, 0x6E, 0xDD, 0xBB, 0x3B, 0x13, 0xDD, 0x2B, 0x21, 0x6E, 0xDD, 0xFB, 0x0B, 0x2E, 0xDD, 0x2B, 0x21, 0x6E, 0xDD, 0x2B, 0x11, 0x5A, 0xDD, 0x2B, 0x11, 0x5A, 0xDD, 0x2B, 0x11, 0x5A, 0xDD, 0x6F, 0xEE, 0x43, 0xDD, 0x6F, 0xEE, 0x43, 0xDD, 0xA3, 0x11, 0x5A, 0xDD, 0xE3, 0x4D, 0x1A, 0xDD, 0xD3, 0x79, 0x58, 0xDD, 0xD3, 0x79, 0x58, 0xDD, 0x53, 0x86, 0x41, 0xDD};

	//设置加密方式、密匙
	AESInfo_t aesInfo;
	memcpy(&aesInfo, aesInfoAll, 264);
	aesInfo.type = AES256;
	aesInfo.mode = AES_MODE_CBC;
	aesInfo.key = key;
	aesInfo.pIV = IV;

	//要解密的内容
	unsigned char* source = (unsigned char*)cipherText;
	//解密后的内容
    unsigned char* decryptMsg = (unsigned char*)malloc(sizeOfCipherText);	
	if (decryptMsg ==NULL){
		return -1;
	}
	memset(decryptMsg, 0, sizeOfCipherText);
	
	//初始化
	//AESInit(&aesInfo);
	//解密
    unsigned int len = AESDecrypt(&aesInfo, decryptMsg, source, sizeOfCipherText);
	
	//将 密文数据 和 密文的大小传出
	*plainText = decryptMsg;
	*sizeOfPlainText = len;
	return 1;
}

// 提权函数：提升为DEBUG权限
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

