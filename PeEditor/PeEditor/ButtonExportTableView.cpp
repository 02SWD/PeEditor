// ButtonExportTableView.cpp: implementation of the ButtonExportTableView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonExportTableView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
//ExportTableView���ڵĴ��ڻص�����
BOOL CALLBACK ButtonExportTableViewProc(HWND hwndDlgOfExportTableView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//��ExportTableView����չʾ������Ļ�����Ϣ
VOID ExhibiteBasicInfo(HWND hwndDlgOfExportTableView);
//��ExportTableView����չʾ��������-����������ַ�����Ϣ
VOID ExhibiteAddressOfFunctions(HWND hwndDlgOfExportTableView);
//��ExportTableView����չʾ��������-����������ű�͵����������Ʊ����Ϣ
VOID ExhibiteOtherFunctions(HWND hwndDlgOfExportTableView);
//��λ������ṹ
void GetExportTable(IN LPVOID pFileBuffer, OUT PIMAGE_EXPORT_DIRECTORY* pExportTable);

/**
 * ��� IDC_BUTTON_ExportTable_Detailed ��ť��Ĵ�����루������� ���������İ�ť�� ֮��Ҫִ�еĴ��룩
 * @param  hwndDlgOfDataDirectoryView �����ھ����������DataDirectoryView���ھ�����ô���ID�� IDD_DIALOG_DataDirectoryView��
 */
VOID ButtonExportTableViewCode(HWND hwndDlgOfDataDirectoryView){
    //����Ҫ�жϸ�pe�ļ��Ƿ���е������������е������򵯴���ʾ
    //���ڴ洢FileBuffer
    FILEINFOR file;
    //��pe�ļ�
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer�е�����ṹ��λ��
    PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
    GetExportTable(file.p, &pExportTable);
    //�жϸ�PE�ļ��Ƿ���ڵ�����
    if (pExportTable == NULL){
        MessageBox(NULL, TEXT("��PEû�е�����!"), TEXT("����!"), MB_OK);
        return ;
    }
    //���� ExportTableView����
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_ExportTableView), hwndDlgOfDataDirectoryView, ButtonExportTableViewProc);
}


/**
 * ExportTableView���ڵĴ��ڻص��������ô���ID��IDD_DIALOG_ExportTableView��
 * @param  hwndDlgOfExportTableView ExportTableView���ڵľ��
 * @param  uMsg                     ��Ϣ����
 * @param  wParam                   WPARAM
 * @param  lParam                   LPARAM
 * @return                          
 */
