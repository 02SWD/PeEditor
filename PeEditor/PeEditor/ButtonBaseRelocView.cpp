// ButtonBaseRelocView.cpp: implementation of the ButtonBaseRelocView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonBaseRelocView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
//BaseRelocView���ڵĴ��ڻص�����
BOOL CALLBACK ButtonBaseRelocViewProc(HWND hwndDlgOfBaseRelocView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//��λFileBuffer�е��ض�λ���λ��
void GetRelocationTable(IN LPVOID pFileBuffer, OUT PIMAGE_BASE_RELOCATION* pRelocationTable);
//չʾ�ض�λ��Ļ�����Ϣ��ÿһ��� virtualAddress �� sizeOfBlock �ȣ�
VOID ExhibiteBaseRelocTableInfo(HWND hwndDlgOfBaseRelocView);
//չʾ�ض�λ�����ϸ��Ϣ��������ĸ�3λ��������ĵ�12λ������������Ӧ��RVA��
VOID ExhibiteBaseRelocDetailed(HWND hwndDlgOfBaseRelocView);
//��ʼ�� չʾ�ض�λ����ϸ��Ϣ���б�ؼ��ı�ͷ
VOID InitListBaseRelocDetailed(HWND hwndDlgOfBaseRelocView);

/**
 * ��� IDC_BUTTON_BaseRelocTable_Detailed ��ť��Ĵ�����루������� ���ض�λ���İ�ť�� ֮��Ҫִ�еĴ��룩
 * @param  hwndDlgOfDataDirectoryView �����ڵľ����������DataDirectoryView���ھ�����ô���ID�� IDD_DIALOG_DataDirectoryView��
 */
VOID ButtonBaseRelocViewCode(HWND hwndDlgOfDataDirectoryView){
    //����Ҫ�жϸ�pe�ļ��Ƿ�����ض�λ�����������ض�λ���򵯴���ʾ
    //���ڴ洢FileBuffer
    FILEINFOR file;
    //��pe�ļ�
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer���ض�λ���λ��
    PIMAGE_BASE_RELOCATION pRelocationTable = NULL;
    GetRelocationTable(file.p, &pRelocationTable);
    if (pRelocationTable == NULL){
        MessageBox(NULL, TEXT("��PEû���ض�λ��!"), TEXT("����!"), MB_OK);
        return ;
    }
    //���� BaseRelocView ����
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_BaseReLocView), hwndDlgOfDataDirectoryView, ButtonBaseRelocViewProc);
}

/**
 * BaseRelocView ���ڵĴ��ڻص��������ô���ID��IDD_DIALOG_BaseReLocView��
 * @param  hwndDlgOfBaseRelocView BaseRelocView ���ڵľ�����ô���ID��IDD_DIALOG_BaseReLocView��
 * @param  uMsg                   ��Ϣ����
 * @param  wParam                 WPARAM
 * @param  lParam                 LPARAM
 * @return                          
 */
BOOL CALLBACK ButtonBaseRelocViewProc(HWND hwndDlgOfBaseRelocView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //���ڳ�ʼ��Ҫִ�еĴ���
        case WM_INITDIALOG:{
			//�������ô��ڱ��⣬�ﵽ������ǰ���ڱ��� - �򿪵��ļ�·���� ��Ч��
			ResetWindowTitle(hwndDlgOfBaseRelocView, gSzFileNameOfPeView);
            //չʾ�ض�λ��Ļ�����Ϣ��ÿһ��� virtualAddress �� sizeOfBlock �ȣ�
            ExhibiteBaseRelocTableInfo(hwndDlgOfBaseRelocView);
            //��ʼ�� չʾ�ض�λ����ϸ��Ϣ���б�ؼ��ı�ͷ
            InitListBaseRelocDetailed(hwndDlgOfBaseRelocView);
            return TRUE;
        }
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //��� ���˳��� ��ť���˳��Ի���
                case IDC_BUTTON_BaseRelocViewExit:{
                    EndDialog(hwndDlgOfBaseRelocView, 0);
                    return TRUE;
                }
            }
			break;
        }
        //��д ͨ�ÿռ���Ϣ �Ĵ������
        case WM_NOTIFY:{
            NMHDR* pNmhdr = (NMHDR*)lParam;
            //���û� ������� �������� ��ָ�����б�ؼ��� �е��еĻ��������ExhibiteBaseRelocDetailed����
            if (wParam == IDC_LIST_BaseRelocInfo && pNmhdr->code == NM_CLICK){
                ExhibiteBaseRelocDetailed(hwndDlgOfBaseRelocView);
                return TRUE;                
            }
			break;
        }
		//����������Ͻǵĺ��ʱ���رմ���
		case WM_CLOSE:{
			EndDialog(hwndDlgOfBaseRelocView, 0);
			return TRUE;
		}
    }
    return FALSE;
}

