// ButtonOptionPeView.cpp: implementation of the ButtonOptionPeView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ButtonOptionPeView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//函数声明
BOOL CALLBACK ButtonOptionPeViewProc(HWND hwndDlgOfOptionPeView, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID GetOptionalPeInfo(HWND hwndDlgOfOptionPeView, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID GetOptionPeHeaders(IN LPVOID pFileBuffer, OUT PIMAGE_OPTIONAL_HEADER32* pOptionalHeader);

/**
 * 点击 IDC_BUTTON_OptionPeVew 按钮后的处理代码（即：点击 “可选PE头” 按钮之后，要执行的代码）
 * @param  hwndDlgOfPeView 父窗口句柄 (在这里是PeView窗口的句柄，该窗口的ID：IDD_DIALOG_PeViewMainDlg)
 */
VOID ButtonOptionPeViewCode(HWND hwndDlgOfPeView){
    DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_OptionPeView), hwndDlgOfPeView,  ButtonOptionPeViewProc);
}


/**
 * OptionPeView对话框的窗口回调函数（该窗口ID：IDD_DIALOG_OptionPeView）
 * @param  hwndDlgOfOptionPeView OptionPeView窗口的句柄
 * @param  uMsg                  消息类型
 * @param  wParam                WPARAM
 * @param  lParam                LPARAM
 * @return                       
 */
BOOL CALLBACK ButtonOptionPeViewProc(HWND hwndDlgOfOptionPeView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        case WM_INITDIALOG:{
			//重新设置窗口标题，达到：“当前窗口标题 - 打开的文件路径” 的效果
			ResetWindowTitle(hwndDlgOfOptionPeView, gSzFileNameOfPeView);
            //调用GetOptionalPeInfo()函数，将可选pe头的信息显示出来
            GetOptionalPeInfo(hwndDlgOfOptionPeView, uMsg, wParam, lParam);
            return TRUE;
        }
		//点击窗口右上角的红叉时，关闭窗口
		case WM_CLOSE:{
			EndDialog(hwndDlgOfOptionPeView, 0);
			return TRUE;
		}
        case WM_COMMAND:{
            switch(LOWORD(wParam)) {
                //编写退出按钮的处理代码
                case IDC_BUTTON_OptionPeViewExit:{
                    //退出窗口
                    EndDialog(hwndDlgOfOptionPeView, 0);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/**
 * 获取可选PE头信息，并将其显示出来
 * @param  hwndDlgOfOptionPeView OptionPeView窗口的句柄（该窗口ID：IDD_DIALOG_OptionPeView）
 * @param  uMsg                  消息类型
 * @param  wParam                WPARAM
 * @param  lParam                LPARAM
 */
VOID GetOptionalPeInfo(HWND hwndDlgOfOptionPeView, UINT uMsg, WPARAM wParam, LPARAM lParam){
    //封装有pFileBuffer指针和FileBuffer大小的变量
    FILEINFOR file;
    //读取PE文件
    file = readFile(gSzFileNameOfPeView);
    //用于存储指向可选pe头的指针
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = NULL;
    //获取 可选PE头 的位置
    GetOptionPeHeaders(file.p, &pOptionalHeader);
    
    //获取OptionPeView对话框中所有编辑框的句柄
    HWND hEdit_Magic = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_Magic);
    HWND hEdit_MajorLinkerVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MajorLinkerVersion);
    HWND hEdit_MinorLinkerVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MinorLinkerVersion);
    HWND hEdit_SizeOfCode = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfCode);
    HWND hEdit_SizeOfInitializedData = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfInitializedData);
    HWND hEdit_SizeOfUninitializedData = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfUninitializedData);
    HWND hEdit_AddressOfEntryPoint = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_AddressOfEntryPoint);
    HWND hEdit_BaseOfCode = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_BaseOfCode);
    HWND hEdit_BaseOfData = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_BaseOfData);
    HWND hEdit_ImageBase = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_ImageBase);

    HWND hEdit_SectionAlignment = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SectionAlignment);
    HWND hEdit_FileAlignment = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_FileAlignment);
    HWND hEdit_MajorOperatingSystemVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MajorOperatingSystemVersion);
    HWND hEdit_MinorOperatingSystemVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MinorOperatingSystemVersion);
    HWND hEdit_MajorImageVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MajorImageVersion);
    HWND hEdit_MinorImageVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MinorImageVersion);
    HWND hEdit_MajorSubsystemVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MajorSubsystemVersion);
    HWND hEdit_MinorSubsystemVersion = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_MinorSubsystemVersion);
    HWND hEdit_Win32VersionValue = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_Win32VersionValue);
    HWND hEdit_SizeOfImage = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfImage);

    HWND hEdit_SizeOfHeaders = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfHeaders);
    HWND hEdit_CheckSum = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_CheckSum);
    HWND hEdit_Subsystem = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_Subsystem);
    HWND hEdit_DllCharacteristics = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_DllCharacteristics);
    HWND hEdit_SizeOfStackReserve = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfStackReserve);
    HWND hEdit_SizeOfStackCommit = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfStackCommit);
    HWND hEdit_SizeOfHeapReserve = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfHeapReserve);
    HWND hEdit_SizeOfHeapCommit = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_SizeOfHeapCommit);
    HWND hEdit_LoaderFlags = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_LoaderFlags);
    HWND hEdit_NumberOfRvaAndSizes = GetDlgItem(hwndDlgOfOptionPeView, IDC_EDIT_NumberOfRvaAndSizes);



    //为每一个编辑框设置值
    char buffer[21] = {0};
    //将DWORD数据类型转化为LPSTR数据类型（将数字转换为数字字符串）
    ultoa(pOptionalHeader->Magic, buffer, 16);
    //设置编辑框的值
    SetWindowText(hEdit_Magic, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MajorLinkerVersion, buffer, 16);
    SetWindowText(hEdit_MajorLinkerVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MinorLinkerVersion, buffer, 16);
    SetWindowText(hEdit_MinorLinkerVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfCode, buffer, 16);
    SetWindowText(hEdit_SizeOfCode, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfInitializedData, buffer, 16);
    SetWindowText(hEdit_SizeOfInitializedData, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfUninitializedData, buffer, 16);
    SetWindowText(hEdit_SizeOfUninitializedData, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->AddressOfEntryPoint, buffer, 16);
    SetWindowText(hEdit_AddressOfEntryPoint, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->BaseOfCode, buffer, 16);
    SetWindowText(hEdit_BaseOfCode, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->BaseOfData, buffer, 16);
    SetWindowText(hEdit_BaseOfData, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->ImageBase, buffer, 16);
    SetWindowText(hEdit_ImageBase, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SectionAlignment, buffer, 16);
    SetWindowText(hEdit_SectionAlignment, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->FileAlignment, buffer, 16);
    SetWindowText(hEdit_FileAlignment, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MajorOperatingSystemVersion, buffer, 16);
    SetWindowText(hEdit_MajorOperatingSystemVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MinorOperatingSystemVersion, buffer, 16);
    SetWindowText(hEdit_MinorOperatingSystemVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MajorImageVersion, buffer, 16);
    SetWindowText(hEdit_MajorImageVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MinorImageVersion, buffer, 16);
    SetWindowText(hEdit_MinorImageVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MajorSubsystemVersion, buffer, 16);
    SetWindowText(hEdit_MajorSubsystemVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->MinorSubsystemVersion, buffer, 16);
    SetWindowText(hEdit_MinorSubsystemVersion, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->Win32VersionValue, buffer, 16);
    SetWindowText(hEdit_Win32VersionValue, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfImage, buffer, 16);
    SetWindowText(hEdit_SizeOfImage, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfHeaders, buffer, 16);
    SetWindowText(hEdit_SizeOfHeaders, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->CheckSum, buffer, 16);
    SetWindowText(hEdit_CheckSum, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->Subsystem, buffer, 16);
    SetWindowText(hEdit_Subsystem, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->DllCharacteristics, buffer, 16);
    SetWindowText(hEdit_DllCharacteristics, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfStackReserve, buffer, 16);
    SetWindowText(hEdit_SizeOfStackReserve, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfStackCommit, buffer, 16);
    SetWindowText(hEdit_SizeOfStackCommit, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfHeapReserve, buffer, 16);
    SetWindowText(hEdit_SizeOfHeapReserve, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->SizeOfHeapCommit, buffer, 16);
    SetWindowText(hEdit_SizeOfHeapCommit, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->LoaderFlags, buffer, 16);
    SetWindowText(hEdit_LoaderFlags, buffer);

    memset(buffer, 0, 21);
    ultoa(pOptionalHeader->NumberOfRvaAndSizes, buffer, 16);
    SetWindowText(hEdit_NumberOfRvaAndSizes, buffer);

}

