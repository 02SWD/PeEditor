// ButtonDataDirectoryView.cpp: implementation of the ButtonDataDirectoryView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonDataDirectoryView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
//DataDirectoryView对话框的窗口回调函数
BOOL CALLBACK ButtonDataDirectoryViewProc(HWND hwndDlgOfDataDirectoryView, UINT uMsg, WPARAM wParam, LPARAM lParam);
//获取第一个数据目录项的地址
VOID GetFirstDataCatalogEntry(IN LPVOID pFileBuffer, OUT PIMAGE_DATA_DIRECTORY* pDataDirectory);
//展示数据目录项的信息
VOID ExhibitionDataDirectory(HWND hwndDlgOfDataDirectoryView, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * 点击 IDC_BUTTON_DirectoryView  按钮后的处理代码（即：点击 “数据目录项” 按钮之后，要执行的代码）
 * @param  hwndDlgOfPeView 父窗口句柄 (在这里是PeView窗口的句柄，该窗口的ID：IDD_DIALOG_PeViewMainDlg)
 */
VOID ButtonDataDirectoryViewCode(HWND hwndDlgOfPeView){
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_DataDirectoryView), hwndDlgOfPeView, ButtonDataDirectoryViewProc);
}

/**
 * DataDirectoryView对话框的窗口回调函数（该窗口ID：IDD_DIALOG_DataDirectoryView）
 * @param  hwndDlgOfDataDirectoryView DataDirectoryView窗口的句柄
 * @param  uMsg                       消息类型
 * @param  wParam                     WPARAM
 * @param  lParam                     WPARAM
 * @return                            
 */
BOOL CALLBACK ButtonDataDirectoryViewProc(HWND hwndDlgOfDataDirectoryView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        //窗口初始化时实行的代码
        case WM_INITDIALOG:{
			//重新设置窗口标题，达到：“当前窗口标题 - 打开的文件路径” 的效果
			ResetWindowTitle(hwndDlgOfDataDirectoryView, gSzFileNameOfPeView);
            //展示 数据目录项 的信息
            ExhibitionDataDirectory(hwndDlgOfDataDirectoryView, uMsg, wParam, lParam);
            return TRUE;
        }
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfDataDirectoryView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //点击 “退出” 按钮后，退出该窗口
                case IDC_BUTTON_DataDirectoryViewExit:{
                    EndDialog(hwndDlgOfDataDirectoryView, 0);
                    return TRUE;
                }
				//点击 “导出表后面的按钮” 后，将会调用 ButtonExportTableViewCode 函数，详细的展示导出表信息
				case IDC_BUTTON_ExportTable_Detailed:{
					ButtonExportTableViewCode(hwndDlgOfDataDirectoryView);
					return TRUE;
				}
				//点击 “导入表后面的按钮” 后，将会调用 ButtonImportTableViewCode 函数，详细的展示导入表信息
				case IDC_BUTTON_ImportTable_Detailed:{
					ButtonImportTableViewCode(hwndDlgOfDataDirectoryView);
					return TRUE;
				}
				//点击 “重定位表后面的按钮” 后，将会调用 ButtonBaseRelocViewCode 函数，详细的展示重定位表信息
				case IDC_BUTTON_BaseRelocTable_Detailed:{
					ButtonBaseRelocViewCode(hwndDlgOfDataDirectoryView);
					return TRUE;
				}
				case IDC_BUTTON_IATTable_Detailed:{
					ButtonIatTableViewCode(hwndDlgOfDataDirectoryView);
					return TRUE;
				}
            }
        }
    }
    return FALSE;
}

/**
 * 展示 数据目录项 的信息
 * @param  hwndDlgOfDataDirectoryView DataDirectoryView窗口的句柄（该窗口ID：IDD_DIALOG_DataDirectoryView）
 * @param  uMsg                       消息类型
 * @param  wParam                     WPARAM
 * @param  lParam                     LPARAM
 */
