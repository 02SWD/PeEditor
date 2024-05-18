// ButtonImportTableView.cpp: implementation of the ButtonImportTableView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonImportTableView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
//ImportTableView窗口的窗口回调函数
BOOL CALLBACK ButtonImportTableViewProc(HWND hwndDlgOfImportTableView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//展示导入表的基本信息
VOID ExhibiteImportTableBaseInfo(HWND hwndDlgOfImportTableView);
//根据模块名，展示此PE文件在该模块中所采用的函数有哪些
VOID ExhibiteImportFuncByName(HWND hwndDlgOfImportTableView);
//定位FileBuffer中的第一个导入表结构
VOID GetImportTable(IN LPVOID pFileBuffer, OUT PIMAGE_IMPORT_DESCRIPTOR* pImportTable);
//初始化 展示导出序号的列表控件
VOID InitListImportFuncByOrdinal(HWND hwndDlgOfImportTableView);
//初始化 展示导出函数名和Hint值的列表控件
VOID InitListImportFuncByName(HWND hwndDlgOfImportTableView);


/**
 * 点击 IDC_BUTTON_ImportTable_Detailed 按钮后的处理代码（即：点击 “导入表后的按钮” 之后，要执行的代码）
 * @param  hwndDlgOfDataDirectoryView 父窗口的句柄（这里是DataDirectoryView窗口句柄，该窗口ID： IDD_DIALOG_DataDirectoryView）
 */
VOID ButtonImportTableViewCode(HWND hwndDlgOfDataDirectoryView){
    //首先要判断该pe文件是否具有导入表，若不具有导入表则弹窗提示
    //用于存储FileBuffer
    FILEINFOR file;
    //读PE文件
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中导入表结构的位置
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    //判断该PE文件中是否存在导入表
    GetImportTable(file.p, &pImportTable);
    if (pImportTable == NULL){
        MessageBox(NULL, TEXT("该PE没有导入表!"), TEXT("警告!"), MB_OK);
        return ;
    }
    //创建 ImportTableView 窗口
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_ImportTableView), hwndDlgOfDataDirectoryView, ButtonImportTableViewProc);
}

/**
 * ImportTableView窗口的窗口回调函数（该窗口ID：IDD_DIALOG_ImportTableView）
 * @param  hwndDlgOfImportTableView ImportTableView窗口的句柄（该窗口ID：IDD_DIALOG_ImportTableView）
 * @param  uMsg                     消息类型
 * @param  wParam                   WPARAM
 * @param  lParam                   LPARAM
 * @return                          
 */
