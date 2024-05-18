// ButtonDriverNormal.cpp: implementation of the ButtonDriverNormal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonDriverNormal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
DWORD WINAPI ThreadDriverLoadNormalProc(LPVOID lpParameter);
BOOL CALLBACK ButtonDriverLoadNormalProc(HWND hwndDlgOfDriverMain, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * 以正常地方式加载驱动的处理代码
 * @return 
 */
VOID buttonDriverLoadNormal(){
    //创建一个线程专门用于处理 以正常方式加载驱动 的代码
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadDriverLoadNormalProc, NULL, 0, NULL);
    //关闭线程句柄
    ::CloseHandle(hThread);
}

/**
 * 线程回调函数
 * @param  lpParameter 回调函数的参数
 * @return             
 */
DWORD WINAPI ThreadDriverLoadNormalProc(LPVOID lpParameter){
    //创建一个对话框（这里没有传入父对话框的句柄，以避免子对话框影响父对话框的使用）
    DialogBox(ghInstance, MAKEINTRESOURCEA(IDD_DIALOG_DriverLoadNormal), NULL, ButtonDriverLoadNormalProc);
    return 0;
}

/**
 * DriverLoadNormal 窗口的窗口回调函数（该窗口ID：IDD_DIALOG_DriverLoadNormal）
 * @param  hwndDlgOfDriverMain DriverLoadNormal 窗口的句柄
 * @param  uMsg                消息类型
 * @param  wParam              wParam
 * @param  lParam              lParam
 * @return                     
 */
