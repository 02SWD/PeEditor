// ButtonImportTableView.cpp: implementation of the ButtonImportTableView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonImportTableView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
//ImportTableView���ڵĴ��ڻص�����
BOOL CALLBACK ButtonImportTableViewProc(HWND hwndDlgOfImportTableView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//չʾ�����Ļ�����Ϣ
VOID ExhibiteImportTableBaseInfo(HWND hwndDlgOfImportTableView);
//����ģ������չʾ��PE�ļ��ڸ�ģ���������õĺ�������Щ
VOID ExhibiteImportFuncByName(HWND hwndDlgOfImportTableView);
//��λFileBuffer�еĵ�һ�������ṹ
VOID GetImportTable(IN LPVOID pFileBuffer, OUT PIMAGE_IMPORT_DESCRIPTOR* pImportTable);
//��ʼ�� չʾ������ŵ��б�ؼ�
VOID InitListImportFuncByOrdinal(HWND hwndDlgOfImportTableView);
//��ʼ�� չʾ������������Hintֵ���б�ؼ�
VOID InitListImportFuncByName(HWND hwndDlgOfImportTableView);


/**
 * ��� IDC_BUTTON_ImportTable_Detailed ��ť��Ĵ�����루������� ��������İ�ť�� ֮��Ҫִ�еĴ��룩
 * @param  hwndDlgOfDataDirectoryView �����ڵľ����������DataDirectoryView���ھ�����ô���ID�� IDD_DIALOG_DataDirectoryView��
 */
VOID ButtonImportTableViewCode(HWND hwndDlgOfDataDirectoryView){
    //����Ҫ�жϸ�pe�ļ��Ƿ���е�����������е�����򵯴���ʾ
    //���ڴ洢FileBuffer
    FILEINFOR file;
    //��PE�ļ�
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer�е����ṹ��λ��
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    //�жϸ�PE�ļ����Ƿ���ڵ����
    GetImportTable(file.p, &pImportTable);
    if (pImportTable == NULL){
        MessageBox(NULL, TEXT("��PEû�е����!"), TEXT("����!"), MB_OK);
        return ;
    }
    //���� ImportTableView ����
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_ImportTableView), hwndDlgOfDataDirectoryView, ButtonImportTableViewProc);
}

/**
 * ImportTableView���ڵĴ��ڻص��������ô���ID��IDD_DIALOG_ImportTableView��
 * @param  hwndDlgOfImportTableView ImportTableView���ڵľ�����ô���ID��IDD_DIALOG_ImportTableView��
 * @param  uMsg                     ��Ϣ����
 * @param  wParam                   WPARAM
 * @param  lParam                   LPARAM
 * @return                          
 */
