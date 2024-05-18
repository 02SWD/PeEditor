// ButtonDataDirectoryView.cpp: implementation of the ButtonDataDirectoryView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonDataDirectoryView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
//DataDirectoryView�Ի���Ĵ��ڻص�����
BOOL CALLBACK ButtonDataDirectoryViewProc(HWND hwndDlgOfDataDirectoryView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//��ȡ��һ������Ŀ¼��ĵ�ַ
VOID GetFirstDataCatalogEntry(IN LPVOID pFileBuffer, OUT PIMAGE_DATA_DIRECTORY* pDataDirectory);
//չʾ����Ŀ¼�����Ϣ
VOID ExhibitionDataDirectory(HWND hwndDlgOfDataDirectoryView, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * ��� IDC_BUTTON_DirectoryView  ��ť��Ĵ�����루������� ������Ŀ¼� ��ť֮��Ҫִ�еĴ��룩
 * @param  hwndDlgOfPeView �����ھ�� (��������PeView���ڵľ�����ô��ڵ�ID��IDD_DIALOG_PeViewMainDlg)
 */
VOID ButtonDataDirectoryViewCode(HWND hwndDlgOfPeView){
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_DataDirectoryView), hwndDlgOfPeView, ButtonDataDirectoryViewProc);
}

/**
 * DataDirectoryView�Ի���Ĵ��ڻص��������ô���ID��IDD_DIALOG_DataDirectoryView��
 * @param  hwndDlgOfDataDirectoryView DataDirectoryView���ڵľ��
 * @param  uMsg                       ��Ϣ����
 * @param  wParam                     WPARAM
 * @param  lParam                     WPARAM
 * @return                            
 */
BOOL CALLBACK ButtonDataDirectoryViewProc(HWND hwndDlgOfDataDirectoryView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //���ڳ�ʼ��ʱʵ�еĴ���
        case WM_INITDIALOG:{
			//�������ô��ڱ��⣬�ﵽ������ǰ���ڱ��� - �򿪵��ļ�·���� ��Ч��
			ResetWindowTitle(hwndDlgOfDataDirectoryView, gSzFileNameOfPeView);
            //չʾ ����Ŀ¼�� ����Ϣ
            ExhibitionDataDirectory(hwndDlgOfDataDirectoryView, uMsg, wParam, lParam);
            return TRUE;
        }
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfDataDirectoryView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //��� ���˳��� ��ť���˳��ô���
                case IDC_BUTTON_DataDirectoryViewExit:{
                    EndDialog(hwndDlgOfDataDirectoryView, 0);
                    return TRUE;
                }
				//��� �����������İ�ť�� �󣬽������ ButtonExportTableViewCode ��������ϸ��չʾ��������Ϣ
				case IDC_BUTTON_ExportTable_Detailed:{
					ButtonExportTableViewCode(hwndDlgOfDataDirectoryView);
					return TRUE;
				}
				//��� ����������İ�ť�� �󣬽������ ButtonImportTableViewCode ��������ϸ��չʾ�������Ϣ
				case IDC_BUTTON_ImportTable_Detailed:{
					ButtonImportTableViewCode(hwndDlgOfDataDirectoryView);
					return TRUE;
				}
				//��� ���ض�λ�����İ�ť�� �󣬽������ ButtonBaseRelocViewCode ��������ϸ��չʾ�ض�λ����Ϣ
				case IDC_BUTTON_BaseRelocTable_Detailed:{
					ButtonBaseRelocViewCode(hwndDlgOfDataDirectoryView);
					return TRUE;
				}
				case IDC_BUTTON_IATTable_Detailed:{
					ButtonIatTableViewCode(hwndDlgOfDataDirectoryView);
					return TRUE;
				}
            }
        }
    }
    return FALSE;
}

/**
 * չʾ ����Ŀ¼�� ����Ϣ
 * @param  hwndDlgOfDataDirectoryView DataDirectoryView���ڵľ�����ô���ID��IDD_DIALOG_DataDirectoryView��
 * @param  uMsg                       ��Ϣ����
 * @param  wParam                     WPARAM
 * @param  lParam                     LPARAM
 */
