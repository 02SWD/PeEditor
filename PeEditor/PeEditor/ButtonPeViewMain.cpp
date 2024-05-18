// ButtonPeViewMain.cpp: implementation of the ButtonPeViewMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonPeViewMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
BOOL CALLBACK ButtonPeViewProc(HWND hwndDlgOfPeView, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID GetAllPeHeaders(IN LPVOID pFileBuffer, OUT PIMAGE_DOS_HEADER* pDosHeader, OUT PIMAGE_NT_HEADERS* pNtHeader, OUT PIMAGE_FILE_HEADER* pFileHeader, OUT PIMAGE_OPTIONAL_HEADER32* pOptionalHeader);
VOID GetDosAndFilePeInfo(HWND hwndDlgOfPeView, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * ���IDC_BUTTON_PeView��ť��Ĵ�����루������� ��PE�鿴���� ��ť֮��Ҫִ�еĴ��룩
 * @param  hInstance  Ӧ�ó���ľ��
 * @param  DialogMain �����ھ�� (��������DialogMain���ڵľ�����ô���ID��IDD_DIALOG_MAIN)
 * @param  szFileName �ļ�·��
 */
VOID ButtonPeViewCodeMain(PTSTR szFileName){
    //���ļ�·����Ϊȫ�ֱ���
    gSzFileNameOfPeView = szFileName;
    //����һ���ӶԻ���
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_PeViewMainDlg), NULL,  ButtonPeViewProc);
}

/**
 * PeView�Ի���Ĵ��ڻص����� (�ô���ID: IDD_DIALOG_PeViewMainDlg)
 * @param  hwndDlgOfPeView PeView���ڵľ��
 * @param  uMsg            ��Ϣ����
 * @param  wParam          WPARAM
 * @param  lParam          LPARAM
 * @return                 
 */
BOOL CALLBACK ButtonPeViewProc(HWND hwndDlgOfPeView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        case WM_INITDIALOG:{
			//�������ô��ڱ��⣬�ﵽ������ǰ���ڱ��� - �򿪵��ļ�·���� ��Ч��
			ResetWindowTitle(hwndDlgOfPeView, gSzFileNameOfPeView);
            //����GetDosAndFilePeInfo��������pe�ļ���DOSͷ�ͱ�׼peͷ��Ϣ��ʾ����
            GetDosAndFilePeInfo(hwndDlgOfPeView, uMsg, wParam, lParam);
            return TRUE;
        }
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfPeView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //��д�˳���ť�Ĵ������
                case IDC_BUTTON_PeViewDlgExit:{
                    //�˳�����
                    EndDialog(hwndDlgOfPeView, 0);
                    return TRUE;
                }
				//��� ����ѡPEͷ�� ��ť�󣬻����ButtonOptionPeViewCode��������pe�ļ��Ŀ�ѡpeͷ��Ϣ��ʾ����
                case IDC_BUTTON_OptionPeVew:{
                    ButtonOptionPeViewCode(hwndDlgOfPeView);
                    return TRUE;
                }
				//��� �����Ρ� ��ť�󣬻����ButtonSectionViewCode��������������Ϣ��ʾ����
				case IDC_BUTTON_SectionView:{
					ButtonSectionViewCode(hwndDlgOfPeView);
					return TRUE;
				}
				//��� ������Ŀ¼� ��ť�󣬻����ButtonDataDirectoryViewCode������������Ŀ¼��Ϣ��ʾ����
				case IDC_BUTTON_DirectoryView:{
					ButtonDataDirectoryViewCode(hwndDlgOfPeView);
					return TRUE;
				}
            }
        }
    }
    return FALSE;
}

/**
 * ��ȡpe�ļ���DOSͷ�ͱ�׼PEͷ��Ϣ��������չʾ����
 * @param  hwndDlgOfPeView PeView���ڵľ�� (�ô���ID: IDD_DIALOG_PeViewMainDlg)
 * @param  uMsg            ��Ϣ����
 * @param  wParam          WPARAM
 * @param  lParam          LPARAM
 * @param  szFileName      �ļ�·��
 */
