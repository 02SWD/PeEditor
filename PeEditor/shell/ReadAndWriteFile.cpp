// ReadAndWriteFile.cpp: implementation of the ReadAndWriteFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReadAndWriteFile.h"


/**
 * 将二进制文件读取到内存中
 * @param  filePath 将要读取的文件的路径
 * @return          文件在内存中的位置信息与文件大小信息
 */
FILEINFOR readFile(char* filePath){
	FILEINFOR Infor;
	FILE* file = fopen(filePath,"rb");        //以二进制只读的形式打开文件
	if (file == NULL)				          //若文件打开失败，就返回“空”信息
	{
		printf("open error\n");
		Infor.p = NULL;
		Infor.size = 0;
		return Infor;
	}
	fseek(file,0,SEEK_END);			           //将文件指针指向文件末尾
	Infor.size = ftell(file);		           //利用ftell函数获取文件大小（单位：字节）
	Infor.p = (char*)malloc(Infor.size);       //根据文件大小申请对应大小的内存空间
	if (!Infor.p)
	{
		printf("分配空间失败!");
		fclose(file);
		Infor.p = NULL;
		Infor.size = 0;
		return Infor;
	}
	fseek(file,0,SEEK_SET);                    //将文件指针重新指向文件头
	size_t n = 0;
	n = fread(Infor.p,1024,Infor.size/1024,file);  //开始以1024B大小为单位读取文件
	if (Infor.size%1024 != 0)			       //由于文件大小不总是会被1024B整除，所以余出来的部分需要我们单独的1字节1字节的读取
	{
		n = n+ fread(Infor.p+ftell(file),1,Infor.size-ftell(file),file);
	}
	if (!n)
	{
		printf("读取数据失败!");
		fclose(file);
		Infor.p = NULL;
		Infor.size = 0;
		return Infor;
	}
	fclose(file);							   //关闭文件
	return Infor;
}

/**
 * 将内存中的二进制数据写入到文件中去
 * @param  filePath 将要写入的文件路径
 * @param  infor    内存中二进制数据信息（包括位置信息、大小信息）
 * @return          若写入成功返回1，否则返回0
 */
int writeFile(char* filePath,FILEINFOR infor){
	FILE* file = fopen(filePath,"wb");         //以二进制只写的形式打开文件
	if (file == NULL)				           //若文件打开失败，就返回0
	{
		printf("open error\n");
		return 0;
	}
	fwrite(infor.p,1024,infor.size/1024,file); //开始以1024B大小为单位写入文件
	if (infor.size%1024 != 0)                  //由于文件大小不总是会被1024整除，所以余出来的部分我们单独的1字节1字节的写入
	{
		fwrite(infor.p+ftell(file),1,infor.size-ftell(file),file);
	}
	fclose(file);							   //关闭文件
	return 1;
}
