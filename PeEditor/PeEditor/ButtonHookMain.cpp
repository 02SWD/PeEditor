// ButtonHookMain.cpp: implementation of the ButtonHookMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonHookMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
DWORD WINAPI ThreadHookMainProc(LPVOID lpParameter);
BOOL CALLBACK ButtonHookMainProc(HWND hwndDlgOfDLLInject, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * HOOK�����������
 * @return 
 */
VOID buttonHookMain(){
    //����һ���߳�ר�Ŵ���HOOK���������
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadHookMainProc, NULL, 0, NULL);
    //�ر��߳̾��
    ::CloseHandle(hThread);
}

/**
 * HOOK�̵߳Ļص�����
 * @param  lpParameter �̻߳ص������Ĳ���
 * @return             
 */
DWORD WINAPI ThreadHookMainProc(LPVOID lpParameter){
    //����һ���Ի�������û�д��븸�Ի���ľ�����Ա����ӶԻ���Ӱ�츸�Ի����ʹ�ã�
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_HookMain), NULL,  ButtonHookMainProc);
    return 0;
}

/**
 * HOOK MAIN ���ڵĴ��ڻص��������ô���ID��IDD_DIALOG_HookMain��
 * @param  hwndDlgOfHookMain HOOK MAIN�Ի���ľ��
 * @param  uMsg              ��Ϣ����
 * @param  wParam            WPARAM
 * @param  lParam            LPARAM
 * @return                   
 */
BOOL CALLBACK ButtonHookMainProc(HWND hwndDlgOfHookMain, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //��� �Ի������Ͻǵĺ�� ֮���˳�����
        case WM_CLOSE:{
            EndDialog(hwndDlgOfHookMain, 0);
            return TRUE;
        }
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //����� ��Iat HOOK�� ��ť֮��ִ�еĴ���
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









