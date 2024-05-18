// ButtonOptionPeView.cpp: implementation of the ButtonOptionPeView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonOptionPeView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
BOOL CALLBACK ButtonOptionPeViewProc(HWND hwndDlgOfOptionPeView, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID GetOptionalPeInfo(HWND hwndDlgOfOptionPeView, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID GetOptionPeHeaders(IN LPVOID pFileBuffer, OUT PIMAGE_OPTIONAL_HEADER32* pOptionalHeader);

/**
 * ��� IDC_BUTTON_OptionPeVew ��ť��Ĵ�����루������� ����ѡPEͷ�� ��ť֮��Ҫִ�еĴ��룩
 * @param  hwndDlgOfPeView �����ھ�� (��������PeView���ڵľ�����ô��ڵ�ID��IDD_DIALOG_PeViewMainDlg)
 */
VOID ButtonOptionPeViewCode(HWND hwndDlgOfPeView){
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_OptionPeView), hwndDlgOfPeView,  ButtonOptionPeViewProc);
}


/**
 * OptionPeView�Ի���Ĵ��ڻص��������ô���ID��IDD_DIALOG_OptionPeView��
 * @param  hwndDlgOfOptionPeView OptionPeView���ڵľ��
 * @param  uMsg                  ��Ϣ����
 * @param  wParam                WPARAM
 * @param  lParam                LPARAM
 * @return                       
 */
BOOL CALLBACK ButtonOptionPeViewProc(HWND hwndDlgOfOptionPeView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        case WM_INITDIALOG:{
			//�������ô��ڱ��⣬�ﵽ������ǰ���ڱ��� - �򿪵��ļ�·���� ��Ч��
			ResetWindowTitle(hwndDlgOfOptionPeView, gSzFileNameOfPeView);
            //����GetOptionalPeInfo()����������ѡpeͷ����Ϣ��ʾ����
            GetOptionalPeInfo(hwndDlgOfOptionPeView, uMsg, wParam, lParam);
            return TRUE;
        }
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfOptionPeView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //��д�˳���ť�Ĵ������
                case IDC_BUTTON_OptionPeViewExit:{
                    //�˳�����
                    EndDialog(hwndDlgOfOptionPeView, 0);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/**
 * ��ȡ��ѡPEͷ��Ϣ����������ʾ����
 * @param  hwndDlgOfOptionPeView OptionPeView���ڵľ�����ô���ID��IDD_DIALOG_OptionPeView��
 * @param  uMsg                  ��Ϣ����
 * @param  wParam                WPARAM
 * @param  lParam                LPARAM
 */
VOID GetOptionalPeInfo(HWND hwndDlgOfOptionPeView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //��װ��pFileBufferָ���FileBuffer��С�ı���
    FILEINFOR file;
    //��ȡPE�ļ�
    file = readFile(gSzFileNameOfPeView);
    //���ڴ洢ָ���ѡpeͷ��ָ��
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = NULL;
    //��ȡ ��ѡPEͷ ��λ��
    GetOptionPeHeaders(file.p, &pOptionalHeader);
    
    //��ȡOptionPeView�Ի��������б༭��ľ��
    HWND hEdit_Magic = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_Magic);
    HWND hEdit_MajorLinkerVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MajorLinkerVersion);
    HWND hEdit_MinorLinkerVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MinorLinkerVersion);
    HWND hEdit_SizeOfCode = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfCode);
    HWND hEdit_SizeOfInitializedData = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfInitializedData);
    HWND hEdit_SizeOfUninitializedData = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfUninitializedData);
    HWND hEdit_AddressOfEntryPoint = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_AddressOfEntryPoint);
    HWND hEdit_BaseOfCode = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_BaseOfCode);
    HWND hEdit_BaseOfData = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_BaseOfData);
    HWND hEdit_ImageBase = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_ImageBase);

    HWND hEdit_SectionAlignment = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SectionAlignment);
    HWND hEdit_FileAlignment = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_FileAlignment);
    HWND hEdit_MajorOperatingSystemVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MajorOperatingSystemVersion);
    HWND hEdit_MinorOperatingSystemVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MinorOperatingSystemVersion);
    HWND hEdit_MajorImageVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MajorImageVersion);
    HWND hEdit_MinorImageVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MinorImageVersion);
    HWND hEdit_MajorSubsystemVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MajorSubsystemVersion);
    HWND hEdit_MinorSubsystemVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MinorSubsystemVersion);
    HWND hEdit_Win32VersionValue = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_Win32VersionValue);
    HWND hEdit_SizeOfImage = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfImage);

    HWND hEdit_SizeOfHeaders = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfHeaders);
    HWND hEdit_CheckSum = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_CheckSum);
    HWND hEdit_Subsystem = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_Subsystem);
    HWND hEdit_DllCharacteristics = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_DllCharacteristics);
    HWND hEdit_SizeOfStackReserve = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfStackReserve);
    HWND hEdit_SizeOfStackCommit = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfStackCommit);
    HWND hEdit_SizeOfHeapReserve = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfHeapReserve);
    HWND hEdit_SizeOfHeapCommit = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfHeapCommit);
    HWND hEdit_LoaderFlags = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_LoaderFlags);
    HWND hEdit_NumberOfRvaAndSizes = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_NumberOfRvaAndSizes);



    //Ϊÿһ���༭������ֵ
    char buffer[21] = {0};
    //��DWORD��������ת��ΪLPSTR�������ͣ�������ת��Ϊ�����ַ�����
    ultoa(pOptionalHeader->Magic, buffer, 16);
    //���ñ༭���ֵ
    SetWindowText(hEdit_Magic, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MajorLinkerVersion, buffer, 16);
    SetWindowText(hEdit_MajorLinkerVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MinorLinkerVersion, buffer, 16);
    SetWindowText(hEdit_MinorLinkerVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfCode, buffer, 16);
    SetWindowText(hEdit_SizeOfCode, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfInitializedData, buffer, 16);
    SetWindowText(hEdit_SizeOfInitializedData, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfUninitializedData, buffer, 16);
    SetWindowText(hEdit_SizeOfUninitializedData, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->AddressOfEntryPoint, buffer, 16);
    SetWindowText(hEdit_AddressOfEntryPoint, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->BaseOfCode, buffer, 16);
    SetWindowText(hEdit_BaseOfCode, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->BaseOfData, buffer, 16);
    SetWindowText(hEdit_BaseOfData, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->ImageBase, buffer, 16);
    SetWindowText(hEdit_ImageBase, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SectionAlignment, buffer, 16);
    SetWindowText(hEdit_SectionAlignment, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->FileAlignment, buffer, 16);
    SetWindowText(hEdit_FileAlignment, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MajorOperatingSystemVersion, buffer, 16);
    SetWindowText(hEdit_MajorOperatingSystemVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MinorOperatingSystemVersion, buffer, 16);
    SetWindowText(hEdit_MinorOperatingSystemVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MajorImageVersion, buffer, 16);
    SetWindowText(hEdit_MajorImageVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MinorImageVersion, buffer, 16);
    SetWindowText(hEdit_MinorImageVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MajorSubsystemVersion, buffer, 16);
    SetWindowText(hEdit_MajorSubsystemVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MinorSubsystemVersion, buffer, 16);
    SetWindowText(hEdit_MinorSubsystemVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->Win32VersionValue, buffer, 16);
    SetWindowText(hEdit_Win32VersionValue, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfImage, buffer, 16);
    SetWindowText(hEdit_SizeOfImage, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfHeaders, buffer, 16);
    SetWindowText(hEdit_SizeOfHeaders, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->CheckSum, buffer, 16);
    SetWindowText(hEdit_CheckSum, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->Subsystem, buffer, 16);
    SetWindowText(hEdit_Subsystem, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->DllCharacteristics, buffer, 16);
    SetWindowText(hEdit_DllCharacteristics, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfStackReserve, buffer, 16);
    SetWindowText(hEdit_SizeOfStackReserve, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfStackCommit, buffer, 16);
    SetWindowText(hEdit_SizeOfStackCommit, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfHeapReserve, buffer, 16);
    SetWindowText(hEdit_SizeOfHeapReserve, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfHeapCommit, buffer, 16);
    SetWindowText(hEdit_SizeOfHeapCommit, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->LoaderFlags, buffer, 16);
    SetWindowText(hEdit_LoaderFlags, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->NumberOfRvaAndSizes, buffer, 16);
    SetWindowText(hEdit_NumberOfRvaAndSizes, buffer);

}

