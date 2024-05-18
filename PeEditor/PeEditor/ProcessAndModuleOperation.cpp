// ProcessAndModuleOperation.cpp: implementation of the ProcessAndModuleOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ProcessAndModuleOperation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * ö�ٵ�ǰ����ϵͳ�����еĽ���
 * @param  hwndDlg ���Ի���ľ��
 * @return         ö�ٳɹ�����1��CreateToolhelp32Snapshot����ʧ�ܷ���-1
 */
int EnumAllProcess(HWND hwndDlg){
    //���� ���Ի���ľ�� �� �����б�ؼ�ID ��� �����б�ؼ� �ľ��
    HWND hListProcess = GetDlgItem(hwndDlg, IDC_LIST_PROCESSLIST);
	//ɾ��ģ���б�ؼ��е���������
    SendMessage(hListProcess, LVM_DELETEALLITEMS, 0, 0);

    //��ſ��ս�����Ϣ��һ���ṹ��
    PROCESSENTRY32 currentProcess;
    //��ʹ������ṹ֮ǰ�����������Ĵ�С
    currentProcess.dwSize = sizeof(currentProcess);
    //��ϵͳ�ڵ����н�����һ������
    HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if (hProcess == INVALID_HANDLE_VALUE){
        CloseHandle(hProcess);
        return -1;
    }
    
    //���б�����������
    LV_ITEM vItem;
    //��ʼ��
    memset(&vItem, 0, sizeof(LV_ITEM));
    //��������
    vItem.mask = LVIF_TEXT;
    //��ȡ��һ��������Ϣ
    int bMore=Process32First(hProcess,&currentProcess);
    //���ڼ�¼��Ҫ��ڼ����������
    int line = 0;
    while(bMore){
        //���ڱ�����̻�ַ
        PVOID pProcessImageBase = NULL;
        //���ڱ�����̴�С
        DWORD processImageSize = 0;
        //���ڱ���ģ����Ϣ
        MODULEENTRY32 medule32 = {0};
        medule32.dwSize = sizeof(MODULEENTRY32);
        //����pidΪ�ý�������һ������
        HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, currentProcess.th32ProcessID);
        //�� hModuleSnap == INVALID_HANDLE_VALUE��˵����������ʧ��
        if(INVALID_HANDLE_VALUE == hModuleSnap){
            CloseHandle(hModuleSnap);
        }else{
            //���򣬻�ȡ�ý��̵ĵ�һ��ģ�����Ϣ
            BOOL bRet = Module32First(hModuleSnap, &medule32);
            if (bRet){
                //��ȡ�ɹ������¼�¸�ģ��Ļ�ַ
                pProcessImageBase = (PVOID)medule32.modBaseAddr;
                //ѭ�������ý��̵�����ģ�飬�ۼ����С���Ӷ��õ��ý��̵Ĵ�С����������ģ����ܴ�С��
                while (Module32Next(hModuleSnap, &medule32)){
                    //�ۼ�
                    processImageSize = processImageSize + medule32.modBaseSize;
                }
                CloseHandle(hModuleSnap);
            }else{
                CloseHandle(hModuleSnap);
            }           
        }
        
        //��process�б�ؼ������� ��������
        vItem.pszText = TEXT(currentProcess.szExeFile);
        vItem.iItem = line;
        vItem.iSubItem = 0;
        ListView_InsertItem(hListProcess, &vItem);
        
        //��process�б�ؼ������� PID
        //����������currentProcess.th32ProcessIDΪDWORD���ͣ���Ҫת��Ϊ�����ַ���
        char buff[21] = {0};
        ultoa(currentProcess.th32ProcessID, buff, 10);
        vItem.pszText = TEXT(buff);
        vItem.iItem = line;
        vItem.iSubItem = 1;
        ListView_SetItem(hListProcess, &vItem);
        
        //��process�б�ؼ������� �����ַ
        //����������pProcessImageBaseΪDWORD���ͣ���Ҫת��Ϊ�����ַ���
        memset(buff, 0, sizeof(21));
        ultoa((DWORD)pProcessImageBase, buff, 16);
        vItem.pszText = TEXT(buff);
        vItem.iItem = line;
        vItem.iSubItem = 2;
        ListView_SetItem(hListProcess, &vItem);

        //��process�б�ؼ������� �����С
        //����������processImageSizeΪDWORD���ͣ���Ҫת��Ϊ�����ַ���
        memset(buff, 0, sizeof(21));
        ultoa(processImageSize, buff, 16);
        vItem.pszText = TEXT(buff);
        vItem.iItem = line;
        vItem.iSubItem = 3;
        ListView_SetItem(hListProcess, &vItem);
        
        //������һ������
        bMore=Process32Next(hProcess,&currentProcess);
        //�б�ؼ���������1
        line++;
    }
    //���hProcess���
    CloseHandle(hProcess);
    return 1;
}

