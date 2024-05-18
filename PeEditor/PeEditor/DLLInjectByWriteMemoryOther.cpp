// DLLInjectByWriteMemoryOther.cpp: implementation of the DLLInjectByWriteMemoryOther class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DLLInjectByWriteMemoryOther.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
int myGetProcAddressCrossProcess(IN DWORD pid, IN HMODULE hRemoteModule, IN char* funcName, IN DWORD funcOrdinal, OUT PDWORD exportFuncAddr);

/**
 * 将指定的DLL直接写入到目标进程中（不使用LoadLibrary函数，且在自身的进程中修复IAT表表项）- 测试成功
 * @param  dllPath                DLL的路径（绝对路径）
 * @param  pid                    目标进程的pid
 * @param  dllAddrInTargetProcess 写入成功后，dll模块在目标进程中的首地址
 * @param  dllSizeOfImage         写入成功后，dll模块在目标进程中的SizeOfImage
 * @return                        -1 dll文件读取失败！
 *                                -2 dll的FileBuffer拉伸失败
 *                                -3 目标进程的句柄获取失败
 *                                -4 目标进程的内存空间申请失败
 *                                -5 dll的ImageBuffer重定位表修复失败
 *                                -6 目标进程中没有本程序所需的DLL
 *                                -7 DLL的IAT表表项值修复失败
 *                                -8 dll写入目标进程失败
 *                                 1 dll注入成功
 * 注意：
 *      本程序对于dll中IAT表表项值的修复，仅限于目标进程中存在修复该表项值所需的对应模块
 *    （即：如果在该dll中，有一个IAT表表项所对应函数，该函数所需的模块未被目标进程加载，那么该IAT表项将会修复失败）
 */
