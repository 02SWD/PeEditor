// ButtonAboutCode.cpp: implementation of the ButtonAboutCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonAboutCode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
BOOL CALLBACK ButtonAboutProc(HWND hwndDlgOfAbout, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * about��ť�����������
 * @param  hInstance  Ӧ�Ƴ���ľ��
 * @param  DialogMain ���Ի���ľ��
 */
VOID ButtonAboutCodeMain(HWND DialogMain){
    //����һ���ӶԻ���
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), DialogMain,  ButtonAboutProc);
}


/**
 * about��ť�ĴӶԻ���Ĵ��ڻص�����
 * @param  hwndDlgOfAbout about��ť�ĴӶԻ���ľ��
 * @param  uMsg           ��Ϣ����
 * @param  wParam         WPARAM
 * @param  lParam         LPARAM
 * @return                
 */
BOOL CALLBACK ButtonAboutProc(HWND hwndDlgOfAbout, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfAbout, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //��д about�Ի������˳���ť����������Ϣ �Ĵ������
                case IDC_BUTTON_AboutExit:{
                    //�˳�about�Ի���
                    EndDialog(hwndDlgOfAbout, 0);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}
