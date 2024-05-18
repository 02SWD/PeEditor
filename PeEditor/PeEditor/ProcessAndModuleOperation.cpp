// ProcessAndModuleOperation.cpp: implementation of the ProcessAndModuleOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ProcessAndModuleOperation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * 枚举当前操作系统中所有的进程
 * @param  hwndDlg 主对话框的句柄
 * @return         枚举成功返回1，CreateToolhelp32Snapshot调用失败返回-1
 */
int EnumAllProcess(HWND hwndDlg){
    //根据 主对话框的句柄 和 进程列表控件ID 获得 进程列表控件 的句柄
    HWND hListProcess = GetDlgItem(hwndDlg, IDC_LIST_PROCESSLIST);
	//删除模块列表控件中的所有数据
    SendMessage(hListProcess, LVM_DELETEALLITEMS, 0, 0);

    //存放快照进程信息的一个结构体
    PROCESSENTRY32 currentProcess;
    //在使用这个结构之前，先设置它的大小
    currentProcess.dwSize = sizeof(currentProcess);
    //给系统内的所有进程拍一个快照
    HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if (hProcess == INVALID_HANDLE_VALUE){
        CloseHandle(hProcess);
        return -1;
    }
    
    //向列表中新增数据
    LV_ITEM vItem;
    //初始化
    memset(&vItem, 0, sizeof(LV_ITEM));
    //设置属性
    vItem.mask = LVIF_TEXT;
    //获取第一个进程信息
    int bMore=Process32First(hProcess,&currentProcess);
    //用于记录将要向第几行添加数据
    int line = 0;
    while(bMore){
        //用于保存进程基址
        PVOID pProcessImageBase = NULL;
        //用于保存进程大小
        DWORD processImageSize = 0;
        //用于保存模块信息
        MODULEENTRY32 medule32 = {0};
        medule32.dwSize = sizeof(MODULEENTRY32);
        //根据pid为该进程拍摄一个快照
        HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, currentProcess.th32ProcessID);
        //若 hModuleSnap == INVALID_HANDLE_VALUE，说明快照拍摄失败
        if(INVALID_HANDLE_VALUE == hModuleSnap){
            CloseHandle(hModuleSnap);
        }else{
            //否则，获取该进程的第一个模块的信息
            BOOL bRet = Module32First(hModuleSnap, &medule32);
            if (bRet){
                //获取成功，则记录下该模块的基址
                pProcessImageBase = (PVOID)medule32.modBaseAddr;
                //循环遍历该进程的所有模块，累加其大小，从而得到该进程的大小（即：所有模块的总大小）
                while (Module32Next(hModuleSnap, &medule32)){
                    //累加
                    processImageSize = processImageSize + medule32.modBaseSize;
                }
                CloseHandle(hModuleSnap);
            }else{
                CloseHandle(hModuleSnap);
            }           
        }
        
        //在process列表控件中设置 进程名称
        vItem.pszText = TEXT(currentProcess.szExeFile);
        vItem.iItem = line;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListProcess, &vItem);
        
        //在process列表控件中设置 PID
        //在这里由于currentProcess.th32ProcessID为DWORD类型，需要转换为数字字符串
        char buff[21] = {0};
        ultoa(currentProcess.th32ProcessID, buff, 10);
        vItem.pszText = TEXT(buff);
        vItem.iItem = line;
        vItem.iSubItem = 1;
        ListView_SetItem(hListProcess, &vItem);
        
        //在process列表控件中设置 镜像基址
        //在这里由于pProcessImageBase为DWORD类型，需要转换为数字字符串
        memset(buff, 0, sizeof(21));
        ultoa((DWORD)pProcessImageBase, buff, 16);
        vItem.pszText = TEXT(buff);
        vItem.iItem = line;
        vItem.iSubItem = 2;
        ListView_SetItem(hListProcess, &vItem);

        //在process列表控件中设置 镜像大小
        //在这里由于processImageSize为DWORD类型，需要转换为数字字符串
        memset(buff, 0, sizeof(21));
        ultoa(processImageSize, buff, 16);
        vItem.pszText = TEXT(buff);
        vItem.iItem = line;
        vItem.iSubItem = 3;
        ListView_SetItem(hListProcess, &vItem);
        
        //遍历下一个进程
        bMore=Process32Next(hProcess,&currentProcess);
        //列表控件行索引加1
        line++;
    }
    //清除hProcess句柄
    CloseHandle(hProcess);
    return 1;
}

/**
 * 根据pid枚举其进程中所有的模块
 * @param  hwndDlg 主对话框句柄
 * @param  wParam  WPARAM
 * @param  lParam  LPARAM
 * @return         枚举成功则返回1，没有获取到行则返回-1，第一个模块信息获取失败则返回-2
 */
