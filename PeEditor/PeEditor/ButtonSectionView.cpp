// ButtonSectionViewCode.cpp: implementation of the ButtonSectionViewCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonSectionView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
//SectionView�Ĵ��ڻص�����
BOOL CALLBACK ButtonSectionViewProc(HWND hwndDlgOfSectionView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//��ʼ���ڱ��б�ؼ���Ϊ�ÿռ���ӱ�ͷ
VOID InitSectionTableList(HWND hwndDlgOfSectionView);
//��ȡFileBuffer�е�һ���ڱ�ĵ�ַ
VOID GetFirstSectionTable(IN LPVOID pFileBuffer, OUT PDWORD numberOfSections, OUT PIMAGE_SECTION_HEADER* pFirstSectionTableHeader);
//���ڱ���Ϣչʾ����
VOID ExhibitionSections(HWND hwndDlgOfSectionView, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * ��� IDC_BUTTON_SectionView ��ť��Ĵ�����루������� �����Ρ� ��ť֮��Ҫִ�еĴ��룩
 * @param  hwndDlgOfPeView �����ھ������������peView���ڵľ�����ô���ID��IDD_DIALOG_PeViewMainDlg��
 */
VOID ButtonSectionViewCode(HWND hwndDlgOfPeView){
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_SectionTableView), hwndDlgOfPeView, ButtonSectionViewProc);

}

/**
 * SectionView���ڵĴ��ڻص��������ô���ID��IDD_DIALOG_SectionTableView��
 * @param  hwndDlgOfSectionView SectionView���ڵľ��
 * @param  uMsg                 ��Ϣ����
 * @param  wParam               WPARAM
 * @param  lParam               LPARAM
 * @return                      
 */
BOOL CALLBACK ButtonSectionViewProc(HWND hwndDlgOfSectionView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //��ʼ�����ڣ����ڱ���Ϣչʾ����
        case WM_INITDIALOG:{
			//�������ô��ڱ��⣬�ﵽ������ǰ���ڱ��� - �򿪵��ļ�·���� ��Ч��
			ResetWindowTitle(hwndDlgOfSectionView, gSzFileNameOfPeView);
            //��ʼ����ͷ
            InitSectionTableList(hwndDlgOfSectionView);
            //���ڱ���Ϣչʾ����
            ExhibitionSections(hwndDlgOfSectionView, uMsg, wParam, lParam);
            return TRUE;
        }
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfSectionView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //������˳���ťʱ���˳�����
                case IDC_BUTTON_SectionTableExit:{
                    EndDialog(hwndDlgOfSectionView, 0);
                    return TRUE;
                }
            }
        }
        
    }
    return FALSE;
}

/**
 * SectionView���ڵľ�����ô���ID��IDD_DIALOG_SectionTableView��
 * @param  hwndDlgOfSectionView SectionView���ڵľ��
 * @param  uMsg                 ��Ϣ����
 * @param  wParam               WPARAM
 * @param  lParam               LPARAM
 */
