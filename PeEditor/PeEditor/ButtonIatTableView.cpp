// ButtonIatTableView.cpp: implementation of the ButtonIatTableView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonIatTableView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
// IatTableView���ڵĴ��ڻص�����
BOOL CALLBACK ButtonIatTableViewProc(HWND hwndDlgOfIatTableView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//��λFileBuffer�е����ṹ��λ��
VOID GetImportTable(IN LPVOID pFileBuffer, OUT PIMAGE_IMPORT_DESCRIPTOR* pImportTable);
//չʾIat��Ļ�����Ϣ����ʵIAT���в�û����Щ��Ϣ����Щ��Ϣ�Ǹ��ݵ��������õģ�
VOID ExhibiteIatTableBaseInfo(HWND hwndDlgOfIatTableView);
//չʾIAT�����ϸ��Ϣ���Ѱ󶨵��� / δ�󶨵��� ʱ����������ݣ�
VOID ExhibiteIatTableDetailed(HWND hwndDlgOfIatTableView);
//��ʼ�� �Ѱ��б��ͷ
VOID InitListIatTableYesBound(HWND hwndDlgOfIatTableView);
//��ʼ�� δ���б��ͷ
VOID InitListIatTableNoBound(HWND hwndDlgOfIatTableView);
//�ж��Ƿ����IAT��
int JudgeIatTable(LPVOID pFileBuffer);


/**
 * ��� IDC_BUTTON_IATTable_Detailed ��ť��Ĵ�����루������� ��IAT���İ�ť�� ֮��Ҫִ�еĴ��룩
 * @param  hwndDlgOfDataDirectoryView �����ڵľ����������DataDirectoryView���ھ�����ô���ID�� IDD_DIALOG_DataDirectoryView��
 */
VOID ButtonIatTableViewCode(HWND hwndDlgOfDataDirectoryView){
    //��ȡPE�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //�ж��Ƿ����IAT��
    int flag = JudgeIatTable(file.p);
    if (flag == -1){
        MessageBox(NULL, TEXT("��PE�ļ�û��IAT��!"), TEXT("����"), MB_OK);
        return ;
    }   
    //���� IatTableView ����
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_IatTableView), hwndDlgOfDataDirectoryView, ButtonIatTableViewProc);
}

/**
 * IatTableView���ڵĴ��ڻص��������ô���ID��IDD_DIALOG_IatTableView��
 * @param  hwndDlgOfImportTableView IatTableView���ڵľ�����ô���ID��IDD_DIALOG_IatTableView��
 * @param  uMsg                     ��Ϣ����
 * @param  wParam                   WPARAM
 * @param  lParam                   LPARAM
 * @return                          
 */
