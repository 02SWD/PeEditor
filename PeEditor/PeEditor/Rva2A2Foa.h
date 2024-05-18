// Rva2A2Foa.h: interface for the Rva2A2Foa class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RVA2A2FOA_H__27DA6F7D_E1CD_459A_91B9_000802825A7F__INCLUDED_)
#define AFX_RVA2A2FOA_H__27DA6F7D_E1CD_459A_91B9_000802825A7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
* ��RVAת��ΪFOA
* ����ֵ               FOA / -1����ת��ʧ��
* ����pFileBuffer      FileBuffer���׵�ַ
* ����RVA             RVA��Ҫ����ʮ�����ƣ�
*/
DWORD RvaToFileOffest(IN LPVOID pFileBuffer,IN DWORD dwRva);


/**
* ��FOAת��ΪRVA
* ����ֵ              RVA / -1����ת��ʧ��
* ����pFileBuffer     FileBuffer���׵�ַ
* ����dwFoa           FOA��Ҫ����ʮ�����ƣ�
*/
DWORD FoaToRva(IN LPVOID pFileBuffer,IN DWORD dwFoa);





#endif // !defined(AFX_RVA2A2FOA_H__27DA6F7D_E1CD_459A_91B9_000802825A7F__INCLUDED_)