VOID ExhibitionDataDirectory(HWND hwndDlgOfDataDirectoryView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //��װ��pFileBufferָ���FileBuffer��С�ı���
    FILEINFOR file;
    //��ȡPE�ļ�
    file = readFile(gSzFileNameOfPeView);
    //ָ������Ŀ¼���ָ��
    PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
    PIMAGE_DATA_DIRECTORY pDataDirectoryBak = NULL;
    //��λ ��һ������Ŀ¼�� ��λ��
    GetFirstDataCatalogEntry(file.p, &pDataDirectory);

    //��ȡDataDirectoryView�Ի��������б༭��ľ��
    HWND hEdit_ExportTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ExportTable_RVA);
    HWND hEdit_ExportTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ExportTable_SIZE);
    
    HWND hEdit_ImportTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ImportTable_RVA);
    HWND hEdit_ImportTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ImportTable_SIZE);
    
    HWND hEdit_ResourceTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ResourceTable_RVA);
    HWND hEdit_ResourceTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ResourceTable_SIZE);

    HWND hEdit_ExceptionTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ExceptionTable_RVA);
    HWND hEdit_ExceptionTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ExceptionTable_SIZE);

    HWND hEdit_SecuryTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_SecurityTable_RVA);
    HWND hEdit_SecuryTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_SecurityTable_SIZE);

    HWND hEdit_BaseRelocTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_BaseRelocTable_RVA);
    HWND hEdit_BaseRelocTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_BaseRelocTable_SIZE);

    HWND hEdit_DebugTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_DebugTable_RVA);
    HWND hEdit_DebugTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_DebugTable_SIZE);

    HWND hEdit_CopyRightTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_CopyRightTable_RVA);
    HWND hEdit_CopyRightTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_CopyRightTable_SIZE);
    
    HWND hEdit_GlobalPtrTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_GlobalPtrTable_RVA);
    HWND hEdit_GlobalPtrTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_GlobalPtrTable_SIZE);

    HWND hEdit_TlsTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_TLSTable_RVA);
    HWND hEdit_TlsTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_TLSTable_SIZE);

    HWND hEdit_ImportConfigTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_LoadConfigTable_RVA);
    HWND hEdit_ImportConfigTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_LoadConfigTable_SIZE);

    HWND hEdit_BoundImportTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_BoundImportTable_RVA);
    HWND hEdit_BoundImportTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_BoundImportTable_SIZE);
    
    HWND hEdit_IatTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_IATTable_RVA);
    HWND hEdit_IatTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_IATTable_SIZE);

    HWND hEdit_DelayImportTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_DelayImportTable_RVA);
    HWND hEdit_DelayImportTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_DelayImportTable_SIZE);

    HWND hEdit_ComTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ComTable_RVA);
    HWND hEdit_ComTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ComTable_SIZE);

    HWND hEdit_RetainTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_RetainTable_RVA);
    HWND hEdit_RetainTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_RetainTable_SIZE);

    //Ϊÿһ���༭������ֵ
    char buffer[21] = {0};
    //��pDataDirectoryBakָ��ָ���1������Ŀ¼��
    pDataDirectoryBak = pDataDirectory + 0;
    //������ת��Ϊ�����ַ���
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    //���ñ༭���ֵ
    SetWindowText(hEdit_ExportTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ExportTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 1;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ImportTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ImportTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 2;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ResourceTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ResourceTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 3;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ExceptionTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ExceptionTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 4;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_SecuryTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_SecuryTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 5;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_BaseRelocTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_BaseRelocTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 6;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_DebugTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_DebugTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 7;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_CopyRightTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_CopyRightTableSize, buffer);
    
    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 8;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_GlobalPtrTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_GlobalPtrTableSize, buffer);
    
    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 9;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_TlsTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_TlsTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 10;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ImportConfigTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ImportConfigTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 11;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_BoundImportTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_BoundImportTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 12;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_IatTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_IatTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 13;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_DelayImportTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_DelayImportTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 14;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ComTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ComTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 15;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_RetainTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_RetainTableSize, buffer);
    
}


/**
 * ��λpFileBuffer�� ��һ������Ŀ¼�� ���׵�ַ
 * @param  pFileBuffer    FileBuffer�׵�ַ
 * @param  pDataDirectory ��һ������Ŀ¼����׵�ַ
 */
VOID GetFirstDataCatalogEntry(IN LPVOID pFileBuffer, OUT PIMAGE_DATA_DIRECTORY* pDataDirectory){
    PIMAGE_DOS_HEADER pDosHeaderBak = NULL;
    PIMAGE_NT_HEADERS pNTHeaderBak = NULL;
    PIMAGE_FILE_HEADER pPEHeaderBak = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeaderBak = NULL;
    PIMAGE_SECTION_HEADER pFirstSectionHeaderBak = NULL;
    PIMAGE_DATA_DIRECTORY pDataDirectoryBak = NULL;

    if(!pFileBuffer){                                   
        printf("�����ָ��Ϊnull��\n");
        return ;
    }
    //�ж��Ƿ�����Ч��MZ��־
    if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE){                                   
        printf("������Ч��MZ��־��\n");
        return ;
    }
    //��pDosHeaderBakָ��ָ��PE�ļ��׵�ַ
    pDosHeaderBak = (PIMAGE_DOS_HEADER)pFileBuffer;
    //�ж��Ƿ�����Ч��PE��־
    if(*((PDWORD)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew)) != IMAGE_NT_SIGNATURE){                                 
        printf("������Ч��PE��־��\n");
        return ;
    }
    pNTHeaderBak = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew);
    pPEHeaderBak = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeaderBak) + 4);
    pOptionHeaderBak = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeaderBak+IMAGE_SIZEOF_FILE_HEADER);
    //��pDataDirectoryBakָ��ָ���һ������Ŀ¼���׵�ַ
    pDataDirectoryBak = pOptionHeaderBak->DataDirectory;
    //���ص�һ������Ŀ¼���׵�ַ
    *pDataDirectory = pDataDirectoryBak;
}


