// ButtonDriverMain.cpp: implementation of the ButtonDriverMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonDriverMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
DWORD WINAPI ThreadDriverMainProc(LPVOID lpParameter);
BOOL CALLBACK ButtonDriverMainProc(HWND hwndDlgOfDriverMain, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * �������������������
 * @return 
 */
VOID buttonDriverMain(){
    //����һ���߳�ר�Ŵ����������������
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadDriverMainProc, NULL, 0, NULL);
    //�ر��߳̾��
    ::CloseHandle(hThread);
}

/**
 * ���������̵߳Ļص�����
 * @param  lpParameter �̻߳ص������Ĳ���
 * @return           
 */
DWORD WINAPI ThreadDriverMainProc(LPVOID lpParameter){
    //����һ���Ի�������û�д��븸�Ի���ľ�����Ա����ӶԻ���Ӱ�츸�Ի����ʹ�ã�
    DialogBox(ghInstance, MAKEINTRESOURCEA(IDD_DIALOG_DriverMain), NULL, ButtonDriverMainProc);
    return 0;

}

/**
 * Driver Main ���ڵĴ��ڻص��������ô���ID��IDD_DIALOG_DriverMain��
 * @param  hwndDlgOfDriverMain Driver Main ���ڵľ��
 * @param  uMsg                ��Ϣ����
 * @param  wParam              wParam
 * @param  lParam              lParam
 * @return                     
 */
BOOL CALLBACK ButtonDriverMainProc(HWND hwndDlgOfDriverMain, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //��� �Ի������Ͻǵĺ�� ֮���˳�����
        case WM_CLOSE:{
            EndDialog(hwndDlgOfDriverMain, 0);
            return TRUE;
        }
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //����� ������������������ʽ���� ��ť֮��ִ�еĴ���
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