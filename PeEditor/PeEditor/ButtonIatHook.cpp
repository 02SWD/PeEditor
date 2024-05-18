// ButtonIatHook.cpp: implementation of the ButtonIatHook class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonIatHook.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
DWORD WINAPI ThreadIatHookProc(LPVOID lpParameter);
BOOL CALLBACK ButtonIatHookProc(HWND hwndDlgOfDLLInject, UINT uMsg, WPARAM wParam, LPARAM lParam);
int IatHook(IN DWORD pid, IN char* targetModuleName , IN char* targetFuncName, IN DWORD targetFuncOrdinal, IN HMODULE dllAddrInTargetProcess, IN char* nastyFuncName, IN DWORD nastyFuncOrdinal, OUT PDWORD oldFuncAddr);
int IatHook(IN DWORD pid, IN char* targetModuleName , IN char* targetFuncName, IN DWORD targetFuncOrdinal, IN DWORD FuncAddr, OUT PDWORD oldFuncAddr);

/**
 * Iat Hook ��ť�����������
 * @return [description]
 */
VOID buttonIatHook(){
    //����һ���߳�ר��ִ�� IAT HOOK �Ĵ���
    HANDLE hThread = ::CreateThread(NULL, 0, ThreadIatHookProc, NULL, 0, NULL);
    //�ر��߳̾��
    ::CloseHandle(hThread);
}

/**
 * Iat Hook �̵߳Ļص�����
 * @param  lpParameter �̻߳ص������Ĳ���
 * @return             [description]
 */
DWORD WINAPI ThreadIatHookProc(LPVOID lpParameter){
    //����һ���Ի�������û�д��븸�Ի���ľ�����Ա����ӶԻ���Ӱ�츸�Ի����ʹ�ã�
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_IATHOOK), NULL,  ButtonIatHookProc);
    return 0;
}

/**
 * IAT HOOK �Ի���Ĵ��ڻص��������ô���ID��IDD_DIALOG_IATHOOK��
 * @param  hwndDlgOfIatHook IAT HOOK�Ի���ľ��
 * @param  uMsg             ��Ϣ����
 * @param  wParam           WPARAM
 * @param  lParam           LPARAM
 * @return                  
 */
