// ReadAndWriteFile.cpp: implementation of the ReadAndWriteFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReadAndWriteFile.h"


/**
 * ���������ļ���ȡ���ڴ���
 * @param  filePath ��Ҫ��ȡ���ļ���·��
 * @return          �ļ����ڴ��е�λ����Ϣ���ļ���С��Ϣ
 */
FILEINFOR readFile(char* filePath){
	FILEINFOR Infor;
	FILE* file = fopen(filePath,"rb");        //�Զ�����ֻ������ʽ���ļ�
	if (file == NULL)				          //���ļ���ʧ�ܣ��ͷ��ء��ա���Ϣ
	{
		printf("open error\n");
		Infor.p = NULL;
		Infor.size = 0;
		return Infor;
	}
	fseek(file,0,SEEK_END);			           //���ļ�ָ��ָ���ļ�ĩβ
	Infor.size = ftell(file);		           //����ftell������ȡ�ļ���С����λ���ֽڣ�
	Infor.p = (char*)malloc(Infor.size);       //�����ļ���С�����Ӧ��С���ڴ�ռ�
	if (!Infor.p)
	{
		printf("����ռ�ʧ��!");
		fclose(file);
		Infor.p = NULL;
		Infor.size = 0;
		return Infor;
	}
	fseek(file,0,SEEK_SET);                    //���ļ�ָ������ָ���ļ�ͷ
	size_t n = 0;
	n = fread(Infor.p,1024,Infor.size/1024,file);  //��ʼ��1024B��СΪ��λ��ȡ�ļ�
	if (Infor.size%1024 != 0)			       //�����ļ���С�����ǻᱻ1024B����������������Ĳ�����Ҫ���ǵ�����1�ֽ�1�ֽڵĶ�ȡ
	{
		n = n+ fread(Infor.p+ftell(file),1,Infor.size-ftell(file),file);
	}
	if (!n)
	{
		printf("��ȡ����ʧ��!");
		fclose(file);
		Infor.p = NULL;
		Infor.size = 0;
		return Infor;
	}
	fclose(file);							   //�ر��ļ�
	return Infor;
}

/**
 * ���ڴ��еĶ���������д�뵽�ļ���ȥ
 * @param  filePath ��Ҫд����ļ�·��
 * @param  infor    �ڴ��ж�����������Ϣ������λ����Ϣ����С��Ϣ��
 * @return          ��д��ɹ�����1�����򷵻�0
 */
int writeFile(char* filePath,FILEINFOR infor){
	FILE* file = fopen(filePath,"wb");         //�Զ�����ֻд����ʽ���ļ�
	if (file == NULL)				           //���ļ���ʧ�ܣ��ͷ���0
	{
		printf("open error\n");
		return 0;
	}
	fwrite(infor.p,1024,infor.size/1024,file); //��ʼ��1024B��СΪ��λд���ļ�
	if (infor.size%1024 != 0)                  //�����ļ���С�����ǻᱻ1024����������������Ĳ������ǵ�����1�ֽ�1�ֽڵ�д��
	{
		fwrite(infor.p+ftell(file),1,infor.size-ftell(file),file);
	}
	fclose(file);							   //�ر��ļ�
	return 1;
}
