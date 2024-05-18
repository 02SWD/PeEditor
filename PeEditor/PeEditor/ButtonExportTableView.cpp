// ButtonExportTableView.cpp: implementation of the ButtonExportTableView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonExportTableView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
//ExportTableView窗口的窗口回调函数
BOOL CALLBACK ButtonExportTableViewProc(HWND hwndDlgOfExportTableView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//在ExportTableView窗口展示导出表的基本信息
VOID ExhibiteBasicInfo(HWND hwndDlgOfExportTableView);
//在ExportTableView窗口展示导出表中-导出函数地址表的信息
VOID ExhibiteAddressOfFunctions(HWND hwndDlgOfExportTableView);
//在ExportTableView窗口展示导出表中-导出函数序号表和导出函数名称表的信息
VOID ExhibiteOtherFunctions(HWND hwndDlgOfExportTableView);
//定位导出表结构
void GetExportTable(IN LPVOID pFileBuffer, OUT PIMAGE_EXPORT_DIRECTORY* pExportTable);

/**
 * 点击 IDC_BUTTON_ExportTable_Detailed 按钮后的处理代码（即：点击 “导出表后的按钮” 之后，要执行的代码）
 * @param  hwndDlgOfDataDirectoryView 父窗口句柄（这里是DataDirectoryView窗口句柄，该窗口ID： IDD_DIALOG_DataDirectoryView）
 */
VOID ButtonExportTableViewCode(HWND hwndDlgOfDataDirectoryView){
    //首先要判断该pe文件是否具有导出表，若不具有导出表则弹窗提示
    //用于存储FileBuffer
    FILEINFOR file;
    //读pe文件
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中导出表结构的位置
    PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
    GetExportTable(file.p, &pExportTable);
    //判断该PE文件是否存在导出表
    if (pExportTable == NULL){
        MessageBox(NULL, TEXT("该PE没有导出表!"), TEXT("警告!"), MB_OK);
        return ;
    }
    //创建 ExportTableView窗口
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_ExportTableView), hwndDlgOfDataDirectoryView, ButtonExportTableViewProc);
}


/**
 * ExportTableView窗口的窗口回调函数（该窗口ID：IDD_DIALOG_ExportTableView）
 * @param  hwndDlgOfExportTableView ExportTableView窗口的句柄
 * @param  uMsg                     消息类型
 * @param  wParam                   WPARAM
 * @param  lParam                   LPARAM
 * @return                          
 */
