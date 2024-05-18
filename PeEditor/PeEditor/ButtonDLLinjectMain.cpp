// ButtonDLLinjectMain.cpp: implementation of the ButtonDLLinjectMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonDLLinjectMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//��������
DWORD WINAPI ThreadDLLInjectProc(LPVOID lpParameter);
BOOL CALLBACK ButtonDLLInjectMainProc(HWND hwndDlgOfDLLInject, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * ע������������
 */
VOID buttonDLLInjectMain(){
    //����һ���߳�ר�Ŵ���ע����������
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadDLLInjectProc, NULL, 0, NULL);
    //�ر��߳̾��
    ::CloseHandle(hThread);
}

/**
 * ע���̵߳Ļص�����
 * @param  lpParameter �̺߳����Ĳ���
 * @return             
 */
DWORD WINAPI ThreadDLLInjectProc(LPVOID lpParameter){
    //����һ���Ի�������û�д��븸�Ի���ľ�����Ա����ӶԻ���Ӱ�츸�Ի����ʹ�ã�
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_DLLInjection), NULL,  ButtonDLLInjectMainProc);
    return 0;
}

/**
 * DLL Inject ���ڵĴ��ڻص��������ô��ڵ�id��IDD_DIALOG_DLLInjection��
 * @param  hwndDlgOfDLLInject DLL Inject�Ի���ľ��
 * @param  uMsg               ��Ϣ����
 * @param  wParam             WPARAM
 * @param  lParam             LPARAM
 * @return                    
 */
