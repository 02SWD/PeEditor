// ButtonBaseRelocView.cpp: implementation of the ButtonBaseRelocView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonBaseRelocView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
//BaseRelocView窗口的窗口回调函数
BOOL CALLBACK ButtonBaseRelocViewProc(HWND hwndDlgOfBaseRelocView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//定位FileBuffer中的重定位表的位置
void GetRelocationTable(IN LPVOID pFileBuffer, OUT PIMAGE_BASE_RELOCATION* pRelocationTable);
//展示重定位表的基本信息（每一块的 virtualAddress 和 sizeOfBlock 等）
VOID ExhibiteBaseRelocTableInfo(HWND hwndDlgOfBaseRelocView);
//展示重定位表的详细信息（具体项的高3位，具体项的低12位，具体项所对应的RVA）
VOID ExhibiteBaseRelocDetailed(HWND hwndDlgOfBaseRelocView);
//初始化 展示重定位表详细信息的列表控件的表头
VOID InitListBaseRelocDetailed(HWND hwndDlgOfBaseRelocView);

/**
 * 点击 IDC_BUTTON_BaseRelocTable_Detailed 按钮后的处理代码（即：点击 “重定位表后的按钮” 之后，要执行的代码）
 * @param  hwndDlgOfDataDirectoryView 父窗口的句柄（这里是DataDirectoryView窗口句柄，该窗口ID： IDD_DIALOG_DataDirectoryView）
 */
VOID ButtonBaseRelocViewCode(HWND hwndDlgOfDataDirectoryView){
    //首先要判断该pe文件是否具有重定位表，若不具有重定位表则弹窗提示
    //用于存储FileBuffer
    FILEINFOR file;
    //读pe文件
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中重定位表的位置
    PIMAGE_BASE_RELOCATION pRelocationTable = NULL;
    GetRelocationTable(file.p, &pRelocationTable);
    if (pRelocationTable == NULL){
        MessageBox(NULL, TEXT("该PE没有重定位表!"), TEXT("警告!"), MB_OK);
        return ;
    }
    //创建 BaseRelocView 窗口
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_BaseReLocView), hwndDlgOfDataDirectoryView, ButtonBaseRelocViewProc);
}

/**
 * BaseRelocView 窗口的窗口回调函数（该窗口ID：IDD_DIALOG_BaseReLocView）
 * @param  hwndDlgOfBaseRelocView BaseRelocView 窗口的句柄（该窗口ID：IDD_DIALOG_BaseReLocView）
 * @param  uMsg                   消息类型
 * @param  wParam                 WPARAM
 * @param  lParam                 LPARAM
 * @return                          
 */
BOOL CALLBACK ButtonBaseRelocViewProc(HWND hwndDlgOfBaseRelocView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //窗口初始化要执行的代码
        case WM_INITDIALOG:{
			//重新设置窗口标题，达到：“当前窗口标题 - 打开的文件路径” 的效果
			ResetWindowTitle(hwndDlgOfBaseRelocView, gSzFileNameOfPeView);
            //展示重定位表的基本信息（每一块的 virtualAddress 和 sizeOfBlock 等）
            ExhibiteBaseRelocTableInfo(hwndDlgOfBaseRelocView);
            //初始化 展示重定位表详细信息的列表控件的表头
            InitListBaseRelocDetailed(hwndDlgOfBaseRelocView);
            return TRUE;
        }
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //点击 “退出” 按钮，退出对话框
                case IDC_BUTTON_BaseRelocViewExit:{
                    EndDialog(hwndDlgOfBaseRelocView, 0);
                    return TRUE;
                }
            }
			break;
        }
        //编写 通用空间消息 的处理代码
        case WM_NOTIFY:{
            NMHDR* pNmhdr = (NMHDR*)lParam;
            //若用户 “点击” 的是我们 “指定的列表控件” 中的行的话，则调用ExhibiteBaseRelocDetailed函数
            if (wParam == IDC_LIST_BaseRelocInfo && pNmhdr->code == NM_CLICK){
                ExhibiteBaseRelocDetailed(hwndDlgOfBaseRelocView);
                return TRUE;                
            }
			break;
        }
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfBaseRelocView, 0);
			return TRUE;
		}
    }
    return FALSE;
}

/**
 * 展示重定位表的详细信息（具体项的高3位，具体项的低12位，具体项所对应的RVA）
 * @param  hwndDlgOfBaseRelocView BaseRelocView 窗口的句柄（该窗口ID：IDD_DIALOG_BaseReLocView）
 */
