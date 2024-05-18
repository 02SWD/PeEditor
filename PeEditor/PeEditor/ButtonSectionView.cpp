// ButtonSectionViewCode.cpp: implementation of the ButtonSectionViewCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonSectionView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
//SectionView的窗口回调函数
BOOL CALLBACK ButtonSectionViewProc(HWND hwndDlgOfSectionView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//初始化节表列表控件，为该空间添加表头
VOID InitSectionTableList(HWND hwndDlgOfSectionView);
//获取FileBuffer中第一个节表的地址
VOID GetFirstSectionTable(IN LPVOID pFileBuffer, OUT PDWORD numberOfSections, OUT PIMAGE_SECTION_HEADER* pFirstSectionTableHeader);
//将节表信息展示出来
VOID ExhibitionSections(HWND hwndDlgOfSectionView, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * 点击 IDC_BUTTON_SectionView 按钮后的处理代码（即：点击 “区段” 按钮之后，要执行的代码）
 * @param  hwndDlgOfPeView 父窗口句柄（在这里是peView窗口的句柄，该窗口ID：IDD_DIALOG_PeViewMainDlg）
 */
VOID ButtonSectionViewCode(HWND hwndDlgOfPeView){
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_SectionTableView), hwndDlgOfPeView, ButtonSectionViewProc);

}

/**
 * SectionView窗口的窗口回调函数（该窗口ID：IDD_DIALOG_SectionTableView）
 * @param  hwndDlgOfSectionView SectionView窗口的句柄
 * @param  uMsg                 消息类型
 * @param  wParam               WPARAM
 * @param  lParam               LPARAM
 * @return                      
 */
BOOL CALLBACK ButtonSectionViewProc(HWND hwndDlgOfSectionView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //初始化窗口，将节表信息展示出来
        case WM_INITDIALOG:{
			//重新设置窗口标题，达到：“当前窗口标题 - 打开的文件路径” 的效果
			ResetWindowTitle(hwndDlgOfSectionView, gSzFileNameOfPeView);
            //初始化表头
            InitSectionTableList(hwndDlgOfSectionView);
            //将节表信息展示出来
            ExhibitionSections(hwndDlgOfSectionView, uMsg, wParam, lParam);
            return TRUE;
        }
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfSectionView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //当点击退出按钮时，退出窗口
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
 * SectionView窗口的句柄（该窗口ID：IDD_DIALOG_SectionTableView）
 * @param  hwndDlgOfSectionView SectionView窗口的句柄
 * @param  uMsg                 消息类型
 * @param  wParam               WPARAM
 * @param  lParam               LPARAM
 */
VOID ExhibitionSections(HWND hwndDlgOfSectionView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //用于存储PE数据信息
    FILEINFOR file;
    //入去PE文件
    file = readFile(gSzFileNameOfPeView);
    //用于存储节表的数量
    DWORD numberOfSections = 0;
    //pFirstSectionTable指针指向第一个节表
    PIMAGE_SECTION_HEADER pFirstSectionTable = NULL;
    PIMAGE_SECTION_HEADER pSectionTableBak = NULL;
    //定位FileBuffer中第一个节表的位置，并返回该PE文件中节的数量
    GetFirstSectionTable(file.p, &numberOfSections, &pFirstSectionTable);

    //根据 SectionView窗口句柄 和 section列表控件ID 获取section列表控件的句柄
    HWND hListSection =  GetDlgItem(hwndDlgOfSectionView, IDC_LIST_SectionTableView);
    //该数据结构用于存储将在列表控件中设置的元素信息
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;

    //向SectionView中添加数据
    for (int i=0; i<numberOfSections; i++){
        //不断令pSectionTableBak指针指向新的节表
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
        
        //设置数据
        vItem.pszText = Section_Name;
        vItem.iItem = i;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListSection, &vItem);

        char buffer[21] = {0};
        //将数字转换为数字字符串
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
 * 定位pFileBuffer中的第一个节表的首地址
 * @param  pFileBuffer              FileBuffer首地址
 * @param  numberOfSections         节的数量
 * @param  pFirstSectionTableHeader 指向FileBuffer中第一个节表的首地址
 */
VOID GetFirstSectionTable(IN LPVOID pFileBuffer, OUT PDWORD numberOfSections, OUT PIMAGE_SECTION_HEADER* pFirstSectionTableHeader){
    PIMAGE_DOS_HEADER pDosHeaderBak = NULL;
    PIMAGE_NT_HEADERS pNTHeaderBak = NULL;
    PIMAGE_FILE_HEADER pPEHeaderBak = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeaderBak = NULL;
    PIMAGE_SECTION_HEADER pFirstSectionHeaderBak = NULL;

    if(!pFileBuffer){                                   
        printf("传入的指针为null！\n");
        return ;
    }
    //判断是否是有效的MZ标志
    if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE){                                   
        printf("不是有效的MZ标志！\n");
        return ;
    }
    //令pDosHeaderBak指针指向PE文件首地址
    pDosHeaderBak = (PIMAGE_DOS_HEADER)pFileBuffer;
    //判断是否是有效的PE标志
    if(*((PDWORD)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew)) != IMAGE_NT_SIGNATURE){                                 
        printf("不是有效的PE标志！\n");
        return ;
    }
    pNTHeaderBak = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew);
    pPEHeaderBak = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeaderBak) + 4);
    pOptionHeaderBak = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeaderBak+IMAGE_SIZEOF_FILE_HEADER);
    pFirstSectionHeaderBak = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeaderBak+pPEHeaderBak->SizeOfOptionalHeader);
    //返回第一个节表的首地址
    *pFirstSectionTableHeader = pFirstSectionHeaderBak;
    //返回节表的数量
    *numberOfSections = pPEHeaderBak->NumberOfSections;
}


/**
 * 初始化节表列表控件的表头
 * @param  hwndDlgOfSectionView SectionView窗口的句柄（该窗口ID：IDD_DIALOG_SectionTableView）
 */
VOID InitSectionTableList(HWND hwndDlgOfSectionView){
    //根据 SectionView窗口句柄 和 列表控件ID 获取节表列表控件的句柄
    HWND hListSectionTable = GetDlgItem(hwndDlgOfSectionView, IDC_LIST_SectionTableView);
    //设置整行选中
    SendMessage(hListSectionTable, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //开始初始化表头
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




