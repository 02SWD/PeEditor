// ButtonIatTableView.cpp: implementation of the ButtonIatTableView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonIatTableView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
// IatTableView窗口的窗口回调函数
BOOL CALLBACK ButtonIatTableViewProc(HWND hwndDlgOfIatTableView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//定位FileBuffer中导入表结构的位置
VOID GetImportTable(IN LPVOID pFileBuffer, OUT PIMAGE_IMPORT_DESCRIPTOR* pImportTable);
//展示Iat表的基本信息（其实IAT表中并没有这些信息，这些信息是根据导入表来获得的）
VOID ExhibiteIatTableBaseInfo(HWND hwndDlgOfIatTableView);
//展示IAT表的详细信息（已绑定导入 / 未绑定导入 时，表项的内容）
VOID ExhibiteIatTableDetailed(HWND hwndDlgOfIatTableView);
//初始化 已绑定列表表头
VOID InitListIatTableYesBound(HWND hwndDlgOfIatTableView);
//初始化 未绑定列表表头
VOID InitListIatTableNoBound(HWND hwndDlgOfIatTableView);
//判断是否存在IAT表
int JudgeIatTable(LPVOID pFileBuffer);


/**
 * 点击 IDC_BUTTON_IATTable_Detailed 按钮后的处理代码（即：点击 “IAT表后的按钮” 之后，要执行的代码）
 * @param  hwndDlgOfDataDirectoryView 父窗口的句柄（这里是DataDirectoryView窗口句柄，该窗口ID： IDD_DIALOG_DataDirectoryView）
 */
VOID ButtonIatTableViewCode(HWND hwndDlgOfDataDirectoryView){
    //读取PE文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //判断是否存在IAT表
    int flag = JudgeIatTable(file.p);
    if (flag == -1){
        MessageBox(NULL, TEXT("该PE文件没有IAT表!"), TEXT("警告"), MB_OK);
        return ;
    }   
    //创建 IatTableView 窗口
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_IatTableView), hwndDlgOfDataDirectoryView, ButtonIatTableViewProc);
}

/**
 * IatTableView窗口的窗口回调函数（该窗口ID：IDD_DIALOG_IatTableView）
 * @param  hwndDlgOfImportTableView IatTableView窗口的句柄（该窗口ID：IDD_DIALOG_IatTableView）
 * @param  uMsg                     消息类型
 * @param  wParam                   WPARAM
 * @param  lParam                   LPARAM
 * @return                          
 */