VOID ExhibiteBaseRelocDetailed(HWND hwndDlgOfBaseRelocView){
    //1. 先得到用户点击的行所对应的 VirtualAddress 字段值
    //根据 BaseRelocView 窗口句柄 和 列表控件ID 获取 展示重定位表基本信息的列表控件的句柄
    HWND hListBaseRelocInfo = GetDlgItem(hwndDlgOfBaseRelocView, IDC_LIST_BaseRelocInfo);
    //用于记录用户点击了第几行
    DWORD dwRowId;
    //开辟一个缓冲区，用于存储内容
    TCHAR sizeVirtualAddr[0x20];
    memset(sizeVirtualAddr, 0, 0x20);
    //用于存储从列表控件中得到的VirtualAddress字段值
    LV_ITEM vItemVirtualAddr;
    memset(&vItemVirtualAddr, 0, sizeof(LV_ITEM));

    //得到用户点击的行数
    dwRowId = SendMessage(hListBaseRelocInfo, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    if (dwRowId == -1){
        return ;
    }

    //想要获取的列
    vItemVirtualAddr.iSubItem = 1;
    //指定存储查询结果的缓冲区
    vItemVirtualAddr.pszText = sizeVirtualAddr;
    //指定缓冲区的大小
    vItemVirtualAddr.cchTextMax = 0x20;
    //获取指定行和列对应元素的内容
    SendMessage(hListBaseRelocInfo, LVM_GETITEMTEXT, dwRowId, (DWORD)&vItemVirtualAddr);

    //用于存储数字类型的VirtualAddress字段值
    DWORD dwVirtualAddr;
    char* leftOver = NULL;
    //将字符串类型的VirtualAddress字段值转换为数字类型（strtoul函数的最后一个参数，指定以几进制进行转化）
    dwVirtualAddr = strtoul(sizeVirtualAddr, &leftOver, 16);
    //========================以上程序的功能主要是：获取用户点击的行所对应VirtualAddress字段值============================

    //根据 BaseRelocView窗口句柄 和 列表控件ID 得到展示重定位表详细信息的列表控件的句柄
    HWND hListBaseRelocDetailed = GetDlgItem(hwndDlgOfBaseRelocView, IDC_LIST_BaseRelocDetailed);
    //删除 展示重定位表详细信息列表控件 中所有的数据
    SendMessage(hListBaseRelocDetailed, LVM_DELETEALLITEMS, 0 , 0); 

    //读取PE文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //声明并初始化vItem
    PIMAGE_BASE_RELOCATION pRelocationTable = NULL;
    GetRelocationTable(file.p, &pRelocationTable);

    //声明一个缓冲区，用于之后的将数字转换为字符串的功能
    char buffer[21];
    //声明并初始化vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //当VirtualAddress与SizeOfBlock的值为零时，则代表所有的重定位表遍历结束，应跳出循环
    while(!(pRelocationTable->VirtualAddress == 0 && pRelocationTable->SizeOfBlock == 0)){
        //这里我们只需要遍历用户指定的块中的信息即可，其他块的信息均跳过
        if (pRelocationTable->VirtualAddress != dwVirtualAddr){
            pRelocationTable = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationTable + pRelocationTable->SizeOfBlock);     
            continue;
        }
        //将items指针指向具体项的首地址
        short* pItems = (short*)((DWORD)pRelocationTable + 8);
        //计算具体项的个数
        DWORD numOfItems = (pRelocationTable->SizeOfBlock - 8)/2;
        //遍历该数据块中所有的具体项，并将其展示出来
        for(DWORD i=0; i<numOfItems; i++){
            
            //向列表控件中添加 行索引
            memset(buffer, 0, 21);
            ultoa(i, buffer, 10);
            vItem.pszText = buffer;
            vItem.iItem = i;
            vItem.iSubItem = 0;
            ListView_InsertItem(hListBaseRelocDetailed, &vItem);

            //向列表控件中添加 具体项高4位的值
            memset(buffer, 0, 21);
            ultoa((pItems[i]&0xF000)>>12, buffer, 16);
            vItem.pszText = buffer;
            vItem.iItem = i;
            vItem.iSubItem = 1;
            ListView_SetItem(hListBaseRelocDetailed, &vItem);

            //向列表控件中添加 具体项低12位的值
            memset(buffer, 0, 21);
            ultoa(pItems[i]&0x0FFF, buffer, 16);
            vItem.pszText = buffer;
            vItem.iItem = i;
            vItem.iSubItem = 2;
            ListView_SetItem(hListBaseRelocDetailed, &vItem);
            
            //向列表控件中添加 具体项所对应的RVA
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
 * //展示重定位表的基本信息（每一块的 virtualAddress 和 sizeOfBlock 等）
 * @param  hwndDlgOfBaseRelocView BaseRelocView 窗口的句柄（该窗口ID：IDD_DIALOG_BaseReLocView）
 */
VOID ExhibiteBaseRelocTableInfo(HWND hwndDlgOfBaseRelocView){
    //根据 BaseRelocView 窗口句柄 和 列表控件ID 获取 展示重定位表基本信息的列表控件的句柄
    HWND hListBaseRelocInfo = GetDlgItem(hwndDlgOfBaseRelocView, IDC_LIST_BaseRelocInfo);
    //设置整行选中
    SendMessage(hListBaseRelocInfo, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //初始化列表表头信息
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("索引 [十进制]");
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
    
    lv.pszText = TEXT("具体项的个数 [十进制]");
    lv.cx = 200;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListBaseRelocInfo, 3, &lv);

    //读取PE文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中重定位表的位置
    PIMAGE_BASE_RELOCATION pRelocationTable = NULL;
    GetRelocationTable(file.p, &pRelocationTable);

    //声明一个缓冲区，用于之后的将数字转换为字符串的功能
    char buffer[21];
    //声明并初始化vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //列表控件的行索引
    DWORD index = 0;
    //当VirtualAddress与SizeOfBlock的值为零时，则代表所有的重定位表遍历结束，应跳出循环
    while(!(pRelocationTable->VirtualAddress == 0 && pRelocationTable->SizeOfBlock == 0)){
        //得到重定位表的virtualAddress属性值
        DWORD virtualAddress = pRelocationTable->VirtualAddress;
        //得到重定位表的sizeOfBlock属性值
        DWORD sizeOfBlock = pRelocationTable->SizeOfBlock;
        
        //向列表控件中添加行索引
        memset(buffer, 0 ,21);
        //将 数字 转化为 数字字符串
        ultoa(index, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListBaseRelocInfo, &vItem);

        //向列表控件中添加 virtualAddress 值
        memset(buffer, 0, 21);
        ultoa(virtualAddress, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 1;
        ListView_SetItem(hListBaseRelocInfo, &vItem);

        //向列表控件中添加 sizeOfBlock 值
        memset(buffer, 0, 21);
        ultoa(sizeOfBlock, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 2;
        ListView_SetItem(hListBaseRelocInfo, &vItem);

        //向列表控件中添加 具体项的个数
        memset(buffer, 0, 21);
        ultoa((sizeOfBlock - 8)/2, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 3;
        ListView_SetItem(hListBaseRelocInfo, &vItem);

        //之所以要这样移动reLocation指针，是因为PIMAGE_BASE_RELOCATION结构后面并不还是紧跟着PIMAGE_BASE_RELOCATION结构的，并且的具体项大小是不确定的。
        pRelocationTable = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationTable + pRelocationTable->SizeOfBlock);
        //行索引
        index++;
    }
}

/**
 * //初始化 展示重定位表详细信息的列表控件的表头
 * @param  hwndDlgOfBaseRelocView BaseRelocView 窗口的句柄（该窗口ID：IDD_DIALOG_BaseReLocView）
 */
VOID InitListBaseRelocDetailed(HWND hwndDlgOfBaseRelocView){
    //根据 BaseRelocView窗口句柄 和 列表控件ID 得到展示重定位表详细信息的列表控件的句柄
    HWND hListBaseRelocDetailed = GetDlgItem(hwndDlgOfBaseRelocView, IDC_LIST_BaseRelocDetailed);
    //设置整行选中
    SendMessage(hListBaseRelocDetailed, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //初始化列表表头信息
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("索引 [十进制]");
    lv.cx = 140;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListBaseRelocDetailed, 0, &lv);

    lv.pszText = TEXT("具体项的高4位");
    lv.cx = 150;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListBaseRelocDetailed, 1, &lv);
    
    lv.pszText = TEXT("具体项的低12位");
    lv.cx = 150;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListBaseRelocDetailed, 2, &lv);
    
    lv.pszText = TEXT("由该具体项所计算的RVA");
    lv.cx = 200;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListBaseRelocDetailed, 3, &lv);   
}


/**
 * 定位FileBuffer中重定位表的位置
 * @param pFileBuffer      FileBuffer
 * @param pRelocationTable 重定位表的首地址
 */
void GetRelocationTable(IN LPVOID pFileBuffer, OUT PIMAGE_BASE_RELOCATION* pRelocationTable){
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //通过数据目录找到"重定位表"的RVA
    DWORD reLocationRva = pOptionHeader->DataDirectory[5].VirtualAddress;
    if(reLocationRva == 0){
        *pRelocationTable = NULL;
        return ;
    }
    //由于我们是直接在FileBuffer中操作的，所以需要将重定位表的RVA转换为FOA
    DWORD reLocationFoa = RvaToFileOffest(pFileBuffer, reLocationRva);
    //reLocation指针指向第一个重定位表的首地址（通过pFileBuffer指针和重定位表的FOA计算出重定位表在内存中的首地址，并令PIMAGE_BASE_RELOCATION类型的指针指向它）
    PIMAGE_BASE_RELOCATION reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + reLocationFoa);
    //将重定位表的首地址传出
    *pRelocationTable = reLocation;
}








