// ButtonEncryptShellMain.cpp: implementation of the ButtonEncryptShellMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonEncryptShellMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
BOOL CALLBACK ButtonEncryptShellMainProc(HWND hwndDlgOfEncryptShell, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadEncryptShellProc(LPVOID lpParameter);

/**
 * "软件加壳" 按钮的主处理代码
 */
VOID buttonEncryptShellMain(){
	//创建一个线程专门处理加壳程序的运行
	HANDLE hThread = ::CreateThread(NULL, 0, ThreadEncryptShellProc, NULL, 0, NULL);
	//关闭线程句柄
	::CloseHandle(hThread);
}

/**
 * 加壳线程的回调函数
 * @param  lpParameter [description]
 * @return             [description]
 */
DWORD WINAPI ThreadEncryptShellProc(LPVOID lpParameter){
	//创建一个对话框（这里没有传入父对话框的句柄，以避免子对话框影响父对话框的使用）
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_EncryptingShell), NULL,  ButtonEncryptShellMainProc);
	return 0;
}

/**
 * EncryptingShell对话框的窗口回调函数（该窗口ID：IDD_DIALOG_EncryptingShell）
 * @param  hwndDlgOfEncryptShell EncryptingShell对话框的句柄
 * @param  uMsg                  消息类型
 * @param  wParam                WPARAM
 * @param  lParam                LPARAM
 * @return                       
 */
BOOL CALLBACK ButtonEncryptShellMainProc(HWND hwndDlgOfEncryptShell, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfEncryptShell, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
            	//点击 “退出” 按钮后，退出窗口
                case IDC_BUTTON_EncryptingShellExit:{
                    EndDialog(hwndDlgOfEncryptShell, 0);
                    return TRUE;
                }
                //点击 “请选择壳源程序按钮” 后，要执行的代码
				case IDC_BUTTON_shell:{
					//用于封装选择窗口所选中的文件的信息
					OPENFILENAME stOpenFile;
					//指定可以选中的扩展名
                    TCHAR szPeFileExt[100] = "*.exe;*.dll;*.scr;*.drv;*.sys";
                    //用于存放文件路径的缓冲区
                    TCHAR szFileName[512];
                    //初始化
                    memset(szFileName,0,256);
                    memset(&stOpenFile, 0, sizeof(OPENFILENAME));
                    stOpenFile.lStructSize = sizeof(OPENFILENAME);
                    stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                    stOpenFile.hwndOwner = hwndDlgOfEncryptShell;
                    stOpenFile.lpstrFilter = szPeFileExt;
                    stOpenFile.lpstrFile = szFileName;
                    stOpenFile.nMaxFile = MAX_PATH;
                    //弹出文件选择框，并将所选中的文件信息封装到stOpenFile中
                    BOOL flag = GetOpenFileName(&stOpenFile);
                    //获取 EncryptingShell 对话框的窗口的 IDC_EDIT_shell 编辑框句柄
					HWND hEdit_shell = GetDlgItem(hwndDlgOfEncryptShell, IDC_EDIT_shell);
					//将获取到的 壳源文件 路径写入到编辑框中
					SetWindowText(hEdit_shell, szFileName);
					return TRUE;
				}
				//点击 “请选择待加壳程序按钮” 后，要执行的代码
				case IDC_BUTTON_srcApp:{
					//用于封装选择窗口所选中的文件的信息
					OPENFILENAME stOpenFile;
					//指定可以选中的扩展名
                    TCHAR szPeFileExt[100] = "*.exe;*.dll;*.scr;*.drv;*.sys";
                    //用于存放文件路径的缓冲区
                    TCHAR szFileName[512];
                    //初始化
                    memset(szFileName,0,256);
                    memset(&stOpenFile, 0, sizeof(OPENFILENAME));
                    stOpenFile.lStructSize = sizeof(OPENFILENAME);
                    stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                    stOpenFile.hwndOwner = hwndDlgOfEncryptShell;
                    stOpenFile.lpstrFilter = szPeFileExt;
                    stOpenFile.lpstrFile = szFileName;
                    stOpenFile.nMaxFile = MAX_PATH;
                    //弹出文件选择框，并将所选中的文件信息封装到stOpenFile中
                    BOOL flag = GetOpenFileName(&stOpenFile);
                    //获取 EncryptingShell 对话框的窗口的 IDC_EDIT_srcApp 编辑框句柄
					HWND hEdit_srcApp = GetDlgItem(hwndDlgOfEncryptShell, IDC_EDIT_srcApp);
					//将获取到的 待加壳文件 路径写入到编辑框中
					SetWindowText(hEdit_srcApp, szFileName);
					return TRUE;
				}
				//点击 “加壳” 按钮后，要执行的代码
				case IDC_BUTTON_startEncrypt:{
					//获取 壳源文件 和 待加壳文件 的文件路径
					HWND hEdit_shell = GetDlgItem(hwndDlgOfEncryptShell, IDC_EDIT_shell);
					HWND hEdit_srcApp = GetDlgItem(hwndDlgOfEncryptShell, IDC_EDIT_srcApp);
					TCHAR shellPath[512] = {0};
					TCHAR srcApp[512] = {0};
					GetWindowText(hEdit_shell,shellPath,512);
					GetWindowText(hEdit_srcApp,srcApp,512);
					//判断用户是否已经选择了壳源文件与待加壳文件，若没有，则提示用户
					if (*(int*)shellPath == 0 || *(int*)srcApp == 0){
						MessageBox(NULL, TEXT("请选择壳源程序与待加壳程序！"), TEXT("警告"), MB_OK);
						return TRUE;
					}
					//进行加壳
					int encryptBool =  EncryptShell(shellPath, srcApp);
					//判断加壳是否成功，1为成功，非1为不成功
					if (encryptBool != 1){
						MessageBox(NULL, TEXT("加壳失败！"), TEXT("警告"), MB_OK);
					}else{
						MessageBox(NULL, TEXT("加壳成功！"), TEXT("警告"), MB_OK);
					}
					return TRUE;
				}
            }
        }
    }
    return FALSE;
}


