// ButtonPeViewMain.cpp: implementation of the ButtonPeViewMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonPeViewMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
BOOL CALLBACK ButtonPeViewProc(HWND hwndDlgOfPeView, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID GetAllPeHeaders(IN LPVOID pFileBuffer, OUT PIMAGE_DOS_HEADER* pDosHeader, OUT PIMAGE_NT_HEADERS* pNtHeader, OUT PIMAGE_FILE_HEADER* pFileHeader, OUT PIMAGE_OPTIONAL_HEADER32* pOptionalHeader);
VOID GetDosAndFilePeInfo(HWND hwndDlgOfPeView, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
 * 点击IDC_BUTTON_PeView按钮后的处理代码（即：点击 “PE查看器” 按钮之后，要执行的代码）
 * @param  hInstance  应用程序的句柄
 * @param  DialogMain 父窗口句柄 (在这里是DialogMain窗口的句柄，该窗口ID：IDD_DIALOG_MAIN)
 * @param  szFileName 文件路径
 */
VOID ButtonPeViewCodeMain(PTSTR szFileName){
    //将文件路径变为全局变量
    gSzFileNameOfPeView = szFileName;
    //创建一个从对话框
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_PeViewMainDlg), NULL,  ButtonPeViewProc);
}

/**
 * PeView对话框的窗口回调函数 (该窗口ID: IDD_DIALOG_PeViewMainDlg)
 * @param  hwndDlgOfPeView PeView窗口的句柄
 * @param  uMsg            消息类型
 * @param  wParam          WPARAM
 * @param  lParam          LPARAM
 * @return                 
 */
BOOL CALLBACK ButtonPeViewProc(HWND hwndDlgOfPeView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        case WM_INITDIALOG:{
			//重新设置窗口标题，达到：“当前窗口标题 - 打开的文件路径” 的效果
			ResetWindowTitle(hwndDlgOfPeView, gSzFileNameOfPeView);
            //调用GetDosAndFilePeInfo函数，将pe文件的DOS头和标准pe头信息显示出来
            GetDosAndFilePeInfo(hwndDlgOfPeView, uMsg, wParam, lParam);
            return TRUE;
        }
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfPeView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //编写退出按钮的处理代码
                case IDC_BUTTON_PeViewDlgExit:{
                    //退出窗口
                    EndDialog(hwndDlgOfPeView, 0);
                    return TRUE;
                }
				//点击 “可选PE头” 按钮后，会调用ButtonOptionPeViewCode函数，将pe文件的可选pe头信息显示出来
                case IDC_BUTTON_OptionPeVew:{
                    ButtonOptionPeViewCode(hwndDlgOfPeView);
                    return TRUE;
                }
				//点击 “区段” 按钮后，会调用ButtonSectionViewCode函数，将区段信息显示出来
				case IDC_BUTTON_SectionView:{
					ButtonSectionViewCode(hwndDlgOfPeView);
					return TRUE;
				}
				//点击 “数据目录项” 按钮后，会调用ButtonDataDirectoryViewCode函数，将数据目录信息显示出来
				case IDC_BUTTON_DirectoryView:{
					ButtonDataDirectoryViewCode(hwndDlgOfPeView);
					return TRUE;
				}
            }
        }
    }
    return FALSE;
}

/**
 * 获取pe文件的DOS头和标准PE头信息，并将其展示出来
 * @param  hwndDlgOfPeView PeView窗口的句柄 (该窗口ID: IDD_DIALOG_PeViewMainDlg)
 * @param  uMsg            消息类型
 * @param  wParam          WPARAM
 * @param  lParam          LPARAM
 * @param  szFileName      文件路径
 */