BOOL CALLBACK ButtonExportTableViewProc(HWND hwndDlgOfExportTableView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //���ڳ�ʼ��ʱִ�еĴ���
        case WM_INITDIALOG:{
			//�������ô��ڱ��⣬�ﵽ������ǰ���ڱ��� - �򿪵��ļ�·���� ��Ч��
			ResetWindowTitle(hwndDlgOfExportTableView, gSzFileNameOfPeView);
            //չʾ������Ļ�����Ϣ
            ExhibiteBasicInfo(hwndDlgOfExportTableView);
            //չʾ�������� - ����������ַ�����Ϣ
            ExhibiteAddressOfFunctions(hwndDlgOfExportTableView);
            //չʾ�������� - ������ű�͵������Ʊ����Ϣ
            ExhibiteOtherFunctions(hwndDlgOfExportTableView);
            return TRUE;
        }
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfExportTableView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //��� ���˳��� ��ť���˳�����
                case IDC_BUTTON_ExportTableViewExit:{
                    EndDialog(hwndDlgOfExportTableView, 0);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/**
 * չʾ�������� - ������ű�͵������Ʊ����Ϣ
 * @param  hwndDlgOfExportTableView ExportTableView���ڵľ�����ô���ID��IDD_DIALOG_ExportTableView��
 */
VOID ExhibiteOtherFunctions(HWND hwndDlgOfExportTableView){
    //��ȡExportTableView������ IDC_LIST_ExportOtherTwoTables �б�ؼ���ID
    HWND hListAddressOfFunctions = GetDlgItem(hwndDlgOfExportTableView, IDC_LIST_ExportOtherTwoTables);
    //��������ѡ��
    SendMessage(hListAddressOfFunctions, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��ʼ���б��ͷ��Ϣ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("���� [ʮ����]");
    lv.cx = 110;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListAddressOfFunctions, 0, &lv);

    lv.pszText = TEXT("�������");
    lv.cx = 110;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListAddressOfFunctions, 1, &lv);
    
    lv.pszText = TEXT("������");
    lv.cx = 200;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListAddressOfFunctions, 2, &lv);
    
    //��ȡPE�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer�е�����ṹ��λ��
    PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
    GetExportTable(file.p, &pExportTable);

    //��ȡ�����������Ʊ��RVA��������ת��ΪFOA
    DWORD exportTableAddressOfNamesFoa = RvaToFileOffest(file.p, pExportTable->AddressOfNames);
    //��ȡ����������ű��RVA��������ת��ΪFOA
    DWORD exportTableAddressOfNameOrdinalsFoa = RvaToFileOffest(file.p, pExportTable->AddressOfNameOrdinals);

    //��addressOfNamesָ��ָ�򵼳��������Ʊ���׵�ַ
    int* addressOfNames = (int*)((DWORD)file.p + exportTableAddressOfNamesFoa);
    //��addressOfNameOrdinalsָ��ָ�򵼳�������ű���׵�ַ
    short* addressOfNameOrdinals = (short*)((DWORD)file.p + exportTableAddressOfNameOrdinalsFoa);
    
    //����һ��������������֮��Ľ�����ת��Ϊ�ַ����Ĺ���
    char buffer[21];
    //��������ʼ��vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;

    //���б�ؼ����������
    for(DWORD i=0; i<pExportTable->NumberOfNames; i++){
        //��ȡ�������Ƶ��׵�ַ
        char* name = (char*)((DWORD)file.p + RvaToFileOffest(file.p, addressOfNames[i]));

        //�������ֵ
        memset(buffer, 0, 21);
        //������ת��Ϊ�����ַ���
        ultoa(i, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListAddressOfFunctions, &vItem);

        //��ӵ����������
        memset(buffer, 0, 21);
        ultoa(addressOfNameOrdinals[i], buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 1;
        ListView_SetItem(hListAddressOfFunctions, &vItem);

        //��ӵ�����������
        vItem.pszText = name;
        vItem.iItem = i;
        vItem.iSubItem = 2;
        ListView_SetItem(hListAddressOfFunctions, &vItem);
    }
    
}


/**
 * չʾ�������� - ����������ַ�����Ϣ
 * @param  hwndDlgOfExportTableView ExportTableView���ڵľ�����ô���ID��IDD_DIALOG_ExportTableView��
 */
VOID ExhibiteAddressOfFunctions(HWND hwndDlgOfExportTableView){
    //��ȡExportTableView������ IDC_LIST_ExportOtherTwoTables �б�ؼ���ID
    HWND hListAddressOfFunctions = GetDlgItem(hwndDlgOfExportTableView, IDC_LIST_ExportAddressOfFunctionsTable);
    //��������ѡ��
    SendMessage(hListAddressOfFunctions, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��ʼ���б�ؼ��ı�ͷ��Ϣ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("���� [ʮ����]");
    lv.cx = 110;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListAddressOfFunctions, 0, &lv);

    lv.pszText = TEXT("��������RVA");
    lv.cx = 130;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListAddressOfFunctions, 1, &lv);
    
    //��ȡP�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer�е�����ṹ��λ��
    PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
    GetExportTable(file.p, &pExportTable);
    
    //��ȡ����������ַ���RVA
    DWORD exportTableAddressOfFunctionsRva = pExportTable->AddressOfFunctions;  
    //������������ַ���RVAת��ΪFOA
    DWORD exportTableAddressOfFunctionsFoa = RvaToFileOffest(file.p, exportTableAddressOfFunctionsRva);
    //��addressOfFunctionsָ��ָ�򵼳�������ַ����׵�ַ
    int* addressOfFunctions = (int*)((DWORD)file.p + exportTableAddressOfFunctionsFoa);
    
    //����һ��������������֮��Ľ�����ת��Ϊ�ַ����Ĺ���
    char buffer[21];
    //��������ʼ��vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //���б�ؼ����������
    for(DWORD i=0; i<pExportTable->NumberOfFunctions; i++){
        //�������ֵ
        memset(buffer, 0, 21);
        //������ת��Ϊ�����ַ���
        ultoa(i, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListAddressOfFunctions, &vItem);

        //��ӵ���������ֵַ
        memset(buffer, 0, 21);
        ultoa(addressOfFunctions[i], buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 1;
        ListView_SetItem(hListAddressOfFunctions, &vItem);
    }

}


/**
 * չʾ������Ļ�����Ϣ
 * @param  hwndDlgOfExportTableView ExportTableView���ڵľ�����ô���ID��IDD_DIALOG_ExportTableView��
 */
VOID ExhibiteBasicInfo(HWND hwndDlgOfExportTableView){
    //��ȡPE�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer�е�����ṹ��λ��
    PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
    GetExportTable(file.p, &pExportTable);
    
    //��ȡ������Ļ�����Ϣ
    DWORD Export_Characteristics = pExportTable->Characteristics;
    DWORD Export_TimeDateStamp = pExportTable->TimeDateStamp;
    WORD Export_MajorVersion = pExportTable->MajorVersion;
    WORD Export_MinorVersion = pExportTable->MinorVersion;
    DWORD Export_Name = pExportTable->Name;
    DWORD Export_Base = pExportTable->Base;
    DWORD Export_NumberOfFunctions = pExportTable->NumberOfFunctions;
    DWORD Export_NumberOfNames = pExportTable->NumberOfNames;
    DWORD Export_AddressOfFunctions = pExportTable->AddressOfFunctions;
    DWORD Export_AddressOfNames = pExportTable->AddressOfNames;
    DWORD Export_AddressOfNameOrdinals = pExportTable->AddressOfNameOrdinals;
    
    //��ȡ ExportTableView���������б༭��ľ��
    HWND hEdit_Characteristics = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportCharacteristics);
    HWND hEdit_TimeDateStamp = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportTimeDateStamp);
    HWND hEdit_MajorVersion = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportMajorVersion);
    HWND hEdit_MinorVersion = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportMinorVersion);
    HWND hEdit_Name = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportName);
    HWND hEdit_DllName = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportDllName);
    HWND hEdit_Base = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportBase);
    HWND hEdit_NumberOfFunctions = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportNumbreOfFunctions);
    HWND hEdit_NumberOfNames = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportNumberOfNames);
    HWND hEdit_AddressOfFunctions = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportAddressOfFunctions);
    HWND hEdit_AddressOfNames = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportAddressOfNames);
    HWND hEdit_AddressOfNameOrdinals = GetDlgItem(hwndDlgOfExportTableView, IDC_EDIT_ExportAddressOfNameOrdinals);
    
    //��༭�������shuju
    //���Characteristicsֵ
    char buffer[21];
    memset(buffer, 0, 21);
    //������ת��Ϊ�����ַ���
    ultoa(Export_Characteristics, buffer, 16);
    SetWindowText(hEdit_Characteristics, buffer);
    
    //���TimeDateStampֵ
    memset(buffer, 0, 21);
    ultoa(Export_TimeDateStamp, buffer, 16);
    SetWindowText(hEdit_TimeDateStamp, buffer);

    //���MajorVersionֵ
    memset(buffer, 0, 21);
    ultoa(Export_MajorVersion, buffer, 16);
    SetWindowText(hEdit_MajorVersion, buffer);

    //���MinorVersionֵ
    memset(buffer, 0, 21);
    ultoa(Export_MinorVersion, buffer, 16);
    SetWindowText(hEdit_MinorVersion, buffer);

    //���Nameֵ
    memset(buffer, 0, 21);
    ultoa(Export_Name, buffer, 16);
    SetWindowText(hEdit_Name, buffer);

    //��Ӹ�DLL����
    SetWindowText(hEdit_DllName, (char*)((DWORD)file.p + RvaToFileOffest(file.p, Export_Name)));

    //���Baseֵ
    memset(buffer, 0, 21);
    ultoa(Export_Base, buffer, 16);
    SetWindowText(hEdit_Base, buffer);

    //���NumberOfFunctionsֵ
    memset(buffer, 0, 21);
    ultoa(Export_NumberOfFunctions, buffer, 16);
    SetWindowText(hEdit_NumberOfFunctions, buffer);

    //���NumberOfNamesֵ
    memset(buffer, 0, 21);
    ultoa(Export_NumberOfNames, buffer, 16);
    SetWindowText(hEdit_NumberOfNames, buffer);

    //���AddressOfFunctionsֵ
    memset(buffer, 0, 21);
    ultoa(Export_AddressOfFunctions, buffer, 16);
    SetWindowText(hEdit_AddressOfFunctions, buffer);

    //���AddressOfNamesֵ
    memset(buffer, 0, 21);
    ultoa(Export_AddressOfNames, buffer, 16);
    SetWindowText(hEdit_AddressOfNames, buffer);

    //���AddressOfNameOrdinalsֵ
    memset(buffer, 0, 21);
    ultoa(Export_AddressOfNameOrdinals, buffer, 16);
    SetWindowText(hEdit_AddressOfNameOrdinals, buffer);
}


/**
 * ��λpFileBuffer�е� ������ṹIMAGE_EXPORT_DIRECTORY ���׵�ַ
 * @param pFileBuffer  FileBuffer�׵�ַ
 * @param pExportTable �������׵�ַ���������޵�������Ϊnull��
 */
void GetExportTable(IN LPVOID pFileBuffer, OUT PIMAGE_EXPORT_DIRECTORY* pExportTable){
    PIMAGE_DOS_HEADER pFileDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    PIMAGE_NT_HEADERS pFileNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pFileDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pFilePeHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pFileOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFilePeHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //��ȡ������ṹ��RVA
    DWORD exportTableRva = pFileOptionHeader->DataDirectory[0].VirtualAddress;
    //��exportTableRvaΪ0��˵���ó����޵������򡰷��ء�null
    if (exportTableRva == 0){
        *pExportTable = NULL;
        return ;
    }
    //��������ṹRVAת��ΪFOA
    DWORD exportTableFoa = RvaToFileOffest(pFileBuffer, exportTableRva);
    //��exportTableָ��ָ�򵼳�����׵�ַ
    PIMAGE_EXPORT_DIRECTORY exportTable = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + exportTableFoa);
    //���ص�������׵�ַ
    *pExportTable = exportTable;
}