BOOL CALLBACK ButtonImportTableViewProc(HWND hwndDlgOfImportTableView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //���ڳ�ʼ��ʱҪִ�еĴ���
        case WM_INITDIALOG:{
			//�������ô��ڱ��⣬�ﵽ������ǰ���ڱ��� - �򿪵��ļ�·���� ��Ч��
			ResetWindowTitle(hwndDlgOfImportTableView, gSzFileNameOfPeView);
            //չʾ�����Ļ�����Ϣ��OriginalFirstThunk��TimeDateStamp��ForwarderChain��Name��FirstThunk��
            ExhibiteImportTableBaseInfo(hwndDlgOfImportTableView);
            //��ʼ�� չʾ����������Ҫ���б�ؼ�
            InitListImportFuncByOrdinal(hwndDlgOfImportTableView);
            //��ʼ�� չʾ�������ƺ�Hintֵ���б�ؼ�
            InitListImportFuncByName(hwndDlgOfImportTableView);
            return TRUE;
        }
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfImportTableView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //��� ���˳��� ��ť���˳��Ի���
                case IDC_BUTTON_ImportTableViewExit:{
                    EndDialog(hwndDlgOfImportTableView,0);
                    return TRUE;
                }
            }
        }
        //��д ͨ�ÿؼ���Ϣ �Ĵ������
        case WM_NOTIFY:{
            NMHDR* pNmhdr = (NMHDR*)lParam;
            //���û� ������� �������� ��ָ�����б�ؼ��� �е��еĻ��������ExhibiteImportFuncByName����
            if (wParam == IDC_LIST_ImportTableBaseInfo && pNmhdr->code == NM_CLICK){
                ExhibiteImportFuncByName(hwndDlgOfImportTableView);
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * ���� pImportTable->Name ֵ��չʾָ���ĵ���ģ��ľ�����Ϣ
 * ������ͨ��pImportTable->Nameֵ����λ���Ҫչʾ�ĸ�����ģ�����Ϣ��Ȼ���PE�ļ�ʹ�õĸ�ģ���еĺ����ĺ�����/�������չʾ����
 * @param  hwndDlgOfImportTableView ImportTableView���ڵľ�����ô���ID��IDD_DIALOG_ImportTableView��
 */
VOID ExhibiteImportFuncByName(HWND hwndDlgOfImportTableView){
    //1. �ȵõ��û������������Ӧ�� Name �ֶ�ֵ
    //���� ImportTableView���ھ�� �� �б�ؼ�ID ��ȡ չʾ����������Ϣ���б�ؼ��ľ��
    HWND hListImportTableBaseInfo = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportTableBaseInfo);
    //���ڼ�¼�û�����˵ڼ���
    DWORD dwRowId;
    //����һ�������������ڴ洢����
    TCHAR sizeNameRva[0x20];
    memset(sizeNameRva, 0, 0x20);
    //���ڴ洢���б�ؼ��еõ���Name�ֶ�ֵ
    LV_ITEM vItemNameRva;
    memset(&vItemNameRva, 0, sizeof(LV_ITEM));
    
    //�õ��û����������
    dwRowId = SendMessage(hListImportTableBaseInfo, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    //���û�л�ȡ�� ���С�������Ϊ
    if (dwRowId == -1){
        return ;
    }

    //��Ҫ��ȡ����
    vItemNameRva.iSubItem = 4;
    //ָ���洢��ѯ����Ļ�����
    vItemNameRva.pszText = sizeNameRva;
    //ָ���������Ĵ�С
    vItemNameRva.cchTextMax = 0x20;
    //��ȡָ���к��ж�ӦԪ�ص�����
    SendMessage(hListImportTableBaseInfo, LVM_GETITEMTEXT, dwRowId, (DWORD)&vItemNameRva);
    
    //���ڴ洢�������͵�Name�ֶ�ֵ
    DWORD dwNameRva;
    char* leftOver = NULL;
    //���ַ������͵�Name�ֶ�ֵת��Ϊ�������ͣ�strtoul���������һ��������ָ���Լ����ƽ���ת����
    dwNameRva = strtoul(sizeNameRva, &leftOver, 16);
    //=======================���ϳ���Ĺ�����Ҫ�ǣ���ȡ�û������������ӦName�ֶ�ֵ===========================================
    
    //���� ImportTableView���ھ�� �� ��������б�ؼ���ID �õ� ��������б�ؼ��ľ��
    HWND hListImportFuncByOrdinal = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportFuncByOrdinal);
    //ɾ�� ��������б�ؼ� �����е�����
    SendMessage(hListImportFuncByOrdinal, LVM_DELETEALLITEMS, 0, 0);
    //���� ImportTableView���ھ�� �� ���������б�ؼ���ID �õ� ���������б�ؼ��ľ��
    HWND hListImportFuncByName = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportFuncByName);
    //ɾ�� ���������б�ؼ� �����е�����
    SendMessage(hListImportFuncByName, LVM_DELETEALLITEMS, 0, 0);

    //��ȡPE�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer�еĵ�һ��������λ��
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    GetImportTable(file.p, &pImportTable);
    
    //����һ��������������֮��Ľ�����ת��Ϊ�ַ����Ĺ���
    char buffer[21];
    //��������ʼ��vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //���ڼ�¼�ܵ�����ֵ�������ģ���м��а�������ŵ���ĺ�����Ҳ�а�����������ĺ�������ô�ñ�����ֵ��Ϊ�����������͵���ĺ������ܺͣ�
    int indexOfTotal = 0;
    //���ڼ�¼������Ӧ���� IDC_LIST_ImportFuncByOrdinal �б�ؼ��ĵڼ����������
    int indexOfListFuncByOrdinal = 0;
    //���ڼ�¼������Ӧ���� IDC_LIST_ImportFuncByName �б�ؼ��ĵڼ����������
    int indexOfListFuncByName = 0;
    //���ڵ����ṹ��ֹһ������������ʹ��whileѭ������������ı�־Ϊsizeof(IMAGE_IMPORT_DESCRIPTOR)��0��Ϊ�˷��㣬��������ֱ��ͨ������һ�����Ƿ�Ϊ0�����ж��Ƿ����
    while (pImportTable->Name != 0){
        //��������ֻ��Ҫ�����û�ָ���ĵ���ģ���е���Ϣ���ɣ���������ģ�����Ϣ������
        if (pImportTable->Name != dwNameRva){
            //��pImportTableָ����һ�������ṹ������ģ�飩
            pImportTable++;
            continue;
        }
        //��ȡINT���RVA
        DWORD intRva = pImportTable->OriginalFirstThunk;
        //�ж� intRva �Ƿ�Ϊ��
        if (intRva == 0){
            //�� intRva Ϊ�� �� �õ���ṹ��TimeDateStampֵΪ-1����ֱ�ӵ������������ TimeDateStamp == -1��˵����PE�ļ� �������˰󶨵��롿����ʱ IAT���е����� �� INT���е����� �ǡ���ȫ��ͬ���ģ����Ǿ͡������ԡ���IAT����INT����д�ӡ�ˣ�
            if(pImportTable->TimeDateStamp == -1){
                MessageBox(NULL, TEXT("�޷���ӡ����ĵ��뺯����Ϣ!"), TEXT("����"), MB_OK);
                return ;
            }
            //��FirstThunk��ֵ�䵱INT���RVA��Ϊʲô���Գ䵱�����Ƕ�֪������PE ��û�н��а󶨵��롿 ʱ��IAT���е����� �� INT���е����� �ǡ���ȫ��ͬ���ģ���ʱ���ǡ���ȫ���ԡ���IAT����INT����д�ӡ��
            intRva = pImportTable->FirstThunk;
            //����ʱ�õ���intRva��Ϊ��Ļ�����ֱ�ӵ����澯
            if (intRva == 0){
                MessageBox(NULL, TEXT("�޷���ӡ����ĵ��뺯����Ϣ!"), TEXT("����"), MB_OK);
                return ;
            }
        }

        //����������ֱ����FileBuffer�в����ģ�������Ҫ��INT���RVAת��ΪFOA
        DWORD intFoa = RvaToFileOffest(file.p, intRva);
        //��intTableָ��ָ��INT����׵�ַ����INT��ĵ�һ�������ͨ��pFileBufferָ����INT���FOA�������INT����׵�ַ������PDWORD����ָ��ָ������
        PDWORD intTable = (PDWORD)((DWORD)file.p + intFoa);
        //INT��ı��ֹһ��������0��Ϊ������־��������������ʹ��whileѭ��������*intTable != 0��Ϊ�ж�����
        while(*intTable != 0){
            //�жϱ���ֵ�����λ�Ƿ���1
            if((*intTable & 0x80000000) == 0x80000000){
                //���㵼����ţ������λΪ1�����ȥ���λ��ֵ�����ǵ�����ţ�
                DWORD exportOrdinal = *intTable & 0x7fffffff;

                //�� ��������б�ؼ� �����������ֵ
                memset(buffer, 0, 21);
                //�� ���� ת��Ϊ �����ַ���
                ultoa(indexOfTotal, buffer, 10);
                vItem.pszText = buffer;
                vItem.iItem = indexOfListFuncByOrdinal;
                vItem.iSubItem = 0;
                ListView_InsertItem(hListImportFuncByOrdinal, &vItem);
                
                //�� ��������б�ؼ� ����ӵ������
                memset(buffer, 0, 21);
                ultoa(exportOrdinal, buffer, 16);
                vItem.pszText = buffer;
                vItem.iItem = indexOfListFuncByOrdinal;
                vItem.iSubItem = 1;
                ListView_SetItem(hListImportFuncByOrdinal, &vItem);

                //��������1
                indexOfTotal++;
                //��indexOfListFuncByOrdinalָ�򵼳�����б�ؼ�����һ����
                indexOfListFuncByOrdinal++;
            }else{
                //�õ�PIMAGE_IMPORT_BY_NAME��RVA�������λ��Ϊ1����ñ���ֵ����PIMAGE_IMPORT_BY_NAME��RVA��
                DWORD imageImportByNameRva = *intTable;
                //����������ֱ����FileBuffer�в����ģ�������Ҫ��PIMAGE_IMPORT_BY_NAME��RVAת��ΪFOA
                DWORD imageImportByNameFoa = RvaToFileOffest(file.p, imageImportByNameRva);
                //��funcNameָ��ָ��PIMAGE_IMPORT_BY_NAME���׵�ַ
                PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)file.p + imageImportByNameFoa);
                
                //�� ���������б�ؼ� �����������ֵ
                memset(buffer, 0, 21);
                ultoa(indexOfTotal, buffer, 10);
                vItem.pszText = buffer;
                vItem.iItem = indexOfListFuncByName;
                vItem.iSubItem = 0;
                ListView_InsertItem(hListImportFuncByName, &vItem);

                //�� ��������б�ؼ� �����Hintֵ
                memset(buffer, 0, 21);
                ultoa(funcName->Hint, buffer, 16);
                vItem.pszText = buffer;
                vItem.iItem = indexOfListFuncByName;
                vItem.iSubItem = 1;
                ListView_SetItem(hListImportFuncByName, &vItem);

                //�� ��������б�ؼ� ����Ӻ�������
                vItem.pszText = (char*)(&(funcName->Name));
                vItem.iItem = indexOfListFuncByName;
                vItem.iSubItem = 2;
                ListView_SetItem(hListImportFuncByName, &vItem);
                
                //��������1
                indexOfTotal++;
                //��indexOfListFuncByNameָ���������б�ؼ�����һ����
                indexOfListFuncByName++;
            }
            //��intTableָ��ָ��INT�����һ������
            intTable++;
        }
        break;
    }
}


