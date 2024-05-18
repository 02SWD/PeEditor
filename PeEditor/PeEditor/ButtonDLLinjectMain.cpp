// ButtonDLLinjectMain.cpp: implementation of the ButtonDLLinjectMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonDLLinjectMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//函数声明
DWORD WINAPI ThreadDLLInjectProc(LPVOID lpParameter);
BOOL CALLBACK ButtonDLLInjectMainProc(HWND hwndDlgOfDLLInject, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * 注入的主处理代码
 */
VOID buttonDLLInjectMain(){
    //创建一个线程专门处理注入程序的运行
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadDLLInjectProc, NULL, 0, NULL);
    //关闭线程句柄
    ::CloseHandle(hThread);
}

/**
 * 注入线程的回调函数
 * @param  lpParameter 线程函数的参数
 * @return             
 */
DWORD WINAPI ThreadDLLInjectProc(LPVOID lpParameter){
    //创建一个对话框（这里没有传入父对话框的句柄，以避免子对话框影响父对话框的使用）
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_DLLInjection), NULL,  ButtonDLLInjectMainProc);
    return 0;
}

/**
 * DLL Inject 窗口的窗口回调函数（该窗口的id：IDD_DIALOG_DLLInjection）
 * @param  hwndDlgOfDLLInject DLL Inject对话框的句柄
 * @param  uMsg               消息类型
 * @param  wParam             WPARAM
 * @param  lParam             LPARAM
 * @return                    
 */