VOID ExhibitionSections(HWND hwndDlgOfSectionView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //���ڴ洢PE������Ϣ
    FILEINFOR file;
    //��ȥPE�ļ�
    file = readFile(gSzFileNameOfPeView);
    //���ڴ洢�ڱ������
    DWORD numberOfSections = 0;
    //pFirstSectionTableָ��ָ���һ���ڱ�
    PIMAGE_SECTION_HEADER pFirstSectionTable = NULL;
    PIMAGE_SECTION_HEADER pSectionTableBak = NULL;
    //��λFileBuffer�е�һ���ڱ��λ�ã������ظ�PE�ļ��нڵ�����
    GetFirstSectionTable(file.p, &numberOfSections, &pFirstSectionTable);

    //���� SectionView���ھ�� �� section�б�ؼ�ID ��ȡsection�б�ؼ��ľ��
    HWND hListSection =  GetDlgItem(hwndDlgOfSectionView, IDC_LIST_SectionTableView);
    //�����ݽṹ���ڴ洢�����б�ؼ������õ�Ԫ����Ϣ
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;

    //��SectionView���������
    for (int i=0; i<numberOfSections; i++){
        //������pSectionTableBakָ��ָ���µĽڱ�
        pSectionTableBak = pFirstSectionTable + i;

        char* Section_Name = (char*)pSectionTableBak->Name;
        DWORD Section_VirtualAddress = pSectionTableBak->VirtualAddress;
        DWORD Section_VirtualSize = pSectionTableBak->Misc.VirtualSize;
        DWORD Section_SizeOfRawData = pSectionTableBak->SizeOfRawData;
        DWORD Section_PointerToRawData = pSectionTableBak->PointerToRawData;
        DWORD Section_Characteristics = pSectionTableBak->Characteristics;
        DWORD Section_PointerToRelocations = pSectionTableBak->PointerToRelocations;
        DWORD Section_PointerToLinenumbers =  pSectionTableBak->PointerToLinenumbers;
        DWORD Section_NumberOfRelocations =  pSectionTableBak->NumberOfRelocations;
        DWORD Section_NumberOfLinenumbers = pSectionTableBak->NumberOfLinenumbers;
        
        //��������
        vItem.pszText = Section_Name;
        vItem.iItem = i;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListSection, &vItem);

        char buffer[21] = {0};
        //������ת��Ϊ�����ַ���
        ultoa(Section_VirtualAddress, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 1;
        ListView_SetItem(hListSection, &vItem);
        
        memset(buffer, 0, 21);
        ultoa(Section_VirtualSize, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 2;
        ListView_SetItem(hListSection, &vItem);
        
        memset(buffer, 0, 21);
        ultoa(Section_PointerToRawData, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 3;
        ListView_SetItem(hListSection, &vItem);

        memset(buffer, 0, 21);
        ultoa(Section_SizeOfRawData, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 4;
        ListView_SetItem(hListSection, &vItem);

        memset(buffer, 0, 21);
        ultoa(Section_Characteristics, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 5;
        ListView_SetItem(hListSection, &vItem);

        memset(buffer, 0, 21);
        ultoa(Section_PointerToRelocations, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 6;
        ListView_SetItem(hListSection, &vItem);

        memset(buffer, 0, 21);
        ultoa(Section_PointerToLinenumbers, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 7;
        ListView_SetItem(hListSection, &vItem);

        memset(buffer, 0, 21);
        ultoa(Section_NumberOfRelocations, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 8;
        ListView_SetItem(hListSection, &vItem);
        
        memset(buffer, 0, 21);
        ultoa(Section_NumberOfLinenumbers, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 9;
        ListView_SetItem(hListSection, &vItem);

    }
}


/**
 * ��λpFileBuffer�еĵ�һ���ڱ���׵�ַ
 * @param  pFileBuffer              FileBuffer�׵�ַ
 * @param  numberOfSections         �ڵ�����
 * @param  pFirstSectionTableHeader ָ��FileBuffer�е�һ���ڱ���׵�ַ
 */
VOID GetFirstSectionTable(IN LPVOID pFileBuffer, OUT PDWORD numberOfSections, OUT PIMAGE_SECTION_HEADER* pFirstSectionTableHeader){
    PIMAGE_DOS_HEADER pDosHeaderBak = NULL;
    PIMAGE_NT_HEADERS pNTHeaderBak = NULL;
    PIMAGE_FILE_HEADER pPEHeaderBak = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeaderBak = NULL;
    PIMAGE_SECTION_HEADER pFirstSectionHeaderBak = NULL;

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
    pFirstSectionHeaderBak = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeaderBak+pPEHeaderBak->SizeOfOptionalHeader);
    //���ص�һ���ڱ���׵�ַ
    *pFirstSectionTableHeader = pFirstSectionHeaderBak;
    //���ؽڱ������
    *numberOfSections = pPEHeaderBak->NumberOfSections;
}


/**
 * ��ʼ���ڱ��б�ؼ��ı�ͷ
 * @param  hwndDlgOfSectionView SectionView���ڵľ�����ô���ID��IDD_DIALOG_SectionTableView��
 */
VOID InitSectionTableList(HWND hwndDlgOfSectionView){
    //���� SectionView���ھ�� �� �б�ؼ�ID ��ȡ�ڱ��б�ؼ��ľ��
    HWND hListSectionTable = GetDlgItem(hwndDlgOfSectionView, IDC_LIST_SectionTableView);
    //��������ѡ��
    SendMessage(hListSectionTable, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��ʼ��ʼ����ͷ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("BYTE Name");
    lv.cx = 150;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListSectionTable, 0, &lv);
    
    lv.pszText = TEXT("VirtualAddress");
    lv.cx = 150;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListSectionTable, 1, &lv);

    lv.pszText = TEXT("VirtualSize");
    lv.cx = 150;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListSectionTable, 2, &lv);

    lv.pszText = TEXT("PointerToRawData");
    lv.cx = 150;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListSectionTable, 3, &lv);

    lv.pszText = TEXT("SizeOfRawData");
    lv.cx = 150;
    lv.iSubItem = 4;
    ListView_InsertColumn(hListSectionTable, 4, &lv);

    lv.pszText = TEXT("Characteristics");
    lv.cx = 150;
    lv.iSubItem = 5;
    ListView_InsertColumn(hListSectionTable, 5, &lv);

    lv.pszText = TEXT("PointerToRelocations");
    lv.cx = 150;
    lv.iSubItem = 6;
    ListView_InsertColumn(hListSectionTable, 6, &lv);

    lv.pszText = TEXT("PointerToLinenumber");
    lv.cx = 150;
    lv.iSubItem = 7;
    ListView_InsertColumn(hListSectionTable, 7, &lv);

    lv.pszText = TEXT("NumberOfRelocations");
    lv.cx = 150;
    lv.iSubItem = 8;
    ListView_InsertColumn(hListSectionTable, 8, &lv);

    lv.pszText = TEXT("NumberOfLinenumber");
    lv.cx = 150;
    lv.iSubItem = 9;
    ListView_InsertColumn(hListSectionTable, 9, &lv);
}




