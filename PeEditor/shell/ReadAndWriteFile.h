// ReadAndWriteFile.h: interface for the ReadAndWriteFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READANDWRITEFILE_H__97C47A23_ED70_4ECB_BE05_C529BE0A3248__INCLUDED_)
#define AFX_READANDWRITEFILE_H__97C47A23_ED70_4ECB_BE05_C529BE0A3248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//Ϊ�˷��㽫�ڴ��е�����д�뵽�ļ��У����ﶨ����һ���ṹ�����ڴ洢�ļ���ȡ���ڴ�����Ϣ
typedef struct fileInfor {
	char* p;		//���ļ����ݶ�ȡ���Ը�ָ����ָ����ڴ�ռ���
	long size;		//�ļ����ݴ�С����λ���ֽڣ�
}FILEINFOR;

/**
 * ���������ļ���ȡ���ڴ���
 * @param  filePath ��Ҫ��ȡ���ļ���·��
 * @return          �ļ����ڴ��е�λ����Ϣ���ļ���С��Ϣ
 */
fileInfor readFile(char* filePath);

/**
 * ���ڴ��еĶ���������д�뵽�ļ���ȥ
 * @param  filePath ��Ҫд����ļ�·��
 * @param  infor    �ڴ��ж�����������Ϣ������λ����Ϣ����С��Ϣ��
 * @return          ��д��ɹ�����1�����򷵻�0
 */
int writeFile(char* filePath, FILEINFOR infor);





#endif // !defined(AFX_READANDWRITEFILE_H__97C47A23_ED70_4ECB_BE05_C529BE0A3248__INCLUDED_)