BOOL CALLBACK ButtonDriverLoadNormalProc(HWND hwndDlgOfDriverLoadNormalMain, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //用于封装选择窗口所选中的文件的信息
    OPENFILENAME stOpenFile;
    switch(uMsg) {
        //点击窗口右上角的红叉时，关闭窗口
        case WM_CLOSE:{
            EndDialog(hwndDlgOfDriverLoadNormalMain, 0);
            break;
        }
        //编写 按钮消息 的处理代码
        case WM_COMMAND:{
            //LOWORD(wParam)为按钮ID
            switch(LOWORD(wParam)) {
                //编写 “选择驱动文件按钮” 的处理代码
                case IDC_BUTTON_SelectDriver:{
                    //在驱动程序未被UnRegister之前，禁止指定新的驱动程序
                    if (g_scManager != NULL){
                        MessageBox(NULL, TEXT("当前驱动还未UnRegister"), TEXT("警告"), MB_OK);
                        break;
                    }

                    TCHAR szDriverExt[100] = ".sys";
                    TCHAR szFileName[MAX_PATH];
                    memset(szFileName, 0, MAX_PATH);
                    memset(&stOpenFile, 0, sizeof(OPENFILENAME));
                    stOpenFile.lStructSize = sizeof(OPENFILENAME);
                    stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                    stOpenFile.hwndOwner = hwndDlgOfDriverLoadNormalMain;
                    stOpenFile.lpstrFilter = szDriverExt;
                    stOpenFile.lpstrFile = szFileName;
                    stOpenFile.nMaxFile = MAX_PATH;

                    BOOL flag = GetOpenFileName(&stOpenFile);
                    if (flag){
                        //向编辑框中写入驱动程序的绝对路径
                        HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                        SetWindowText(hEdit_DriverPath, szFileName);
                    }
                    break;
                }
                //编写 “Register按钮” 的处理代码
                case IDC_BUTTON_Register:{
                    //用于存储驱动的绝对路径
                    char driverPathBuffer[MAX_PATH] = {0};
                    //用于存储驱动名称
                    char* driverName;
                    
                    //获取编辑框中的驱动绝对路径
                    HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                    GetWindowText(hEdit_DriverPath, driverPathBuffer, MAX_PATH);
                    if (*(DWORD*)driverPathBuffer == 0){
                        MessageBox(NULL, TEXT("请选择要加载的驱动路径"), TEXT("警告"), MB_OK);
                        break;
                    }

                    //从绝对路径中，分割出驱动的名称
                    char s[] = "\\";
                    char DriverPathBufferBak[MAX_PATH] = {0};
                    memcpy(DriverPathBufferBak, driverPathBuffer, MAX_PATH);
                    char* token = strtok(DriverPathBufferBak, s);
                    while (token != NULL){
                        driverName = token;
                        token = strtok(NULL, s);
                    }

                    //打开SCM管理器
                    g_scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
                    if (g_scManager == NULL){
                        MessageBox(NULL, TEXT("服务管理器打开失败"), TEXT("警告"), MB_OK);
                        break;
                    }
                    
                    //打开驱动所对应的服务
                    SC_HANDLE serviceHandle = CreateService(
                        g_scManager,                //SCM管理器的句柄
                        driverName,                 //驱动程序在注册表中的名字
                        driverName,                 //注册表驱动程序的DisplayName值
                        SERVICE_ALL_ACCESS,         //加载驱动程序的访问权限
                        SERVICE_KERNEL_DRIVER,      //表示加载的服务是kernel驱动程序
                        SERVICE_DEMAND_START,       //注册表驱动程序的Start值
                        SERVICE_ERROR_NORMAL,       //注册表驱动程序的ErrorControl值
                        driverPathBuffer,           //注册表驱动程序的ImagePath值
                        NULL, 
                        NULL, 
                        NULL, 
                        NULL ,
                        NULL);
                    if (serviceHandle == NULL){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_EXISTS){
                            MessageBox(NULL, TEXT("服务已经存在不需要创建了"), TEXT("警告"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("错误代码"), MB_OK);
                        }
                        break;
                    }

                    //关闭服务句柄
                    CloseServiceHandle(serviceHandle);
                    MessageBox(NULL, TEXT("驱动 注册 成功"), TEXT("警告"), MB_OK);
                    break;
                }
                //编写 “Run按钮” 的处理代码
                case IDC_BUTTON_Run:{
                    //用于存储驱动的绝对路径
                    char driverPathBuffer[MAX_PATH] = {0};
                    //用于存储驱动名称
                    char* driverName;
                    
                    //获取编辑框中的驱动绝对路径
                    HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                    GetWindowText(hEdit_DriverPath, driverPathBuffer, MAX_PATH);
                    if (*(DWORD*)driverPathBuffer == 0){
                        MessageBox(NULL, TEXT("请选择要加载的驱动路径"), TEXT("警告"), MB_OK);
                        break;
                    }

                    //从绝对路径中，分割出驱动的名称
                    char s[] = "\\";
                    char DriverPathBufferBak[MAX_PATH] = {0};
                    memcpy(DriverPathBufferBak, driverPathBuffer, MAX_PATH);
                    char* token = strtok(DriverPathBufferBak, s);
                    while (token != NULL){
                        driverName = token;
                        token = strtok(NULL, s);
                    }

                    if (g_scManager == NULL){
                        MessageBox(NULL, TEXT("请重新启动，服务打开失败"), TEXT("警告"), MB_OK);
                        break;
                    }

                    //驱动程序已经加载，只需要打开  
                    SC_HANDLE serviceHandle = OpenService(g_scManager, driverName, SERVICE_ALL_ACCESS);
                    if (serviceHandle == NULL){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_DOES_NOT_EXIST){
                            MessageBox(NULL, TEXT("服务已经不存在"), TEXT("警告"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("错误代码"), MB_OK);
                        }
                        break;
                    }

                    //开启此项服务
                    int result = StartService(serviceHandle, 0, NULL);
                    if (result == 0){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_ALREADY_RUNNING){
                            MessageBox(NULL, TEXT("已经运行"), TEXT("警告"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("错误代码"), MB_OK);
                        }
                        break;
                    }

                    //关闭服务句柄
                    CloseServiceHandle(serviceHandle);
                    MessageBox(NULL, TEXT("驱动 运行 成功"), TEXT("警告"), MB_OK);
                    break;
                }
                //编写 “Stop按钮” 的处理代码
                case IDC_BUTTON_Stop:{
                    //用于存储驱动的绝对路径
                    char driverPathBuffer[MAX_PATH] = {0};
                    //用于存储驱动名称
                    char* driverName;
                    
                    //获取编辑框中的驱动绝对路径
                    HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                    GetWindowText(hEdit_DriverPath, driverPathBuffer, MAX_PATH);
                    if (*(DWORD*)driverPathBuffer == 0){
                        MessageBox(NULL, TEXT("请选择要加载的驱动路径"), TEXT("警告"), MB_OK);
                        break;
                    }

                    //从绝对路径中，分割出驱动的名称
                    char s[] = "\\";
                    char DriverPathBufferBak[MAX_PATH] = {0};
                    memcpy(DriverPathBufferBak, driverPathBuffer, MAX_PATH);
                    char* token = strtok(DriverPathBufferBak, s);
                    while (token != NULL){
                        driverName = token;
                        token = strtok(NULL, s);
                    }

                    if (g_scManager == NULL){
                        MessageBox(NULL, TEXT("请重新启动，服务打开失败"), TEXT("警告"), MB_OK);
                        break;
                    }
                    
                    //驱动程序已经加载，只需要打开  
                    SC_HANDLE serviceHandle = OpenService(g_scManager, driverName, SERVICE_ALL_ACCESS);
                    if (serviceHandle == NULL){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_DOES_NOT_EXIST){
                            MessageBox(NULL, TEXT("服务已经不存在"), TEXT("警告"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("错误代码"), MB_OK);
                        }
                        break;
                    }
                    
                    //停止驱动程序.
                    SERVICE_STATUS error = {0};
                    int result = ControlService(serviceHandle, SERVICE_CONTROL_STOP, &error);
                    if (result == 0){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_NOT_ACTIVE){
                            MessageBox(NULL, TEXT("服务没有在运行"), TEXT("警告"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("错误代码"), MB_OK);
                        }
                        break;
                    }

                    //关闭服务句柄
                    CloseServiceHandle(serviceHandle);
                    MessageBox(NULL, TEXT("驱动 停止 成功"), TEXT("警告"), MB_OK);
                    break;
                }
                //编写 “UnRegister按钮” 的处理代码
                case IDC_BUTTON_UnRegister:{
                    //用于存储驱动的绝对路径
                    char driverPathBuffer[MAX_PATH] = {0};
                    //用于存储驱动名称
                    char* driverName;
                    
                    //获取编辑框中的驱动绝对路径
                    HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                    GetWindowText(hEdit_DriverPath, driverPathBuffer, MAX_PATH);
                    if (*(DWORD*)driverPathBuffer == 0){
                        MessageBox(NULL, TEXT("请选择要加载的驱动路径"), TEXT("警告"), MB_OK);
                        break;
                    }

                    //从绝对路径中，分割出驱动的名称
                    char s[] = "\\";
                    char DriverPathBufferBak[MAX_PATH] = {0};
                    memcpy(DriverPathBufferBak, driverPathBuffer, MAX_PATH);
                    char* token = strtok(DriverPathBufferBak, s);
                    while (token != NULL){
                        driverName = token;
                        token = strtok(NULL, s);
                    }

                    if (g_scManager == NULL){
                        MessageBox(NULL, TEXT("请重新启动，服务打开失败"), TEXT("警告"), MB_OK);
                        break;
                    }
                    
                    //驱动程序已经加载，只需要打开  
                    SC_HANDLE serviceHandle = OpenService(g_scManager, driverName, SERVICE_ALL_ACCESS);
                    if (serviceHandle == NULL){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_DOES_NOT_EXIST){
                            MessageBox(NULL, TEXT("服务已经不存在"), TEXT("警告"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("错误代码"), MB_OK);
                        }
                        break;
                    }
                    //动态卸载驱动程序。
                    if (!DeleteService(serviceHandle)){
                        DWORD error = GetLastError();
                        char buffer[21] = {0};
                        ultoa((DWORD)error, buffer, 10);
                        MessageBox(NULL, buffer, TEXT("错误代码"), MB_OK);
                        break;
                    }

                    //关闭服务句柄
                    CloseServiceHandle(serviceHandle);
                    CloseServiceHandle(g_scManager);
                    //令g_scManager变量置空
                    g_scManager = NULL;
                    MessageBox(NULL, TEXT("驱动 注销 成功"), TEXT("警告"), MB_OK);
                    break;
                }
            }
            break;
        }
    }
    return FALSE;
}