BOOL CALLBACK ButtonImportTableViewProc(HWND hwndDlgOfImportTableView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //窗口初始化时要执行的代码
        case WM_INITDIALOG:{
			//重新设置窗口标题，达到：“当前窗口标题 - 打开的文件路径” 的效果
			ResetWindowTitle(hwndDlgOfImportTableView, gSzFileNameOfPeView);
            //展示导入表的基本信息（OriginalFirstThunk、TimeDateStamp、ForwarderChain、Name、FirstThunk）
            ExhibiteImportTableBaseInfo(hwndDlgOfImportTableView);
            //初始化 展示函数导出需要的列表控件
            InitListImportFuncByOrdinal(hwndDlgOfImportTableView);
            //初始化 展示函数名称和Hint值的列表控件
            InitListImportFuncByName(hwndDlgOfImportTableView);
            return TRUE;
        }
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfImportTableView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //点击 “退出” 按钮，退出对话框
                case IDC_BUTTON_ImportTableViewExit:{
                    EndDialog(hwndDlgOfImportTableView,0);
                    return TRUE;
                }
            }
        }
        //编写 通用控件消息 的处理代码
        case WM_NOTIFY:{
            NMHDR* pNmhdr = (NMHDR*)lParam;
            //若用户 “点击” 的是我们 “指定的列表控件” 中的行的话，则调用ExhibiteImportFuncByName函数
            if (wParam == IDC_LIST_ImportTableBaseInfo && pNmhdr->code == NM_CLICK){
                ExhibiteImportFuncByName(hwndDlgOfImportTableView);
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * 根据 pImportTable->Name 值来展示指定的导入模块的具体信息
 * 就是先通过pImportTable->Name值，定位这次要展示哪个导入模块的信息，然年后将PE文件使用的该模块中的函数的函数名/导出序号展示出来
 * @param  hwndDlgOfImportTableView ImportTableView窗口的句柄（该窗口ID：IDD_DIALOG_ImportTableView）
 */
VOID ExhibiteImportFuncByName(HWND hwndDlgOfImportTableView){
    //1. 先得到用户点击的行所对应的 Name 字段值
    //根据 ImportTableView窗口句柄 和 列表控件ID 获取 展示导入表基本信息的列表控件的句柄
    HWND hListImportTableBaseInfo = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportTableBaseInfo);
    //用于记录用户点击了第几行
    DWORD dwRowId;
    //开辟一个缓冲区，用于存储内容
    TCHAR sizeNameRva[0x20];
    memset(sizeNameRva, 0, 0x20);
    //用于存储从列表控件中得到的Name字段值
    LV_ITEM vItemNameRva;
    memset(&vItemNameRva, 0, sizeof(LV_ITEM));
    
    //得到用户点击的行数
    dwRowId = SendMessage(hListImportTableBaseInfo, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    //如果没有获取到 “行”，则不作为
    if (dwRowId == -1){
        return ;
    }

    //想要获取的列
    vItemNameRva.iSubItem = 4;
    //指定存储查询结果的缓冲区
    vItemNameRva.pszText = sizeNameRva;
    //指定缓冲区的大小
    vItemNameRva.cchTextMax = 0x20;
    //获取指定行和列对应元素的内容
    SendMessage(hListImportTableBaseInfo, LVM_GETITEMTEXT, dwRowId, (DWORD)&vItemNameRva);
    
    //用于存储数字类型的Name字段值
    DWORD dwNameRva;
    char* leftOver = NULL;
    //将字符串类型的Name字段值转换为数字类型（strtoul函数的最后一个参数，指定以几进制进行转化）
    dwNameRva = strtoul(sizeNameRva, &leftOver, 16);
    //=======================以上程序的功能主要是：获取用户点击的行所对应Name字段值===========================================
    
    //根据 ImportTableView窗口句柄 和 导出序号列表控件的ID 得到 导出序号列表控件的句柄
    HWND hListImportFuncByOrdinal = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportFuncByOrdinal);
    //删除 导出序号列表控件 中所有的数据
    SendMessage(hListImportFuncByOrdinal, LVM_DELETEALLITEMS, 0, 0);
    //根据 ImportTableView窗口句柄 和 函数名称列表控件的ID 得到 函数名称列表控件的句柄
    HWND hListImportFuncByName = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportFuncByName);
    //删除 函数名称列表控件 中所有的数据
    SendMessage(hListImportFuncByName, LVM_DELETEALLITEMS, 0, 0);

    //读取PE文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中的第一个导入表的位置
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    GetImportTable(file.p, &pImportTable);
    
    //声明一个缓冲区，用于之后的将数字转换为字符串的功能
    char buffer[21];
    //声明并初始化vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //用于记录总的索引值（如果该模块中既有按导出序号导入的函数，也有按函数名导入的函数，那么该变量的值则为按这两种类型导入的函数的总和）
    int indexOfTotal = 0;
    //用于记录接下来应该向 IDC_LIST_ImportFuncByOrdinal 列表控件的第几行添加数据
    int indexOfListFuncByOrdinal = 0;
    //用于记录接下来应该向 IDC_LIST_ImportFuncByName 列表控件的第几行添加数据
    int indexOfListFuncByName = 0;
    //由于导入表结构不止一个，所以这里使用while循环，而其结束的标志为sizeof(IMAGE_IMPORT_DESCRIPTOR)个0。为了方便，在这里我直接通过其中一部分是否为0，来判断是否结束
    while (pImportTable->Name != 0){
        //这里我们只需要遍历用户指定的导入模块中的信息即可，其他导入模块的信息均跳过
        if (pImportTable->Name != dwNameRva){
            //令pImportTable指向下一个导入表结构（导入模块）
            pImportTable++;
            continue;
        }
        //获取INT表的RVA
        DWORD intRva = pImportTable->OriginalFirstThunk;
        //判断 intRva 是否为零
        if (intRva == 0){
            //若 intRva 为零 且 该导入结构的TimeDateStamp值为-1，则直接弹窗报警（如果 TimeDateStamp == -1，说明该PE文件 【进行了绑定导入】，此时 IAT表中的内容 与 INT表中的内容 是【完全不同】的，我们就【不可以】将IAT表当做INT表进行打印了）
            if(pImportTable->TimeDateStamp == -1){
                MessageBox(NULL, TEXT("无法打印具体的导入函数信息!"), TEXT("警告"), MB_OK);
                return ;
            }
            //将FirstThunk的值充当INT表的RVA（为什么可以充当？我们都知道，当PE 【没有进行绑定导入】 时，IAT表中的内容 和 INT表中的内容 是【完全相同】的，此时我们【完全可以】将IAT表当做INT表进行打印）
            intRva = pImportTable->FirstThunk;
            //若此时得到的intRva仍为零的话，则直接弹窗告警
            if (intRva == 0){
                MessageBox(NULL, TEXT("无法打印具体的导入函数信息!"), TEXT("警告"), MB_OK);
                return ;
            }
        }

        //由于我们是直接在FileBuffer中操作的，所以需要将INT表的RVA转换为FOA
        DWORD intFoa = RvaToFileOffest(file.p, intRva);
        //将intTable指针指向INT表的首地址，即INT表的第一个表项（即通过pFileBuffer指针与INT表的FOA，计算出INT表的首地址，并用PDWORD类型指针指向它）
        PDWORD intTable = (PDWORD)((DWORD)file.p + intFoa);
        //INT表的表项不止一个，且以0作为结束标志；所以我们这里使用while循环，并以*intTable != 0作为判断条件
        while(*intTable != 0){
            //判断表项值的最高位是否是1
            if((*intTable & 0x80000000) == 0x80000000){
                //计算导出序号（若最高位为1，则除去最高位的值，就是导出序号）
                DWORD exportOrdinal = *intTable & 0x7fffffff;

                //向 导出序号列表控件 中添加总索引值
                memset(buffer, 0, 21);
                //将 数字 转化为 数字字符串
                ultoa(indexOfTotal, buffer, 10);
                vItem.pszText = buffer;
                vItem.iItem = indexOfListFuncByOrdinal;
                vItem.iSubItem = 0;
                ListView_InsertItem(hListImportFuncByOrdinal, &vItem);
                
                //向 导出序号列表控件 中添加导出序号
                memset(buffer, 0, 21);
                ultoa(exportOrdinal, buffer, 16);
                vItem.pszText = buffer;
                vItem.iItem = indexOfListFuncByOrdinal;
                vItem.iSubItem = 1;
                ListView_SetItem(hListImportFuncByOrdinal, &vItem);

                //总索引加1
                indexOfTotal++;
                //令indexOfListFuncByOrdinal指向导出序号列表控件的下一空行
                indexOfListFuncByOrdinal++;
            }else{
                //得到PIMAGE_IMPORT_BY_NAME的RVA（若最高位不为1，则该表项值便是PIMAGE_IMPORT_BY_NAME的RVA）
                DWORD imageImportByNameRva = *intTable;
                //由于我们是直接在FileBuffer中操作的，所以需要将PIMAGE_IMPORT_BY_NAME的RVA转换为FOA
                DWORD imageImportByNameFoa = RvaToFileOffest(file.p, imageImportByNameRva);
                //将funcName指针指向PIMAGE_IMPORT_BY_NAME的首地址
                PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)file.p + imageImportByNameFoa);
                
                //向 函数名称列表控件 中添加总索引值
                memset(buffer, 0, 21);
                ultoa(indexOfTotal, buffer, 10);
                vItem.pszText = buffer;
                vItem.iItem = indexOfListFuncByName;
                vItem.iSubItem = 0;
                ListView_InsertItem(hListImportFuncByName, &vItem);

                //向 导出序号列表控件 中添加Hint值
                memset(buffer, 0, 21);
                ultoa(funcName->Hint, buffer, 16);
                vItem.pszText = buffer;
                vItem.iItem = indexOfListFuncByName;
                vItem.iSubItem = 1;
                ListView_SetItem(hListImportFuncByName, &vItem);

                //向 导出序号列表控件 中添加函数名称
                vItem.pszText = (char*)(&(funcName->Name));
                vItem.iItem = indexOfListFuncByName;
                vItem.iSubItem = 2;
                ListView_SetItem(hListImportFuncByName, &vItem);
                
                //总索引加1
                indexOfTotal++;
                //令indexOfListFuncByName指向函数名称列表控件的下一空行
                indexOfListFuncByName++;
            }
            //将intTable指针指向INT表的下一个表项
            intTable++;
        }
        break;
    }
}