/**
 * ����pidö������������е�ģ��
 * @param  hwndDlg ���Ի�����
 * @param  wParam  WPARAM
 * @param  lParam  LPARAM
 * @return         ö�ٳɹ��򷵻�1��û�л�ȡ�����򷵻�-1����һ��ģ����Ϣ��ȡʧ���򷵻�-2
 */
int EnumModuleListByPid(HWND hwndDlg, WPARAM wParam, LPARAM lParam) {
    //1. �ȵõ��û������PID
    //���� ���Ի����� �� �����б�ؼ�ID �õ� �б�ؼ� �ľ��
    HWND hListProcess = GetDlgItem(hwndDlg, IDC_LIST_PROCESSLIST);
    //���ڼ�¼�û�����˵ڼ���
    DWORD dwRowId;
    //����һ�������������ڴ洢����
    TCHAR sizePid[0x20];
    //���ڴ洢���б�ռ��еõ���PID
    LV_ITEM vItemPid;
    //��ʼ��
    memset(&vItemPid, 0, sizeof(LV_ITEM));
    memset(sizePid, 0, 0x20);
    
    //�õ��û����������
    dwRowId = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
    //���û�л�ȡ�����С�������Ϊ
    if (dwRowId == -1){
        return -1;
    }

    //Ҫ��ȡ����
    vItemPid.iSubItem = 1;
    //ָ���洢��ѯ����Ļ�����
    vItemPid.pszText = sizePid;
    //ָ���������Ĵ�С
    vItemPid.cchTextMax = 0x20;
    //��ȡָ���к��ж�ӦԪ�ص�����
    SendMessage(hListProcess, LVM_GETITEMTEXT, dwRowId, (DWORD)&vItemPid);

    //���ڴ洢�������͵�pid
    DWORD dwPid;
    char* leftOver = NULL;
    //���ַ������͵�pidת��Ϊ��������
    dwPid = strtoul(sizePid,&leftOver,10);

    //============ ���ϳ���Ĺ�����Ҫ�ǻ�ȡ�û� �����롱 ��pid =============
    //���� ���Ի����� �� ģ���б�ؼ�ID �õ� ģ���б�ؼ� �ľ��
    HWND hListModule = GetDlgItem(hwndDlg, IDC_LIST_MODULELIST);
    //ɾ��ģ���б�ؼ��е���������
    SendMessage(hListModule, LVM_DELETEALLITEMS, 0, 0);
    //����pidΪ��������һ������
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 medule32 = { sizeof(MODULEENTRY32) };
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwPid);
    //��ȡ�ý��̵ĵ�һ��ģ����Ϣ������ȡʧ����رվ��������
    if (!Module32First(hModuleSnap, &medule32)) {
        CloseHandle(hModuleSnap);
		return -1;
    }
    //���ڼ�¼����ڼ������������
    int line = 0;
    //���б����������
    LV_ITEM vitem;
    //��ʼ��
    memset(&vitem, 0 ,sizeof(LV_ITEM));
    //��������
    vitem.mask = LVIF_TEXT;
    do {
        //���ģ��������Ϣ
        vitem.pszText = medule32.szModule;
        vitem.iItem = line;
        vitem.iSubItem = 0;
        ListView_InsertItem(hListModule, &vitem);
        
        //���ģ���ַ��Ϣ
        LPVOID moduleBase = (LPVOID)medule32.modBaseAddr;
        char buffer[21];
        memset(buffer, 0, sizeof(21));
        ultoa((DWORD)moduleBase, buffer, 16);
        vitem.pszText = buffer;
        vitem.iItem = line;
        vitem.iSubItem = 1;
        ListView_SetItem(hListModule, &vitem);

        //���ģ���С��Ϣ
        memset(buffer, 0, sizeof(21));
        ultoa(medule32.modBaseSize, buffer, 16);
        vitem.pszText = buffer;
        vitem.iItem = line;
        vitem.iSubItem = 2;
        ListView_SetItem(hListModule, &vitem);
        
        //���ģ��·����Ϣ
        vitem.pszText = (LPSTR)medule32.szExePath;
        vitem.iItem = line;
        vitem.iSubItem = 3;
        ListView_SetItem(hListModule, &vitem);
        //��lineָ����һ��
        line++;
        //��ȡ��һ��ģ�����Ϣ
    } while (Module32Next(hModuleSnap, &medule32));
    //�ر�ģ����
    CloseHandle(hModuleSnap);
    return 1;
}