BOOL CALLBACK ButtonDLLInjectMainProc(HWND hwndDlgOfDLLInject, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
				//挑选待注入的DLL模块，并将该DLL模块的路径写入到编辑框中
				case IDC_BUTTON_selectDll:{
					//指定可以选中的扩展名
					TCHAR szPeFileExt[100] = "*.exe;*.dll;*.scr;*.drv;*.sys";
					//用于存放文件路径的缓冲区
					TCHAR szDllFileName[MAX_PATH] = {0};
					OPENFILENAME stOpenFile;
					memset(&stOpenFile, 0, sizeof(OPENFILENAME));
					stOpenFile.lStructSize = sizeof(OPENFILENAME);
					stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
					stOpenFile.hwndOwner = hwndDlgOfDLLInject;
					stOpenFile.lpstrFilter = szPeFileExt;
					stOpenFile.lpstrFile = szDllFileName;
					stOpenFile.nMaxFile = MAX_PATH;
					//弹出文件选择框，并将所选中的文件信息封装到stOpenFile中
					BOOL flag = GetOpenFileName(&stOpenFile);
					//判断用户是否选中了PE文件，若选中的话, 则将该DLL模块的路径写入到编辑框中
					if (flag){
						HWND hEdit_DllPath = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_DllPath);
						SetWindowText(hEdit_DllPath, szDllFileName);
					}
					return TRUE;
				}
                //当点击 “注入” 按钮之后，要执行的代码
                case IDC_BUTTON_startInject:{
                    //获取 各个单选框按钮、pid编辑框 和 dll编辑框 的句柄
                    HWND hLoadLibraryRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_LoadLibrary);
                    HWND hMemoryRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_memoryWrite);
                    HWND hProcessRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_loadProcess);
                    HWND hTypeWritingRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_typewriting);
                    HWND hMemoryOtherRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_WriteMemoryWithoutRemoteThread);
                    HWND hEditPid = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_PID_dll);
                    HWND hEditDllPath = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_DllPath);
                    
                    //获取各个单选框的状态
                    int statusLoadLibrary = SendMessage(hLoadLibraryRadio, BM_GETCHECK, 0, 0);
                    int statusMemory = SendMessage(hMemoryRadio, BM_GETCHECK, 0, 0);
                    int statusProcess = SendMessage(hProcessRadio, BM_GETCHECK, 0, 0);
                    int statusTypeWriting = SendMessage(hTypeWritingRadio, BM_GETCHECK, 0, 0);
                    int statusMemoryOther = SendMessage(hMemoryOtherRadio, BM_GETCHECK, 0, 0);
                    
                    
                    //获取pid编辑框中的值，并将其转换为数字
                    char szBuffer[MAX_PATH] = {0};
                    memset(szBuffer, 0, MAX_PATH);
                    GetWindowText(hEditPid, szBuffer, MAX_PATH);
                    char* leftOvew = NULL;
                    DWORD pid = strtoul(szBuffer, &leftOvew, 10);;
                    
                    //获取dll编辑框中的值
                    memset(szBuffer, 0, MAX_PATH);
                    GetWindowText(hEditDllPath, szBuffer, MAX_PATH);

                    //判断用户是否在编辑框中输入了值，若没有则提示用户输入
                    if (pid <= 0){
                        MessageBox(NULL, TEXT("请输入正确的目标进程PID!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }

                    //判断用户是否选择了注入的方式，若没有选择，则提示用户
                    //DbgPrintf("statusLoadLibrary %d, statusMemory %d, statusProcess %d, statusTypeWriting %d\n", statusLoadLibrary, statusMemory, statusProcess, statusTypeWriting);
                    if (statusLoadLibrary == NULL && statusMemory == NULL && statusProcess == NULL && statusTypeWriting == NULL && statusMemoryOther == NULL){
                        MessageBox(NULL, TEXT("请选择注入的方式!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }

                    //如果hLoadLibraryRadio单选框处于选中状态，说明用户将采用远程线程注入的方式
                    if (statusLoadLibrary == BST_CHECKED){
                        //判断用户是否在编辑框中输入了值，若没有则提示用户输入
                        if (*(int*)szBuffer == 0){
                            MessageBox(NULL, TEXT("请输入DLL路径!"), TEXT("警告"), MB_OK);
                            return TRUE;
                        }
                        int flag = RemoteThreadInjectionModule(pid, szBuffer);
                        if (flag > 0){
                            MessageBox(NULL, TEXT("远程线程注入成功!"), TEXT("警告"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("远程线程注入失败!"), TEXT("警告"), MB_OK);
                        }
                    //如果hMemoryRadio单选框处于选中状态，说明用户将采用远程线程注入的方式
                    }else if (statusMemory == BST_CHECKED){
                        int flag = 0;
                        gInjectByWriteMemory = RemoteDllInjectMemoryWrite(pid, &flag);
                        if (flag > 0){
                            MessageBox(NULL, TEXT("远程线程内存写入成功!"), TEXT("警告"), MB_OK);
                            MessageBox(NULL, TEXT("如果您还需卸载注入模块，请不要关闭\n本工具之后再点击卸载，否则您只能以终\n止目标进程的方式来卸载注入模块!"), TEXT("警告"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("远程线程内存写入失败!"), TEXT("警告"), MB_OK);
                        }
                    }else if (statusProcess == BST_CHECKED){

                    }else if (statusTypeWriting == BST_CHECKED){

                    //如果hMemoryOtherRadio单选框处于选中状态，说明用户将采用内存写入的方式（不使用LoadLibrary() 和 且在自身进程中修复IAT表表项值）
                    }else if (statusMemoryOther == BST_CHECKED){
                        //判断用户是否在编辑框中输入了值，若没有则提示用户输入
                        if (*(int*)szBuffer == 0){
                            MessageBox(NULL, TEXT("请输入DLL路径!"), TEXT("警告"), MB_OK);
                            return TRUE;
                        }
                        //用于记录注入模块在目标进程中的首地址
                        DWORD dllAddrInTargetProcess = 0;
                        //用于记录注入模块的SizeOfImage
                        DWORD dllSizeOfImage = 0;
                        //将注入模块注入到目标进程空间中
                        int flag = injectDllByWriteMemoryWithoutRemoteThread(szBuffer, pid, &dllAddrInTargetProcess, &dllSizeOfImage);
                        //将注入模块的首地址保存为全局变量
                        gDllAddrInTargetProcess = dllAddrInTargetProcess;
                        //将注入模块的SizeOfImage保存为全局变量
                        gDllSizeOfImage = dllSizeOfImage;
                        //判断模块是否注入成功
                        if (flag > 0){
                            //注入成功的话，就将DLL模块在目标进程中的首地址和DLL模块的SizeOfImage显示出来
                            char str[1024] = {0};
                            sprintf(str, "注入的DLL在目标进程中的首地址：0x%x \n注入的DLL的SizeOfImage：0x%x", dllAddrInTargetProcess, dllSizeOfImage);
                            MessageBox(NULL, TEXT("内存写入成功!"), TEXT("警告"), MB_OK);
                            MessageBox(NULL, str, TEXT("警告"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("内存写入失败!"), TEXT("警告"), MB_OK);
                        }
                    }
                    
                    return TRUE;
                }
                //当点击 “卸载” 按钮之后，要执行的代码
                case IDC_BUTTON_startUninstall:{
                    //获取 各个单选框按钮、pid编辑框 和 dll编辑框 的句柄
                    HWND hLoadLibraryRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_LoadLibrary);
                    HWND hMemoryRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_memoryWrite);
                    HWND hProcessRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_loadProcess);
                    HWND hTypeWritingRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_typewriting);
                    HWND hMemoryOtherRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_WriteMemoryWithoutRemoteThread);
                    HWND hEditPid = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_PID_dll);
                    HWND hEditDllPath = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_DllPath);
                    int statusMemoryOther = SendMessage(hMemoryOtherRadio, BM_GETCHECK, 0, 0);
                    
                    //获取各个单选框的状态
                    int statusLoadLibrary = SendMessage(hLoadLibraryRadio, BM_GETCHECK, 0, 0);
                    int statusMemory = SendMessage(hMemoryRadio, BM_GETCHECK, 0, 0);
                    int statusProcess = SendMessage(hProcessRadio, BM_GETCHECK, 0, 0);
                    int statusTypeWriting = SendMessage(hTypeWritingRadio, BM_GETCHECK, 0, 0);
                    
                    //获取pid编辑框中的值，并将其转换为数字
                    char szBuffer[MAX_PATH] = {0};
                    memset(szBuffer, 0, MAX_PATH);
                    GetWindowText(hEditPid, szBuffer, MAX_PATH);
                    char* leftOvew = NULL;
                    DWORD pid = strtoul(szBuffer, &leftOvew, 10);;
                    
                    //获取dll编辑框中的值
                    memset(szBuffer, 0, MAX_PATH);
                    GetWindowText(hEditDllPath, szBuffer, MAX_PATH);
                    
                    //判断用户是否在编辑框中输入了值，若没有则提示用户输入
                    if (pid <= 0){
                        MessageBox(NULL, TEXT("请输入正确的目标进程PID!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }

                    //判断用户是否选择了注入的方式，若没有选择，则提示用户
                    //DbgPrintf("statusLoadLibrary %d, statusMemory %d, statusProcess %d, statusTypeWriting %d\n", statusLoadLibrary, statusMemory, statusProcess, statusTypeWriting);
                    if (statusLoadLibrary == NULL && statusMemory == NULL && statusProcess == NULL && statusTypeWriting == NULL && statusMemoryOther == NULL){
                        MessageBox(NULL, TEXT("请选择卸载的方式!"), TEXT("警告"), MB_OK);
                        return TRUE;
                    }

                    //如果hLoadLibraryRadio单选框处于选中状态，说明用户将采用远程线程注入的方式
                    if (statusLoadLibrary == BST_CHECKED){
                        //判断用户是否在编辑框中输入了值，若没有则提示用户输入
                        if (*(int*)szBuffer == 0){
                            MessageBox(NULL, TEXT("请输入待卸载的DLL名称!"), TEXT("警告"), MB_OK);
                            return TRUE;
                        }
                        DWORD dwHModule =  RemoteThreadUninstallModule(pid, szBuffer);
                        if (dwHModule == 1){
                            MessageBox(NULL, TEXT("卸载成功!"), TEXT("警告"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("卸载失败!\n注意：在卸载时，只需输入模块的名称即可，不能输入路径!"), TEXT("警告"), MB_OK);
                        }
                    //如果hMemoryRadio单选框处于选中状态，说明用户将采用远程线程注入的方式
                    }else if (statusMemory == BST_CHECKED){
                        //开始卸载注入模块
                        if (gInjectByWriteMemory.FlagOfAddr <= 0 || gInjectByWriteMemory.HRemoteThread <= 0 || gInjectByWriteMemory.ModuleOfAddr <= 0 || gInjectByWriteMemory.ModuleSizeOfImage <= 0){
                            MessageBox(NULL, TEXT("您还未进行内存写入!"), TEXT("警告"), MB_OK);
                            return TRUE;
                        }
                        int flag = RemoteUninstallModule(pid, gInjectByWriteMemory);
                        if (flag > 0){
                            MessageBox(NULL, TEXT("内存写入卸载成功!"), TEXT("警告"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("内存写入卸载失败!"), TEXT("警告"), MB_OK);
                        }
                    }else if (statusProcess == BST_CHECKED){

                    }else if (statusTypeWriting == BST_CHECKED){

                    }else if (statusMemoryOther == BST_CHECKED){
                        MessageBox(NULL, TEXT("暂不支持卸载!"), TEXT("警告"), MB_OK);
                    }

                    return TRUE;
                }
                //点击 “退出” 按钮之后，退出窗口
                case IDC_BUTTON_exitOfDll:{
                    EndDialog(hwndDlgOfDLLInject, 0);
                    return TRUE;
                }
            }
            break;
        }
        case WM_CLOSE:{
            EndDialog(hwndDlgOfDLLInject, 0);
            return TRUE;
        }
        
    }
    return FALSE;
}