/**
 * ��λpFileBuffer�е� ��ѡPEͷ ���׵�ַ
 * @param  pFileBuffer     FileBuffer�׵�ַ
 * @param  pOptionalHeader ����DOSͷ�ĵ�ַ
 */
VOID GetOptionPeHeaders(IN LPVOID pFileBuffer, OUT PIMAGE_OPTIONAL_HEADER32* pOptionalHeader){                                                               
    PIMAGE_DOS_HEADER pDosHeaderBak = NULL;
    PIMAGE_NT_HEADERS pNTHeaderBak = NULL;
    PIMAGE_FILE_HEADER pPEHeaderBak = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeaderBak = NULL;
    PIMAGE_SECTION_HEADER pSectionHeaderBak = NULL;

    if(!pFileBuffer){                                   
        printf("�����ָ��Ϊnull��\n");
        return ;
    }
    //�ж��Ƿ�����Ч��MZ��־
    if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE){                                   
        printf("������Ч��MZ��־��\n");
        return ;
    }
    pDosHeaderBak = (PIMAGE_DOS_HEADER)pFileBuffer;
    //�ж��Ƿ�����Ч��PE��־
    if(*((PDWORD)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew)) != IMAGE_NT_SIGNATURE){                                 
        printf("������Ч��PE��־��\n");
        return ;
    }
    pNTHeaderBak = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew);
    pPEHeaderBak = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeaderBak) + 4);
    pOptionHeaderBak = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeaderBak+IMAGE_SIZEOF_FILE_HEADER);
    //����ѡPEͷ���׵�ַ����
    *pOptionalHeader = pOptionHeaderBak;
}