int injectDllByWriteMemoryWithoutRemoteThread(IN char* dllPath, IN DWORD pid, OUT PDWORD dllAddrInTargetProcess, OUT PDWORD dllSizeOfImage){
    //读取待注入的DLL文件数据
    FILEINFOR dllFile = readFile(dllPath);
    if (dllFile.p == NULL){
        printf("dll文件读取失败！\n");
        return -1;
    }
    //将DLL的FileBuffer拉伸为ImageBuffer
    LPVOID dllImageBuffer = NULL;
    DWORD dwFlag =  CopyFileBufferToImageBuffer(dllFile.p, &dllImageBuffer);
    if(dwFlag == 0){
        printf("拉伸失败!\n");
        free(dllFile.p);
        return -2;
    }

    //定位ImageBuffer的 dos头、nt头、标准pe头 和 可选pe头
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)dllImageBuffer;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //获取dll ImageBuffer 的大小
    DWORD SizeOfImageTemp = pOptionalHeader->SizeOfImage;

    //根据pid获取目标进程的句柄
    HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hRemoteProcess == NULL){
        printf("远程进程打开失败!\n");
        free(dllFile.p);
        free(dllImageBuffer);
        return -3;
    }
    
    //在目标进程的任意位置申请足够容纳dll ImageBuffer的空间
    LPVOID pVirtualAddr = VirtualAllocEx(hRemoteProcess, NULL, SizeOfImageTemp, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (pVirtualAddr == NULL){
        printf("目标进程内存空间申请失败!\n");
        free(dllFile.p);
        free(dllImageBuffer);
        CloseHandle(hRemoteProcess);
        return -4;
    }

    //============================================ 进行重定位修复 ==============================================
    //根据dll的重定位表修复ImageBuffer
    int relocFlag = repairRelocTableOfImageBuffer(dllImageBuffer, (DWORD)pVirtualAddr);
    if (relocFlag != 1){
        printf("重定位表修复失败!\n");
        free(dllFile.p);
        free(dllImageBuffer);
        CloseHandle(hRemoteProcess);
        return -5;
    }
    
    //============================================= 修复IAT表的表项值 =============================================
    //定位 经重定位表修复之后的ImageBuffer的 dos头、nt头、标准pe头 和 可选pe头
    PIMAGE_DOS_HEADER pDosHeaderAfterReloc = (PIMAGE_DOS_HEADER)dllImageBuffer;
    PIMAGE_NT_HEADERS pNtHeaderAfterReloc = (PIMAGE_NT_HEADERS)((DWORD)pDosHeaderAfterReloc + pDosHeaderAfterReloc->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeaderAfterReloc = (PIMAGE_FILE_HEADER)((DWORD)pNtHeaderAfterReloc + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeaderAfterReloc = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeaderAfterReloc + IMAGE_SIZEOF_FILE_HEADER);

    //获取导入表的RVA
    DWORD importTableRva = pOptionalHeaderAfterReloc->DataDirectory[1].VirtualAddress;
    //令 importTableAddr 指针指向导入表的首地址
    PIMAGE_IMPORT_DESCRIPTOR importTableAddr = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)dllImageBuffer + importTableRva);

    //循环遍历dll导入表
    while (importTableAddr->Name != 0){
        //令 IatTableAddr 指针指向IAT表首地址
        int* IatTableAddr = (int*)((DWORD)dllImageBuffer + importTableAddr->FirstThunk);
        //令 IntTableAddr 指针指向INT表首地址
        int* IntTableAddr = (int*)((DWORD)dllImageBuffer + importTableAddr->OriginalFirstThunk);
        //令 dllNameAddr 指针指向该导入表名称的首地址
        char* dllNameAddr  = (char*)((DWORD)dllImageBuffer + importTableAddr->Name);
        
        //用于暂存目标进程的模块句柄
        HMODULE hRemoteModule = NULL;
        //根据pid为目标进程拍摄快照
        HANDLE hRemoteSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        //用于记录目标进程中模块的信息
        MODULEENTRY32 mem32 = {0};
        mem32.dwSize = sizeof(MODULEENTRY32);
        //获取目标进程第一个模块的信息
        BOOL isNext = Module32First(hRemoteSnap, &mem32);
        BOOL moduleFlag = FALSE;
        while (isNext){
            //判断当前目标进程中的模块是否是dll所需要的模块
            if (stricmp(dllNameAddr, mem32.szModule) == 0){
                //若是，则记录下目标进程中该模块的句柄，并跳出循环
                moduleFlag = TRUE;
                hRemoteModule = mem32.hModule;
                break;
            }
            //获取目标进程中下一个模块的句柄
            isNext = Module32Next(hRemoteSnap, &mem32);
        }
        if (moduleFlag == FALSE){
            printf("目标进程中没有本dll所需的模块!\n");
            free(dllFile.p);
            free(dllImageBuffer);
            CloseHandle(hRemoteProcess);
            CloseHandle(hRemoteSnap);
            return -6;
        }
        //循环遍历dll的INT表
        while (*IntTableAddr != 0){
            //判断INT表项值的最高位是否为1，若为1，说明该函数是按导出序号导入的；否则，说明该函数是按函数名导入的
            if ((*IntTableAddr & 0x80000000) == 0x80000000){
                //根据导出序号，得到目标进程中指定模块中的函数的函数地址
                DWORD exportFuncAddr = 0;
                int funcAddrFlag = myGetProcAddressCrossProcess(pid, hRemoteModule, NULL, (DWORD)(*IntTableAddr & 0x7fffffff), &exportFuncAddr);
                if (funcAddrFlag != 1){
                    printf("DLL的IAT表表项值修复失败!\n");
                    free(dllFile.p);
                    free(dllImageBuffer);
                    CloseHandle(hRemoteProcess);
                    CloseHandle(hRemoteSnap);
                    return -7;
                }
                //修复dll的IAT表项值
                *IatTableAddr = exportFuncAddr;
				
            }else{
                //根据函数名，得到目标进程中指定模块中的函数的函数地址
                PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)dllImageBuffer + *IntTableAddr);
                DWORD exportFuncAddr = 0;
				//printf("func->name %s\n", funcName->Name);
                int funcAddrFlag = myGetProcAddressCrossProcess(pid, hRemoteModule, (char*)funcName->Name, NULL, &exportFuncAddr);
                if (funcAddrFlag != 1){
                    printf("DLL的IAT表表项值修复失败!\n");
                    free(dllFile.p);
                    free(dllImageBuffer);
                    CloseHandle(hRemoteProcess);
                    CloseHandle(hRemoteSnap);
                    return -7;
                }
				printf("oldExportFuncAddr %x\n ",exportFuncAddr);
                //修复dll的IAT表项值
                *IatTableAddr = exportFuncAddr;
				printf("funcName %s  exportFuncAddr %x\n\n ",funcName->Name, exportFuncAddr);
            }
            //令 IatTableAddr 指针指向下一个IAT表项
            IatTableAddr++;
            //令 IntTableAddr 指针指向下一个INT表项
            IntTableAddr++;
        }
        //令 importTableAddr 指针指向下一个导入表
        importTableAddr++;
        //关闭快照
        CloseHandle(hRemoteSnap);
    }
    //===========================================================================================================

    //修改dll的ImageBase
    pOptionalHeaderAfterReloc->ImageBase = (DWORD)pVirtualAddr;

    //将修复完毕的 dll ImageBuffer 写入到目标进程中
    if (WriteProcessMemory(hRemoteProcess, pVirtualAddr, dllImageBuffer, SizeOfImageTemp, NULL) == 0){
        free(dllFile.p);
        free(dllImageBuffer);
        CloseHandle(hRemoteProcess);
        return -8;
    }
    
    //将dll在目标进程中的首地址传出
    *dllAddrInTargetProcess = (DWORD)pVirtualAddr;
    //将dll的SizeOfImage传出
    *dllSizeOfImage = SizeOfImageTemp;
    
    //释放资源
    free(dllFile.p);
    free(dllImageBuffer);
    //关闭句柄
    CloseHandle(hRemoteProcess);
    return 1;
}