/**
 * ��ʼ�� IDC_LIST_ImportFuncByOrdinal �б�ؼ��ı�ͷ
 * @param  hwndDlgOfImportTableView ImportTableView���ڵľ�����ô���ID��IDD_DIALOG_ImportTableView��
 */
VOID InitListImportFuncByOrdinal(HWND hwndDlgOfImportTableView){
    //���� ImportTableView���ھ�� �� IDC_LIST_ImportFuncByOrdinal�ؼ�ID �õ� ��������б�ؼ��ľ��
    HWND hListImportFuncByOrdinal = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportFuncByOrdinal);
    //��������ѡ��
    SendMessage(hListImportFuncByOrdinal, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��ʼ���б�ؼ��ı�ͷ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("���� [ʮ����]");
    lv.cx = 130;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListImportFuncByOrdinal, 0, &lv);

    lv.pszText = TEXT("�������");
    lv.cx = 120;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListImportFuncByOrdinal, 1, &lv);
}


/**
 * ��ʼ�� IDC_LIST_ImportFuncByName �б�ؼ��ı�ͷ
 * @param  hwndDlgOfImportTableView ImportTableView���ڵľ�����ô���ID��IDD_DIALOG_ImportTableView��
 */
VOID InitListImportFuncByName(HWND hwndDlgOfImportTableView){
    //���� ImportTableView���ھ�� �� IDC_LIST_ImportFuncByName�ؼ�ID �õ� ���������б�ؼ��ľ��
    HWND hListImportFuncByName = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportFuncByName);
    //��������ѡ��
    SendMessage(hListImportFuncByName, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��ʼ���б�ؼ��ı�ͷ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("���� [ʮ����]");
    lv.cx = 130;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListImportFuncByName, 0, &lv);

    lv.pszText = TEXT("Hint");
    lv.cx = 100;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListImportFuncByName, 1, &lv);

    lv.pszText = TEXT("������");
    lv.cx = 240;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListImportFuncByName, 2, &lv);

}

