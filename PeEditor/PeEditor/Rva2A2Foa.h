// Rva2A2Foa.h: interface for the Rva2A2Foa class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RVA2A2FOA_H__27DA6F7D_E1CD_459A_91B9_000802825A7F__INCLUDED_)
#define AFX_RVA2A2FOA_H__27DA6F7D_E1CD_459A_91B9_000802825A7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
* 将RVA转换为FOA
* 返回值               FOA / -1代表转换失败
* 参数pFileBuffer      FileBuffer的首地址
* 参数RVA             RVA（要输入十六进制）
*/
DWORD RvaToFileOffest(IN LPVOID pFileBuffer,IN DWORD dwRva);


/**
* 将FOA转换为RVA
* 返回值              RVA / -1代表转换失败
* 参数pFileBuffer     FileBuffer的首地址
* 参数dwFoa           FOA（要输入十六进制）
*/
DWORD FoaToRva(IN LPVOID pFileBuffer,IN DWORD dwFoa);





#endif // !defined(AFX_RVA2A2FOA_H__27DA6F7D_E1CD_459A_91B9_000802825A7F__INCLUDED_)