/**
 * 定位pFileBuffer中的 可选PE头 的首地址
 * @param  pFileBuffer     FileBuffer首地址
 * @param  pOptionalHeader 传出DOS头的地址
 */
VOID GetOptionPeHeaders(IN LPVOID pFileBuffer, OUT PIMAGE_OPTIONAL_HEADER32* pOptionalHeader){                                                               
    PIMAGE_DOS_HEADER pDosHeaderBak = NULL;
    PIMAGE_NT_HEADERS pNTHeaderBak = NULL;
    PIMAGE_FILE_HEADER pPEHeaderBak = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeaderBak = NULL;
    PIMAGE_SECTION_HEADER pSectionHeaderBak = NULL;

    if(!pFileBuffer){                                   
        printf("传入的指针为null！\n");
        return ;
    }
    //判断是否是有效的MZ标志
    if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE){                                   
        printf("不是有效的MZ标志！\n");
        return ;
    }
    pDosHeaderBak = (PIMAGE_DOS_HEADER)pFileBuffer;
    //判断是否是有效的PE标志
    if(*((PDWORD)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew)) != IMAGE_NT_SIGNATURE){                                 
        printf("不是有效的PE标志！\n");
        return ;
    }
    pNTHeaderBak = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeaderBak->e_lfanew);
    pPEHeaderBak = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeaderBak) + 4);
    pOptionHeaderBak = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeaderBak+IMAGE_SIZEOF_FILE_HEADER);
    //将可选PE头的首地址传出
    *pOptionalHeader = pOptionHeaderBak;
}


