// ReadAndWriteFile.h: interface for the ReadAndWriteFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READANDWRITEFILE_H__97C47A23_ED70_4ECB_BE05_C529BE0A3248__INCLUDED_)
#define AFX_READANDWRITEFILE_H__97C47A23_ED70_4ECB_BE05_C529BE0A3248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//为了方便将内存中的数据写入到文件中，这里定义了一个结构体用于存储文件读取到内存后的信息
typedef struct fileInfor {
	char* p;		//将文件数据读取到以该指针所指向的内存空间中
	long size;		//文件数据大小（单位：字节）
}FILEINFOR;

/**
 * 将二进制文件读取到内存中
 * @param  filePath 将要读取的文件的路径
 * @return          文件在内存中的位置信息与文件大小信息
 */
fileInfor readFile(char* filePath);

/**
 * 将内存中的二进制数据写入到文件中去
 * @param  filePath 将要写入的文件路径
 * @param  infor    内存中二进制数据信息（包括位置信息、大小信息）
 * @return          若写入成功返回1，否则返回0
 */
int writeFile(char* filePath, FILEINFOR infor);





#endif // !defined(AFX_READANDWRITEFILE_H__97C47A23_ED70_4ECB_BE05_C529BE0A3248__INCLUDED_)