BOOL CALLBACK ButtonIatTableViewProc(HWND hwndDlgOfIatTableView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //窗口初始化时要执行的代码
        case WM_INITDIALOG:{
			//重新设置窗口标题，达到：“当前窗口标题 - 打开的文件路径” 的效果
			ResetWindowTitle(hwndDlgOfIatTableView, gSzFileNameOfPeView);
            //展示Iat表的基本信息（其实IAT表中并没有这些信息，这些信息是根据导入表来获得的）
            ExhibiteIatTableBaseInfo(hwndDlgOfIatTableView);
            //初始化 已绑定列表表头
            InitListIatTableYesBound(hwndDlgOfIatTableView);
            //初始化 未绑定列表表头
            InitListIatTableNoBound(hwndDlgOfIatTableView);
            return TRUE;
        }
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfIatTableView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //点击 “退出” 按钮，退出对话框
                case IDC_BUTTON_IatTableViewExit:{
                    EndDialog(hwndDlgOfIatTableView, 0);
                    return TRUE;
                }
            }
        }
        case WM_NOTIFY:{
            NMHDR* pNmhdr = (NMHDR*)lParam;
            //若用户 “点击” 的是我们 “指定的列表控件” 中的行的话，则调用ExhibiteIatTableDetailed函数
            if (wParam == IDC_LIST_IatTableInfo && pNmhdr->code == NM_CLICK){
                ExhibiteIatTableDetailed(hwndDlgOfIatTableView);
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * 展示IAT表的详细信息（已绑定导入 / 未绑定导入 时，表项的内容）
 * 注意：
 *     为了使得IAT表的信息更具有可读性（这些函数地址都是属于哪些Dll的），所以我们这里是通过导入表来间接获取IAT表的内容的
 *     而不是直接通过数据目录项定位解析IAT表。
 * @param  hwndDlgOfIatTableView IatTableView窗口的句柄（该窗口ID：IDD_DIALOG_IatTableView）
 */
VOID ExhibiteIatTableDetailed(HWND hwndDlgOfIatTableView){
    //1. 先得到用户点击的行所对应的 Name 字段值
    //根据 IatTableView窗口句柄 和 列表控件ID 获取 展示IAT基本信息的列表控件的句柄
    HWND hListIatTableInfo = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_IatTableInfo);
    //用于记录用户点击了第几行
    DWORD dwRowId;
    //开辟一个缓冲区，用于存储内容
    TCHAR sizeNameRva[0x20];
    memset(sizeNameRva, 0, 0x20);
    //用于存储从列表控件中得到的Name字段值
    LV_ITEM vItemNameRva;
    memset(&vItemNameRva, 0, sizeof(LV_ITEM));

    //得到用户点击的行数
    dwRowId = SendMessage(hListIatTableInfo, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    //如果没有获取到 “行”，则不作为
    if (dwRowId == -1){
        return ;
    }

    //想要获取的列
    vItemNameRva.iSubItem = 2;
    //指定存储查询结果的缓冲区
    vItemNameRva.pszText = sizeNameRva;
    //指定缓冲区的大小
    vItemNameRva.cchTextMax = 0x20;
    //获取指定行和列对应元素的内容
    SendMessage(hListIatTableInfo, LVM_GETITEMTEXT, dwRowId, (DWORD)&vItemNameRva);
    
    //获取指定行和列对应元素的内容
    DWORD dwNameRva;
    char* leftOver = NULL;
    //将字符串类型的Name字段值转换为数字类型（strtoul函数的最后一个参数，指定以几进制进行转化）
    dwNameRva = strtoul(sizeNameRva, &leftOver, 16);
    //=====================以上程序的功能主要是：获取用户点击的行所对应Name字段值========================

    //根据 IatTableView窗口句柄 和 已绑定列表控件 得到 已绑定列表控件的句柄
    HWND hListYesBound = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_YesBound);
    //删除 已绑定列表控件 中所有的数据
    SendMessage(hListYesBound, LVM_DELETEALLITEMS, 0 , 0); 
    //根据 IatTableView窗口句柄 和 未绑定列表控件 得到 未绑定列表控件的句柄
    HWND hListNoBound = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_NoBound);
    //删除 未绑定列表控件 中所有的数据
    SendMessage(hListNoBound, LVM_DELETEALLITEMS, 0 , 0); 

    //读取PE文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中导入表结构的位置
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    GetImportTable(file.p, &pImportTable);

    //声明一个缓冲区，用于之后的将数字转换为字符串的功能
    char buffer[21];
    //声明并初始化vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;

    //列表控件的行索引
    int index = 0;
    //由于导入表结构不止一个，所以这里使用while循环，而其结束的标志为sizeof(IMAGE_IMPORT_DESCRIPTOR)个0。为了方便，在这里我直接通过其中一部分是否为0，来判断是否结束
    while(pImportTable->FirstThunk != 0){
        //这里我们只需要遍历用户指定的导入模块中的信息即可，其他导入模块的信息均跳过
        if (pImportTable->Name != dwNameRva){
            //令pImportTable指向下一个导入表结构（导入模块）
            pImportTable++;
            continue;
        }
        //得到IAT表的RVA
        DWORD iatRva = pImportTable->FirstThunk;
        if (iatRva != 0){
            //由于我们是直接在FileBuffer中操作的，所以需要将IAT表的RVA转换为FOA
            DWORD iatFoa = RvaToFileOffest(file.p, iatRva);
            //将iatTable指针指向IAT表的首地址，即IAT表的第一个表项（即通过pFileBuffer指针与IAT表的FOA，计算出IAT表的首地址，并用PDWORD类型指针指向它）
            PDWORD iatTable = (PDWORD)((DWORD)file.p + iatFoa);
            //判断该导入表的TimeDataStamp属性值是否为0
            if(pImportTable->TimeDateStamp == 0){
                //若TimeDataStamp为0，则说明未绑定"绑定导入表"
                //循环遍历IAT表，当表项值为0时表示该表结束
                while(*iatTable != 0){
                    //向 未绑定列表控件 中，添加行索引值
                    memset(buffer, 0, 21);
                    //将 数字 转换为 数字字符串
                    ultoa(index, buffer, 10);
                    vItem.pszText = buffer;
                    vItem.iItem = index;
                    vItem.iSubItem = 0;
                    ListView_InsertItem(hListNoBound, &vItem);
                    //判断表项值的最高位是否为1
                    if((*iatTable & 0x80000000) == 0x80000000){
                        //若最高位为1，那么去除最高位的表项值，就是导出序号，并将导出序号存入exportOrdinal变量中
                        DWORD exportOrdinal = *iatTable & 0x7fffffff;
                        //向 未绑定列表控件 中，添加导出序号值
                        memset(buffer, 0, 21);
                        ultoa(exportOrdinal, buffer, 16);
                        vItem.pszText = buffer;
                        vItem.iItem = index;
                        vItem.iSubItem = 1;
                        ListView_SetItem(hListNoBound, &vItem);
                    }else{
                        //得到PIMAGE_IMPORT_BY_NAME的RVA（若最高位不为1，则该表项值便是PIMAGE_IMPORT_BY_NAME的RVA）
                        DWORD imageImportByNameRva = *iatTable;
                        //由于我们是直接在FileBuffer中操作的，所以需要将PIMAGE_IMPORT_BY_NAME的RVA转换为FOA
                        DWORD imageImportByNameFoa = RvaToFileOffest(file.p, imageImportByNameRva);
                        //将funcName指针指向PIMAGE_IMPORT_BY_NAME的首地址
                        PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)file.p + imageImportByNameFoa);
                        
                        //向 未绑定列表控件 中，添加Hint值
                        memset(buffer, 0, 21);
                        ultoa(funcName->Hint, buffer, 16);
                        vItem.pszText = buffer;
                        vItem.iItem = index;
                        vItem.iSubItem = 2;
                        ListView_SetItem(hListNoBound, &vItem);

                        //向 未绑定列表控件 中，添加模块名（DLL名称）
                        vItem.pszText = (char*)(&(funcName->Name));
                        vItem.iItem = index;
                        vItem.iSubItem = 3;
                        ListView_SetItem(hListNoBound, &vItem);
                    }
                    //将iatTable指针指向IAT表的下一个表项
                    iatTable++;
                    //列表控件的行索引加1
                    index++;
                }
            }else{
                //若TimeDataStamp为-1，则说明已绑定"绑定导入表"
                while(*iatTable != 0){
                    //向 已绑定列表控件 中，添加行索引值
                    memset(buffer, 0, 21);
                    ultoa(index, buffer, 10);
                    vItem.pszText = buffer;
                    vItem.iItem = index;
                    vItem.iSubItem = 0;
                    ListView_InsertItem(hListYesBound, &vItem);
                    
                    //向 已绑定列表控件 中，添加函数的真实地址
                    memset(buffer, 0, 21);
                    ultoa(*iatTable, buffer, 16);
                    vItem.pszText = buffer;
                    vItem.iItem = index;
                    vItem.iSubItem = 1;
                    ListView_SetItem(hListYesBound, &vItem);
                    
                    //将iatTable指针指向IAT表的下一个表项
                    iatTable++;
                    //行索引加1
                    index++;
                }
            }
        }else{
            MessageBox(NULL, TEXT("该PE文件无IAT表!"), TEXT("警告"), MB_OK);
        }
        break;
    }
}