/**
 * 获取目标函数在目标进程中的函数地址（GetProcAddress函数的跨进程版本，自，测试成功）
 * @param  pid            目标进程的pid
 * @param  hRemoteModule  目标进程中的模块句柄
 * @param  funcName       想要获取的函数的函数名（不区分大小写）
 * @param  funcOrdinal    想要获取的函数的导出序号（不区分10进制与16进制）
 * @param  exportFuncAddr 想要获取的函数在目标进程中的地址
 * @return                -1 函数名和函数序号不能同时输入
 *                        -2 内存申请失败
 *                        -3 DOS头读取失败
 *                        -4 内存申请失败
 *                        -5 远程进程内存读取失败
 *                        -6 内存申请失败
 *                        -7 远程进程内存读取失败
 *                        -8 导出表中没有用户查找的函数（导出序号形式）
 *                        -9 导出表中没有用户查找的函数（函数名形式）
 *                         1 函数地址获取成功
 * 注意：
 *      参数funcName 和 参数funcOrdinal 必须有一个为NULL（即：不能同时有值）
 *      参数funcName：表示根据函数名查找函数地址
 *      参数funcOrdinal：表示根据函数的导出序号查找函数地址
 */
int myGetProcAddressCrossProcess(IN DWORD pid, IN HMODULE hRemoteModule, IN char* funcName, IN DWORD funcOrdinal, OUT PDWORD exportFuncAddr){
    if (funcName != NULL && funcOrdinal != 0){
        printf("函数名和函数序号不能同时输入!\n");
        return -1;
    }

    //===============================================将目标dll模块的数据复制到自己的进程空间中=========================================================
    //获取目标进程的句柄
    HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    //由于此时我们并不知道目标模块ImageBuffer的大小，所以我们先申请足够容纳 dos头，nt头、标准pe头 和 可选pe头 的空间（因为其所占的空间是固定的），然后读取目标模块，根据可选pe头中的SizeOfImage得出目标模块的ImgaeBuffer大小
    //还有一个问题就是：虽然 dos头、nt头、标准pe头 和 可选pe头的大小是固定的，但是dos头和nt头之间的垃圾数据所占的空间是不固定的，所以我们要先取得dos头数据，计算出垃圾数据所占的空间 
    //为dos头申请内存空间
    char* pDosHeaderSizeSpace = (char*)malloc(64);
    if (pDosHeaderSizeSpace == NULL){
        printf("内存申请失败!\n");
        return -2;
    }
    //初始化
    memset(pDosHeaderSizeSpace, 0, 64);
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)hRemoteModule, pDosHeaderSizeSpace, 64, NULL) == 0){
        printf("DOS头读取失败!\n");
        free(pDosHeaderSizeSpace);
        return -3;
    }
    //解析dos头
    PIMAGE_DOS_HEADER pDosHeaderTemp = (PIMAGE_DOS_HEADER)pDosHeaderSizeSpace;
    //sizeOfRubbishSpare 记录垃圾数据所占空间的大小
    DWORD sizeOfRubbishSpare = pDosHeaderTemp->e_lfanew - 0x40;
    //已经计算出了垃圾数据所占空间大小，可以释放掉dos头数据所占的内存了
    free(pDosHeaderSizeSpace);
    
    // sizeOfBeforeSection = dos头大小 + 垃圾数据大小 + pe标志大小 + 标准pe头大小 + 可选pe头大小
    DWORD sizeOfBeforeSection = 64 + sizeOfRubbishSpare + 4 + 20 + 224;
    char* partImageBuffer = (char*)malloc(sizeOfBeforeSection);
    if (partImageBuffer == NULL){
        printf("内存申请失败!\n");
        free(partImageBuffer);
        CloseHandle(hRemoteProcess);
        return -4;
    }
    //初始化内存空间
    memset(partImageBuffer, 0, sizeOfBeforeSection);
    //将目标模块的 dos头、nt头、标准pe头 和 可选pe头 读取到内存中
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)hRemoteModule, partImageBuffer, sizeOfBeforeSection, NULL) == 0){
        printf("远程进程内存读取失败!\n");
        free(partImageBuffer);
        CloseHandle(hRemoteProcess);
        return -5;
    }
    
    //解析目标模块的 dos头、nt头、标准pe头 和 可选pe头
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)partImageBuffer;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //根据可选pe头来获取目标模块ImageBuffer的大小
    DWORD targetModuleSizeOfImage = pOptionalHeader->SizeOfImage;
    //因为我们已经得知目标模块ImageBuffer的大小，所以partImageBuffer内存可以被释放了
    free(partImageBuffer);
    
    //申请足够容纳 目标模块ImageBuffer大小 的内存空间
    char* ImageBuffer = (char*)malloc(targetModuleSizeOfImage);
    if(ImageBuffer == NULL){
        printf("内存申请失败！\n");
        CloseHandle(hRemoteProcess);
        return -6;
    }
    //将目标模块读取到新申请的内存空间中
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)hRemoteModule, ImageBuffer, targetModuleSizeOfImage, NULL) == 0){
        printf("远程进程内存读取失败!\n");
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -7;
    }

    //======解析目标DLL的导出表，获取目标函数的函数地址的RVA，并根据 函数地址的RVA 和 目标DLL模块在目标进程中的首地址 计算出该函数在目标进程中的函数地址=======
    //重新解析 dos头、nt头、标准pe头 和 可选pe头
    pDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;
    pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //得到目标模块导出表的RVA
    DWORD exportTableRva = pOptionalHeader->DataDirectory[0].VirtualAddress;
    //令pExportTableAddr指针指向目标模块导出表的首地址
    PIMAGE_EXPORT_DIRECTORY pExportTableAddr = (PIMAGE_EXPORT_DIRECTORY)((DWORD)ImageBuffer + exportTableRva);
    //得到导出表的 Base 属性值
    DWORD exportBase = pExportTableAddr->Base;
	//得到导出表的 NumberOfFunctions 属性值（导出函数的总数）
	DWORD exportNumberOfFunctions = pExportTableAddr->NumberOfFunctions;
    //得到导出表的 NumberOfNames 属性值（以函数名来导出的函数个数）
    DWORD exportNumberOfNames = pExportTableAddr->NumberOfNames;
    //得到导出表的 AddressOfFunctions 属性值（函数地址表的TVA）
    DWORD exportAddressOfFunctionsRva = pExportTableAddr->AddressOfFunctions;
    //得到导出表的 AddressOfNames 属性值（函数名称表的RVA）
    DWORD exportAddressOfNamesRva = pExportTableAddr->AddressOfNames;
    //得到导出表的 AddressOfNameOrdinals 属性值（函数序号表的RVA）
    DWORD exportAddressOfNameOrdinalsRva = pExportTableAddr->AddressOfNameOrdinals;

    //令 addressOfFunctions 指针指向目标模块的函数地址表首地址
    int* addressOfFunctions = (int*)((DWORD)ImageBuffer + exportAddressOfFunctionsRva);
    //令 addressOfNames 指针指向目标模块的函数名称表首地址
    int* addressOfNames = (int*)((DWORD)ImageBuffer + exportAddressOfNamesRva);
    //令 addressOfNameOrdinals 指针指向目标模块的函数序号表首地址
    short* addressOfNameOrdinals = (short*)((DWORD)ImageBuffer + exportAddressOfNameOrdinalsRva);
    //用于存储目标函数在目标进程中的地址
    DWORD exportFuncAddrTemp = 0;
    if (funcOrdinal != 0){
		if ((funcOrdinal - exportBase) > (exportNumberOfFunctions - 1)){
			printf("导出表中没有您要查找的函数, 请输入正确的导出序号!\n");
			free(ImageBuffer);
			CloseHandle(hRemoteProcess);
			return -8;
		}
        //根据函数导出序号来获取函数地址
        DWORD exportFuncRva = addressOfFunctions[funcOrdinal - exportBase];
        exportFuncAddrTemp = (DWORD)hRemoteModule + exportFuncRva;
    }else{
		BOOL flag = FALSE;
        //根据函数名来获取函数地址
        for(DWORD j=0; j<exportNumberOfNames; j++){
            char* name = (char*)((DWORD)ImageBuffer + addressOfNames[j]);
            if (stricmp(funcName, name) == 0){
                DWORD exportFuncRva = addressOfFunctions[addressOfNameOrdinals[j]];
                exportFuncAddrTemp = (DWORD)hRemoteModule + exportFuncRva;
				flag = TRUE;
                break;
            }
        }
		if (flag == FALSE){
			printf("导出表中没有您要查找的函数, 请输入正确的函数名!");
			free(ImageBuffer);
			CloseHandle(hRemoteProcess);
			return -9;
		}
    }
    //===============================================将计算得到的函数地址传出=========================================================
    //将目标函数在目标进程中的地址传出
    *exportFuncAddr = exportFuncAddrTemp;
    //释放掉申请的空间
    free(ImageBuffer);
    //关闭远程进程的句柄
    CloseHandle(hRemoteProcess);
    return 1;

}







