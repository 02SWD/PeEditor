// ButtonIatHook.cpp: implementation of the ButtonIatHook class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonIatHook.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
DWORD WINAPI ThreadIatHookProc(LPVOID lpParameter);
BOOL CALLBACK ButtonIatHookProc(HWND hwndDlgOfDLLInject, UINT uMsg, WPARAM wParam, LPARAM lParam);
int IatHook(IN DWORD pid, IN char* targetModuleName , IN char* targetFuncName, IN DWORD targetFuncOrdinal, IN HMODULE dllAddrInTargetProcess, IN char* nastyFuncName, IN DWORD nastyFuncOrdinal, OUT PDWORD oldFuncAddr);
int IatHook(IN DWORD pid, IN char* targetModuleName , IN char* targetFuncName, IN DWORD targetFuncOrdinal, IN DWORD FuncAddr, OUT PDWORD oldFuncAddr);

/**
 * Iat Hook 按钮的主处理代码
 * @return [description]
 */
VOID buttonIatHook(){
    //创建一个线程专门执行 IAT HOOK 的代码
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadIatHookProc, NULL, 0, NULL);
    //关闭线程句柄
    ::CloseHandle(hThread);
}

/**
 * Iat Hook 线程的回调函数
 * @param  lpParameter 线程回调函数的参数
 * @return             [description]
 */
DWORD WINAPI ThreadIatHookProc(LPVOID lpParameter){
    //创建一个对话框（这里没有传入父对话框的句柄，以避免子对话框影响父对话框的使用）
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_IATHOOK), NULL,  ButtonIatHookProc);
    return 0;
}

/**
 * IAT HOOK 对话框的窗口回调函数（该窗口ID：IDD_DIALOG_IATHOOK）
 * @param  hwndDlgOfIatHook IAT HOOK对话框的句柄
 * @param  uMsg             消息类型
 * @param  wParam           WPARAM
 * @param  lParam           LPARAM
 * @return                  
 */
