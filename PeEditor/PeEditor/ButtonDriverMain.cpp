// ButtonDriverMain.cpp: implementation of the ButtonDriverMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonDriverMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
DWORD WINAPI ThreadDriverMainProc(LPVOID lpParameter);
BOOL CALLBACK ButtonDriverMainProc(HWND hwndDlgOfDriverMain, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * 驱动操作的主处理代码
 * @return 
 */
VOID buttonDriverMain(){
    //创建一个线程专门处理驱动程序的运行
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadDriverMainProc, NULL, 0, NULL);
    //关闭线程句柄
    ::CloseHandle(hThread);
}

/**
 * 驱动操作线程的回调函数
 * @param  lpParameter 线程回调函数的参数
 * @return           
 */
DWORD WINAPI ThreadDriverMainProc(LPVOID lpParameter){
    //创建一个对话框（这里没有传入父对话框的句柄，以避免子对话框影响父对话框的使用）
    DialogBox(ghInstance, MAKEINTRESOURCEA(IDD_DIALOG_DriverMain), NULL, ButtonDriverMainProc);
    return 0;

}

/**
 * Driver Main 窗口的窗口回调函数（该窗口ID：IDD_DIALOG_DriverMain）
 * @param  hwndDlgOfDriverMain Driver Main 窗口的句柄
 * @param  uMsg                消息类型
 * @param  wParam              wParam
 * @param  lParam              lParam
 * @return                     
 */
BOOL CALLBACK ButtonDriverMainProc(HWND hwndDlgOfDriverMain, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //点击 对话框右上角的红叉 之后，退出窗口
        case WM_CLOSE:{
            EndDialog(hwndDlgOfDriverMain, 0);
            return TRUE;
        }
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //当点击 “加载驱动（正常方式）” 按钮之后执行的代码
                case IDC_BUTTON_DriveLoadNormal:{
                    buttonDriverLoadNormal();
                    return TRUE;
                }
            }
            break;
        }
    }
    return FALSE;
}