BOOL CALLBACK ButtonIatTableViewProc(HWND hwndDlgOfIatTableView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //���ڳ�ʼ��ʱҪִ�еĴ���
        case WM_INITDIALOG:{
			//�������ô��ڱ��⣬�ﵽ������ǰ���ڱ��� - �򿪵��ļ�·���� ��Ч��
			ResetWindowTitle(hwndDlgOfIatTableView, gSzFileNameOfPeView);
            //չʾIat��Ļ�����Ϣ����ʵIAT���в�û����Щ��Ϣ����Щ��Ϣ�Ǹ��ݵ��������õģ�
            ExhibiteIatTableBaseInfo(hwndDlgOfIatTableView);
            //��ʼ�� �Ѱ��б��ͷ
            InitListIatTableYesBound(hwndDlgOfIatTableView);
            //��ʼ�� δ���б��ͷ
            InitListIatTableNoBound(hwndDlgOfIatTableView);
            return TRUE;
        }
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfIatTableView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //��� ���˳��� ��ť���˳��Ի���
                case IDC_BUTTON_IatTableViewExit:{
                    EndDialog(hwndDlgOfIatTableView, 0);
                    return TRUE;
                }
            }
        }
        case WM_NOTIFY:{
            NMHDR* pNmhdr = (NMHDR*)lParam;
            //���û� ������� �������� ��ָ�����б�ؼ��� �е��еĻ��������ExhibiteIatTableDetailed����
            if (wParam == IDC_LIST_IatTableInfo && pNmhdr->code == NM_CLICK){
                ExhibiteIatTableDetailed(hwndDlgOfIatTableView);
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * չʾIAT�����ϸ��Ϣ���Ѱ󶨵��� / δ�󶨵��� ʱ����������ݣ�
 * ע�⣺
 *     Ϊ��ʹ��IAT�����Ϣ�����пɶ��ԣ���Щ������ַ����������ЩDll�ģ�����������������ͨ�����������ӻ�ȡIAT������ݵ�
 *     ������ֱ��ͨ������Ŀ¼�λ����IAT��
 * @param  hwndDlgOfIatTableView IatTableView���ڵľ�����ô���ID��IDD_DIALOG_IatTableView��
 */
VOID ExhibiteIatTableDetailed(HWND hwndDlgOfIatTableView){
    //1. �ȵõ��û������������Ӧ�� Name �ֶ�ֵ
    //���� IatTableView���ھ�� �� �б�ؼ�ID ��ȡ չʾIAT������Ϣ���б�ؼ��ľ��
    HWND hListIatTableInfo = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_IatTableInfo);
    //���ڼ�¼�û�����˵ڼ���
    DWORD dwRowId;
    //����һ�������������ڴ洢����
    TCHAR sizeNameRva[0x20];
    memset(sizeNameRva, 0, 0x20);
    //���ڴ洢���б�ؼ��еõ���Name�ֶ�ֵ
    LV_ITEM vItemNameRva;
    memset(&vItemNameRva, 0, sizeof(LV_ITEM));

    //�õ��û����������
    dwRowId = SendMessage(hListIatTableInfo, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    //���û�л�ȡ�� ���С�������Ϊ
    if (dwRowId == -1){
        return ;
    }

    //��Ҫ��ȡ����
    vItemNameRva.iSubItem = 2;
    //ָ���洢��ѯ����Ļ�����
    vItemNameRva.pszText = sizeNameRva;
    //ָ���������Ĵ�С
    vItemNameRva.cchTextMax = 0x20;
    //��ȡָ���к��ж�ӦԪ�ص�����
    SendMessage(hListIatTableInfo, LVM_GETITEMTEXT, dwRowId, (DWORD)&vItemNameRva);
    
    //��ȡָ���к��ж�ӦԪ�ص�����
    DWORD dwNameRva;
    char* leftOver = NULL;
    //���ַ������͵�Name�ֶ�ֵת��Ϊ�������ͣ�strtoul���������һ��������ָ���Լ����ƽ���ת����
    dwNameRva = strtoul(sizeNameRva, &leftOver, 16);
    //=====================���ϳ���Ĺ�����Ҫ�ǣ���ȡ�û������������ӦName�ֶ�ֵ========================

    //���� IatTableView���ھ�� �� �Ѱ��б�ؼ� �õ� �Ѱ��б�ؼ��ľ��
    HWND hListYesBound = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_YesBound);
    //ɾ�� �Ѱ��б�ؼ� �����е�����
    SendMessage(hListYesBound, LVM_DELETEALLITEMS, 0 , 0); 
    //���� IatTableView���ھ�� �� δ���б�ؼ� �õ� δ���б�ؼ��ľ��
    HWND hListNoBound = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_NoBound);
    //ɾ�� δ���б�ؼ� �����е�����
    SendMessage(hListNoBound, LVM_DELETEALLITEMS, 0 , 0); 

    //��ȡPE�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer�е����ṹ��λ��
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    GetImportTable(file.p, &pImportTable);

    //����һ��������������֮��Ľ�����ת��Ϊ�ַ����Ĺ���
    char buffer[21];
    //��������ʼ��vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;

    //�б�ؼ���������
    int index = 0;
    //���ڵ����ṹ��ֹһ������������ʹ��whileѭ������������ı�־Ϊsizeof(IMAGE_IMPORT_DESCRIPTOR)��0��Ϊ�˷��㣬��������ֱ��ͨ������һ�����Ƿ�Ϊ0�����ж��Ƿ����
    while(pImportTable->FirstThunk != 0){
        //��������ֻ��Ҫ�����û�ָ���ĵ���ģ���е���Ϣ���ɣ���������ģ�����Ϣ������
        if (pImportTable->Name != dwNameRva){
            //��pImportTableָ����һ�������ṹ������ģ�飩
            pImportTable++;
            continue;
        }
        //�õ�IAT���RVA
        DWORD iatRva = pImportTable->FirstThunk;
        if (iatRva != 0){
            //����������ֱ����FileBuffer�в����ģ�������Ҫ��IAT���RVAת��ΪFOA
            DWORD iatFoa = RvaToFileOffest(file.p, iatRva);
            //��iatTableָ��ָ��IAT����׵�ַ����IAT��ĵ�һ�������ͨ��pFileBufferָ����IAT���FOA�������IAT����׵�ַ������PDWORD����ָ��ָ������
            PDWORD iatTable = (PDWORD)((DWORD)file.p + iatFoa);
            //�жϸõ�����TimeDataStamp����ֵ�Ƿ�Ϊ0
            if(pImportTable->TimeDateStamp == 0){
                //��TimeDataStampΪ0����˵��δ��"�󶨵����"
                //ѭ������IAT��������ֵΪ0ʱ��ʾ�ñ����
                while(*iatTable != 0){
                    //�� δ���б�ؼ� �У����������ֵ
                    memset(buffer, 0, 21);
                    //�� ���� ת��Ϊ �����ַ���
                    ultoa(index, buffer, 10);
                    vItem.pszText = buffer;
                    vItem.iItem = index;
                    vItem.iSubItem = 0;
                    ListView_InsertItem(hListNoBound, &vItem);
                    //�жϱ���ֵ�����λ�Ƿ�Ϊ1
                    if((*iatTable & 0x80000000) == 0x80000000){
                        //�����λΪ1����ôȥ�����λ�ı���ֵ�����ǵ�����ţ�����������Ŵ���exportOrdinal������
                        DWORD exportOrdinal = *iatTable & 0x7fffffff;
                        //�� δ���б�ؼ� �У���ӵ������ֵ
                        memset(buffer, 0, 21);
                        ultoa(exportOrdinal, buffer, 16);
                        vItem.pszText = buffer;
                        vItem.iItem = index;
                        vItem.iSubItem = 1;
                        ListView_SetItem(hListNoBound, &vItem);
                    }else{
                        //�õ�PIMAGE_IMPORT_BY_NAME��RVA�������λ��Ϊ1����ñ���ֵ����PIMAGE_IMPORT_BY_NAME��RVA��
                        DWORD imageImportByNameRva = *iatTable;
                        //����������ֱ����FileBuffer�в����ģ�������Ҫ��PIMAGE_IMPORT_BY_NAME��RVAת��ΪFOA
                        DWORD imageImportByNameFoa = RvaToFileOffest(file.p, imageImportByNameRva);
                        //��funcNameָ��ָ��PIMAGE_IMPORT_BY_NAME���׵�ַ
                        PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)file.p + imageImportByNameFoa);
                        
                        //�� δ���б�ؼ� �У����Hintֵ
                        memset(buffer, 0, 21);
                        ultoa(funcName->Hint, buffer, 16);
                        vItem.pszText = buffer;
                        vItem.iItem = index;
                        vItem.iSubItem = 2;
                        ListView_SetItem(hListNoBound, &vItem);

                        //�� δ���б�ؼ� �У����ģ������DLL���ƣ�
                        vItem.pszText = (char*)(&(funcName->Name));
                        vItem.iItem = index;
                        vItem.iSubItem = 3;
                        ListView_SetItem(hListNoBound, &vItem);
                    }
                    //��iatTableָ��ָ��IAT�����һ������
                    iatTable++;
                    //�б�ؼ�����������1
                    index++;
                }
            }else{
                //��TimeDataStampΪ-1����˵���Ѱ�"�󶨵����"
                while(*iatTable != 0){
                    //�� �Ѱ��б�ؼ� �У����������ֵ
                    memset(buffer, 0, 21);
                    ultoa(index, buffer, 10);
                    vItem.pszText = buffer;
                    vItem.iItem = index;
                    vItem.iSubItem = 0;
                    ListView_InsertItem(hListYesBound, &vItem);
                    
                    //�� �Ѱ��б�ؼ� �У���Ӻ�������ʵ��ַ
                    memset(buffer, 0, 21);
                    ultoa(*iatTable, buffer, 16);
                    vItem.pszText = buffer;
                    vItem.iItem = index;
                    vItem.iSubItem = 1;
                    ListView_SetItem(hListYesBound, &vItem);
                    
                    //��iatTableָ��ָ��IAT�����һ������
                    iatTable++;
                    //��������1
                    index++;
                }
            }
        }else{
            MessageBox(NULL, TEXT("��PE�ļ���IAT��!"), TEXT("����"), MB_OK);
        }
        break;
    }
}

