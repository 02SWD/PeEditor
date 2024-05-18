// ProcessAndModuleOperation.h: interface for the ProcessAndModuleOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSANDMODULEOPERATION_H__8C3D8B00_C6E5_47BB_A5FC_C439CEA6EFB6__INCLUDED_)
#define AFX_PROCESSANDMODULEOPERATION_H__8C3D8B00_C6E5_47BB_A5FC_C439CEA6EFB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/**
 * ö�ٵ�ǰ����ϵͳ�����еĽ���
 * @param  hwndDlg ���Ի���ľ��
 * @return         ö�ٳɹ�����1��CreateToolhelp32Snapshot����ʧ�ܷ���-1
 */
int EnumAllProcess(HWND hwndDlg);

/**
 * ����pidö������������е�ģ��
 * @param  hwndDlg ���Ի�����
 * @param  wParam  WPARAM
 * @param  lParam  LPARAM
 * @return         ö�ٳɹ��򷵻�1��û�л�ȡ�����򷵻�-1����һ��ģ����Ϣ��ȡʧ���򷵻�-2
 */
int EnumModuleListByPid(HWND hwndDlg, WPARAM wParam, LPARAM lParam);

/**
 * ��ʼ�� �����б�ؼ���Ϊ�����б�ؼ���ӱ�ͷ��
 * @param  hwndDlg ���Ի�����
 */
VOID InitProcessList(HWND hwndDlg);

/**
 * ��ʼ�� ģ���б�ؼ���Ϊģ���б�ؼ���ӱ�ͷ��
 * @param  hwndDlg ���Ի�����
 */
VOID InitModuleList(HWND hwndDlg);





#endif // !defined(AFX_PROCESSANDMODULEOPERATION_H__8C3D8B00_C6E5_47BB_A5FC_C439CEA6EFB6__INCLUDED_)
