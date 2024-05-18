// ButtonEncryptShellMain.cpp: implementation of the ButtonEncryptShellMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonEncryptShellMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
BOOL CALLBACK ButtonEncryptShellMainProc(HWND hwndDlgOfEncryptShell, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadEncryptShellProc(LPVOID lpParameter);

/**
 * "����ӿ�" ��ť�����������
 */
VOID buttonEncryptShellMain(){
	//����һ���߳�ר�Ŵ���ӿǳ��������
	HANDLE hThread = ::CreateThread(NULL, 0, ThreadEncryptShellProc, NULL, 0, NULL);
	//�ر��߳̾��
	::CloseHandle(hThread);
}

/**
 * �ӿ��̵߳Ļص�����
 * @param  lpParameter [description]
 * @return             [description]
 */
DWORD WINAPI ThreadEncryptShellProc(LPVOID lpParameter){
	//����һ���Ի�������û�д��븸�Ի���ľ�����Ա����ӶԻ���Ӱ�츸�Ի����ʹ�ã�
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_EncryptingShell), NULL,  ButtonEncryptShellMainProc);
	return 0;
}

/**
 * EncryptingShell�Ի���Ĵ��ڻص��������ô���ID��IDD_DIALOG_EncryptingShell��
 * @param  hwndDlgOfEncryptShell EncryptingShell�Ի���ľ��
 * @param  uMsg                  ��Ϣ����
 * @param  wParam                WPARAM
 * @param  lParam                LPARAM
 * @return                       
 */
BOOL CALLBACK ButtonEncryptShellMainProc(HWND hwndDlgOfEncryptShell, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfEncryptShell, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
            	//��� ���˳��� ��ť���˳�����
                case IDC_BUTTON_EncryptingShellExit:{
                    EndDialog(hwndDlgOfEncryptShell, 0);
                    return TRUE;
                }
                //��� ����ѡ���Դ����ť�� ��Ҫִ�еĴ���
				case IDC_BUTTON_shell:{
					//���ڷ�װѡ�񴰿���ѡ�е��ļ�����Ϣ
					OPENFILENAME stOpenFile;
					//ָ������ѡ�е���չ��
                    TCHAR szPeFileExt[100] = "*.exe;*.dll;*.scr;*.drv;*.sys";
                    //���ڴ���ļ�·���Ļ�����
                    TCHAR szFileName[512];
                    //��ʼ��
                    memset(szFileName,0,256);
                    memset(&stOpenFile, 0, sizeof(OPENFILENAME));
                    stOpenFile.lStructSize = sizeof(OPENFILENAME);
                    stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                    stOpenFile.hwndOwner = hwndDlgOfEncryptShell;
                    stOpenFile.lpstrFilter = szPeFileExt;
                    stOpenFile.lpstrFile = szFileName;
                    stOpenFile.nMaxFile = MAX_PATH;
                    //�����ļ�ѡ��򣬲�����ѡ�е��ļ���Ϣ��װ��stOpenFile��
                    BOOL flag = GetOpenFileName(&stOpenFile);
                    //��ȡ EncryptingShell �Ի���Ĵ��ڵ� IDC_EDIT_shell �༭����
					HWND hEdit_shell = GetDlgItem(hwndDlgOfEncryptShell, IDC_EDIT_shell);
					//����ȡ���� ��Դ�ļ� ·��д�뵽�༭����
					SetWindowText(hEdit_shell, szFileName);
					return TRUE;
				}
				//��� ����ѡ����ӿǳ���ť�� ��Ҫִ�еĴ���
				case IDC_BUTTON_srcApp:{
					//���ڷ�װѡ�񴰿���ѡ�е��ļ�����Ϣ
					OPENFILENAME stOpenFile;
					//ָ������ѡ�е���չ��
                    TCHAR szPeFileExt[100] = "*.exe;*.dll;*.scr;*.drv;*.sys";
                    //���ڴ���ļ�·���Ļ�����
                    TCHAR szFileName[512];
                    //��ʼ��
                    memset(szFileName,0,256);
                    memset(&stOpenFile, 0, sizeof(OPENFILENAME));
                    stOpenFile.lStructSize = sizeof(OPENFILENAME);
                    stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                    stOpenFile.hwndOwner = hwndDlgOfEncryptShell;
                    stOpenFile.lpstrFilter = szPeFileExt;
                    stOpenFile.lpstrFile = szFileName;
                    stOpenFile.nMaxFile = MAX_PATH;
                    //�����ļ�ѡ��򣬲�����ѡ�е��ļ���Ϣ��װ��stOpenFile��
                    BOOL flag = GetOpenFileName(&stOpenFile);
                    //��ȡ EncryptingShell �Ի���Ĵ��ڵ� IDC_EDIT_srcApp �༭����
					HWND hEdit_srcApp = GetDlgItem(hwndDlgOfEncryptShell, IDC_EDIT_srcApp);
					//����ȡ���� ���ӿ��ļ� ·��д�뵽�༭����
					SetWindowText(hEdit_srcApp, szFileName);
					return TRUE;
				}
				//��� ���ӿǡ� ��ť��Ҫִ�еĴ���
				case IDC_BUTTON_startEncrypt:{
					//��ȡ ��Դ�ļ� �� ���ӿ��ļ� ���ļ�·��
					HWND hEdit_shell = GetDlgItem(hwndDlgOfEncryptShell, IDC_EDIT_shell);
					HWND hEdit_srcApp = GetDlgItem(hwndDlgOfEncryptShell, IDC_EDIT_srcApp);
					TCHAR shellPath[512] = {0};
					TCHAR srcApp[512] = {0};
					GetWindowText(hEdit_shell,shellPath,512);
					GetWindowText(hEdit_srcApp,srcApp,512);
					//�ж��û��Ƿ��Ѿ�ѡ���˿�Դ�ļ�����ӿ��ļ�����û�У�����ʾ�û�
					if (*(int*)shellPath == 0 || *(int*)srcApp == 0){
						MessageBox(NULL, TEXT("��ѡ���Դ��������ӿǳ���"), TEXT("����"), MB_OK);
						return TRUE;
					}
					//���мӿ�
					int encryptBool =  EncryptShell(shellPath, srcApp);
					//�жϼӿ��Ƿ�ɹ���1Ϊ�ɹ�����1Ϊ���ɹ�
					if (encryptBool != 1){
						MessageBox(NULL, TEXT("�ӿ�ʧ�ܣ�"), TEXT("����"), MB_OK);
					}else{
						MessageBox(NULL, TEXT("�ӿǳɹ���"), TEXT("����"), MB_OK);
					}
					return TRUE;
				}
            }
        }
    }
    return FALSE;
}


