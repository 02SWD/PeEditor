// PeEditor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

//函数声明
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    //在使用通用控件之前，需要通过 INITCOMMONCONTROLSEX 进行初始化
    //只要在程序中的任意地方引用了该函数，就会使得WINDOWS的程序加载器加载该库
    INITCOMMONCONTROLSEX icex;          
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);         
    icex.dwICC = ICC_WIN95_CLASSES;         
    InitCommonControlsEx(&icex);            
    //创建主对话框
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);
	
	//将应用程序的句柄全局化
    ghInstance = hInstance;
    return 0;
}

/**
 * 编写主对话框的窗口回调函数
 * @param  hwndDlg 主对话框的句柄
 * @param  uMsg    消息类型
 * @param  wParam  WPARAM
 * @param  lParam  LPARAM
 * @return
 */
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //用于封装选择窗口所选中的文件的信息
    OPENFILENAME stOpenFile;
    switch(uMsg) {
        //编写 初始化消息 的处理代码
        case WM_INITDIALOG:{
			//加载图标						
			HICON hIcon = LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_ICON_QianYiShen));						
			//设置图标						
			SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (DWORD)hIcon);	
			SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (DWORD)hIcon);	

            //初始化 进程列表控件
            InitProcessList(hwndDlg);
            //初始化 模块列表控件
            InitModuleList(hwndDlg);
            //枚举所有进程
            EnumAllProcess(hwndDlg);
            return TRUE;
        }
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
        //编写 按钮消息 的处理代码
        case WM_COMMAND:{
            //LOWORD(wParam)为按钮ID
            switch(LOWORD(wParam)) {
                //编写 退出按钮 的处理代码
                case IDC_BUTTON_EXIT:{
                    //关闭对话框
                    EndDialog(hwndDlg, 0);
                    return TRUE;
                }
                //点击 “关于” 按钮后的处理代码
                case IDC_BUTTON_About:{
                    //调用about按钮的处理代码
                    ButtonAboutCodeMain(hwndDlg);
                    return TRUE;
                }
                //点击 “PE查看器” 按钮后的处理代码
                case IDC_BUTTON_PeView:{
                    //指定可以选中的扩展名
                    TCHAR szPeFileExt[100] = "*.exe;*.dll;*.scr;*.drv;*.sys";
                    //用于存放文件路径的缓冲区
                    TCHAR szFileName[MAX_PATH];
                    //初始化
                    memset(szFileName,0,256);
                    memset(&stOpenFile, 0, sizeof(OPENFILENAME));
                    stOpenFile.lStructSize = sizeof(OPENFILENAME);
                    stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                    stOpenFile.hwndOwner = hwndDlg;
                    stOpenFile.lpstrFilter = szPeFileExt;
                    stOpenFile.lpstrFile = szFileName;
                    stOpenFile.nMaxFile = MAX_PATH;
                    //弹出文件选择框，并将所选中的文件信息封装到stOpenFile中
                    BOOL flag = GetOpenFileName(&stOpenFile);
                    //判断用户是否选中了PE文件，若选中的话, 则调用ButtonPeViewCodeMain函数，进入PeViwew对话框
					if(flag){
						ButtonPeViewCodeMain(szFileName);
					}
                    return TRUE;
                }
                //点击 “软件加壳” 按钮后的处理代码
				case IDC_BUTTON_SoftwareShell:{
					buttonEncryptShellMain();
					return TRUE;
				}
				//单击 “dll注入” 按钮后的处理代码
				case IDC_BUTTON_Dllinjection:{
					buttonDLLInjectMain();
					return TRUE;
				}
				//点击 “HOOK” 按钮后的处理代码
				case IDC_BUTTON_HOOK:{
					buttonHookMain();
					return TRUE;
				}
				case IDC_BUTTON_PROCESSLIST_flushed:{
					EnumAllProcess(hwndDlg);
					return TRUE;
				}
				//点击 “驱动” 按钮后的处理代码
				case IDC_BUTTON_DriverMain:{
					buttonDriverMain();
					return TRUE;
				}
            }
        }
        //编写 通用控件消息 的处理代码
        case WM_NOTIFY:{
            NMHDR* pNmhdr = (NMHDR*)lParam;
            //若用户 “点击” 的是我们  “指定的列表控件” 中的行的话，则调用EnumModuleListByPid函数
            if (wParam == IDC_LIST_PROCESSLIST && pNmhdr->code == NM_CLICK){
                //根据pid枚举其对应进程的模块信息
                EnumModuleListByPid(hwndDlg, wParam, lParam);
                return TRUE;
            }
        }
    }
    return FALSE;
    
}
    