BOOL CALLBACK ButtonDLLInjectMainProc(HWND hwndDlgOfDLLInject, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
				//��ѡ��ע���DLLģ�飬������DLLģ���·��д�뵽�༭����
				case IDC_BUTTON_selectDll:{
					//ָ������ѡ�е���չ��
					TCHAR szPeFileExt[100] = "*.exe;*.dll;*.scr;*.drv;*.sys";
					//���ڴ���ļ�·���Ļ�����
					TCHAR szDllFileName[MAX_PATH] = {0};
					OPENFILENAME stOpenFile;
					memset(&stOpenFile, 0, sizeof(OPENFILENAME));
					stOpenFile.lStructSize = sizeof(OPENFILENAME);
					stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
					stOpenFile.hwndOwner = hwndDlgOfDLLInject;
					stOpenFile.lpstrFilter = szPeFileExt;
					stOpenFile.lpstrFile = szDllFileName;
					stOpenFile.nMaxFile = MAX_PATH;
					//�����ļ�ѡ��򣬲�����ѡ�е��ļ���Ϣ��װ��stOpenFile��
					BOOL flag = GetOpenFileName(&stOpenFile);
					//�ж��û��Ƿ�ѡ����PE�ļ�����ѡ�еĻ�, �򽫸�DLLģ���·��д�뵽�༭����
					if (flag){
						HWND hEdit_DllPath = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_DllPath);
						SetWindowText(hEdit_DllPath, szDllFileName);
					}
					return TRUE;
				}
                //����� ��ע�롱 ��ť֮��Ҫִ�еĴ���
                case IDC_BUTTON_startInject:{
                    //��ȡ ������ѡ��ť��pid�༭�� �� dll�༭�� �ľ��
                    HWND hLoadLibraryRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_LoadLibrary);
                    HWND hMemoryRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_memoryWrite);
                    HWND hProcessRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_loadProcess);
                    HWND hTypeWritingRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_typewriting);
                    HWND hMemoryOtherRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_WriteMemoryWithoutRemoteThread);
                    HWND hEditPid = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_PID_dll);
                    HWND hEditDllPath = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_DllPath);
                    
                    //��ȡ������ѡ���״̬
                    int statusLoadLibrary = SendMessage(hLoadLibraryRadio, BM_GETCHECK, 0, 0);
                    int statusMemory = SendMessage(hMemoryRadio, BM_GETCHECK, 0, 0);
                    int statusProcess = SendMessage(hProcessRadio, BM_GETCHECK, 0, 0);
                    int statusTypeWriting = SendMessage(hTypeWritingRadio, BM_GETCHECK, 0, 0);
                    int statusMemoryOther = SendMessage(hMemoryOtherRadio, BM_GETCHECK, 0, 0);
                    
                    
                    //��ȡpid�༭���е�ֵ��������ת��Ϊ����
                    char szBuffer[MAX_PATH] = {0};
                    memset(szBuffer, 0, MAX_PATH);
                    GetWindowText(hEditPid, szBuffer, MAX_PATH);
                    char* leftOvew = NULL;
                    DWORD pid = strtoul(szBuffer, &leftOvew, 10);;
                    
                    //��ȡdll�༭���е�ֵ
                    memset(szBuffer, 0, MAX_PATH);
                    GetWindowText(hEditDllPath, szBuffer, MAX_PATH);

                    //�ж��û��Ƿ��ڱ༭����������ֵ����û������ʾ�û�����
                    if (pid <= 0){
                        MessageBox(NULL, TEXT("��������ȷ��Ŀ�����PID!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }

                    //�ж��û��Ƿ�ѡ����ע��ķ�ʽ����û��ѡ������ʾ�û�
                    //DbgPrintf("statusLoadLibrary %d, statusMemory %d, statusProcess %d, statusTypeWriting %d\n", statusLoadLibrary, statusMemory, statusProcess, statusTypeWriting);
                    if (statusLoadLibrary == NULL && statusMemory == NULL && statusProcess == NULL && statusTypeWriting == NULL && statusMemoryOther == NULL){
                        MessageBox(NULL, TEXT("��ѡ��ע��ķ�ʽ!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }

                    //���hLoadLibraryRadio��ѡ����ѡ��״̬��˵���û�������Զ���߳�ע��ķ�ʽ
                    if (statusLoadLibrary == BST_CHECKED){
                        //�ж��û��Ƿ��ڱ༭����������ֵ����û������ʾ�û�����
                        if (*(int*)szBuffer == 0){
                            MessageBox(NULL, TEXT("������DLL·��!"), TEXT("����"), MB_OK);
                            return TRUE;
                        }
                        int flag = RemoteThreadInjectionModule(pid, szBuffer);
                        if (flag > 0){
                            MessageBox(NULL, TEXT("Զ���߳�ע��ɹ�!"), TEXT("����"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("Զ���߳�ע��ʧ��!"), TEXT("����"), MB_OK);
                        }
                    //���hMemoryRadio��ѡ����ѡ��״̬��˵���û�������Զ���߳�ע��ķ�ʽ
                    }else if (statusMemory == BST_CHECKED){
                        int flag = 0;
                        gInjectByWriteMemory = RemoteDllInjectMemoryWrite(pid, &flag);
                        if (flag > 0){
                            MessageBox(NULL, TEXT("Զ���߳��ڴ�д��ɹ�!"), TEXT("����"), MB_OK);
                            MessageBox(NULL, TEXT("���������ж��ע��ģ�飬�벻Ҫ�ر�\n������֮���ٵ��ж�أ�������ֻ������\nֹĿ����̵ķ�ʽ��ж��ע��ģ��!"), TEXT("����"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("Զ���߳��ڴ�д��ʧ��!"), TEXT("����"), MB_OK);
                        }
                    }else if (statusProcess == BST_CHECKED){

                    }else if (statusTypeWriting == BST_CHECKED){

                    //���hMemoryOtherRadio��ѡ����ѡ��״̬��˵���û��������ڴ�д��ķ�ʽ����ʹ��LoadLibrary() �� ��������������޸�IAT�����ֵ��
                    }else if (statusMemoryOther == BST_CHECKED){
                        //�ж��û��Ƿ��ڱ༭����������ֵ����û������ʾ�û�����
                        if (*(int*)szBuffer == 0){
                            MessageBox(NULL, TEXT("������DLL·��!"), TEXT("����"), MB_OK);
                            return TRUE;
                        }
                        //���ڼ�¼ע��ģ����Ŀ������е��׵�ַ
                        DWORD dllAddrInTargetProcess = 0;
                        //���ڼ�¼ע��ģ���SizeOfImage
                        DWORD dllSizeOfImage = 0;
                        //��ע��ģ��ע�뵽Ŀ����̿ռ���
                        int flag = injectDllByWriteMemoryWithoutRemoteThread(szBuffer, pid, &dllAddrInTargetProcess, &dllSizeOfImage);
                        //��ע��ģ����׵�ַ����Ϊȫ�ֱ���
                        gDllAddrInTargetProcess = dllAddrInTargetProcess;
                        //��ע��ģ���SizeOfImage����Ϊȫ�ֱ���
                        gDllSizeOfImage = dllSizeOfImage;
                        //�ж�ģ���Ƿ�ע��ɹ�
                        if (flag > 0){
                            //ע��ɹ��Ļ����ͽ�DLLģ����Ŀ������е��׵�ַ��DLLģ���SizeOfImage��ʾ����
                            char str[1024] = {0};
                            sprintf(str, "ע���DLL��Ŀ������е��׵�ַ��0x%x \nע���DLL��SizeOfImage��0x%x", dllAddrInTargetProcess, dllSizeOfImage);
                            MessageBox(NULL, TEXT("�ڴ�д��ɹ�!"), TEXT("����"), MB_OK);
                            MessageBox(NULL, str, TEXT("����"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("�ڴ�д��ʧ��!"), TEXT("����"), MB_OK);
                        }
                    }
                    
                    return TRUE;
                }
                //����� ��ж�ء� ��ť֮��Ҫִ�еĴ���
                case IDC_BUTTON_startUninstall:{
                    //��ȡ ������ѡ��ť��pid�༭�� �� dll�༭�� �ľ��
                    HWND hLoadLibraryRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_LoadLibrary);
                    HWND hMemoryRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_memoryWrite);
                    HWND hProcessRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_loadProcess);
                    HWND hTypeWritingRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_typewriting);
                    HWND hMemoryOtherRadio = GetDlgItem(hwndDlgOfDLLInject, IDC_RADIO_WriteMemoryWithoutRemoteThread);
                    HWND hEditPid = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_PID_dll);
                    HWND hEditDllPath = GetDlgItem(hwndDlgOfDLLInject, IDC_EDIT_DllPath);
                    int statusMemoryOther = SendMessage(hMemoryOtherRadio, BM_GETCHECK, 0, 0);
                    
                    //��ȡ������ѡ���״̬
                    int statusLoadLibrary = SendMessage(hLoadLibraryRadio, BM_GETCHECK, 0, 0);
                    int statusMemory = SendMessage(hMemoryRadio, BM_GETCHECK, 0, 0);
                    int statusProcess = SendMessage(hProcessRadio, BM_GETCHECK, 0, 0);
                    int statusTypeWriting = SendMessage(hTypeWritingRadio, BM_GETCHECK, 0, 0);
                    
                    //��ȡpid�༭���е�ֵ��������ת��Ϊ����
                    char szBuffer[MAX_PATH] = {0};
                    memset(szBuffer, 0, MAX_PATH);
                    GetWindowText(hEditPid, szBuffer, MAX_PATH);
                    char* leftOvew = NULL;
                    DWORD pid = strtoul(szBuffer, &leftOvew, 10);;
                    
                    //��ȡdll�༭���е�ֵ
                    memset(szBuffer, 0, MAX_PATH);
                    GetWindowText(hEditDllPath, szBuffer, MAX_PATH);
                    
                    //�ж��û��Ƿ��ڱ༭����������ֵ����û������ʾ�û�����
                    if (pid <= 0){
                        MessageBox(NULL, TEXT("��������ȷ��Ŀ�����PID!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }

                    //�ж��û��Ƿ�ѡ����ע��ķ�ʽ����û��ѡ������ʾ�û�
                    //DbgPrintf("statusLoadLibrary %d, statusMemory %d, statusProcess %d, statusTypeWriting %d\n", statusLoadLibrary, statusMemory, statusProcess, statusTypeWriting);
                    if (statusLoadLibrary == NULL && statusMemory == NULL && statusProcess == NULL && statusTypeWriting == NULL && statusMemoryOther == NULL){
                        MessageBox(NULL, TEXT("��ѡ��ж�صķ�ʽ!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }

                    //���hLoadLibraryRadio��ѡ����ѡ��״̬��˵���û�������Զ���߳�ע��ķ�ʽ
                    if (statusLoadLibrary == BST_CHECKED){
                        //�ж��û��Ƿ��ڱ༭����������ֵ����û������ʾ�û�����
                        if (*(int*)szBuffer == 0){
                            MessageBox(NULL, TEXT("�������ж�ص�DLL����!"), TEXT("����"), MB_OK);
                            return TRUE;
                        }
                        DWORD dwHModule =  RemoteThreadUninstallModule(pid, szBuffer);
                        if (dwHModule == 1){
                            MessageBox(NULL, TEXT("ж�سɹ�!"), TEXT("����"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("ж��ʧ��!\nע�⣺��ж��ʱ��ֻ������ģ������Ƽ��ɣ���������·��!"), TEXT("����"), MB_OK);
                        }
                    //���hMemoryRadio��ѡ����ѡ��״̬��˵���û�������Զ���߳�ע��ķ�ʽ
                    }else if (statusMemory == BST_CHECKED){
                        //��ʼж��ע��ģ��
                        if (gInjectByWriteMemory.FlagOfAddr <= 0 || gInjectByWriteMemory.HRemoteThread <= 0 || gInjectByWriteMemory.ModuleOfAddr <= 0 || gInjectByWriteMemory.ModuleSizeOfImage <= 0){
                            MessageBox(NULL, TEXT("����δ�����ڴ�д��!"), TEXT("����"), MB_OK);
                            return TRUE;
                        }
                        int flag = RemoteUninstallModule(pid, gInjectByWriteMemory);
                        if (flag > 0){
                            MessageBox(NULL, TEXT("�ڴ�д��ж�سɹ�!"), TEXT("����"), MB_OK);
                        }else{
                            MessageBox(NULL, TEXT("�ڴ�д��ж��ʧ��!"), TEXT("����"), MB_OK);
                        }
                    }else if (statusProcess == BST_CHECKED){

                    }else if (statusTypeWriting == BST_CHECKED){

                    }else if (statusMemoryOther == BST_CHECKED){
                        MessageBox(NULL, TEXT("�ݲ�֧��ж��!"), TEXT("����"), MB_OK);
                    }

                    return TRUE;
                }
                //��� ���˳��� ��ť֮���˳�����
                case IDC_BUTTON_exitOfDll:{
                    EndDialog(hwndDlgOfDLLInject, 0);
                    return TRUE;
                }
            }
            break;
        }
        case WM_CLOSE:{
            EndDialog(hwndDlgOfDLLInject, 0);
            return TRUE;
        }
        
    }
    return FALSE;
}