BOOL CALLBACK ButtonExportTableViewProc(HWND hwndDlgOfExportTableView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //窗口初始化时执行的代码
        case WM_INITDIALOG:{
			//重新设置窗口标题，达到：“当前窗口标题 - 打开的文件路径” 的效果
			ResetWindowTitle(hwndDlgOfExportTableView, gSzFileNameOfPeView);
            //展示导出表的基本信息
            ExhibiteBasicInfo(hwndDlgOfExportTableView);
            //展示导出表中 - 导出函数地址表的信息
            ExhibiteAddressOfFunctions(hwndDlgOfExportTableView);
            //展示导出表中 - 导出序号表和导出名称表的信息
            ExhibiteOtherFunctions(hwndDlgOfExportTableView);
            return TRUE;
        }
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfExportTableView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //点击 “退出” 按钮后，退出窗口
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
 * 展示导出表中 - 导出序号表和导出名称表的信息
 * @param  hwndDlgOfExportTableView ExportTableView窗口的句柄（该窗口ID：IDD_DIALOG_ExportTableView）
 */
VOID ExhibiteOtherFunctions(HWND hwndDlgOfExportTableView){
    //获取ExportTableView窗口中 IDC_LIST_ExportOtherTwoTables 列表控件的ID
    HWND hListAddressOfFunctions = GetDlgItem(hwndDlgOfExportTableView, IDC_LIST_ExportOtherTwoTables);
    //设置整行选中
    SendMessage(hListAddressOfFunctions, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //初始化列表表头信息
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("索引 [十进制]");
    lv.cx = 110;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListAddressOfFunctions, 0, &lv);

    lv.pszText = TEXT("导出序号");
    lv.cx = 110;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListAddressOfFunctions, 1, &lv);
    
    lv.pszText = TEXT("函数名");
    lv.cx = 200;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListAddressOfFunctions, 2, &lv);
    
    //读取PE文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中导出表结构的位置
    PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
    GetExportTable(file.p, &pExportTable);

    //获取导出函数名称表的RVA，并将其转换为FOA
    DWORD exportTableAddressOfNamesFoa = RvaToFileOffest(file.p, pExportTable->AddressOfNames);
    //获取导出函数序号表的RVA，并将其转换为FOA
    DWORD exportTableAddressOfNameOrdinalsFoa = RvaToFileOffest(file.p, pExportTable->AddressOfNameOrdinals);

    //令addressOfNames指针指向导出函数名称表的首地址
    int* addressOfNames = (int*)((DWORD)file.p + exportTableAddressOfNamesFoa);
    //令addressOfNameOrdinals指针指向导出函数序号表的首地址
    short* addressOfNameOrdinals = (short*)((DWORD)file.p + exportTableAddressOfNameOrdinalsFoa);
    
    //声明一个缓冲区，用于之后的将数字转换为字符串的功能
    char buffer[21];
    //声明并初始化vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;

    //向列表控件中添加数据
    for(DWORD i=0; i<pExportTable->NumberOfNames; i++){
        //获取函数名称的首地址
        char* name = (char*)((DWORD)file.p + RvaToFileOffest(file.p, addressOfNames[i]));

        //添加索引值
        memset(buffer, 0, 21);
        //将数字转换为数字字符串
        ultoa(i, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListAddressOfFunctions, &vItem);

        //添加导出函数序号
        memset(buffer, 0, 21);
        ultoa(addressOfNameOrdinals[i], buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 1;
        ListView_SetItem(hListAddressOfFunctions, &vItem);

        //添加导出函数名称
        vItem.pszText = name;
        vItem.iItem = i;
        vItem.iSubItem = 2;
        ListView_SetItem(hListAddressOfFunctions, &vItem);
    }
    
}


/**
 * 展示导出表中 - 导出函数地址表的信息
 * @param  hwndDlgOfExportTableView ExportTableView窗口的句柄（该窗口ID：IDD_DIALOG_ExportTableView）
 */
VOID ExhibiteAddressOfFunctions(HWND hwndDlgOfExportTableView){
    //获取ExportTableView窗口中 IDC_LIST_ExportOtherTwoTables 列表控件的ID
    HWND hListAddressOfFunctions = GetDlgItem(hwndDlgOfExportTableView, IDC_LIST_ExportAddressOfFunctionsTable);
    //设置整行选中
    SendMessage(hListAddressOfFunctions, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //初始化列表控件的表头信息
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("索引 [十进制]");
    lv.cx = 110;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListAddressOfFunctions, 0, &lv);

    lv.pszText = TEXT("导出函数RVA");
    lv.cx = 130;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListAddressOfFunctions, 1, &lv);
    
    //读取P文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中导出表结构的位置
    PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
    GetExportTable(file.p, &pExportTable);
    
    //获取导出函数地址表的RVA
    DWORD exportTableAddressOfFunctionsRva = pExportTable->AddressOfFunctions;  
    //将导出函数地址表的RVA转换为FOA
    DWORD exportTableAddressOfFunctionsFoa = RvaToFileOffest(file.p, exportTableAddressOfFunctionsRva);
    //令addressOfFunctions指针指向导出函数地址表的首地址
    int* addressOfFunctions = (int*)((DWORD)file.p + exportTableAddressOfFunctionsFoa);
    
    //声明一个缓冲区，用于之后的将数字转换为字符串的功能
    char buffer[21];
    //声明并初始化vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //向列表控件中添加数据
    for(DWORD i=0; i<pExportTable->NumberOfFunctions; i++){
        //添加索引值
        memset(buffer, 0, 21);
        //将数字转换为数字字符串
        ultoa(i, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListAddressOfFunctions, &vItem);

        //添加导出函数地址值
        memset(buffer, 0, 21);
        ultoa(addressOfFunctions[i], buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = i;
        vItem.iSubItem = 1;
        ListView_SetItem(hListAddressOfFunctions, &vItem);
    }

}


