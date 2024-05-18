// ButtonHookMain.cpp: implementation of the ButtonHookMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonHookMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
DWORD WINAPI ThreadHookMainProc(LPVOID lpParameter);
BOOL CALLBACK ButtonHookMainProc(HWND hwndDlgOfDLLInject, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * HOOK的主处理代码
 * @return 
 */
VOID buttonHookMain(){
    //创建一个线程专门处理HOOK程序的运行
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadHookMainProc, NULL, 0, NULL);
    //关闭线程句柄
    ::CloseHandle(hThread);
}

/**
 * HOOK线程的回调函数
 * @param  lpParameter 线程回调函数的参数
 * @return             
 */
DWORD WINAPI ThreadHookMainProc(LPVOID lpParameter){
    //创建一个对话框（这里没有传入父对话框的句柄，以避免子对话框影响父对话框的使用）
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_HookMain), NULL,  ButtonHookMainProc);
    return 0;
}

/**
 * HOOK MAIN 窗口的窗口回调函数（该窗口ID：IDD_DIALOG_HookMain）
 * @param  hwndDlgOfHookMain HOOK MAIN对话框的句柄
 * @param  uMsg              消息类型
 * @param  wParam            WPARAM
 * @param  lParam            LPARAM
 * @return                   
 */
BOOL CALLBACK ButtonHookMainProc(HWND hwndDlgOfHookMain, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //点击 对话框右上角的红叉 之后，退出窗口
        case WM_CLOSE:{
            EndDialog(hwndDlgOfHookMain, 0);
            return TRUE;
        }
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //当点击 “Iat HOOK” 按钮之后执行的代码
                case IDC_BUTTON_IatHook:{
                    buttonIatHook();
                    return TRUE;
                }
                case IDC_BUTTON_InLineHook:{
                    
                }
            }
            break;
        }
    }


    return FALSE;
}