/**
 * չʾ�ض�λ�����ϸ��Ϣ��������ĸ�3λ��������ĵ�12λ������������Ӧ��RVA��
 * @param  hwndDlgOfBaseRelocView BaseRelocView ���ڵľ�����ô���ID��IDD_DIALOG_BaseReLocView��
 */
VOID ExhibiteBaseRelocDetailed(HWND hwndDlgOfBaseRelocView){
    //1. �ȵõ��û������������Ӧ�� VirtualAddress �ֶ�ֵ
    //���� BaseRelocView ���ھ�� �� �б�ؼ�ID ��ȡ չʾ�ض�λ�������Ϣ���б�ؼ��ľ��
    HWND hListBaseRelocInfo = GetDlgItem(hwndDlgOfBaseRelocView, IDC_LIST_BaseRelocInfo);
    //���ڼ�¼�û�����˵ڼ���
    DWORD dwRowId;
    //����һ�������������ڴ洢����
    TCHAR sizeVirtualAddr[0x20];
    memset(sizeVirtualAddr, 0, 0x20);
    //���ڴ洢���б�ؼ��еõ���VirtualAddress�ֶ�ֵ
    LV_ITEM vItemVirtualAddr;
    memset(&vItemVirtualAddr, 0, sizeof(LV_ITEM));

    //�õ��û����������
    dwRowId = SendMessage(hListBaseRelocInfo, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    if (dwRowId == -1){
        return ;
    }

    //��Ҫ��ȡ����
    vItemVirtualAddr.iSubItem = 1;
    //ָ���洢��ѯ����Ļ�����
    vItemVirtualAddr.pszText = sizeVirtualAddr;
    //ָ���������Ĵ�С
    vItemVirtualAddr.cchTextMax = 0x20;
    //��ȡָ���к��ж�ӦԪ�ص�����
    SendMessage(hListBaseRelocInfo, LVM_GETITEMTEXT, dwRowId, (DWORD)&vItemVirtualAddr);

    //���ڴ洢�������͵�VirtualAddress�ֶ�ֵ
    DWORD dwVirtualAddr;
    char* leftOver = NULL;
    //���ַ������͵�VirtualAddress�ֶ�ֵת��Ϊ�������ͣ�strtoul���������һ��������ָ���Լ����ƽ���ת����
    dwVirtualAddr = strtoul(sizeVirtualAddr, &leftOver, 16);
    //========================���ϳ���Ĺ�����Ҫ�ǣ���ȡ�û������������ӦVirtualAddress�ֶ�ֵ============================

    //���� BaseRelocView���ھ�� �� �б�ؼ�ID �õ�չʾ�ض�λ����ϸ��Ϣ���б�ؼ��ľ��
    HWND hListBaseRelocDetailed = GetDlgItem(hwndDlgOfBaseRelocView, IDC_LIST_BaseRelocDetailed);
    //ɾ�� չʾ�ض�λ����ϸ��Ϣ�б�ؼ� �����е�����
    SendMessage(hListBaseRelocDetailed, LVM_DELETEALLITEMS, 0 , 0); 

    //��ȡPE�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //��������ʼ��vItem
    PIMAGE_BASE_RELOCATION pRelocationTable = NULL;
    GetRelocationTable(file.p, &pRelocationTable);

    //����һ��������������֮��Ľ�����ת��Ϊ�ַ����Ĺ���
    char buffer[21];
    //��������ʼ��vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //��VirtualAddress��SizeOfBlock��ֵΪ��ʱ����������е��ض�λ�����������Ӧ����ѭ��
    while(!(pRelocationTable->VirtualAddress == 0 && pRelocationTable->SizeOfBlock == 0)){
        //��������ֻ��Ҫ�����û�ָ���Ŀ��е���Ϣ���ɣ����������Ϣ������
        if (pRelocationTable->VirtualAddress != dwVirtualAddr){
            pRelocationTable = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationTable + pRelocationTable->SizeOfBlock);     
            continue;
        }
        //��itemsָ��ָ���������׵�ַ
        short* pItems = (short*)((DWORD)pRelocationTable + 8);
        //���������ĸ���
        DWORD numOfItems = (pRelocationTable->SizeOfBlock - 8)/2;
        //���������ݿ������еľ����������չʾ����
        for(DWORD i=0; i<numOfItems; i++){
            
            //���б�ؼ������ ������
            memset(buffer, 0, 21);
            ultoa(i, buffer, 10);
            vItem.pszText = buffer;
            vItem.iItem = i;
            vItem.iSubItem = 0;
            ListView_InsertItem(hListBaseRelocDetailed, &vItem);

            //���б�ؼ������ �������4λ��ֵ
            memset(buffer, 0, 21);
            ultoa((pItems[i]&0xF000)>>12, buffer, 16);
            vItem.pszText = buffer;
            vItem.iItem = i;
            vItem.iSubItem = 1;
            ListView_SetItem(hListBaseRelocDetailed, &vItem);

            //���б�ؼ������ �������12λ��ֵ
            memset(buffer, 0, 21);
            ultoa(pItems[i]&0x0FFF, buffer, 16);
            vItem.pszText = buffer;
            vItem.iItem = i;
            vItem.iSubItem = 2;
            ListView_SetItem(hListBaseRelocDetailed, &vItem);
            
            //���б�ؼ������ ����������Ӧ��RVA
            memset(buffer, 0, 21);
            ultoa(pRelocationTable->VirtualAddress + (pItems[i]&0x0FFF), buffer, 16);
            vItem.pszText = buffer;
            vItem.iItem = i;
            vItem.iSubItem = 3;
            ListView_SetItem(hListBaseRelocDetailed, &vItem);
        }
        break;
    }
}

