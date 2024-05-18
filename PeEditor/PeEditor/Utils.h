// Utils.h: interface for the Utils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILS_H__31FED870_19A2_49C9_BAC0_22B5EEEB8543__INCLUDED_)
#define AFX_UTILS_H__31FED870_19A2_49C9_BAC0_22B5EEEB8543__INCLUDED_

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
FILEINFOR readFile(char* filePath);

/**
 * ���ڴ��еĶ���������д�뵽�ļ���ȥ
 * @param  filePath ��Ҫд����ļ�·��
 * @param  infor    �ڴ��ж�����������Ϣ������λ����Ϣ����С��Ϣ��
 * @return          ��д��ɹ�����1�����򷵻�0
 */
int writeFile(char* filePath,FILEINFOR infor);



#endif // !defined(AFX_UTILS_H__31FED870_19A2_49C9_BAC0_22B5EEEB8543__INCLUDED_)