VOID ExhibitionDataDirectory(HWND hwndDlgOfDataDirectoryView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //封装有pFileBuffer指针和FileBuffer大小的变量
    FILEINFOR file;
    //读取PE文件
    file = readFile(gSzFileNameOfPeView);
    //指向数据目录项的指针
    PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
    PIMAGE_DATA_DIRECTORY pDataDirectoryBak = NULL;
    //定位 第一个数据目录项 的位置
    GetFirstDataCatalogEntry(file.p, &pDataDirectory);

    //获取DataDirectoryView对话框中所有编辑框的句柄
    HWND hEdit_ExportTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ExportTable_RVA);
    HWND hEdit_ExportTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ExportTable_SIZE);
    
    HWND hEdit_ImportTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ImportTable_RVA);
    HWND hEdit_ImportTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ImportTable_SIZE);
    
    HWND hEdit_ResourceTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ResourceTable_RVA);
    HWND hEdit_ResourceTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ResourceTable_SIZE);

    HWND hEdit_ExceptionTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ExceptionTable_RVA);
    HWND hEdit_ExceptionTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ExceptionTable_SIZE);

    HWND hEdit_SecuryTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_SecurityTable_RVA);
    HWND hEdit_SecuryTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_SecurityTable_SIZE);

    HWND hEdit_BaseRelocTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_BaseRelocTable_RVA);
    HWND hEdit_BaseRelocTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_BaseRelocTable_SIZE);

    HWND hEdit_DebugTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_DebugTable_RVA);
    HWND hEdit_DebugTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_DebugTable_SIZE);

    HWND hEdit_CopyRightTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_CopyRightTable_RVA);
    HWND hEdit_CopyRightTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_CopyRightTable_SIZE);
    
    HWND hEdit_GlobalPtrTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_GlobalPtrTable_RVA);
    HWND hEdit_GlobalPtrTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_GlobalPtrTable_SIZE);

    HWND hEdit_TlsTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_TLSTable_RVA);
    HWND hEdit_TlsTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_TLSTable_SIZE);

    HWND hEdit_ImportConfigTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_LoadConfigTable_RVA);
    HWND hEdit_ImportConfigTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_LoadConfigTable_SIZE);

    HWND hEdit_BoundImportTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_BoundImportTable_RVA);
    HWND hEdit_BoundImportTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_BoundImportTable_SIZE);
    
    HWND hEdit_IatTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_IATTable_RVA);
    HWND hEdit_IatTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_IATTable_SIZE);

    HWND hEdit_DelayImportTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_DelayImportTable_RVA);
    HWND hEdit_DelayImportTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_DelayImportTable_SIZE);

    HWND hEdit_ComTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ComTable_RVA);
    HWND hEdit_ComTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_ComTable_SIZE);

    HWND hEdit_RetainTableRva = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_RetainTable_RVA);
    HWND hEdit_RetainTableSize = GetDlgItem(hwndDlgOfDataDirectoryView, IDC_EDIT_RetainTable_SIZE);

    //为每一个编辑框设置值
    char buffer[21] = {0};
    //令pDataDirectoryBak指针指向第1个数据目录项
    pDataDirectoryBak = pDataDirectory + 0;
    //将数字转换为数字字符串
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    //设置编辑框的值
    SetWindowText(hEdit_ExportTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ExportTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 1;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ImportTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ImportTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 2;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ResourceTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ResourceTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 3;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ExceptionTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ExceptionTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 4;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_SecuryTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_SecuryTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 5;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_BaseRelocTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_BaseRelocTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 6;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_DebugTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_DebugTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 7;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_CopyRightTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_CopyRightTableSize, buffer);
    
    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 8;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_GlobalPtrTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_GlobalPtrTableSize, buffer);
    
    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 9;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_TlsTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_TlsTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 10;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ImportConfigTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ImportConfigTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 11;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_BoundImportTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_BoundImportTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 12;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_IatTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_IatTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 13;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_DelayImportTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_DelayImportTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 14;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_ComTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_ComTableSize, buffer);

    memset(buffer, 0, 21);
    pDataDirectoryBak = pDataDirectory + 15;
    ultoa(pDataDirectoryBak->VirtualAddress, buffer, 16);
    SetWindowText(hEdit_RetainTableRva, buffer);
    ultoa(pDataDirectoryBak->Size, buffer, 16);
    SetWindowText(hEdit_RetainTableSize, buffer);
    
}


/**
 * 定位pFileBuffer中 第一个数据目录项 的首地址
 * @param  pFileBuffer    FileBuffer首地址
 * @param  pDataDirectory 第一个数据目录项的首地址
 */
VOID GetFirstDataCatalogEntry(IN LPVOID pFileBuffer, OUT PIMAGE_DATA_DIRECTORY* pDataDirectory){
    PIMAGE_DOS_HEADER pDosHeaderBak = NULL;
    PIMAGE_NT_HEADERS pNTHeaderBak = NULL;
    PIMAGE_FILE_HEADER pPEHeaderBak = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeaderBak = NULL;
    PIMAGE_SECTION_HEADER pFirstSectionHeaderBak = NULL;
    PIMAGE_DATA_DIRECTORY pDataDirectoryBak = NULL;

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
    //令pDataDirectoryBak指针指向第一个数据目录的首地址
    pDataDirectoryBak = pOptionHeaderBak->DataDirectory;
    //返回第一个数据目录的首地址
    *pDataDirectory = pDataDirectoryBak;
}


