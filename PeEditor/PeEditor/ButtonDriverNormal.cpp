// ButtonDriverNormal.cpp: implementation of the ButtonDriverNormal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonDriverNormal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
DWORD WINAPI ThreadDriverLoadNormalProc(LPVOID lpParameter);
BOOL CALLBACK ButtonDriverLoadNormalProc(HWND hwndDlgOfDriverMain, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * �������ط�ʽ���������Ĵ������
 * @return 
 */
VOID buttonDriverLoadNormal(){
    //����һ���߳�ר�����ڴ��� ��������ʽ�������� �Ĵ���
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadDriverLoadNormalProc, NULL, 0, NULL);
    //�ر��߳̾��
    ::CloseHandle(hThread);
}

/**
 * �̻߳ص�����
 * @param  lpParameter �ص������Ĳ���
 * @return             
 */
DWORD WINAPI ThreadDriverLoadNormalProc(LPVOID lpParameter){
    //����һ���Ի�������û�д��븸�Ի���ľ�����Ա����ӶԻ���Ӱ�츸�Ի����ʹ�ã�
    DialogBox(ghInstance, MAKEINTRESOURCEA(IDD_DIALOG_DriverLoadNormal), NULL, ButtonDriverLoadNormalProc);
    return 0;
}

/**
 * DriverLoadNormal ���ڵĴ��ڻص��������ô���ID��IDD_DIALOG_DriverLoadNormal��
 * @param  hwndDlgOfDriverMain DriverLoadNormal ���ڵľ��
 * @param  uMsg                ��Ϣ����
 * @param  wParam              wParam
 * @param  lParam              lParam
 * @return                     
 */