/**
 * ��ʼ�� δ���б��ͷ
 * @param  hwndDlgOfIatTableView IatTableView���ڵľ�����ô���ID��IDD_DIALOG_IatTableView��
 */
VOID InitListIatTableNoBound(HWND hwndDlgOfIatTableView){
    //���� IatTableView���ھ�� �� δ���б�ؼ� �õ� δ���б�ؼ��ľ��
    HWND hListNoBound = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_NoBound);
    //��������ѡ��
    SendMessage(hListNoBound, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    
    //��ʼ���б�ؼ��ı�ͷ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("���� [ʮ����]");
    lv.cx = 130;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListNoBound, 0, &lv);

    lv.pszText = TEXT("�������");
    lv.cx = 130;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListNoBound, 1, &lv);

    lv.pszText = TEXT("Hint");
    lv.cx = 85;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListNoBound, 2, &lv);

    lv.pszText = TEXT("������");
    lv.cx = 140;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListNoBound, 3, &lv);
}


/**
 * ��ʼ�� �Ѱ��б��ͷ
 * @param  hwndDlgOfIatTableView IatTableView���ڵľ�����ô���ID��IDD_DIALOG_IatTableView��
 */
VOID InitListIatTableYesBound(HWND hwndDlgOfIatTableView){
    //���� IatTableView���ھ�� �� �Ѱ��б�ؼ� �õ� �Ѱ��б�ؼ��ľ��
    HWND hListYesBound = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_YesBound);
    //��������ѡ��
    SendMessage(hListYesBound, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    
    //��ʼ���б�ؼ��ı�ͷ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("���� [ʮ����]");
    lv.cx = 110;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListYesBound, 0, &lv);

    lv.pszText = TEXT("������ʵ��ַ��IAT�������ݣ�");
    lv.cx = 200;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListYesBound, 1, &lv);

}