BOOL CALLBACK ButtonIatHookProc(HWND hwndDlgOfIatHook, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //点击 窗口右上角的红叉 之后，退出窗口
        case WM_CLOSE:{
            EndDialog(hwndDlgOfIatHook, 0);
            return TRUE;
        }
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //第一种设置Iat Hook的方式
                case IDC_BUTTON_IatHook_start:{
                    //获取第一种Iathook所需的各个编辑框的句柄
                    HWND hEdit_pid = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_pid);
                    HWND hEdit_targetModuleName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetModuleName);
                    HWND hEdit_targetFuncName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncName);
                    HWND hEdit_targetFuncOrdinal = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncOrdinal);
                    HWND hEdit_dllAddrInTargetProcess = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_dllAddrInTargetProcess);
                    HWND hEdit_nastyFuncName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_nastyFuncName);
                    HWND hEdit_nastyFuncOrdinal = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_nastyFuncOrdinal);

                    //用于暂存编辑框中的内容
                    char pid_Buffer[1024] = {0};
                    char targetModuleName_Buffer[1024] = {0};
                    char targetFuncName_Buffer[1024] = {0};
                    char targetFuncOrdinal_Buffer[1024] = {0};
                    char dllAddrInTargetProcess_Buffer[1024] = {0};
                    char nastyFuncName_Buffer[1024] = {0};
                    char nastyFuncOrdinal_Buffer[1024] = {0};

                    //获取各个编辑框中的内容
                    GetWindowText(hEdit_pid, pid_Buffer, 1024);
                    GetWindowText(hEdit_targetModuleName, targetModuleName_Buffer, 1024);
                    GetWindowText(hEdit_targetFuncName, targetFuncName_Buffer, 1024);
                    GetWindowText(hEdit_targetFuncOrdinal, targetFuncOrdinal_Buffer, 1024);
                    GetWindowText(hEdit_dllAddrInTargetProcess, dllAddrInTargetProcess_Buffer, 1024);
                    GetWindowText(hEdit_nastyFuncName, nastyFuncName_Buffer, 1024);
                    GetWindowText(hEdit_nastyFuncOrdinal, nastyFuncOrdinal_Buffer, 1024);

                    //将pid字符串，以10进制的方式转化为数字
                    char* leftOvew_pid = NULL;
                    DWORD pid = strtoul(pid_Buffer, &leftOvew_pid, 10);

                    //将目标函数的导出序号字符串，以16进制的方式转化为数字
                    char* leftOvew_targetFuncOrdinal = NULL;
                    DWORD targetFuncOrdinal = strtoul(targetFuncOrdinal_Buffer, &leftOvew_targetFuncOrdinal, 16);

                    //将注入模块的首地址字符串，以16进制的方式转化为数字
                    char* leftOvew_dllAddrInTargetProcess = NULL;
                    DWORD dllAddrInTargetProcess = strtoul(dllAddrInTargetProcess_Buffer, &leftOvew_dllAddrInTargetProcess, 16);

                    //将 要替换为的函数的函数导出序号字符串，以16进制的方式转化为数字
                    char* leftOvew_nastyFuncOrdinal = NULL;
                    DWORD nastyFuncOrdinal = strtoul(nastyFuncOrdinal_Buffer, &leftOvew_nastyFuncOrdinal, 16);

                    //判断用户是否输入的目标进程的pid
                    if (pid == 0){
                        MessageBox(NULL, TEXT("请输入目标进程的pid!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }

                    //判断用户是否输入了待劫持的目标模块名称
                    if (*(int*)targetModuleName_Buffer == 0){
                        MessageBox(NULL, TEXT("请输入要劫持的目标模块名称!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }
                    
                    //判断用户是否输入了待劫持的目标函数的函数名称/导出序号
                    if (*(int*)targetFuncName_Buffer == 0 && targetFuncOrdinal == 0){
                        MessageBox(NULL, TEXT("请输入待被劫持的目标函数的 函数名称/函数导出序号!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }
                    
                    //待被劫持的目标函数的函数名与函数导出序号只能填写一个
                    if (*(int*)targetFuncName_Buffer != 0 && targetFuncOrdinal != 0){
                        MessageBox(NULL, TEXT("待被劫持的目标函数的函数名与函数导出序号只能填写一个!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }
                    
                    //判断用户是否输入了目标进程中dll模块的首地址
                    if (dllAddrInTargetProcess == 0){
                        MessageBox(NULL, TEXT("请输入目标进程中dll模块的首地址!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }

                    //判断用户是否输入了 要替换为的函数的函数名称/导出序号
                    if (*(int*)nastyFuncName_Buffer == 0 && nastyFuncOrdinal == 0){
                        MessageBox(NULL, TEXT("请输入要替换为的函数的 函数名称/函数导出序号!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }
                    
                    //要替换的函数的函数名与函数导出序号只能填写一个
                    if (*(int*)nastyFuncName_Buffer != 0 && nastyFuncOrdinal != 0){
                        MessageBox(NULL, TEXT("要替换的函数的函数名与函数导出序号只能填写一个!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }
                    //用于记录IatHook之前，IAT表表项的值
                    DWORD oldFuncAddr = 0;
                    //开始IAT HOOK
                    int iatHookFlag = IatHook(pid, targetModuleName_Buffer, targetFuncName_Buffer, targetFuncOrdinal, (HMODULE)dllAddrInTargetProcess, nastyFuncName_Buffer, nastyFuncOrdinal, &oldFuncAddr);
                    //判断IAT HOOK是否执行成功
                    if (iatHookFlag > 0){
                        //若执行成功，则将IAT表表项原来的值保存为全局变量，并将该值显示给用户
                        gOldFuncAddr = oldFuncAddr;
                        HWND hEdit_oldFuncAddr = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_oldFuncAddrValue);
                        char oldFuncAddrBuffer[10] = {0};
                        ultoa(oldFuncAddr, oldFuncAddrBuffer, 16);
                        SetWindowText(hEdit_oldFuncAddr, oldFuncAddrBuffer);
                        MessageBox(NULL, TEXT("IAT HOOK 成功!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }else{
                        MessageBox(NULL, TEXT("IAT HOOK 失败!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }
                }
                //第二种设置Iat Hook的方式
                case IDC_BUTTON_IatHook_start_method2:{
                    //获取第二种Iathook所需的各个编辑框的句柄
                    HWND hEdit_pid = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_pid_Method2);
                    HWND hEdit_targetModuleName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetModuleName_Method2);
                    HWND hEdit_targetFuncName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncName_Method2);
                    HWND hEdit_targetFuncOrdinal = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncOrdinal_Method2);
                    HWND hEdit_funcAddr = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncOrdinalMethod2);
                    
                    //用于暂存编辑框中的内容
                    char pid_Buffer[1024] = {0};
                    char targetModuleName_Buffer[1024] = {0};
                    char targetFuncName_Buffer[1024] = {0};
                    char targetFuncOrdinal_Buffer[1024] = {0};
                    char funcAddr_Buffer[1024] = {0};

                    //获取各个编辑框中的内容
                    GetWindowText(hEdit_pid, pid_Buffer, 1024);
                    GetWindowText(hEdit_targetModuleName, targetModuleName_Buffer, 1024);
                    GetWindowText(hEdit_targetFuncName, targetFuncName_Buffer, 1024);
                    GetWindowText(hEdit_targetFuncOrdinal, targetFuncOrdinal_Buffer, 1024);
                    GetWindowText(hEdit_funcAddr, funcAddr_Buffer, 1024);

                    //将pid字符串，以10进制的方式转化为数字
                    char* leftOvew_pid = NULL;
                    DWORD pid = strtoul(pid_Buffer, &leftOvew_pid, 10);

                    //将目标函数的导出序号字符串，以16进制的方式转化为数字
                    char* leftOvew_targetFuncOrdinal = NULL;
                    DWORD targetFuncOrdinal = strtoul(targetFuncOrdinal_Buffer, &leftOvew_targetFuncOrdinal, 16);

                    //将 要替换为的函数地址的字符串，以16进制的方式转化为数字
                    char* leftOvew_funcAddr = NULL;
                    DWORD funcAddr = strtoul(funcAddr_Buffer, &leftOvew_funcAddr, 16);

                    //判断用户是否输入的目标进程的pid
                    if (pid == 0){
                        MessageBox(NULL, TEXT("请输入目标进程的pid!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }

                    //判断用户是否输入了待劫持的目标模块名称
                    if (*(int*)targetModuleName_Buffer == 0){
                        MessageBox(NULL, TEXT("请输入要劫持的目标模块名称!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }
                    
                    //判断用户是否输入了待劫持的目标函数的函数名称/导出序号
                    if (*(int*)targetFuncName_Buffer == 0 && targetFuncOrdinal == 0){
                        MessageBox(NULL, TEXT("请输入待被劫持的目标函数的 函数名称/函数导出序号!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }
                    
                    //待被劫持的目标函数的函数名与函数导出序号只能填写一个
                    if (*(int*)targetFuncName_Buffer != 0 && targetFuncOrdinal != 0){
                        MessageBox(NULL, TEXT("待被劫持的目标函数的函数名与函数导出序号只能填写一个!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }

                    //判断用户是否输入了要替换为的函数地址
                    if (funcAddr == 0){
                        MessageBox(NULL, TEXT("请输入要替换为的函数地址!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }

                    //原来的IAT表表项值
                    DWORD oldFuncAddr = 0;
                    //开始进行IAT HOOK
                    int IatHookFlag = IatHook(pid, targetModuleName_Buffer, targetFuncName_Buffer, targetFuncOrdinal, funcAddr, &oldFuncAddr);
                    //判断IAT HOOK是否执行成功
                    if (IatHookFlag > 0){
                        //若执行成功，则将IAT表表项原来的值保存为全局变量，并将该值显示给用户
                        gOldFuncAddr = oldFuncAddr;
                        HWND hEdit_oldFuncAddr = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_oldFuncAddrValue_Method2);
                        char oldFuncAddrBuffer[10] = {0};
                        ultoa(oldFuncAddr, oldFuncAddrBuffer, 16);
                        SetWindowText(hEdit_oldFuncAddr, oldFuncAddrBuffer);
                        MessageBox(NULL, TEXT("IAT HOOK 成功!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }else{
                        MessageBox(NULL, TEXT("IAT HOOK 失败!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }
                }
            }

        }
    }

    return FALSE;
}


/**
 * 修改目标进程中的目标模块中的目标函数，在IAT表中所对应的函数地址。修改之后的函数地址，为目标进程中其他dll模块中的函数的函数地址（该dll模块一般为我们注入的dll模块）
 * @param  pid                    目标进程的pid
 * @param  targetModuleName       待被劫持的模块名称
 * @param  targetFuncName         targetModuleName模块中待被劫持的函数的函数名称（不区分大小写）
 * @param  targetFuncOrdinal      targetModuleName模块中待被劫持的函数的导出序号（十六进制、十进制均可）
 * @param  dllAddrInTargetProcess 目标进程中dll模块的首地址（该dll模块一般是我们注入的dll）
 * @param  nastyFuncName          要替换成的函数的函数名称（不区分大小写）
 * @param  nastyFuncOrdinal       要替换成的函数的导出序号（十六进制、十进制均可）
 * @param  oldFuncAddr            原来的函数地址（替换之前的IAT表表项值）
 * @return                        -1 远程进程打开失败
 *                                -2 远程进程内存读取失败
 *                                -3 远程进程内存读取失败
 *                                -4 远程进程内存写入失败
 *                                -5 远程进程内存写入失败
 *                                -6 目标进程的第一个模块中没有用户想要劫持的函数
 *                                -7 待劫持函数的函数名与导出序号不能同时输入
 *                                -8 要替换为的函数的函数名与导出序号不能同时输入
 *                                -9 IAT表表项原来的值读取失败
 *                                -10 IAT表表项原来的值读取失败
 *                                 1 IAT修改成功
 * 注意：
 *      1、targetFuncName参数 和 targetFuncOrdinal参数不能同时有值（即：其中一个必须为NULL）
 *      2、nastyFuncName参数 和 nastyFuncOrdinal参数也不能同时有值（即：其中一个必须为NULL）
 * 本程序作用：
 *      将目标进程中，targetModuleName模块中的targetFuncName/targetFuncOrdinal函数，将该函数所对应的IAT表项值(即目标函数的函数地址)，
 *      修改为dllAddrInTargetProcess模块中nastyFuncName/nastyFuncOrdinal函数的函数地址。
 */
int IatHook(IN DWORD pid, IN char* targetModuleName , IN char* targetFuncName, IN DWORD targetFuncOrdinal, IN HMODULE dllAddrInTargetProcess, IN char* nastyFuncName, IN DWORD nastyFuncOrdinal, OUT PDWORD oldFuncAddr){
    if(targetFuncName != NULL && targetFuncOrdinal != 0){
        printf("待劫持函数的函数名与导出序号不能同时输入！\n");
        return -7;
    }

    if(nastyFuncName != NULL && nastyFuncOrdinal != 0){
        printf("要替换为的函数的函数名与导出序号不能同时输入！\n");
        return -8;
    }

    //根据pid获取目标进程的句柄
    HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hRemoteProcess == NULL){
        printf("远程进程打开失败!\n");
        return -1;
    }
    //根据pid为目标进程拍摄快照
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    //用于暂存目标进程中的模块信息
    MODULEENTRY32 mem32 = {0};
    mem32.dwSize = sizeof(MODULEENTRY32);
    //获取目标进程中第一个模块的信息
    BOOL isNext2 = Module32First(hSnap, &mem32);
    DWORD a = GetLastError();
    //记录目标模块的首地址
    DWORD targetProcessOfModuleAddr = NULL;
    while (isNext2){
        if (stricmp(targetModuleName, mem32.szModule) == 0){
            targetProcessOfModuleAddr = (DWORD)mem32.hModule;
            break;
        }
        isNext2 = Module32Next(hSnap, &mem32);
    }
    //关闭快照句柄
    CloseHandle(hSnap);

    //接下来，我们要定位该目标模块的IAT表中，存放目标函数的函数地址的IAT表项的地址，然后将该iat表项的值修改为我们自己函数的函数地址
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
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, pDosHeaderSizeSpace, 64, NULL) == 0){
        printf("DOS头读取失败!\n");
        free(pDosHeaderSizeSpace);
        return -3;
    }
    //解析dos头
    PIMAGE_DOS_HEADER pDosHeaderTemp = (PIMAGE_DOS_HEADER)pDosHeaderSizeSpace;
    //sizeOfRubbishSpare记录垃圾数据所占空间的大小
    DWORD sizeOfRubbishSpare = pDosHeaderTemp->e_lfanew - 0x40;
    //已经计算出了垃圾数据所占空间大小，可以释放掉dos头数据所占的内存了
    free(pDosHeaderSizeSpace);

    // sizeOfBeforeSection = dos头大小 + 垃圾数据大小 + pe标志大小 + 标准pe头大小 + 可选pe头大小
    DWORD sizeOfBeforeSection = 64 + sizeOfRubbishSpare + 4 + 20 + 224;
    char* partExeImageBuffer = (char*)malloc(sizeOfBeforeSection);
    //初始化内存空间
    memset(partExeImageBuffer, 0, sizeOfBeforeSection);
    //将目标模块的 dos头、nt头、标准pe头 和 可选pe头 读取到内存中
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, partExeImageBuffer, sizeOfBeforeSection, NULL) == 0){
        printf("远程进程内存读取失败!\n");
        free(partExeImageBuffer);
        CloseHandle(hRemoteProcess);
        return -2;
    }
    
    //解析目标模块的 dos头、nt头、标准pe头 和 可选pe头
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)partExeImageBuffer;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //根据可选pe头来获取目标模块的ImageBuffer的大小
    DWORD targetProcessOfModuleSizeOfImage = pOptionalHeader->SizeOfImage;
    //因为我们已经得知目标模块ImageBuffer的大小，所以partImageBuffer内存可以被释放了
    free(partExeImageBuffer);

    //申请足够容纳 目标模块ImageBuffer大小 的内存空间
    char* ImageBuffer = (char*)malloc(targetProcessOfModuleSizeOfImage);
    memset(ImageBuffer, 0, targetProcessOfModuleSizeOfImage);
    //将目标模块读取到新申请的内存空间中
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, ImageBuffer, targetProcessOfModuleSizeOfImage, NULL) == 0){
        printf("远程进程内存读取失败!\n");
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -3;
    }
    //重新解析 dos头、nt头、标准pe头 和 可选pe头
    pDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;
    pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //定位第一个导入表的位置
    DWORD importTableRva = pOptionalHeader->DataDirectory[1].VirtualAddress;
    PIMAGE_IMPORT_DESCRIPTOR importTableAddr = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)ImageBuffer + importTableRva);
    BOOL flag = FALSE, isSuccess = FALSE;
    while (importTableAddr->Name != 0){
        //令 IatTableAddr 指针指向iat表的首地址
        int* IatTableAddr = (int*)((DWORD)ImageBuffer + importTableAddr->FirstThunk);
        //令 IntTableAddr 指针指向int表的首地址
        int* IntTableAddr = (int*)((DWORD)ImageBuffer + importTableAddr->OriginalFirstThunk);
        
        //循环遍历INT表，并修改目标函数所对应的iat表项的值
        while (*IntTableAddr != 0){
            //若 该INT表项值的最高位为1，且用户传入的是目标函数的导出序号，则根据导出序号进行修改
            if ((*IntTableAddr & 0x80000000) == 0x80000000 && targetFuncOrdinal != 0){
                //判断此时INT表项后15位的值是否等于用户传入的导出序号（即：判断当前的函数是否是用户想要劫持的函数）
                if ((DWORD)(*IntTableAddr & 0x7fffffff) == targetFuncOrdinal){
                    //记录修改之前的IAT表表项值
                    DWORD oldFuncAddrTemp = 0;
                    if(0 == ReadProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &oldFuncAddrTemp, 4, NULL)){
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -9;
                    }
                    //将该处IAT表表项原来的值传出
                    *oldFuncAddr = oldFuncAddrTemp;

                    //获取要替换成的函数的函数地址
                    DWORD myFunc = 0;
                    myGetProcAddressCrossProcess(pid, (HMODULE)dllAddrInTargetProcess, NULL, nastyFuncOrdinal, &myFunc);
                    //修改 目标进程 第一个模块的IAT表中，目标函数所对应的IAT表项，将该IAT表项的值修改为我们要替换成的函数的函数地址
                    if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &myFunc, 4, NULL)){
                        printf("远程进程内存写入失败!\n");
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -4;
                    }
                    flag = TRUE;
                    break;
                }
            }
            //若 该INT表项的最高位不为1，且用户传入的是目标函数的函数名，则根据函数名进行修改
            if ((*IntTableAddr & 0x80000000) != 0x80000000 && targetFuncName != 0){
                PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)ImageBuffer + *IntTableAddr);
                //判断当前的函数是否是用户想要劫持的函数
                if (stricmp((char*)(funcName->Name),targetFuncName) == 0){
                    //记录修改之前的IAT表表项值
                    DWORD oldFuncAddrTemp = 0;
                    if(0 == ReadProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &oldFuncAddrTemp, 4, NULL)){
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -10;
                    }
                    //将该处IAT表表项原来的值传出
                    *oldFuncAddr = oldFuncAddrTemp;

                    //获取要替换成的函数的函数地址
                    DWORD myFunc = 0;
                    myGetProcAddressCrossProcess(pid, (HMODULE)dllAddrInTargetProcess, nastyFuncName, NULL, &myFunc);
                    //修改 目标进程 第一个模块的IAT表中，目标函数所对应的IAT表项，将该IAT表项的值修改为我们要替换成的函数的函数地址
                    if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &myFunc, 4, NULL)){
                        printf("远程进程内存写入失败!\n");
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -5;
                    }
                    flag = TRUE;
                    break;
                }
            }
            //IatTableAddr指针指向下一个iat表表项
            IatTableAddr++;
            //IntTableAddr指针指向下一个int表表项
            IntTableAddr++;
        }
        //判断本次的IAT表中是否具有用户想要劫持的函数，且是否劫持成功。（如果不为TRUE，说明本次的IAT表并没有用户想要劫持的函数）
        if (flag == TRUE){
            //若成功修改，则将isSuccess置为TRUE，并跳出循环
            isSuccess = TRUE;
            break;
        }
        //令importTableAddr指针指向下一个导入表
        importTableAddr++;
    }
    //如果为FALSE，说明目标进程的第一个模块中没有用户想要劫持的函数
    if (isSuccess == FALSE){
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -6;
    }
    //释放资源
    free(ImageBuffer);
    CloseHandle(hRemoteProcess);
    return 1;
}

/**
 * 修改目标进程中的目标模块中的目标函数，在IAT表中所对应的函数地址，为我们自己传入的函数地址
 * @param  pid               目标进程的pid
 * @param  targetModuleName  待被劫持的模块名称
 * @param  targetFuncName    targetModuleName模块中待被劫持的函数的函数名称（不区分大小写）
 * @param  targetFuncOrdinal targetModuleName模块中待被劫持的函数的导出序号（十六进制、十进制均可）
 * @param  FuncAddr          要修改为的函数地址
 * @param  oldFuncAddr       原来的函数地址（替换之前的IAT表表项值）
 * @return                   -1 远程进程打开失败
 *                           -2 远程进程内存读取失败
 *                           -3 远程进程内存读取失败
 *                           -4 远程进程内存写入失败
 *                           -5 远程进程内存写入失败
 *                           -6 目标进程的第一个模块中没有用户想要劫持的函数
 *                           -7 待劫持函数的函数名与导出序号不能同时输入
 *                           -8 要替换为的函数的函数名与导出序号不能同时输入
 *                           -9 IAT表表项原来的值读取失败
 *                           -10 IAT表表项原来的值读取失败
 *                            1 IAT修改成功
 * 注意：
 *      1、targetFuncName参数 和 targetFuncOrdinal参数不能同时有值（即：其中一个必须为NULL）
 */
int IatHook(IN DWORD pid, IN char* targetModuleName , IN char* targetFuncName, IN DWORD targetFuncOrdinal, IN DWORD FuncAddr, OUT PDWORD oldFuncAddr){
    if(targetFuncName != NULL && targetFuncOrdinal != 0){
        printf("待劫持函数的函数名与导出序号不能同时输入！\n");
        return -7;
    }

    //根据pid获取目标进程的句柄
    HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hRemoteProcess == NULL){
        printf("远程进程打开失败!\n");
        return -1;
    }
    //根据pid为目标进程拍摄快照
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    //用于暂存目标进程中的模块信息
    MODULEENTRY32 mem32 = {0};
    mem32.dwSize = sizeof(MODULEENTRY32);
    //获取目标进程中第一个模块的信息
    BOOL isNext2 = Module32First(hSnap, &mem32);
    DWORD a = GetLastError();
    //记录目标模块的首地址
    DWORD targetProcessOfModuleAddr = NULL;
    while (isNext2){
        if (stricmp(targetModuleName, mem32.szModule) == 0){
            targetProcessOfModuleAddr = (DWORD)mem32.hModule;
            break;
        }
        isNext2 = Module32Next(hSnap, &mem32);
    }
    //关闭快照句柄
    CloseHandle(hSnap);

    //接下来，我们要定位该目标模块的IAT表中，存放目标函数的函数地址的IAT表项的地址，然后将该iat表项的值修改为我们自己函数的函数地址
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
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, pDosHeaderSizeSpace, 64, NULL) == 0){
        printf("DOS头读取失败!\n");
        free(pDosHeaderSizeSpace);
        return -3;
    }
    //解析dos头
    PIMAGE_DOS_HEADER pDosHeaderTemp = (PIMAGE_DOS_HEADER)pDosHeaderSizeSpace;
    //sizeOfRubbishSpare记录垃圾数据所占空间的大小
    DWORD sizeOfRubbishSpare = pDosHeaderTemp->e_lfanew - 0x40;
    //已经计算出了垃圾数据所占空间大小，可以释放掉dos头数据所占的内存了
    free(pDosHeaderSizeSpace);

    // sizeOfBeforeSection = dos头大小 + 垃圾数据大小 + pe标志大小 + 标准pe头大小 + 可选pe头大小
    DWORD sizeOfBeforeSection = 64 + sizeOfRubbishSpare + 4 + 20 + 224;
    char* partExeImageBuffer = (char*)malloc(sizeOfBeforeSection);
    //初始化内存空间
    memset(partExeImageBuffer, 0, sizeOfBeforeSection);
    //将目标模块的 dos头、nt头、标准pe头 和 可选pe头 读取到内存中
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, partExeImageBuffer, sizeOfBeforeSection, NULL) == 0){
        printf("远程进程内存读取失败!\n");
        free(partExeImageBuffer);
        CloseHandle(hRemoteProcess);
        return -2;
    }
    
    //解析目标模块的 dos头、nt头、标准pe头 和 可选pe头
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)partExeImageBuffer;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //根据可选pe头来获取目标模块的ImageBuffer的大小
    DWORD targetProcessOfModuleSizeOfImage = pOptionalHeader->SizeOfImage;
    //因为我们已经得知目标模块ImageBuffer的大小，所以partImageBuffer内存可以被释放了
    free(partExeImageBuffer);

    //申请足够容纳 目标模块ImageBuffer大小 的内存空间
    char* ImageBuffer = (char*)malloc(targetProcessOfModuleSizeOfImage);
    memset(ImageBuffer, 0, targetProcessOfModuleSizeOfImage);
    //将目标模块读取到新申请的内存空间中
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, ImageBuffer, targetProcessOfModuleSizeOfImage, NULL) == 0){
        printf("远程进程内存读取失败!\n");
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -3;
    }
    //重新解析 dos头、nt头、标准pe头 和 可选pe头
    pDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;
    pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //定位第一个导入表的位置
    DWORD importTableRva = pOptionalHeader->DataDirectory[1].VirtualAddress;
    PIMAGE_IMPORT_DESCRIPTOR importTableAddr = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)ImageBuffer + importTableRva);
    BOOL flag = FALSE, isSuccess = FALSE;
    while (importTableAddr->Name != 0){
        //令 IatTableAddr 指针指向iat表的首地址
        int* IatTableAddr = (int*)((DWORD)ImageBuffer + importTableAddr->FirstThunk);
        //令 IntTableAddr 指针指向int表的首地址
        int* IntTableAddr = (int*)((DWORD)ImageBuffer + importTableAddr->OriginalFirstThunk);
        
        //循环遍历INT表，并修改目标函数所对应的iat表项的值
        while (*IntTableAddr != 0){
            //若 该INT表项值的最高位为1，且用户传入的是目标函数的导出序号，则根据导出序号进行修改
            if ((*IntTableAddr & 0x80000000) == 0x80000000 && targetFuncOrdinal != 0){
                //判断此时INT表项后15位的值是否等于用户传入的导出序号（即：判断当前的函数是否是用户想要劫持的函数）
                if ((DWORD)(*IntTableAddr & 0x7fffffff) == targetFuncOrdinal){
                    //记录修改之前的IAT表表项值
                    DWORD oldFuncAddrTemp = 0;
                    if(0 == ReadProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &oldFuncAddrTemp, 4, NULL)){
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -9;
                    }
                    //将该处IAT表表项原来的值传出
                    *oldFuncAddr = oldFuncAddrTemp;

                    //获取要替换成的函数的函数地址
                    DWORD myFunc = FuncAddr;
                    //修改 目标进程 第一个模块的IAT表中，目标函数所对应的IAT表项，将该IAT表项的值修改为我们要替换成的函数的函数地址
                    if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &myFunc, 4, NULL)){
                        printf("远程进程内存写入失败!\n");
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -4;
                    }
                    flag = TRUE;
                    break;
                }
            }
            //若 该INT表项的最高位不为1，且用户传入的是目标函数的函数名，则根据函数名进行修改
            if ((*IntTableAddr & 0x80000000) != 0x80000000 && targetFuncName != 0){
                PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)ImageBuffer + *IntTableAddr);
                //判断当前的函数是否是用户想要劫持的函数
                if (stricmp((char*)(funcName->Name),targetFuncName) == 0){
                    //记录修改之前的IAT表表项值
                    DWORD oldFuncAddrTemp = 0;
                    if(0 == ReadProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &oldFuncAddrTemp, 4, NULL)){
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -10;
                    }
                    //将该处IAT表表项原来的值传出
                    *oldFuncAddr = oldFuncAddrTemp;

                    //获取要替换成的函数的函数地址
                    DWORD myFunc = FuncAddr;
                    //修改 目标进程 第一个模块的IAT表中，目标函数所对应的IAT表项，将该IAT表项的值修改为我们要替换成的函数的函数地址
                    if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &myFunc, 4, NULL)){
                        printf("远程进程内存写入失败!\n");
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -5;
                    }
                    flag = TRUE;
                    break;
                }
            }
            //IatTableAddr指针指向下一个iat表表项
            IatTableAddr++;
            //IntTableAddr指针指向下一个int表表项
            IntTableAddr++;
        }
        //判断本次的IAT表中是否具有用户想要劫持的函数，且是否劫持成功。（如果不为TRUE，说明本次的IAT表并没有用户想要劫持的函数）
        if (flag == TRUE){
            //若成功修改，则将isSuccess置为TRUE，并跳出循环
            isSuccess = TRUE;
            break;
        }
        //令importTableAddr指针指向下一个导入表
        importTableAddr++;
    }
    //如果为FALSE，说明目标进程的第一个模块中没有用户想要劫持的函数
    if (isSuccess == FALSE){
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -6;
    }
    //释放资源
    free(ImageBuffer);
    CloseHandle(hRemoteProcess);
    return 1;
}