VOID GetDosAndFilePeInfo(HWND hwndDlgOfPeView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //��װ��pFileBufferָ���FileBuffer��С�ı���
    FILEINFOR file;
    //��ȡPE�ļ�
    file = readFile(gSzFileNameOfPeView);
    PIMAGE_DOS_HEADER pDosHeader = NULL;
    PIMAGE_NT_HEADERS pNtHeader = NULL;
    PIMAGE_FILE_HEADER pFileHeader = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = NULL;
    //��ȡDOSͷ��NTͷ����׼PEͷ�Ϳ�ѡPEͷ��λ��
    GetAllPeHeaders(file.p, &pDosHeader, &pNtHeader, &pFileHeader, &pOptionalHeader);
    
    //��ȡPeView�Ի��������б༭��ľ��
    HWND hEdit_E_Magic = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_e_magic);
    HWND hEdit_E_Lfanew = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_e_lfanew);
    HWND hEdit_Machine = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_Machine);
    HWND hEdit_NumberOfSections = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_NumberOfSections);
    HWND hEdit_TimeDateStamp = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_TimeDateStamp);
    HWND hEdit_PointerToSymbolTable = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_PointerToSymbolTable);
    HWND hEdit_NumberOfSymbols = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_NumberOfSymbols);
    HWND hEdit_SizeOfOptionalHeader = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_SizeOfOptionalHeader);
    HWND hEdit_Characteristics = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_Characteristics);

    //Ϊÿһ���༭������ֵ
    char buffer[21] = {0};
    //��DWORD��������ת��ΪLPSTR�������ͣ�������ת��Ϊ�����ַ�����
    ultoa(pDosHeader->e_magic, buffer, 16);
    SetWindowText(hEdit_E_Magic, buffer);
    memset(buffer, 0, 21);
    ultoa(pDosHeader->e_lfanew, buffer, 16);
    SetWindowText(hEdit_E_Lfanew, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->Machine, buffer, 16);
    SetWindowText(hEdit_Machine, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->NumberOfSections, buffer, 16);
    SetWindowText(hEdit_NumberOfSections, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->TimeDateStamp, buffer, 16);
    SetWindowText(hEdit_TimeDateStamp, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->PointerToSymbolTable, buffer, 16);
    SetWindowText(hEdit_PointerToSymbolTable, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->NumberOfSymbols, buffer, 16);
    SetWindowText(hEdit_NumberOfSymbols, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->SizeOfOptionalHeader, buffer, 16);
    SetWindowText(hEdit_SizeOfOptionalHeader, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->Characteristics, buffer, 16);
    SetWindowText(hEdit_Characteristics, buffer);
}


/**
 * ��λpFileBuffer�е� DOSͷ�׵�ַ��NTͷ�׵�ַ����׼PEͷ�׵�ַ����ѡPEͷ�׵�ַ
 * @param  pFileBuffer     FileBuffer�׵�ַ
 * @param  pDosHeader      PIMAGE_DOS_HEADERָ���ַ
 * @param  pNtHeader       PIMAGE_NT_HEADERSָ���ַ
 * @param  pFileHeader     PIMAGE_FILE_HEADERָ���ַ
 * @param  pOptionalHeader PIMAGE_OPTIONAL_HEADER32ָ���ַ
 */
VOID GetAllPeHeaders(IN LPVOID pFileBuffer, 
                  OUT PIMAGE_DOS_HEADER* pDosHeader, 
                  OUT PIMAGE_NT_HEADERS* pNtHeader, 
                  OUT PIMAGE_FILE_HEADER* pFileHeader, 
                  OUT PIMAGE_OPTIONAL_HEADER32* pOptionalHeader)
{                                                               
    PIMAGE_DOS_HEADER pDosHeaderBak = NULL;
    PIMAGE_NT_HEADERS pNTHeaderBak = NULL;
    PIMAGE_FILE_HEADER pPEHeaderBak = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeaderBak = NULL;

    if(!pFileBuffer){                                   
        printf("�����ָ��Ϊnull��\n");
        return ;
    }
    //�ж��Ƿ�����Ч��MZ��־
    if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE){                                   
        printf("������Ч��MZ��־��\n");
        return ;
    }
    //��DOSͷ���׵�ַ����
    pDosHeaderBak = (PIMAGE_DOS_HEADER)pFileBuffer;
    *pDosHeader = pDosHeaderBak;
    //�ж��Ƿ�����Ч��PE��־
    if(*((PDWORD)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew)) != IMAGE_NT_SIGNATURE){                                 
        printf("������Ч��PE��־��\n");
        return ;
    }
    //��NTͷ���׵�ַ����
    pNTHeaderBak = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew);
    *pNtHeader = pNTHeaderBak;
    //����׼PEͷ���׵�ַ����
    pPEHeaderBak = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeaderBak) + 4);
    *pFileHeader = pPEHeaderBak;
    //����ѡPEͷ���׵�ַ����
    pOptionHeaderBak = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeaderBak+IMAGE_SIZEOF_FILE_HEADER);
    *pOptionalHeader = pOptionHeaderBak;
}