/**
 * 初始化 IDC_LIST_ImportFuncByOrdinal 列表控件的表头
 * @param  hwndDlgOfImportTableView ImportTableView窗口的句柄（该窗口ID：IDD_DIALOG_ImportTableView）
 */
VOID InitListImportFuncByOrdinal(HWND hwndDlgOfImportTableView){
    //根据 ImportTableView窗口句柄 和 IDC_LIST_ImportFuncByOrdinal控件ID 得到 导出序号列表控件的句柄
    HWND hListImportFuncByOrdinal = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportFuncByOrdinal);
    //设置整行选中
    SendMessage(hListImportFuncByOrdinal, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //初始化列表控件的表头
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("索引 [十进制]");
    lv.cx = 130;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListImportFuncByOrdinal, 0, &lv);

    lv.pszText = TEXT("导出序号");
    lv.cx = 120;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListImportFuncByOrdinal, 1, &lv);
}


/**
 * 初始化 IDC_LIST_ImportFuncByName 列表控件的表头
 * @param  hwndDlgOfImportTableView ImportTableView窗口的句柄（该窗口ID：IDD_DIALOG_ImportTableView）
 */
VOID InitListImportFuncByName(HWND hwndDlgOfImportTableView){
    //根据 ImportTableView窗口句柄 和 IDC_LIST_ImportFuncByName控件ID 得到 函数名称列表控件的句柄
    HWND hListImportFuncByName = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportFuncByName);
    //设置整行选中
    SendMessage(hListImportFuncByName, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //初始化列表控件的表头
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("索引 [十进制]");
    lv.cx = 130;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListImportFuncByName, 0, &lv);

    lv.pszText = TEXT("Hint");
    lv.cx = 100;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListImportFuncByName, 1, &lv);

    lv.pszText = TEXT("函数名");
    lv.cx = 240;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListImportFuncByName, 2, &lv);

}

