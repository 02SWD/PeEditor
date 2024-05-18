// ResetWindowTileUtil.h: interface for the ResetWindowTileUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESETWINDOWTILEUTIL_H__80D51486_52A6_4484_B74E_50F54572ED1C__INCLUDED_)
#define AFX_RESETWINDOWTILEUTIL_H__80D51486_52A6_4484_B74E_50F54572ED1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * 重新设置窗口的标题，达到：“原来窗口的标题 - appendTile” 样式的效果
 * @param  hwndWindow 窗口的句柄
 * @param  appendTile 要追加到标题的字符串
 */
VOID ResetWindowTitle(HWND hwndWindow, char* appendTile);



#endif // !defined(AFX_RESETWINDOWTILEUTIL_H__80D51486_52A6_4484_B74E_50F54572ED1C__INCLUDED_)