/**
 * չʾ �����ṹ �Ļ�����Ϣ��OriginalFirstThunk��TimeDateStamp��ForwarderChain��Name��FirstThunk��
 * @param  hwndDlgOfImportTableView ImportTableView���ڵľ�����ô���ID��IDD_DIALOG_ImportTableView��
 */
VOID ExhibiteImportTableBaseInfo(HWND hwndDlgOfImportTableView){
    //���� ImportTableView���ھ�� �� IDC_LIST_ImportTableBaseInfo�ؼ�ID �õ� չʾ�����������Ϣ���б�ؼ����
    HWND hListImportTableBaseInfo = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportTableBaseInfo);
    //��������ѡ��
    SendMessage(hListImportTableBaseInfo, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��ʼ���б�ؼ��ı�ͷ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("Dll Name");
    lv.cx = 160;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListImportTableBaseInfo, 0, &lv);

    lv.pszText = TEXT("OriginalFirstThunk");
    lv.cx = 150;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListImportTableBaseInfo, 1, &lv);

    lv.pszText = TEXT("TimeDateStamp");
    lv.cx = 130;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListImportTableBaseInfo, 2, &lv);

    lv.pszText = TEXT("ForwarderChain");
    lv.cx = 130;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListImportTableBaseInfo, 3, &lv);

    lv.pszText = TEXT("Name");
    lv.cx = 70;
    lv.iSubItem = 4;
    ListView_InsertColumn(hListImportTableBaseInfo, 4, &lv);

    lv.pszText = TEXT("FirstThunk");
    lv.cx = 100;
    lv.iSubItem = 5;
    ListView_InsertColumn(hListImportTableBaseInfo, 5, &lv);
    
    //��ȡPE�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer�е�һ�������ṹ��λ��
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    GetImportTable(file.p, &pImportTable);
    
    //����һ��������������֮��Ľ�����ת��Ϊ�ַ����Ĺ���
    char buffer[21];
    //��������ʼ��vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //IDC_LIST_ImportTableBaseInfo�б�ؼ���������
    int index = 0;
    while (pImportTable->Name != 0 ){
        //�õ�������name����ֵ(��ֵΪ�ַ�����RVA�����ַ������Ǹõ��������Ӧ��dll����)
        DWORD nameRva = pImportTable->Name;
        //���ַ���RVAת��ΪFOA
        DWORD nameFoa = RvaToFileOffest(file.p, nameRva);
        //��nameָ��ָ���ַ������׵�ַ��ͨ��pFileBufferָ������ַ�����FOA��������׵�ַ������char*����ָ��ָ������
        char* Import_DllName = (char*)((DWORD)file.p + nameFoa);
        //�õ� OriginalFirstThunkֵ
        DWORD Import_OriginalFirstThunk = pImportTable->OriginalFirstThunk;
        //�õ� TimeDateStampֵ
        DWORD Import_TimeDateStamp = pImportTable->TimeDateStamp;
        //�õ� ForwarderChainֵ
        DWORD Import_ForwarderChain = pImportTable->ForwarderChain;
        //�õ� Nameֵ
        DWORD Import_Name = pImportTable->Name;
        //�õ� FirstThunkֵ
        DWORD Import_FirstThunk = pImportTable->FirstThunk;
        
        //Ϊ�б�ؼ�����ֵ
        vItem.pszText = Import_DllName;
        vItem.iItem = index;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListImportTableBaseInfo, &vItem);

        memset(buffer, 0 ,21);
        //�� ���� ת��Ϊ �����ַ���
        ultoa(Import_OriginalFirstThunk, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 1;
        ListView_SetItem(hListImportTableBaseInfo, &vItem);

        memset(buffer, 0 ,21);
        ultoa(Import_TimeDateStamp, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 2;
        ListView_SetItem(hListImportTableBaseInfo, &vItem);

        memset(buffer, 0 ,21);
        ultoa(Import_ForwarderChain, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 3;
        ListView_SetItem(hListImportTableBaseInfo, &vItem);

        memset(buffer, 0 ,21);
        ultoa(Import_Name, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 4;
        ListView_SetItem(hListImportTableBaseInfo, &vItem);

        memset(buffer, 0 ,21);
        ultoa(Import_FirstThunk, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 5;
        ListView_SetItem(hListImportTableBaseInfo, &vItem);
        
        //��importTableָ��ָ����һ�������ṹ
        pImportTable++;
        //����ֵ��1
        index++;
    }    
}


/**
 * ��λFileBuffer�е�һ�������ṹ��λ��
 * @param  pFileBuffer  FileBuffer
 * @param  pImportTable ��һ�������ṹ���׵�ַ����PE�޵������ΪNULL��
 * @return              [description]
 */
VOID GetImportTable(IN LPVOID pFileBuffer, OUT PIMAGE_IMPORT_DESCRIPTOR* pImportTable){
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //ͨ������Ŀ¼�ҵ�"�����"��RVA
    DWORD importTableRva = pOptionHeader->DataDirectory[1].VirtualAddress;
    if (importTableRva == 0){
        *pImportTable = NULL;
        return ;
    }
    //����������ֱ����FileBuffer�в����ģ�������Ҫ��������RVAת��ΪFOA
    DWORD importTableFoa = RvaToFileOffest(pFileBuffer, importTableRva);
    //��importTableָ��ָ���һ�������ṹ���׵�ַ��ͨ��pFileBufferָ��͵�����FOA�������������ڴ��е��׵�ַ������PIMAGE_IMPORT_DESCRIPTOR���͵�ָ��ָ������
    PIMAGE_IMPORT_DESCRIPTOR importTable = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + importTableFoa);
    //����һ������ṹ�ĵ�ַ����
    *pImportTable = importTable;
}