int EnumModuleListByPid(HWND hwndDlg, WPARAM wParam, LPARAM lParam) {
    //1. 先得到用户输入的PID
    //根据 主对话框句柄 和 进程列表控件ID 得到 列表控件 的句柄
    HWND hListProcess = GetDlgItem(hwndDlg, IDC_LIST_PROCESSLIST);
    //用于记录用户点击了第几行
    DWORD dwRowId;
    //开辟一个缓冲区，用于存储内容
    TCHAR sizePid[0x20];
    //用于存储从列表空间中得到的PID
    LV_ITEM vItemPid;
    //初始化
    memset(&vItemPid, 0, sizeof(LV_ITEM));
    memset(sizePid, 0, 0x20);
    
    //得到用户点击的行数
    dwRowId = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    //如果没有获取到“行”，则不作为
    if (dwRowId == -1){
        return -1;
    }

    //要获取的列
    vItemPid.iSubItem = 1;
    //指定存储查询结果的缓冲区
    vItemPid.pszText = sizePid;
    //指定缓冲区的大小
    vItemPid.cchTextMax = 0x20;
    //获取指定行和列对应元素的内容
    SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowId, (DWORD)&vItemPid);

    //用于存储数字类型的pid
    DWORD dwPid;
    char* leftOver = NULL;
    //将字符串类型的pid转换为数字类型
    dwPid = strtoul(sizePid,&leftOver,10);

    //============ 以上程序的功能主要是获取用户 “输入” 的pid =============
    //根据 主对话框句柄 和 模块列表控件ID 得到 模块列表控件 的句柄
    HWND hListModule = GetDlgItem(hwndDlg, IDC_LIST_MODULELIST);
    //删除模块列表控件中的所有数据
    SendMessage(hListModule, LVM_DELETEALLITEMS, 0, 0);
    //根据pid为进程拍摄一个快照
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 medule32 = { sizeof(MODULEENTRY32) };
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwPid);
    //获取该进程的第一个模块信息，若获取失败则关闭句柄并返回
    if (!Module32First(hModuleSnap, &medule32)) {
        CloseHandle(hModuleSnap);
		return -1;
    }
    //用于记录该向第几行添加数据了
    int line = 0;
    //向列表中添加数据
    LV_ITEM vitem;
    //初始化
    memset(&vitem, 0 ,sizeof(LV_ITEM));
    //设置属性
    vitem.mask = LVIF_TEXT;
    do {
        //添加模块名称信息
        vitem.pszText = medule32.szModule;
        vitem.iItem = line;
        vitem.iSubItem = 0;
        ListView_InsertItem(hListModule, &vitem);
        
        //添加模块基址信息
        LPVOID moduleBase = (LPVOID)medule32.modBaseAddr;
        char buffer[21];
        memset(buffer, 0, sizeof(21));
        ultoa((DWORD)moduleBase, buffer, 16);
        vitem.pszText = buffer;
        vitem.iItem = line;
        vitem.iSubItem = 1;
        ListView_SetItem(hListModule, &vitem);

        //添加模块大小信息
        memset(buffer, 0, sizeof(21));
        ultoa(medule32.modBaseSize, buffer, 16);
        vitem.pszText = buffer;
        vitem.iItem = line;
        vitem.iSubItem = 2;
        ListView_SetItem(hListModule, &vitem);
        
        //添加模块路径信息
        vitem.pszText = (LPSTR)medule32.szExePath;
        vitem.iItem = line;
        vitem.iSubItem = 3;
        ListView_SetItem(hListModule, &vitem);
        //令line指向下一行
        line++;
        //获取下一个模块的信息
    } while (Module32Next(hModuleSnap, &medule32));
    //关闭模块句柄
    CloseHandle(hModuleSnap);
    return 1;
}

/**
 * 初始化 进程列表控件（为进程列表控件添加表头）
 * @param  hwndDlg 主对话框句柄
 */
VOID InitProcessList(HWND hwndDlg){
    //根据 主对话框句柄 和 列表控件ID 获取列表控件的句柄
	HWND hListProcess = GetDlgItem(hwndDlg, IDC_LIST_PROCESSLIST);
    //设置整行选中
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	
    //设置表头
	LV_COLUMN lv;
	memset(&lv, 0, sizeof(LV_COLUMN));
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	lv.pszText = TEXT("进程名称");
	lv.cx = 200;
	lv.iSubItem = 0;
	ListView_InsertColumn(hListProcess, 0, &lv);

	lv.pszText = TEXT("PID");
	lv.cx = 100;
	lv.iSubItem = 1;
	ListView_InsertColumn(hListProcess, 1, &lv);

	lv.pszText = TEXT("镜像基址");
	lv.cx = 100;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);

	lv.pszText = TEXT("镜像大小");
	lv.cx = 100;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);

}

/**
 * 初始化 模块列表控件（为模块列表控件添加表头）
 * @param  hwndDlg 主对话框句柄
 */
VOID InitModuleList(HWND hwndDlg){
    //根据 主对话框句柄 和 模块列表控件ID 获取 模块列表控件句柄
	HWND hListModule = GetDlgItem(hwndDlg, IDC_LIST_MODULELIST);
    //设置整行选中
    SendMessage(hListModule, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	
    //设置表头
	LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("模块名称");
    lv.cx = 200;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListModule, 0, &lv);

    lv.pszText = TEXT("模块基址");
    lv.cx = 100;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListModule, 1, &lv);

    lv.pszText = TEXT("模块大小");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListModule, 2, &lv);

    lv.pszText = TEXT("模块位置");
    lv.cx = 300;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListModule, 3, &lv);
	
}


