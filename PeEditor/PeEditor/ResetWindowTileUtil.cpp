// ResetWindowTileUtil.cpp: implementation of the ResetWindowTileUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResetWindowTileUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * 重新设置窗口的标题，达到：“原来窗口的标题 - appendTile” 样式的效果
 * @param  hwndWindow 窗口的句柄
 * @param  appendTile 要追加到标题的字符串
 */
VOID ResetWindowTitle(HWND hwndWindow, char* appendTile){
    //开辟一个缓冲区，用于存储标题
    char tileBuffer[533];
    //初始化
    memset(tileBuffer, 0, 21);
    //获取当前窗口的标题
    SendMessage(hwndWindow, WM_GETTEXT, 533, (LPARAM)tileBuffer);
    //字符串拼接
    strcat(tileBuffer, " - ");
    //字符串拼接
    strcat(tileBuffer, appendTile);
    //设置当前窗口的标题
    SendMessage(hwndWindow, WM_SETTEXT, 0, (LPARAM)tileBuffer);
}