/**
 * 初始化 未绑定列表表头
 * @param  hwndDlgOfIatTableView IatTableView窗口的句柄（该窗口ID：IDD_DIALOG_IatTableView）
 */
VOID InitListIatTableNoBound(HWND hwndDlgOfIatTableView){
    //根据 IatTableView窗口句柄 和 未绑定列表控件 得到 未绑定列表控件的句柄
    HWND hListNoBound = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_NoBound);
    //设置整行选中
    SendMessage(hListNoBound, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    
    //初始化列表控件的表头
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("索引 [十进制]");
    lv.cx = 130;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListNoBound, 0, &lv);

    lv.pszText = TEXT("导出序号");
    lv.cx = 130;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListNoBound, 1, &lv);

    lv.pszText = TEXT("Hint");
    lv.cx = 85;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListNoBound, 2, &lv);

    lv.pszText = TEXT("函数名");
    lv.cx = 140;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListNoBound, 3, &lv);
}


/**
 * 初始化 已绑定列表表头
 * @param  hwndDlgOfIatTableView IatTableView窗口的句柄（该窗口ID：IDD_DIALOG_IatTableView）
 */
VOID InitListIatTableYesBound(HWND hwndDlgOfIatTableView){
    //根据 IatTableView窗口句柄 和 已绑定列表控件 得到 已绑定列表控件的句柄
    HWND hListYesBound = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_YesBound);
    //设置整行选中
    SendMessage(hListYesBound, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    
    //初始化列表控件的表头
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("索引 [十进制]");
    lv.cx = 110;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListYesBound, 0, &lv);

    lv.pszText = TEXT("函数真实地址（IAT表项内容）");
    lv.cx = 200;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListYesBound, 1, &lv);

}

