// ButtonAboutCode.cpp: implementation of the ButtonAboutCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonAboutCode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
BOOL CALLBACK ButtonAboutProc(HWND hwndDlgOfAbout, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * about按钮的主处理代码
 * @param  hInstance  应称程序的句柄
 * @param  DialogMain 主对话框的句柄
 */
VOID ButtonAboutCodeMain(HWND DialogMain){
    //创建一个从对话框
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), DialogMain,  ButtonAboutProc);
}


/**
 * about按钮的从对话框的窗口回调函数
 * @param  hwndDlgOfAbout about按钮的从对话框的句柄
 * @param  uMsg           消息类型
 * @param  wParam         WPARAM
 * @param  lParam         LPARAM
 * @return                
 */
BOOL CALLBACK ButtonAboutProc(HWND hwndDlgOfAbout, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfAbout, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //编写 about对话框中退出按钮所产生的消息 的处理代码
                case IDC_BUTTON_AboutExit:{
                    //退出about对话框
                    EndDialog(hwndDlgOfAbout, 0);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}