/**
 * չʾIat��Ļ�����Ϣ����ʵIAT���в�û����Щ��Ϣ����Щ��Ϣ�Ǹ��ݵ��������õģ�
 * @param  hwndDlgOfIatTableView IatTableView���ڵľ�����ô���ID��IDD_DIALOG_IatTableView��
 */
VOID ExhibiteIatTableBaseInfo(HWND hwndDlgOfIatTableView){
    //���� IatTableView���ھ�� �� �б�ؼ�ID ��ȡ չʾIAT������Ϣ���б�ؼ��ľ��
    HWND hListIatTableInfo = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_IatTableInfo);
    //��������ѡ��
    SendMessage(hListIatTableInfo, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��ʼ���б�ؼ��ı�ͷ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("���� [ʮ����]");
    lv.cx = 140;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListIatTableInfo, 0, &lv);

    lv.pszText = TEXT("Dll Name");
    lv.cx = 500;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListIatTableInfo, 1, &lv);

    lv.pszText = TEXT("NameRva");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListIatTableInfo, 2, &lv);
    
    //��ȡPE�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer�е����ṹ��λ��
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    GetImportTable(file.p, &pImportTable);

    //����һ��������������֮��Ľ�����ת��Ϊ�ַ����Ĺ���
    char buffer[21];
    //��������ʼ��vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //�б�ؼ���������
    DWORD index = 0;
    while(pImportTable->FirstThunk != 0){
        //�õ�������name����ֵ(��ֵΪ�ַ�����RVA�����ַ������Ǹõ��������Ӧ��dll����)
        DWORD nameRva = pImportTable->Name;
        //���ַ���RVAת��ΪFOA
        DWORD nameFoa = RvaToFileOffest(file.p, nameRva);
        //��nameָ��ָ���ַ������׵�ַ��ͨ��pFileBufferָ������ַ�����FOA��������׵�ַ������char*����ָ��ָ������
        char* name = (char*)((DWORD)file.p + nameFoa);

        //���б�ؼ��У����������ֵ
        memset(buffer, 0, 21);
        ultoa(index, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListIatTableInfo, &vItem);

        //���б�ؼ��У����DLL���ƣ�����ģ�����ƣ�
        vItem.pszText = name;
        vItem.iItem = index;
        vItem.iSubItem = 1;
        ListView_SetItem(hListIatTableInfo, &vItem);

        //���б�ؼ��У���� pImportTable->Name ֵ
        memset(buffer, 0, 21);
        ultoa(nameRva, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 2;
        ListView_SetItem(hListIatTableInfo, &vItem);
        //��importTableָ��ָ����һ�������ṹ
        pImportTable++;
        //����������1
        index++;
    }
}

/**
 * �жϸ�PE���Ƿ����IAT��
 * @param  pFileBuffer FileBufferָ��
 * @return             ����IAT�򷵻�1�����򷵻�-1
 */
int JudgeIatTable(IN LPVOID pFileBuffer){
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //ͨ������Ŀ¼�ҵ�"�����"��RVA
    DWORD iatTableRva = pOptionHeader->DataDirectory[12].VirtualAddress;
    //��������IAR���򷵻�-1
    if (iatTableRva == 0){
        return -1;
    }
    return 1;
}