/**
 * //չʾ�ض�λ��Ļ�����Ϣ��ÿһ��� virtualAddress �� sizeOfBlock �ȣ�
 * @param  hwndDlgOfBaseRelocView BaseRelocView ���ڵľ�����ô���ID��IDD_DIALOG_BaseReLocView��
 */
VOID ExhibiteBaseRelocTableInfo(HWND hwndDlgOfBaseRelocView){
    //���� BaseRelocView ���ھ�� �� �б�ؼ�ID ��ȡ չʾ�ض�λ�������Ϣ���б�ؼ��ľ��
    HWND hListBaseRelocInfo = GetDlgItem(hwndDlgOfBaseRelocView, IDC_LIST_BaseRelocInfo);
    //��������ѡ��
    SendMessage(hListBaseRelocInfo, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��ʼ���б��ͷ��Ϣ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("���� [ʮ����]");
    lv.cx = 140;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListBaseRelocInfo, 0, &lv);

    lv.pszText = TEXT("VirtualAddress");
    lv.cx = 150;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListBaseRelocInfo, 1, &lv);
    
    lv.pszText = TEXT("SizeOfBlock");
    lv.cx = 150;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListBaseRelocInfo, 2, &lv);
    
    lv.pszText = TEXT("������ĸ��� [ʮ����]");
    lv.cx = 200;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListBaseRelocInfo, 3, &lv);

    //��ȡPE�ļ�
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //��λFileBuffer���ض�λ���λ��
    PIMAGE_BASE_RELOCATION pRelocationTable = NULL;
    GetRelocationTable(file.p, &pRelocationTable);

    //����һ��������������֮��Ľ�����ת��Ϊ�ַ����Ĺ���
    char buffer[21];
    //��������ʼ��vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //�б�ؼ���������
    DWORD index = 0;
    //��VirtualAddress��SizeOfBlock��ֵΪ��ʱ����������е��ض�λ�����������Ӧ����ѭ��
    while(!(pRelocationTable->VirtualAddress == 0 && pRelocationTable->SizeOfBlock == 0)){
        //�õ��ض�λ���virtualAddress����ֵ
        DWORD virtualAddress = pRelocationTable->VirtualAddress;
        //�õ��ض�λ���sizeOfBlock����ֵ
        DWORD sizeOfBlock = pRelocationTable->SizeOfBlock;
        
        //���б�ؼ������������
        memset(buffer, 0 ,21);
        //�� ���� ת��Ϊ �����ַ���
        ultoa(index, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListBaseRelocInfo, &vItem);

        //���б�ؼ������ virtualAddress ֵ
        memset(buffer, 0, 21);
        ultoa(virtualAddress, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 1;
        ListView_SetItem(hListBaseRelocInfo, &vItem);

        //���б�ؼ������ sizeOfBlock ֵ
        memset(buffer, 0, 21);
        ultoa(sizeOfBlock, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 2;
        ListView_SetItem(hListBaseRelocInfo, &vItem);

        //���б�ؼ������ ������ĸ���
        memset(buffer, 0, 21);
        ultoa((sizeOfBlock - 8)/2, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 3;
        ListView_SetItem(hListBaseRelocInfo, &vItem);

        //֮����Ҫ�����ƶ�reLocationָ�룬����ΪPIMAGE_BASE_RELOCATION�ṹ���沢�����ǽ�����PIMAGE_BASE_RELOCATION�ṹ�ģ����ҵľ������С�ǲ�ȷ���ġ�
        pRelocationTable = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationTable + pRelocationTable->SizeOfBlock);
        //������
        index++;
    }
}

/**
 * //��ʼ�� չʾ�ض�λ����ϸ��Ϣ���б�ؼ��ı�ͷ
 * @param  hwndDlgOfBaseRelocView BaseRelocView ���ڵľ�����ô���ID��IDD_DIALOG_BaseReLocView��
 */
VOID InitListBaseRelocDetailed(HWND hwndDlgOfBaseRelocView){
    //���� BaseRelocView���ھ�� �� �б�ؼ�ID �õ�չʾ�ض�λ����ϸ��Ϣ���б�ؼ��ľ��
    HWND hListBaseRelocDetailed = GetDlgItem(hwndDlgOfBaseRelocView, IDC_LIST_BaseRelocDetailed);
    //��������ѡ��
    SendMessage(hListBaseRelocDetailed, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //��ʼ���б��ͷ��Ϣ
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("���� [ʮ����]");
    lv.cx = 140;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListBaseRelocDetailed, 0, &lv);

    lv.pszText = TEXT("������ĸ�4λ");
    lv.cx = 150;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListBaseRelocDetailed, 1, &lv);
    
    lv.pszText = TEXT("������ĵ�12λ");
    lv.cx = 150;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListBaseRelocDetailed, 2, &lv);
    
    lv.pszText = TEXT("�ɸþ������������RVA");
    lv.cx = 200;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListBaseRelocDetailed, 3, &lv);   
}


/**
 * ��λFileBuffer���ض�λ���λ��
 * @param pFileBuffer      FileBuffer
 * @param pRelocationTable �ض�λ����׵�ַ
 */
void GetRelocationTable(IN LPVOID pFileBuffer, OUT PIMAGE_BASE_RELOCATION* pRelocationTable){
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //ͨ������Ŀ¼�ҵ�"�ض�λ��"��RVA
    DWORD reLocationRva = pOptionHeader->DataDirectory[5].VirtualAddress;
    if(reLocationRva == 0){
        *pRelocationTable = NULL;
        return ;
    }
    //����������ֱ����FileBuffer�в����ģ�������Ҫ���ض�λ���RVAת��ΪFOA
    DWORD reLocationFoa = RvaToFileOffest(pFileBuffer, reLocationRva);
    //reLocationָ��ָ���һ���ض�λ����׵�ַ��ͨ��pFileBufferָ����ض�λ���FOA������ض�λ�����ڴ��е��׵�ַ������PIMAGE_BASE_RELOCATION���͵�ָ��ָ������
    PIMAGE_BASE_RELOCATION reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + reLocationFoa);
    //���ض�λ����׵�ַ����
    *pRelocationTable = reLocation;
}