/**
 * 展示Iat表的基本信息（其实IAT表中并没有这些信息，这些信息是根据导入表来获得的）
 * @param  hwndDlgOfIatTableView IatTableView窗口的句柄（该窗口ID：IDD_DIALOG_IatTableView）
 */
VOID ExhibiteIatTableBaseInfo(HWND hwndDlgOfIatTableView){
    //根据 IatTableView窗口句柄 和 列表控件ID 获取 展示IAT基本信息的列表控件的句柄
    HWND hListIatTableInfo = GetDlgItem(hwndDlgOfIatTableView, IDC_LIST_IatTableInfo);
    //设置整行选中
    SendMessage(hListIatTableInfo, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    //初始化列表控件的表头
    LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("索引 [十进制]");
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
    
    //读取PE文件
    FILEINFOR file;
    file = readFile(gSzFileNameOfPeView);
    //定位FileBuffer中导入表结构的位置
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = NULL;
    GetImportTable(file.p, &pImportTable);

    //声明一个缓冲区，用于之后的将数字转换为字符串的功能
    char buffer[21];
    //声明并初始化vItem
    LV_ITEM vItem;
    memset(&vItem, 0, sizeof(LV_ITEM));
    vItem.mask = LVIF_TEXT;
    
    //列表控件的行索引
    DWORD index = 0;
    while(pImportTable->FirstThunk != 0){
        //得到导入表的name属性值(该值为字符串的RVA，而字符串就是该导入表所对应的dll名字)
        DWORD nameRva = pImportTable->Name;
        //将字符串RVA转换为FOA
        DWORD nameFoa = RvaToFileOffest(file.p, nameRva);
        //令name指针指向字符串的首地址（通过pFileBuffer指针与该字符串的FOA，计算出首地址，并用char*类型指针指向它）
        char* name = (char*)((DWORD)file.p + nameFoa);

        //向列表控件中，添加行索引值
        memset(buffer, 0, 21);
        ultoa(index, buffer, 10);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListIatTableInfo, &vItem);

        //向列表控件中，添加DLL名称（导入模块名称）
        vItem.pszText = name;
        vItem.iItem = index;
        vItem.iSubItem = 1;
        ListView_SetItem(hListIatTableInfo, &vItem);

        //向列表控件中，添加 pImportTable->Name 值
        memset(buffer, 0, 21);
        ultoa(nameRva, buffer, 16);
        vItem.pszText = buffer;
        vItem.iItem = index;
        vItem.iSubItem = 2;
        ListView_SetItem(hListIatTableInfo, &vItem);
        //令importTable指针指向下一个导入表结构
        pImportTable++;
        //令行索引加1
        index++;
    }
}

/**
 * 判断该PE中是否具有IAT表
 * @param  pFileBuffer FileBuffer指针
 * @return             存在IAT则返回1，否则返回-1
 */
int JudgeIatTable(IN LPVOID pFileBuffer){
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    
    //通过数据目录找到"导入表"的RVA
    DWORD iatTableRva = pOptionHeader->DataDirectory[12].VirtualAddress;
    //若不存在IAR，则返回-1
    if (iatTableRva == 0){
        return -1;
    }
    return 1;
}