BOOL CALLBACK ButtonDriverLoadNormalProc(HWND hwndDlgOfDriverLoadNormalMain, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //���ڷ�װѡ�񴰿���ѡ�е��ļ�����Ϣ
    OPENFILENAME stOpenFile;
    switch(uMsg) {
        //����������Ͻǵĺ��ʱ���رմ���
        case WM_CLOSE:{
            EndDialog(hwndDlgOfDriverLoadNormalMain, 0);
            break;
        }
        //��д ��ť��Ϣ �Ĵ������
        case WM_COMMAND:{
            //LOWORD(wParam)Ϊ��ťID
            switch(LOWORD(wParam)) {
                //��д ��ѡ�������ļ���ť�� �Ĵ������
                case IDC_BUTTON_SelectDriver:{
                    //����������δ��UnRegister֮ǰ����ָֹ���µ���������
                    if (g_scManager != NULL){
                        MessageBox(NULL, TEXT("��ǰ������δUnRegister"), TEXT("����"), MB_OK);
                        break;
                    }

                    TCHAR szDriverExt[100] = ".sys";
                    TCHAR szFileName[MAX_PATH];
                    memset(szFileName, 0, MAX_PATH);
                    memset(&stOpenFile, 0, sizeof(OPENFILENAME));
                    stOpenFile.lStructSize = sizeof(OPENFILENAME);
                    stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                    stOpenFile.hwndOwner = hwndDlgOfDriverLoadNormalMain;
                    stOpenFile.lpstrFilter = szDriverExt;
                    stOpenFile.lpstrFile = szFileName;
                    stOpenFile.nMaxFile = MAX_PATH;

                    BOOL flag = GetOpenFileName(&stOpenFile);
                    if (flag){
                        //��༭����д����������ľ���·��
                        HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                        SetWindowText(hEdit_DriverPath, szFileName);
                    }
                    break;
                }
                //��д ��Register��ť�� �Ĵ������
                case IDC_BUTTON_Register:{
                    //���ڴ洢�����ľ���·��
                    char driverPathBuffer[MAX_PATH] = {0};
                    //���ڴ洢��������
                    char* driverName;
                    
                    //��ȡ�༭���е���������·��
                    HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                    GetWindowText(hEdit_DriverPath, driverPathBuffer, MAX_PATH);
                    if (*(DWORD*)driverPathBuffer == 0){
                        MessageBox(NULL, TEXT("��ѡ��Ҫ���ص�����·��"), TEXT("����"), MB_OK);
                        break;
                    }

                    //�Ӿ���·���У��ָ������������
                    char s[] = "\\";
                    char DriverPathBufferBak[MAX_PATH] = {0};
                    memcpy(DriverPathBufferBak, driverPathBuffer, MAX_PATH);
                    char* token = strtok(DriverPathBufferBak, s);
                    while (token != NULL){
                        driverName = token;
                        token = strtok(NULL, s);
                    }

                    //��SCM������
                    g_scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
                    if (g_scManager == NULL){
                        MessageBox(NULL, TEXT("�����������ʧ��"), TEXT("����"), MB_OK);
                        break;
                    }
                    
                    //����������Ӧ�ķ���
                    SC_HANDLE serviceHandle = CreateService(
                        g_scManager,                //SCM�������ľ��
                        driverName,                 //����������ע����е�����
                        driverName,                 //ע������������DisplayNameֵ
                        SERVICE_ALL_ACCESS,         //������������ķ���Ȩ��
                        SERVICE_KERNEL_DRIVER,      //��ʾ���صķ�����kernel��������
                        SERVICE_DEMAND_START,       //ע������������Startֵ
                        SERVICE_ERROR_NORMAL,       //ע������������ErrorControlֵ
                        driverPathBuffer,           //ע������������ImagePathֵ
                        NULL, 
                        NULL, 
                        NULL, 
                        NULL ,
                        NULL);
                    if (serviceHandle == NULL){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_EXISTS){
                            MessageBox(NULL, TEXT("�����Ѿ����ڲ���Ҫ������"), TEXT("����"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("�������"), MB_OK);
                        }
                        break;
                    }

                    //�رշ�����
                    CloseServiceHandle(serviceHandle);
                    MessageBox(NULL, TEXT("���� ע�� �ɹ�"), TEXT("����"), MB_OK);
                    break;
                }
                //��д ��Run��ť�� �Ĵ������
                case IDC_BUTTON_Run:{
                    //���ڴ洢�����ľ���·��
                    char driverPathBuffer[MAX_PATH] = {0};
                    //���ڴ洢��������
                    char* driverName;
                    
                    //��ȡ�༭���е���������·��
                    HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                    GetWindowText(hEdit_DriverPath, driverPathBuffer, MAX_PATH);
                    if (*(DWORD*)driverPathBuffer == 0){
                        MessageBox(NULL, TEXT("��ѡ��Ҫ���ص�����·��"), TEXT("����"), MB_OK);
                        break;
                    }

                    //�Ӿ���·���У��ָ������������
                    char s[] = "\\";
                    char DriverPathBufferBak[MAX_PATH] = {0};
                    memcpy(DriverPathBufferBak, driverPathBuffer, MAX_PATH);
                    char* token = strtok(DriverPathBufferBak, s);
                    while (token != NULL){
                        driverName = token;
                        token = strtok(NULL, s);
                    }

                    if (g_scManager == NULL){
                        MessageBox(NULL, TEXT("�����������������ʧ��"), TEXT("����"), MB_OK);
                        break;
                    }

                    //���������Ѿ����أ�ֻ��Ҫ��  
                    SC_HANDLE serviceHandle = OpenService(g_scManager, driverName, SERVICE_ALL_ACCESS);
                    if (serviceHandle == NULL){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_DOES_NOT_EXIST){
                            MessageBox(NULL, TEXT("�����Ѿ�������"), TEXT("����"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("�������"), MB_OK);
                        }
                        break;
                    }

                    //�����������
                    int result = StartService(serviceHandle, 0, NULL);
                    if (result == 0){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_ALREADY_RUNNING){
                            MessageBox(NULL, TEXT("�Ѿ�����"), TEXT("����"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("�������"), MB_OK);
                        }
                        break;
                    }

                    //�رշ�����
                    CloseServiceHandle(serviceHandle);
                    MessageBox(NULL, TEXT("���� ���� �ɹ�"), TEXT("����"), MB_OK);
                    break;
                }
                //��д ��Stop��ť�� �Ĵ������
                case IDC_BUTTON_Stop:{
                    //���ڴ洢�����ľ���·��
                    char driverPathBuffer[MAX_PATH] = {0};
                    //���ڴ洢��������
                    char* driverName;
                    
                    //��ȡ�༭���е���������·��
                    HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                    GetWindowText(hEdit_DriverPath, driverPathBuffer, MAX_PATH);
                    if (*(DWORD*)driverPathBuffer == 0){
                        MessageBox(NULL, TEXT("��ѡ��Ҫ���ص�����·��"), TEXT("����"), MB_OK);
                        break;
                    }

                    //�Ӿ���·���У��ָ������������
                    char s[] = "\\";
                    char DriverPathBufferBak[MAX_PATH] = {0};
                    memcpy(DriverPathBufferBak, driverPathBuffer, MAX_PATH);
                    char* token = strtok(DriverPathBufferBak, s);
                    while (token != NULL){
                        driverName = token;
                        token = strtok(NULL, s);
                    }

                    if (g_scManager == NULL){
                        MessageBox(NULL, TEXT("�����������������ʧ��"), TEXT("����"), MB_OK);
                        break;
                    }
                    
                    //���������Ѿ����أ�ֻ��Ҫ��  
                    SC_HANDLE serviceHandle = OpenService(g_scManager, driverName, SERVICE_ALL_ACCESS);
                    if (serviceHandle == NULL){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_DOES_NOT_EXIST){
                            MessageBox(NULL, TEXT("�����Ѿ�������"), TEXT("����"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("�������"), MB_OK);
                        }
                        break;
                    }
                    
                    //ֹͣ��������.
                    SERVICE_STATUS error = {0};
                    int result = ControlService(serviceHandle, SERVICE_CONTROL_STOP, &error);
                    if (result == 0){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_NOT_ACTIVE){
                            MessageBox(NULL, TEXT("����û��������"), TEXT("����"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("�������"), MB_OK);
                        }
                        break;
                    }

                    //�رշ�����
                    CloseServiceHandle(serviceHandle);
                    MessageBox(NULL, TEXT("���� ֹͣ �ɹ�"), TEXT("����"), MB_OK);
                    break;
                }
                //��д ��UnRegister��ť�� �Ĵ������
                case IDC_BUTTON_UnRegister:{
                    //���ڴ洢�����ľ���·��
                    char driverPathBuffer[MAX_PATH] = {0};
                    //���ڴ洢��������
                    char* driverName;
                    
                    //��ȡ�༭���е���������·��
                    HWND hEdit_DriverPath = GetDlgItem(hwndDlgOfDriverLoadNormalMain, IDC_EDIT_SelectDriver);
                    GetWindowText(hEdit_DriverPath, driverPathBuffer, MAX_PATH);
                    if (*(DWORD*)driverPathBuffer == 0){
                        MessageBox(NULL, TEXT("��ѡ��Ҫ���ص�����·��"), TEXT("����"), MB_OK);
                        break;
                    }

                    //�Ӿ���·���У��ָ������������
                    char s[] = "\\";
                    char DriverPathBufferBak[MAX_PATH] = {0};
                    memcpy(DriverPathBufferBak, driverPathBuffer, MAX_PATH);
                    char* token = strtok(DriverPathBufferBak, s);
                    while (token != NULL){
                        driverName = token;
                        token = strtok(NULL, s);
                    }

                    if (g_scManager == NULL){
                        MessageBox(NULL, TEXT("�����������������ʧ��"), TEXT("����"), MB_OK);
                        break;
                    }
                    
                    //���������Ѿ����أ�ֻ��Ҫ��  
                    SC_HANDLE serviceHandle = OpenService(g_scManager, driverName, SERVICE_ALL_ACCESS);
                    if (serviceHandle == NULL){
                        DWORD error = GetLastError();
                        if (error == ERROR_SERVICE_DOES_NOT_EXIST){
                            MessageBox(NULL, TEXT("�����Ѿ�������"), TEXT("����"), MB_OK);
                        }else{
                            char buffer[21] = {0};
                            ultoa((DWORD)error, buffer, 10);
                            MessageBox(NULL, buffer, TEXT("�������"), MB_OK);
                        }
                        break;
                    }
                    //��̬ж����������
                    if (!DeleteService(serviceHandle)){
                        DWORD error = GetLastError();
                        char buffer[21] = {0};
                        ultoa((DWORD)error, buffer, 10);
                        MessageBox(NULL, buffer, TEXT("�������"), MB_OK);
                        break;
                    }

                    //�رշ�����
                    CloseServiceHandle(serviceHandle);
                    CloseServiceHandle(g_scManager);
                    //��g_scManager�����ÿ�
                    g_scManager = NULL;
                    MessageBox(NULL, TEXT("���� ע�� �ɹ�"), TEXT("����"), MB_OK);
                    break;
                }
            }
            break;
        }
    }
    return FALSE;
}