/**
 * ��ʼ�� �����б�ؼ���Ϊ�����б�ؼ���ӱ�ͷ��
 * @param  hwndDlg ���Ի�����
 */
VOID InitProcessList(HWND hwndDlg){
    //���� ���Ի����� �� �б�ؼ�ID ��ȡ�б�ؼ��ľ��
	HWND hListProcess = GetDlgItem(hwndDlg, IDC_LIST_PROCESSLIST);
    //��������ѡ��
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	
    //���ñ�ͷ
	LV_COLUMN lv;
	memset(&lv, 0, sizeof(LV_COLUMN));
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	lv.pszText = TEXT("��������");
	lv.cx = 200;
	lv.iSubItem = 0;
	ListView_InsertColumn(hListProcess, 0, &lv);

	lv.pszText = TEXT("PID");
	lv.cx = 100;
	lv.iSubItem = 1;
	ListView_InsertColumn(hListProcess, 1, &lv);

	lv.pszText = TEXT("�����ַ");
	lv.cx = 100;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);

	lv.pszText = TEXT("�����С");
	lv.cx = 100;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);

}

/**
 * ��ʼ�� ģ���б�ؼ���Ϊģ���б�ؼ���ӱ�ͷ��
 * @param  hwndDlg ���Ի�����
 */
VOID InitModuleList(HWND hwndDlg){
    //���� ���Ի����� �� ģ���б�ؼ�ID ��ȡ ģ���б�ؼ����
	HWND hListModule = GetDlgItem(hwndDlg, IDC_LIST_MODULELIST);
    //��������ѡ��
    SendMessage(hListModule, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	
    //���ñ�ͷ
	LV_COLUMN lv;
    memset(&lv, 0, sizeof(LV_COLUMN));
    lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lv.pszText = TEXT("ģ������");
    lv.cx = 200;
    lv.iSubItem = 0;
    ListView_InsertColumn(hListModule, 0, &lv);

    lv.pszText = TEXT("ģ���ַ");
    lv.cx = 100;
    lv.iSubItem = 1;
    ListView_InsertColumn(hListModule, 1, &lv);

    lv.pszText = TEXT("ģ���С");
    lv.cx = 100;
    lv.iSubItem = 2;
    ListView_InsertColumn(hListModule, 2, &lv);

    lv.pszText = TEXT("ģ��λ��");
    lv.cx = 300;
    lv.iSubItem = 3;
    ListView_InsertColumn(hListModule, 3, &lv);
	
}


