// PeEditor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

//��������
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    //��ʹ��ͨ�ÿؼ�֮ǰ����Ҫͨ�� INITCOMMONCONTROLSEX ���г�ʼ��
    //ֻҪ�ڳ����е�����ط������˸ú������ͻ�ʹ��WINDOWS�ĳ�����������ظÿ�
    INITCOMMONCONTROLSEX icex;          
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);         
    icex.dwICC = ICC_WIN95_CLASSES;         
    InitCommonControlsEx(&icex);            
    //�������Ի���
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);
	
	//��Ӧ�ó���ľ��ȫ�ֻ�
    ghInstance = hInstance;
    return 0;
}

/**
 * ��д���Ի���Ĵ��ڻص�����
 * @param  hwndDlg ���Ի���ľ��
 * @param  uMsg    ��Ϣ����
 * @param  wParam  WPARAM
 * @param  lParam  LPARAM
 * @return
 */
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //���ڷ�װѡ�񴰿���ѡ�е��ļ�����Ϣ
    OPENFILENAME stOpenFile;
    switch(uMsg) {
        //��д ��ʼ����Ϣ �Ĵ������
        case WM_INITDIALOG:{
			//����ͼ��						
			HICON hIcon = LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_ICON_QianYiShen));						
			//����ͼ��						
			SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (DWORD)hIcon);	
			SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (DWORD)hIcon);	

            //��ʼ�� �����б�ؼ�
            InitProcessList(hwndDlg);
            //��ʼ�� ģ���б�ؼ�
            InitModuleList(hwndDlg);
            //ö�����н���
            EnumAllProcess(hwndDlg);
            return TRUE;
        }
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
        //��д ��ť��Ϣ �Ĵ������
        case WM_COMMAND:{
            //LOWORD(wParam)Ϊ��ťID
            switch(LOWORD(wParam)) {
                //��д �˳���ť �Ĵ������
                case IDC_BUTTON_EXIT:{
                    //�رնԻ���
                    EndDialog(hwndDlg, 0);
                    return TRUE;
                }
                //��� �����ڡ� ��ť��Ĵ������
                case IDC_BUTTON_About:{
                    //����about��ť�Ĵ������
                    ButtonAboutCodeMain(hwndDlg);
                    return TRUE;
                }
                //��� ��PE�鿴���� ��ť��Ĵ������
                case IDC_BUTTON_PeView:{
                    //ָ������ѡ�е���չ��
                    TCHAR szPeFileExt[100] = "*.exe;*.dll;*.scr;*.drv;*.sys";
                    //���ڴ���ļ�·���Ļ�����
                    TCHAR szFileName[MAX_PATH];
                    //��ʼ��
                    memset(szFileName,0,256);
                    memset(&stOpenFile, 0, sizeof(OPENFILENAME));
                    stOpenFile.lStructSize = sizeof(OPENFILENAME);
                    stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                    stOpenFile.hwndOwner = hwndDlg;
                    stOpenFile.lpstrFilter = szPeFileExt;
                    stOpenFile.lpstrFile = szFileName;
                    stOpenFile.nMaxFile = MAX_PATH;
                    //�����ļ�ѡ��򣬲�����ѡ�е��ļ���Ϣ��װ��stOpenFile��
                    BOOL flag = GetOpenFileName(&stOpenFile);
                    //�ж��û��Ƿ�ѡ����PE�ļ�����ѡ�еĻ�, �����ButtonPeViewCodeMain����������PeViwew�Ի���
					if(flag){
						ButtonPeViewCodeMain(szFileName);
					}
                    return TRUE;
                }
                //��� ������ӿǡ� ��ť��Ĵ������
				case IDC_BUTTON_SoftwareShell:{
					buttonEncryptShellMain();
					return TRUE;
				}
				//���� ��dllע�롱 ��ť��Ĵ������
				case IDC_BUTTON_Dllinjection:{
					buttonDLLInjectMain();
					return TRUE;
				}
				//��� ��HOOK�� ��ť��Ĵ������
				case IDC_BUTTON_HOOK:{
					buttonHookMain();
					return TRUE;
				}
				case IDC_BUTTON_PROCESSLIST_flushed:{
					EnumAllProcess(hwndDlg);
					return TRUE;
				}
				//��� �������� ��ť��Ĵ������
				case IDC_BUTTON_DriverMain:{
					buttonDriverMain();
					return TRUE;
				}
            }
        }
        //��д ͨ�ÿؼ���Ϣ �Ĵ������
        case WM_NOTIFY:{
            NMHDR* pNmhdr = (NMHDR*)lParam;
            //���û� ������� ��������  ��ָ�����б�ؼ��� �е��еĻ��������EnumModuleListByPid����
            if (wParam == IDC_LIST_PROCESSLIST && pNmhdr->code == NM_CLICK){
                //����pidö�����Ӧ���̵�ģ����Ϣ
                EnumModuleListByPid(hwndDlg, wParam, lParam);
                return TRUE;
            }
        }
    }
    return FALSE;
    
}
    