/**
 * 展示导出表的基本信息
 * @param  hwndDlgOfExportTableView ExportTableView窗口的句柄（该窗口ID：IDD_DIALOG_ExportTableView）
 */
VOID ExhibiteBasicInfo(HWND hwndDlgOfExportTableView){
    //读取PE文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中导出表结构的位置
    PIMAGE_EXPORT_DIRECTORY pExportTable = NULL;
    GetExportTable(file.p, &pExportTable);
    
    //获取导出表的基本信息
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
    
    //获取 ExportTableView窗口中所有编辑框的句柄
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
    
    //向编辑框中添加shuju
    //添加Characteristics值
    char buffer[21];
    memset(buffer, 0, 21);
    //将数字转化为数字字符串
    ultoa(Export_Characteristics, buffer, 16);
    SetWindowText(hEdit_Characteristics, buffer);
    
    //添加TimeDateStamp值
    memset(buffer, 0, 21);
    ultoa(Export_TimeDateStamp, buffer, 16);
    SetWindowText(hEdit_TimeDateStamp, buffer);

    //添加MajorVersion值
    memset(buffer, 0, 21);
    ultoa(Export_MajorVersion, buffer, 16);
    SetWindowText(hEdit_MajorVersion, buffer);

    //添加MinorVersion值
    memset(buffer, 0, 21);
    ultoa(Export_MinorVersion, buffer, 16);
    SetWindowText(hEdit_MinorVersion, buffer);

    //添加Name值
    memset(buffer, 0, 21);
    ultoa(Export_Name, buffer, 16);
    SetWindowText(hEdit_Name, buffer);

    //添加该DLL名称
    SetWindowText(hEdit_DllName, (char*)((DWORD)file.p + RvaToFileOffest(file.p, Export_Name)));

    //添加Base值
    memset(buffer, 0, 21);
    ultoa(Export_Base, buffer, 16);
    SetWindowText(hEdit_Base, buffer);

    //添加NumberOfFunctions值
    memset(buffer, 0, 21);
    ultoa(Export_NumberOfFunctions, buffer, 16);
    SetWindowText(hEdit_NumberOfFunctions, buffer);

    //添加NumberOfNames值
    memset(buffer, 0, 21);
    ultoa(Export_NumberOfNames, buffer, 16);
    SetWindowText(hEdit_NumberOfNames, buffer);

    //添加AddressOfFunctions值
    memset(buffer, 0, 21);
    ultoa(Export_AddressOfFunctions, buffer, 16);
    SetWindowText(hEdit_AddressOfFunctions, buffer);

    //添加AddressOfNames值
    memset(buffer, 0, 21);
    ultoa(Export_AddressOfNames, buffer, 16);
    SetWindowText(hEdit_AddressOfNames, buffer);

    //添加AddressOfNameOrdinals值
    memset(buffer, 0, 21);
    ultoa(Export_AddressOfNameOrdinals, buffer, 16);
    SetWindowText(hEdit_AddressOfNameOrdinals, buffer);
}


/**
 * 定位pFileBuffer中的 导出表结构IMAGE_EXPORT_DIRECTORY 的首地址
 * @param pFileBuffer  FileBuffer首地址
 * @param pExportTable 导出表首地址（若程序无导出表，则为null）
 */
void GetExportTable(IN LPVOID pFileBuffer, OUT PIMAGE_EXPORT_DIRECTORY* pExportTable){
    PIMAGE_DOS_HEADER pFileDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    PIMAGE_NT_HEADERS pFileNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pFileDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pFilePeHeader = (PIMAGE_FILE_HEADER)((DWORD)pFileNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pFileOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pFilePeHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //获取导出表结构的RVA
    DWORD exportTableRva = pFileOptionHeader->DataDirectory[0].VirtualAddress;
    //若exportTableRva为0，说明该程序无导出表，则“返回”null
    if (exportTableRva == 0){
        *pExportTable = NULL;
        return ;
    }
    //将导出表结构RVA转化为FOA
    DWORD exportTableFoa = RvaToFileOffest(pFileBuffer, exportTableRva);
    //令exportTable指针指向导出表的首地址
    PIMAGE_EXPORT_DIRECTORY exportTable = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pFileBuffer + exportTableFoa);
    //返回导出表的首地址
    *pExportTable = exportTable;
}