BOOL CALLBACK ButtonIatHookProc(HWND hwndDlgOfIatHook, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //��� �������Ͻǵĺ�� ֮���˳�����
        case WM_CLOSE:{
            EndDialog(hwndDlgOfIatHook, 0);
            return TRUE;
        }
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //��һ������Iat Hook�ķ�ʽ
                case IDC_BUTTON_IatHook_start:{
                    //��ȡ��һ��Iathook����ĸ����༭��ľ��
                    HWND hEdit_pid = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_pid);
                    HWND hEdit_targetModuleName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetModuleName);
                    HWND hEdit_targetFuncName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncName);
                    HWND hEdit_targetFuncOrdinal = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncOrdinal);
                    HWND hEdit_dllAddrInTargetProcess = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_dllAddrInTargetProcess);
                    HWND hEdit_nastyFuncName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_nastyFuncName);
                    HWND hEdit_nastyFuncOrdinal = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_nastyFuncOrdinal);

                    //�����ݴ�༭���е�����
                    char pid_Buffer[1024] = {0};
                    char targetModuleName_Buffer[1024] = {0};
                    char targetFuncName_Buffer[1024] = {0};
                    char targetFuncOrdinal_Buffer[1024] = {0};
                    char dllAddrInTargetProcess_Buffer[1024] = {0};
                    char nastyFuncName_Buffer[1024] = {0};
                    char nastyFuncOrdinal_Buffer[1024] = {0};

                    //��ȡ�����༭���е�����
                    GetWindowText(hEdit_pid, pid_Buffer, 1024);
                    GetWindowText(hEdit_targetModuleName, targetModuleName_Buffer, 1024);
                    GetWindowText(hEdit_targetFuncName, targetFuncName_Buffer, 1024);
                    GetWindowText(hEdit_targetFuncOrdinal, targetFuncOrdinal_Buffer, 1024);
                    GetWindowText(hEdit_dllAddrInTargetProcess, dllAddrInTargetProcess_Buffer, 1024);
                    GetWindowText(hEdit_nastyFuncName, nastyFuncName_Buffer, 1024);
                    GetWindowText(hEdit_nastyFuncOrdinal, nastyFuncOrdinal_Buffer, 1024);

                    //��pid�ַ�������10���Ƶķ�ʽת��Ϊ����
                    char* leftOvew_pid = NULL;
                    DWORD pid = strtoul(pid_Buffer, &leftOvew_pid, 10);

                    //��Ŀ�꺯���ĵ�������ַ�������16���Ƶķ�ʽת��Ϊ����
                    char* leftOvew_targetFuncOrdinal = NULL;
                    DWORD targetFuncOrdinal = strtoul(targetFuncOrdinal_Buffer, &leftOvew_targetFuncOrdinal, 16);

                    //��ע��ģ����׵�ַ�ַ�������16���Ƶķ�ʽת��Ϊ����
                    char* leftOvew_dllAddrInTargetProcess = NULL;
                    DWORD dllAddrInTargetProcess = strtoul(dllAddrInTargetProcess_Buffer, &leftOvew_dllAddrInTargetProcess, 16);

                    //�� Ҫ�滻Ϊ�ĺ����ĺ�����������ַ�������16���Ƶķ�ʽת��Ϊ����
                    char* leftOvew_nastyFuncOrdinal = NULL;
                    DWORD nastyFuncOrdinal = strtoul(nastyFuncOrdinal_Buffer, &leftOvew_nastyFuncOrdinal, 16);

                    //�ж��û��Ƿ������Ŀ����̵�pid
                    if (pid == 0){
                        MessageBox(NULL, TEXT("������Ŀ����̵�pid!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }

                    //�ж��û��Ƿ������˴��ٳֵ�Ŀ��ģ������
                    if (*(int*)targetModuleName_Buffer == 0){
                        MessageBox(NULL, TEXT("������Ҫ�ٳֵ�Ŀ��ģ������!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }
                    
                    //�ж��û��Ƿ������˴��ٳֵ�Ŀ�꺯���ĺ�������/�������
                    if (*(int*)targetFuncName_Buffer == 0 && targetFuncOrdinal == 0){
                        MessageBox(NULL, TEXT("����������ٳֵ�Ŀ�꺯���� ��������/�����������!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }
                    
                    //�����ٳֵ�Ŀ�꺯���ĺ������뺯���������ֻ����дһ��
                    if (*(int*)targetFuncName_Buffer != 0 && targetFuncOrdinal != 0){
                        MessageBox(NULL, TEXT("�����ٳֵ�Ŀ�꺯���ĺ������뺯���������ֻ����дһ��!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }
                    
                    //�ж��û��Ƿ�������Ŀ�������dllģ����׵�ַ
                    if (dllAddrInTargetProcess == 0){
                        MessageBox(NULL, TEXT("������Ŀ�������dllģ����׵�ַ!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }

                    //�ж��û��Ƿ������� Ҫ�滻Ϊ�ĺ����ĺ�������/�������
                    if (*(int*)nastyFuncName_Buffer == 0 && nastyFuncOrdinal == 0){
                        MessageBox(NULL, TEXT("������Ҫ�滻Ϊ�ĺ����� ��������/�����������!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }
                    
                    //Ҫ�滻�ĺ����ĺ������뺯���������ֻ����дһ��
                    if (*(int*)nastyFuncName_Buffer != 0 && nastyFuncOrdinal != 0){
                        MessageBox(NULL, TEXT("Ҫ�滻�ĺ����ĺ������뺯���������ֻ����дһ��!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }
                    //���ڼ�¼IatHook֮ǰ��IAT������ֵ
                    DWORD oldFuncAddr = 0;
                    //��ʼIAT HOOK
                    int iatHookFlag = IatHook(pid, targetModuleName_Buffer, targetFuncName_Buffer, targetFuncOrdinal, (HMODULE)dllAddrInTargetProcess, nastyFuncName_Buffer, nastyFuncOrdinal, &oldFuncAddr);
                    //�ж�IAT HOOK�Ƿ�ִ�гɹ�
                    if (iatHookFlag > 0){
                        //��ִ�гɹ�����IAT�����ԭ����ֵ����Ϊȫ�ֱ�����������ֵ��ʾ���û�
                        gOldFuncAddr = oldFuncAddr;
                        HWND hEdit_oldFuncAddr = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_oldFuncAddrValue);
                        char oldFuncAddrBuffer[10] = {0};
                        ultoa(oldFuncAddr, oldFuncAddrBuffer, 16);
                        SetWindowText(hEdit_oldFuncAddr, oldFuncAddrBuffer);
                        MessageBox(NULL, TEXT("IAT HOOK �ɹ�!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }else{
                        MessageBox(NULL, TEXT("IAT HOOK ʧ��!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }
                }
                //�ڶ�������Iat Hook�ķ�ʽ
                case IDC_BUTTON_IatHook_start_method2:{
                    //��ȡ�ڶ���Iathook����ĸ����༭��ľ��
                    HWND hEdit_pid = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_pid_Method2);
                    HWND hEdit_targetModuleName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetModuleName_Method2);
                    HWND hEdit_targetFuncName = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncName_Method2);
                    HWND hEdit_targetFuncOrdinal = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncOrdinal_Method2);
                    HWND hEdit_funcAddr = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_IatHook_targetFuncOrdinalMethod2);
                    
                    //�����ݴ�༭���е�����
                    char pid_Buffer[1024] = {0};
                    char targetModuleName_Buffer[1024] = {0};
                    char targetFuncName_Buffer[1024] = {0};
                    char targetFuncOrdinal_Buffer[1024] = {0};
                    char funcAddr_Buffer[1024] = {0};

                    //��ȡ�����༭���е�����
                    GetWindowText(hEdit_pid, pid_Buffer, 1024);
                    GetWindowText(hEdit_targetModuleName, targetModuleName_Buffer, 1024);
                    GetWindowText(hEdit_targetFuncName, targetFuncName_Buffer, 1024);
                    GetWindowText(hEdit_targetFuncOrdinal, targetFuncOrdinal_Buffer, 1024);
                    GetWindowText(hEdit_funcAddr, funcAddr_Buffer, 1024);

                    //��pid�ַ�������10���Ƶķ�ʽת��Ϊ����
                    char* leftOvew_pid = NULL;
                    DWORD pid = strtoul(pid_Buffer, &leftOvew_pid, 10);

                    //��Ŀ�꺯���ĵ�������ַ�������16���Ƶķ�ʽת��Ϊ����
                    char* leftOvew_targetFuncOrdinal = NULL;
                    DWORD targetFuncOrdinal = strtoul(targetFuncOrdinal_Buffer, &leftOvew_targetFuncOrdinal, 16);

                    //�� Ҫ�滻Ϊ�ĺ�����ַ���ַ�������16���Ƶķ�ʽת��Ϊ����
                    char* leftOvew_funcAddr = NULL;
                    DWORD funcAddr = strtoul(funcAddr_Buffer, &leftOvew_funcAddr, 16);

                    //�ж��û��Ƿ������Ŀ����̵�pid
                    if (pid == 0){
                        MessageBox(NULL, TEXT("������Ŀ����̵�pid!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }

                    //�ж��û��Ƿ������˴��ٳֵ�Ŀ��ģ������
                    if (*(int*)targetModuleName_Buffer == 0){
                        MessageBox(NULL, TEXT("������Ҫ�ٳֵ�Ŀ��ģ������!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }
                    
                    //�ж��û��Ƿ������˴��ٳֵ�Ŀ�꺯���ĺ�������/�������
                    if (*(int*)targetFuncName_Buffer == 0 && targetFuncOrdinal == 0){
                        MessageBox(NULL, TEXT("����������ٳֵ�Ŀ�꺯���� ��������/�����������!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }
                    
                    //�����ٳֵ�Ŀ�꺯���ĺ������뺯���������ֻ����дһ��
                    if (*(int*)targetFuncName_Buffer != 0 && targetFuncOrdinal != 0){
                        MessageBox(NULL, TEXT("�����ٳֵ�Ŀ�꺯���ĺ������뺯���������ֻ����дһ��!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }

                    //�ж��û��Ƿ�������Ҫ�滻Ϊ�ĺ�����ַ
                    if (funcAddr == 0){
                        MessageBox(NULL, TEXT("������Ҫ�滻Ϊ�ĺ�����ַ!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }

                    //ԭ����IAT�����ֵ
                    DWORD oldFuncAddr = 0;
                    //��ʼ����IAT HOOK
                    int IatHookFlag = IatHook(pid, targetModuleName_Buffer, targetFuncName_Buffer, targetFuncOrdinal, funcAddr, &oldFuncAddr);
                    //�ж�IAT HOOK�Ƿ�ִ�гɹ�
                    if (IatHookFlag > 0){
                        //��ִ�гɹ�����IAT�����ԭ����ֵ����Ϊȫ�ֱ�����������ֵ��ʾ���û�
                        gOldFuncAddr = oldFuncAddr;
                        HWND hEdit_oldFuncAddr = GetDlgItem(hwndDlgOfIatHook, IDC_EDIT_oldFuncAddrValue_Method2);
                        char oldFuncAddrBuffer[10] = {0};
                        ultoa(oldFuncAddr, oldFuncAddrBuffer, 16);
                        SetWindowText(hEdit_oldFuncAddr, oldFuncAddrBuffer);
                        MessageBox(NULL, TEXT("IAT HOOK �ɹ�!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }else{
                        MessageBox(NULL, TEXT("IAT HOOK ʧ��!"), TEXT("����"), MB_OK);
                        return TRUE;
                    }
                }
            }

        }
    }

    return FALSE;
}


/**
 * �޸�Ŀ������е�Ŀ��ģ���е�Ŀ�꺯������IAT��������Ӧ�ĺ�����ַ���޸�֮��ĺ�����ַ��ΪĿ�����������dllģ���еĺ����ĺ�����ַ����dllģ��һ��Ϊ����ע���dllģ�飩
 * @param  pid                    Ŀ����̵�pid
 * @param  targetModuleName       �����ٳֵ�ģ������
 * @param  targetFuncName         targetModuleNameģ���д����ٳֵĺ����ĺ������ƣ������ִ�Сд��
 * @param  targetFuncOrdinal      targetModuleNameģ���д����ٳֵĺ����ĵ�����ţ�ʮ�����ơ�ʮ���ƾ��ɣ�
 * @param  dllAddrInTargetProcess Ŀ�������dllģ����׵�ַ����dllģ��һ��������ע���dll��
 * @param  nastyFuncName          Ҫ�滻�ɵĺ����ĺ������ƣ������ִ�Сд��
 * @param  nastyFuncOrdinal       Ҫ�滻�ɵĺ����ĵ�����ţ�ʮ�����ơ�ʮ���ƾ��ɣ�
 * @param  oldFuncAddr            ԭ���ĺ�����ַ���滻֮ǰ��IAT�����ֵ��
 * @return                        -1 Զ�̽��̴�ʧ��
 *                                -2 Զ�̽����ڴ��ȡʧ��
 *                                -3 Զ�̽����ڴ��ȡʧ��
 *                                -4 Զ�̽����ڴ�д��ʧ��
 *                                -5 Զ�̽����ڴ�д��ʧ��
 *                                -6 Ŀ����̵ĵ�һ��ģ����û���û���Ҫ�ٳֵĺ���
 *                                -7 ���ٳֺ����ĺ������뵼����Ų���ͬʱ����
 *                                -8 Ҫ�滻Ϊ�ĺ����ĺ������뵼����Ų���ͬʱ����
 *                                -9 IAT�����ԭ����ֵ��ȡʧ��
 *                                -10 IAT�����ԭ����ֵ��ȡʧ��
 *                                 1 IAT�޸ĳɹ�
 * ע�⣺
 *      1��targetFuncName���� �� targetFuncOrdinal��������ͬʱ��ֵ����������һ������ΪNULL��
 *      2��nastyFuncName���� �� nastyFuncOrdinal����Ҳ����ͬʱ��ֵ����������һ������ΪNULL��
 * ���������ã�
 *      ��Ŀ������У�targetModuleNameģ���е�targetFuncName/targetFuncOrdinal���������ú�������Ӧ��IAT����ֵ(��Ŀ�꺯���ĺ�����ַ)��
 *      �޸�ΪdllAddrInTargetProcessģ����nastyFuncName/nastyFuncOrdinal�����ĺ�����ַ��
 */
int IatHook(IN DWORD pid, IN char* targetModuleName , IN char* targetFuncName, IN DWORD targetFuncOrdinal, IN HMODULE dllAddrInTargetProcess, IN char* nastyFuncName, IN DWORD nastyFuncOrdinal, OUT PDWORD oldFuncAddr){
    if(targetFuncName != NULL && targetFuncOrdinal != 0){
        printf("���ٳֺ����ĺ������뵼����Ų���ͬʱ���룡\n");
        return -7;
    }

    if(nastyFuncName != NULL && nastyFuncOrdinal != 0){
        printf("Ҫ�滻Ϊ�ĺ����ĺ������뵼����Ų���ͬʱ���룡\n");
        return -8;
    }

    //����pid��ȡĿ����̵ľ��
    HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hRemoteProcess == NULL){
        printf("Զ�̽��̴�ʧ��!\n");
        return -1;
    }
    //����pidΪĿ������������
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    //�����ݴ�Ŀ������е�ģ����Ϣ
    MODULEENTRY32 mem32 = {0};
    mem32.dwSize = sizeof(MODULEENTRY32);
    //��ȡĿ������е�һ��ģ�����Ϣ
    BOOL isNext2 = Module32First(hSnap, &mem32);
    DWORD a = GetLastError();
    //��¼Ŀ��ģ����׵�ַ
    DWORD targetProcessOfModuleAddr = NULL;
    while (isNext2){
        if (stricmp(targetModuleName, mem32.szModule) == 0){
            targetProcessOfModuleAddr = (DWORD)mem32.hModule;
            break;
        }
        isNext2 = Module32Next(hSnap, &mem32);
    }
    //�رտ��վ��
    CloseHandle(hSnap);

    //������������Ҫ��λ��Ŀ��ģ���IAT���У����Ŀ�꺯���ĺ�����ַ��IAT����ĵ�ַ��Ȼ�󽫸�iat�����ֵ�޸�Ϊ�����Լ������ĺ�����ַ
    //���ڴ�ʱ���ǲ���֪��Ŀ��ģ��ImageBuffer�Ĵ�С�����������������㹻���� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ �Ŀռ䣨��Ϊ����ռ�Ŀռ��ǹ̶��ģ���Ȼ���ȡĿ��ģ�飬���ݿ�ѡpeͷ�е�SizeOfImage�ó�Ŀ��ģ���ImgaeBuffer��С
    //����һ��������ǣ���Ȼ dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ�Ĵ�С�ǹ̶��ģ�����dosͷ��ntͷ֮�������������ռ�Ŀռ��ǲ��̶��ģ���������Ҫ��ȡ��dosͷ���ݣ����������������ռ�Ŀռ� 
    //Ϊdosͷ�����ڴ�ռ�
    char* pDosHeaderSizeSpace = (char*)malloc(64);
    if (pDosHeaderSizeSpace == NULL){
        printf("�ڴ�����ʧ��!\n");
        return -2;
    }
    //��ʼ��
    memset(pDosHeaderSizeSpace, 0, 64);
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, pDosHeaderSizeSpace, 64, NULL) == 0){
        printf("DOSͷ��ȡʧ��!\n");
        free(pDosHeaderSizeSpace);
        return -3;
    }
    //����dosͷ
    PIMAGE_DOS_HEADER pDosHeaderTemp = (PIMAGE_DOS_HEADER)pDosHeaderSizeSpace;
    //sizeOfRubbishSpare��¼����������ռ�ռ�Ĵ�С
    DWORD sizeOfRubbishSpare = pDosHeaderTemp->e_lfanew - 0x40;
    //�Ѿ������������������ռ�ռ��С�������ͷŵ�dosͷ������ռ���ڴ���
    free(pDosHeaderSizeSpace);

    // sizeOfBeforeSection = dosͷ��С + �������ݴ�С + pe��־��С + ��׼peͷ��С + ��ѡpeͷ��С
    DWORD sizeOfBeforeSection = 64 + sizeOfRubbishSpare + 4 + 20 + 224;
    char* partExeImageBuffer = (char*)malloc(sizeOfBeforeSection);
    //��ʼ���ڴ�ռ�
    memset(partExeImageBuffer, 0, sizeOfBeforeSection);
    //��Ŀ��ģ��� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ ��ȡ���ڴ���
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, partExeImageBuffer, sizeOfBeforeSection, NULL) == 0){
        printf("Զ�̽����ڴ��ȡʧ��!\n");
        free(partExeImageBuffer);
        CloseHandle(hRemoteProcess);
        return -2;
    }
    
    //����Ŀ��ģ��� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)partExeImageBuffer;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //���ݿ�ѡpeͷ����ȡĿ��ģ���ImageBuffer�Ĵ�С
    DWORD targetProcessOfModuleSizeOfImage = pOptionalHeader->SizeOfImage;
    //��Ϊ�����Ѿ���֪Ŀ��ģ��ImageBuffer�Ĵ�С������partImageBuffer�ڴ���Ա��ͷ���
    free(partExeImageBuffer);

    //�����㹻���� Ŀ��ģ��ImageBuffer��С ���ڴ�ռ�
    char* ImageBuffer = (char*)malloc(targetProcessOfModuleSizeOfImage);
    memset(ImageBuffer, 0, targetProcessOfModuleSizeOfImage);
    //��Ŀ��ģ���ȡ����������ڴ�ռ���
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, ImageBuffer, targetProcessOfModuleSizeOfImage, NULL) == 0){
        printf("Զ�̽����ڴ��ȡʧ��!\n");
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -3;
    }
    //���½��� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ
    pDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;
    pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //��λ��һ��������λ��
    DWORD importTableRva = pOptionalHeader->DataDirectory[1].VirtualAddress;
    PIMAGE_IMPORT_DESCRIPTOR importTableAddr = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)ImageBuffer + importTableRva);
    BOOL flag = FALSE, isSuccess = FALSE;
    while (importTableAddr->Name != 0){
        //�� IatTableAddr ָ��ָ��iat����׵�ַ
        int* IatTableAddr = (int*)((DWORD)ImageBuffer + importTableAddr->FirstThunk);
        //�� IntTableAddr ָ��ָ��int����׵�ַ
        int* IntTableAddr = (int*)((DWORD)ImageBuffer + importTableAddr->OriginalFirstThunk);
        
        //ѭ������INT�����޸�Ŀ�꺯������Ӧ��iat�����ֵ
        while (*IntTableAddr != 0){
            //�� ��INT����ֵ�����λΪ1�����û��������Ŀ�꺯���ĵ�����ţ�����ݵ�����Ž����޸�
            if ((*IntTableAddr & 0x80000000) == 0x80000000 && targetFuncOrdinal != 0){
                //�жϴ�ʱINT�����15λ��ֵ�Ƿ�����û�����ĵ�����ţ������жϵ�ǰ�ĺ����Ƿ����û���Ҫ�ٳֵĺ�����
                if ((DWORD)(*IntTableAddr & 0x7fffffff) == targetFuncOrdinal){
                    //��¼�޸�֮ǰ��IAT�����ֵ
                    DWORD oldFuncAddrTemp = 0;
                    if(0 == ReadProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &oldFuncAddrTemp, 4, NULL)){
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -9;
                    }
                    //���ô�IAT�����ԭ����ֵ����
                    *oldFuncAddr = oldFuncAddrTemp;

                    //��ȡҪ�滻�ɵĺ����ĺ�����ַ
                    DWORD myFunc = 0;
                    myGetProcAddressCrossProcess(pid, (HMODULE)dllAddrInTargetProcess, NULL, nastyFuncOrdinal, &myFunc);
                    //�޸� Ŀ����� ��һ��ģ���IAT���У�Ŀ�꺯������Ӧ��IAT�������IAT�����ֵ�޸�Ϊ����Ҫ�滻�ɵĺ����ĺ�����ַ
                    if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &myFunc, 4, NULL)){
                        printf("Զ�̽����ڴ�д��ʧ��!\n");
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -4;
                    }
                    flag = TRUE;
                    break;
                }
            }
            //�� ��INT��������λ��Ϊ1�����û��������Ŀ�꺯���ĺ�����������ݺ����������޸�
            if ((*IntTableAddr & 0x80000000) != 0x80000000 && targetFuncName != 0){
                PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)ImageBuffer + *IntTableAddr);
                //�жϵ�ǰ�ĺ����Ƿ����û���Ҫ�ٳֵĺ���
                if (stricmp((char*)(funcName->Name),targetFuncName) == 0){
                    //��¼�޸�֮ǰ��IAT�����ֵ
                    DWORD oldFuncAddrTemp = 0;
                    if(0 == ReadProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &oldFuncAddrTemp, 4, NULL)){
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -10;
                    }
                    //���ô�IAT�����ԭ����ֵ����
                    *oldFuncAddr = oldFuncAddrTemp;

                    //��ȡҪ�滻�ɵĺ����ĺ�����ַ
                    DWORD myFunc = 0;
                    myGetProcAddressCrossProcess(pid, (HMODULE)dllAddrInTargetProcess, nastyFuncName, NULL, &myFunc);
                    //�޸� Ŀ����� ��һ��ģ���IAT���У�Ŀ�꺯������Ӧ��IAT�������IAT�����ֵ�޸�Ϊ����Ҫ�滻�ɵĺ����ĺ�����ַ
                    if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &myFunc, 4, NULL)){
                        printf("Զ�̽����ڴ�д��ʧ��!\n");
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -5;
                    }
                    flag = TRUE;
                    break;
                }
            }
            //IatTableAddrָ��ָ����һ��iat�����
            IatTableAddr++;
            //IntTableAddrָ��ָ����һ��int�����
            IntTableAddr++;
        }
        //�жϱ��ε�IAT�����Ƿ�����û���Ҫ�ٳֵĺ��������Ƿ�ٳֳɹ����������ΪTRUE��˵�����ε�IAT��û���û���Ҫ�ٳֵĺ�����
        if (flag == TRUE){
            //���ɹ��޸ģ���isSuccess��ΪTRUE��������ѭ��
            isSuccess = TRUE;
            break;
        }
        //��importTableAddrָ��ָ����һ�������
        importTableAddr++;
    }
    //���ΪFALSE��˵��Ŀ����̵ĵ�һ��ģ����û���û���Ҫ�ٳֵĺ���
    if (isSuccess == FALSE){
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -6;
    }
    //�ͷ���Դ
    free(ImageBuffer);
    CloseHandle(hRemoteProcess);
    return 1;
}

/**
 * �޸�Ŀ������е�Ŀ��ģ���е�Ŀ�꺯������IAT��������Ӧ�ĺ�����ַ��Ϊ�����Լ�����ĺ�����ַ
 * @param  pid               Ŀ����̵�pid
 * @param  targetModuleName  �����ٳֵ�ģ������
 * @param  targetFuncName    targetModuleNameģ���д����ٳֵĺ����ĺ������ƣ������ִ�Сд��
 * @param  targetFuncOrdinal targetModuleNameģ���д����ٳֵĺ����ĵ�����ţ�ʮ�����ơ�ʮ���ƾ��ɣ�
 * @param  FuncAddr          Ҫ�޸�Ϊ�ĺ�����ַ
 * @param  oldFuncAddr       ԭ���ĺ�����ַ���滻֮ǰ��IAT�����ֵ��
 * @return                   -1 Զ�̽��̴�ʧ��
 *                           -2 Զ�̽����ڴ��ȡʧ��
 *                           -3 Զ�̽����ڴ��ȡʧ��
 *                           -4 Զ�̽����ڴ�д��ʧ��
 *                           -5 Զ�̽����ڴ�д��ʧ��
 *                           -6 Ŀ����̵ĵ�һ��ģ����û���û���Ҫ�ٳֵĺ���
 *                           -7 ���ٳֺ����ĺ������뵼����Ų���ͬʱ����
 *                           -8 Ҫ�滻Ϊ�ĺ����ĺ������뵼����Ų���ͬʱ����
 *                           -9 IAT�����ԭ����ֵ��ȡʧ��
 *                           -10 IAT�����ԭ����ֵ��ȡʧ��
 *                            1 IAT�޸ĳɹ�
 * ע�⣺
 *      1��targetFuncName���� �� targetFuncOrdinal��������ͬʱ��ֵ����������һ������ΪNULL��
 */
int IatHook(IN DWORD pid, IN char* targetModuleName , IN char* targetFuncName, IN DWORD targetFuncOrdinal, IN DWORD FuncAddr, OUT PDWORD oldFuncAddr){
    if(targetFuncName != NULL && targetFuncOrdinal != 0){
        printf("���ٳֺ����ĺ������뵼����Ų���ͬʱ���룡\n");
        return -7;
    }

    //����pid��ȡĿ����̵ľ��
    HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hRemoteProcess == NULL){
        printf("Զ�̽��̴�ʧ��!\n");
        return -1;
    }
    //����pidΪĿ������������
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    //�����ݴ�Ŀ������е�ģ����Ϣ
    MODULEENTRY32 mem32 = {0};
    mem32.dwSize = sizeof(MODULEENTRY32);
    //��ȡĿ������е�һ��ģ�����Ϣ
    BOOL isNext2 = Module32First(hSnap, &mem32);
    DWORD a = GetLastError();
    //��¼Ŀ��ģ����׵�ַ
    DWORD targetProcessOfModuleAddr = NULL;
    while (isNext2){
        if (stricmp(targetModuleName, mem32.szModule) == 0){
            targetProcessOfModuleAddr = (DWORD)mem32.hModule;
            break;
        }
        isNext2 = Module32Next(hSnap, &mem32);
    }
    //�رտ��վ��
    CloseHandle(hSnap);

    //������������Ҫ��λ��Ŀ��ģ���IAT���У����Ŀ�꺯���ĺ�����ַ��IAT����ĵ�ַ��Ȼ�󽫸�iat�����ֵ�޸�Ϊ�����Լ������ĺ�����ַ
    //���ڴ�ʱ���ǲ���֪��Ŀ��ģ��ImageBuffer�Ĵ�С�����������������㹻���� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ �Ŀռ䣨��Ϊ����ռ�Ŀռ��ǹ̶��ģ���Ȼ���ȡĿ��ģ�飬���ݿ�ѡpeͷ�е�SizeOfImage�ó�Ŀ��ģ���ImgaeBuffer��С
    //����һ��������ǣ���Ȼ dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ�Ĵ�С�ǹ̶��ģ�����dosͷ��ntͷ֮�������������ռ�Ŀռ��ǲ��̶��ģ���������Ҫ��ȡ��dosͷ���ݣ����������������ռ�Ŀռ� 
    //Ϊdosͷ�����ڴ�ռ�
    char* pDosHeaderSizeSpace = (char*)malloc(64);
    if (pDosHeaderSizeSpace == NULL){
        printf("�ڴ�����ʧ��!\n");
        return -2;
    }
    //��ʼ��
    memset(pDosHeaderSizeSpace, 0, 64);
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, pDosHeaderSizeSpace, 64, NULL) == 0){
        printf("DOSͷ��ȡʧ��!\n");
        free(pDosHeaderSizeSpace);
        return -3;
    }
    //����dosͷ
    PIMAGE_DOS_HEADER pDosHeaderTemp = (PIMAGE_DOS_HEADER)pDosHeaderSizeSpace;
    //sizeOfRubbishSpare��¼����������ռ�ռ�Ĵ�С
    DWORD sizeOfRubbishSpare = pDosHeaderTemp->e_lfanew - 0x40;
    //�Ѿ������������������ռ�ռ��С�������ͷŵ�dosͷ������ռ���ڴ���
    free(pDosHeaderSizeSpace);

    // sizeOfBeforeSection = dosͷ��С + �������ݴ�С + pe��־��С + ��׼peͷ��С + ��ѡpeͷ��С
    DWORD sizeOfBeforeSection = 64 + sizeOfRubbishSpare + 4 + 20 + 224;
    char* partExeImageBuffer = (char*)malloc(sizeOfBeforeSection);
    //��ʼ���ڴ�ռ�
    memset(partExeImageBuffer, 0, sizeOfBeforeSection);
    //��Ŀ��ģ��� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ ��ȡ���ڴ���
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, partExeImageBuffer, sizeOfBeforeSection, NULL) == 0){
        printf("Զ�̽����ڴ��ȡʧ��!\n");
        free(partExeImageBuffer);
        CloseHandle(hRemoteProcess);
        return -2;
    }
    
    //����Ŀ��ģ��� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)partExeImageBuffer;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //���ݿ�ѡpeͷ����ȡĿ��ģ���ImageBuffer�Ĵ�С
    DWORD targetProcessOfModuleSizeOfImage = pOptionalHeader->SizeOfImage;
    //��Ϊ�����Ѿ���֪Ŀ��ģ��ImageBuffer�Ĵ�С������partImageBuffer�ڴ���Ա��ͷ���
    free(partExeImageBuffer);

    //�����㹻���� Ŀ��ģ��ImageBuffer��С ���ڴ�ռ�
    char* ImageBuffer = (char*)malloc(targetProcessOfModuleSizeOfImage);
    memset(ImageBuffer, 0, targetProcessOfModuleSizeOfImage);
    //��Ŀ��ģ���ȡ����������ڴ�ռ���
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)targetProcessOfModuleAddr, ImageBuffer, targetProcessOfModuleSizeOfImage, NULL) == 0){
        printf("Զ�̽����ڴ��ȡʧ��!\n");
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -3;
    }
    //���½��� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ
    pDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;
    pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //��λ��һ��������λ��
    DWORD importTableRva = pOptionalHeader->DataDirectory[1].VirtualAddress;
    PIMAGE_IMPORT_DESCRIPTOR importTableAddr = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)ImageBuffer + importTableRva);
    BOOL flag = FALSE, isSuccess = FALSE;
    while (importTableAddr->Name != 0){
        //�� IatTableAddr ָ��ָ��iat����׵�ַ
        int* IatTableAddr = (int*)((DWORD)ImageBuffer + importTableAddr->FirstThunk);
        //�� IntTableAddr ָ��ָ��int����׵�ַ
        int* IntTableAddr = (int*)((DWORD)ImageBuffer + importTableAddr->OriginalFirstThunk);
        
        //ѭ������INT�����޸�Ŀ�꺯������Ӧ��iat�����ֵ
        while (*IntTableAddr != 0){
            //�� ��INT����ֵ�����λΪ1�����û��������Ŀ�꺯���ĵ�����ţ�����ݵ�����Ž����޸�
            if ((*IntTableAddr & 0x80000000) == 0x80000000 && targetFuncOrdinal != 0){
                //�жϴ�ʱINT�����15λ��ֵ�Ƿ�����û�����ĵ�����ţ������жϵ�ǰ�ĺ����Ƿ����û���Ҫ�ٳֵĺ�����
                if ((DWORD)(*IntTableAddr & 0x7fffffff) == targetFuncOrdinal){
                    //��¼�޸�֮ǰ��IAT�����ֵ
                    DWORD oldFuncAddrTemp = 0;
                    if(0 == ReadProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &oldFuncAddrTemp, 4, NULL)){
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -9;
                    }
                    //���ô�IAT�����ԭ����ֵ����
                    *oldFuncAddr = oldFuncAddrTemp;

                    //��ȡҪ�滻�ɵĺ����ĺ�����ַ
                    DWORD myFunc = FuncAddr;
                    //�޸� Ŀ����� ��һ��ģ���IAT���У�Ŀ�꺯������Ӧ��IAT�������IAT�����ֵ�޸�Ϊ����Ҫ�滻�ɵĺ����ĺ�����ַ
                    if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &myFunc, 4, NULL)){
                        printf("Զ�̽����ڴ�д��ʧ��!\n");
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -4;
                    }
                    flag = TRUE;
                    break;
                }
            }
            //�� ��INT��������λ��Ϊ1�����û��������Ŀ�꺯���ĺ�����������ݺ����������޸�
            if ((*IntTableAddr & 0x80000000) != 0x80000000 && targetFuncName != 0){
                PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)ImageBuffer + *IntTableAddr);
                //�жϵ�ǰ�ĺ����Ƿ����û���Ҫ�ٳֵĺ���
                if (stricmp((char*)(funcName->Name),targetFuncName) == 0){
                    //��¼�޸�֮ǰ��IAT�����ֵ
                    DWORD oldFuncAddrTemp = 0;
                    if(0 == ReadProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &oldFuncAddrTemp, 4, NULL)){
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -10;
                    }
                    //���ô�IAT�����ԭ����ֵ����
                    *oldFuncAddr = oldFuncAddrTemp;

                    //��ȡҪ�滻�ɵĺ����ĺ�����ַ
                    DWORD myFunc = FuncAddr;
                    //�޸� Ŀ����� ��һ��ģ���IAT���У�Ŀ�꺯������Ӧ��IAT�������IAT�����ֵ�޸�Ϊ����Ҫ�滻�ɵĺ����ĺ�����ַ
                    if(0 == WriteProcessMemory(hRemoteProcess, (LPVOID)((DWORD)IatTableAddr - (DWORD)ImageBuffer + targetProcessOfModuleAddr), &myFunc, 4, NULL)){
                        printf("Զ�̽����ڴ�д��ʧ��!\n");
                        free(ImageBuffer);
                        CloseHandle(hRemoteProcess);
                        return -5;
                    }
                    flag = TRUE;
                    break;
                }
            }
            //IatTableAddrָ��ָ����һ��iat�����
            IatTableAddr++;
            //IntTableAddrָ��ָ����һ��int�����
            IntTableAddr++;
        }
        //�жϱ��ε�IAT�����Ƿ�����û���Ҫ�ٳֵĺ��������Ƿ�ٳֳɹ����������ΪTRUE��˵�����ε�IAT��û���û���Ҫ�ٳֵĺ�����
        if (flag == TRUE){
            //���ɹ��޸ģ���isSuccess��ΪTRUE��������ѭ��
            isSuccess = TRUE;
            break;
        }
        //��importTableAddrָ��ָ����һ�������
        importTableAddr++;
    }
    //���ΪFALSE��˵��Ŀ����̵ĵ�һ��ģ����û���û���Ҫ�ٳֵĺ���
    if (isSuccess == FALSE){
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -6;
    }
    //�ͷ���Դ
    free(ImageBuffer);
    CloseHandle(hRemoteProcess);
    return 1;
}