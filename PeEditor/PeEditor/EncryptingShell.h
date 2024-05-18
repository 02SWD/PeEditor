// EncryptingShell.h: interface for the EncryptingShell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCRYPTINGSHELL_H__67F6DD84_3CA5_475C_A868_EE8F83492EF6__INCLUDED_)
#define AFX_ENCRYPTINGSHELL_H__67F6DD84_3CA5_475C_A868_EE8F83492EF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * �ӿ�
 * @param  shellSourcePath ��Դ����·��
 * @param  appPath         �����ӿǵ�Ӧ�ó���·��
 * @return                  1 - �ӿǳɹ�
 *                         -1 - �ӿ�ʧ�� - ��Դ������ܳ����ȡʧ��
 *                         -2 - �ӿ�ʧ�� - �����ܳ���aes����ʧ��
 *                         -3 - �ӿ�ʧ�� - �����ܳ�����ܺ���ӵ���������ʧ��
 *                         -4 - �ӿ�ʧ�� - ���ӿǺ������д��ʧ��
 */
int EncryptShell(char* shellSourcePath, char* appPath);


#endif // !defined(AFX_ENCRYPTINGSHELL_H__67F6DD84_3CA5_475C_A868_EE8F83492EF6__INCLUDED_)
