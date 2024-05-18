// ResetWindowTileUtil.cpp: implementation of the ResetWindowTileUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResetWindowTileUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * �������ô��ڵı��⣬�ﵽ����ԭ�����ڵı��� - appendTile�� ��ʽ��Ч��
 * @param  hwndWindow ���ڵľ��
 * @param  appendTile Ҫ׷�ӵ�������ַ���
 */
VOID ResetWindowTitle(HWND hwndWindow, char* appendTile){
    //����һ�������������ڴ洢����
    char tileBuffer[533];
    //��ʼ��
    memset(tileBuffer, 0, 21);
    //��ȡ��ǰ���ڵı���
    SendMessage(hwndWindow, WM_GETTEXT, 533, (LPARAM)tileBuffer);
    //�ַ���ƴ��
    strcat(tileBuffer, " - ");
    //�ַ���ƴ��
    strcat(tileBuffer, appendTile);
    //���õ�ǰ���ڵı���
    SendMessage(hwndWindow, WM_SETTEXT, 0, (LPARAM)tileBuffer);
}