/**
 * 展示 导入表结构 的基本信息（OriginalFirstThunk、TimeDateStamp、ForwarderChain、Name、FirstThunk）
 * @param  hwndDlgOfImportTableView ImportTableView窗口的句柄（该窗口ID：IDD_DIALOG_ImportTableView）
 */
VOID ExhibiteImportTableBaseInfo(HWND hwndDlgOfImportTableView){
    //根据 ImportTableView窗口句柄 和 IDC_LIST_ImportTableBaseInfo控件ID 得到 展示导出表基本信息的列表控件句柄
    HWND hListImportTableBaseInfo = GetDlgItem(hwndDlgOfImportTableView, IDC_LIST_ImportTableBaseInfo);
    //设置整行选中
    SendMessage(hListImportTableBaseInfo, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //初始化列表控件的表头
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
    
    //读取PE文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中第一个导入表结构的位置
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    GetImportTable(file.p, &pImportTable);
    
    //声明一个缓冲区，用于之后的将数字转换为字符串的功能
    char buffer[21];
    //声明并初始化vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //IDC_LIST_ImportTableBaseInfo列表控件的行索引
    int index = 0;
    while (pImportTable->Name != 0 ){
        //得到导入表的name属性值(该值为字符串的RVA，而字符串就是该导入表所对应的dll名字)
        DWORD nameRva = pImportTable->Name;
        //将字符串RVA转换为FOA
        DWORD nameFoa = RvaToFileOffest(file.p, nameRva);
        //令name指针指向字符串的首地址（通过pFileBuffer指针与该字符串的FOA，计算出首地址，并用char*类型指针指向它）
        char* Import_DllName = (char*)((DWORD)file.p + nameFoa);
        //得到 OriginalFirstThunk值
        DWORD Import_OriginalFirstThunk = pImportTable->OriginalFirstThunk;
        //得到 TimeDateStamp值
        DWORD Import_TimeDateStamp = pImportTable->TimeDateStamp;
        //得到 ForwarderChain值
        DWORD Import_ForwarderChain = pImportTable->ForwarderChain;
        //得到 Name值
        DWORD Import_Name = pImportTable->Name;
        //得到 FirstThunk值
        DWORD Import_FirstThunk = pImportTable->FirstThunk;
        
        //为列表控件设置值
        vItem.pszText = Import_DllName;
        vItem.iItem = index;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListImportTableBaseInfo, &vItem);

        memset(buffer, 0 ,21);
        //将 数字 转化为 数字字符串
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
        
        //令importTable指针指向下一个导入表结构
        pImportTable++;
        //索引值加1
        index++;
    }    
}


/**
 * 定位FileBuffer中第一个导入表结构的位置
 * @param  pFileBuffer  FileBuffer
 * @param  pImportTable 第一个导入表结构的首地址（若PE无导入表，则为NULL）
 * @return              [description]
 */
VOID GetImportTable(IN LPVOID pFileBuffer, OUT PIMAGE_IMPORT_DESCRIPTOR* pImportTable){
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //通过数据目录找到"导入表"的RVA
    DWORD importTableRva = pOptionHeader->DataDirectory[1].VirtualAddress;
    if (importTableRva == 0){
        *pImportTable = NULL;
        return ;
    }
    //由于我们是直接在FileBuffer中操作的，所以需要将导入表的RVA转换为FOA
    DWORD importTableFoa = RvaToFileOffest(pFileBuffer, importTableRva);
    //令importTable指针指向第一个导入表结构的首地址（通过pFileBuffer指针和导入表的FOA计算出导入表在内存中的首地址，并令PIMAGE_IMPORT_DESCRIPTOR类型的指针指向它）
    PIMAGE_IMPORT_DESCRIPTOR importTable = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pFileBuffer + importTableFoa);
    //将第一个导入结构的地址传出
    *pImportTable = importTable;
}