VOID GetDosAndFilePeInfo(HWND hwndDlgOfPeView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //封装有pFileBuffer指针和FileBuffer大小的变量
    FILEINFOR file;
    //读取PE文件
    file = readFile(gSzFileNameOfPeView);
    PIMAGE_DOS_HEADER pDosHeader = NULL;
    PIMAGE_NT_HEADERS pNtHeader = NULL;
    PIMAGE_FILE_HEADER pFileHeader = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = NULL;
    //获取DOS头，NT头，标准PE头和可选PE头的位置
    GetAllPeHeaders(file.p, &pDosHeader, &pNtHeader, &pFileHeader, &pOptionalHeader);
    
    //获取PeView对话框中所有编辑框的句柄
    HWND hEdit_E_Magic = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_e_magic);
    HWND hEdit_E_Lfanew = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_e_lfanew);
    HWND hEdit_Machine = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_Machine);
    HWND hEdit_NumberOfSections = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_NumberOfSections);
    HWND hEdit_TimeDateStamp = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_TimeDateStamp);
    HWND hEdit_PointerToSymbolTable = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_PointerToSymbolTable);
    HWND hEdit_NumberOfSymbols = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_NumberOfSymbols);
    HWND hEdit_SizeOfOptionalHeader = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_SizeOfOptionalHeader);
    HWND hEdit_Characteristics = GetDlgItem(hwndDlgOfPeView, IDC_EDIT_Characteristics);

    //为每一个编辑框设置值
    char buffer[21] = {0};
    //将DWORD数据类型转化为LPSTR数据类型（将数字转换为数字字符串）
    ultoa(pDosHeader->e_magic, buffer, 16);
    SetWindowText(hEdit_E_Magic, buffer);
    memset(buffer, 0, 21);
    ultoa(pDosHeader->e_lfanew, buffer, 16);
    SetWindowText(hEdit_E_Lfanew, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->Machine, buffer, 16);
    SetWindowText(hEdit_Machine, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->NumberOfSections, buffer, 16);
    SetWindowText(hEdit_NumberOfSections, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->TimeDateStamp, buffer, 16);
    SetWindowText(hEdit_TimeDateStamp, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->PointerToSymbolTable, buffer, 16);
    SetWindowText(hEdit_PointerToSymbolTable, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->NumberOfSymbols, buffer, 16);
    SetWindowText(hEdit_NumberOfSymbols, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->SizeOfOptionalHeader, buffer, 16);
    SetWindowText(hEdit_SizeOfOptionalHeader, buffer);
    memset(buffer, 0, 21);
    ultoa(pFileHeader->Characteristics, buffer, 16);
    SetWindowText(hEdit_Characteristics, buffer);
}


/**
 * 定位pFileBuffer中的 DOS头首地址、NT头首地址、标准PE头首地址、可选PE头首地址
 * @param  pFileBuffer     FileBuffer首地址
 * @param  pDosHeader      PIMAGE_DOS_HEADER指针地址
 * @param  pNtHeader       PIMAGE_NT_HEADERS指针地址
 * @param  pFileHeader     PIMAGE_FILE_HEADER指针地址
 * @param  pOptionalHeader PIMAGE_OPTIONAL_HEADER32指针地址
 */
VOID GetAllPeHeaders(IN LPVOID pFileBuffer, 
                  OUT PIMAGE_DOS_HEADER* pDosHeader, 
                  OUT PIMAGE_NT_HEADERS* pNtHeader, 
                  OUT PIMAGE_FILE_HEADER* pFileHeader, 
                  OUT PIMAGE_OPTIONAL_HEADER32* pOptionalHeader)
{                                                               
    PIMAGE_DOS_HEADER pDosHeaderBak = NULL;
    PIMAGE_NT_HEADERS pNTHeaderBak = NULL;
    PIMAGE_FILE_HEADER pPEHeaderBak = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeaderBak = NULL;

    if(!pFileBuffer){                                   
        printf("传入的指针为null！\n");
        return ;
    }
    //判断是否是有效的MZ标志
    if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE){                                   
        printf("不是有效的MZ标志！\n");
        return ;
    }
    //将DOS头的首地址传出
    pDosHeaderBak = (PIMAGE_DOS_HEADER)pFileBuffer;
    *pDosHeader = pDosHeaderBak;
    //判断是否是有效的PE标志
    if(*((PDWORD)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew)) != IMAGE_NT_SIGNATURE){                                 
        printf("不是有效的PE标志！\n");
        return ;
    }
    //将NT头的首地址传出
    pNTHeaderBak = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew);
    *pNtHeader = pNTHeaderBak;
    //将标准PE头的首地址传出
    pPEHeaderBak = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeaderBak) + 4);
    *pFileHeader = pPEHeaderBak;
    //将可选PE头的首地址传出
    pOptionHeaderBak = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeaderBak+IMAGE_SIZEOF_FILE_HEADER);
    *pOptionalHeader = pOptionHeaderBak;
}
