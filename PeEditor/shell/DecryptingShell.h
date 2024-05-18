// DecryptingShell.h: interface for the DecryptingShell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DECRYPTINGSHELL_H__0A16B20A_D37B_4088_800F_CD26C9BDEAEF__INCLUDED_)
#define AFX_DECRYPTINGSHELL_H__0A16B20A_D37B_4088_800F_CD26C9BDEAEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * �ѿǣ�����������Ҫִ�еĳ���
 * @return 		1 �ѿǲ����гɹ�
 *             -1 ���ѿǳ����ȡʧ��
 *             -2 aes����ʧ��
 *             -3 ���ܳ��������Ĳ���PE�ļ���FileBuffer
 *             -4 ���ܳ��������Ĳ���PE�ļ���FileBuffer
 *             -5 FileBuffer ����Ϊ ImageBuffer ʧ��
 *             -6 ntdll.dll ����ʧ��
 *             -7 ZwUnmapViewOfSection()������ַ��ȡʧ��
 *             -9 �����Ĵ����г����ImageBufferд���ڴ�ʧ��
 *            -10 ImageBase�޸�ʧ��
 *            -11 û����ָ����λ�����뵽�ڴ棬�Ҹ�FileBufferû���ض�λ��
 *            -12 û����ָ����λ�����뵽�ڴ棬��FileBuffer�����ض�λ������������λ�������ڴ�ʱ����Ȼ����ʧ��
 *            -13 �ض�λ���޸�ʧ��
 *             
 */
int decryptShell();

/**
 * AES����
 * @param  cipherText       ����
 * @param  sizeOfCipherText ���Ĵ�С
 * @param  plainText        ����
 * @param  sizeOfPlainText  ���Ĵ�С
 * @return                  1 ���ܳɹ�
 *                         -1 ���Ŀռ�����ʧ��
 */
int aesDecryptApp(IN LPVOID cipherText, IN DWORD sizeOfCipherText, OUT LPVOID* plainText, OUT PDWORD sizeOfPlainText);

#endif // !defined(AFX_DECRYPTINGSHELL_H__0A16B20A_D37B_4088_800F_CD26C9BDEAEF__INCLUDED_)
