// peOperateAboutShell.h: interface for the peOperateAboutShell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PEOPERATEABOUTSHELL_H__A604767E_7541_44C3_8D80_0A07F25BD8ED__INCLUDED_)
#define AFX_PEOPERATEABOUTSHELL_H__A604767E_7541_44C3_8D80_0A07F25BD8ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * ���û�����Ĵ�Сת��Ϊ�����Ĵ�С
 * @param  realSize           [�û��������ʵ��С]
 * @param  AlignSpecification [������]
 * @return                    �����Ĵ�С
 */
DWORD Align(DWORD realSize, DWORD AlignSpecification);

/**
 * ������
 * @param  pFileBuffer        �������FileBuffer
 * @param  size               �û�Ҫ�����Ĵ�С
 * @param  pNewFileBuffer     �����ֺ���µ�FileBuffer
 * @param  dwSizeOfFileBuffer �µ�FileBuffer�Ĵ�С
 * @return                    �����ڳɹ������½ڵ�FOA�����½ڵ�PointerToRawData)
 *                            -1 ���һ���ڱ�ĺ��������ݵ�������������� + ���һ���ں���Ŀ��пռ� ��Ȼ����������һ���ڱ�������ʧ��
 *                            -2 ���һ���ڱ�ĺ��������ݵ���������������ݡ� ��ռ�Ŀռ䲻��������һ���½ڱ�������ʧ��
 * 
 * ע�⣺
 *      ���ڵ�VirtualAddress = (��һ�ڵ�VirtualAddress + ��һ�ڵ�VirtualSize)�ڴ������ֵ
 *      ���ڵ�PointerToRawData = ��һ�ڵ�PointerToRawData + ��һ�ڵ�SizeOfRawData
 *      SizeOfRawData�������ļ������������
 *      VirtualSize����Ҫ���ļ��������������Ҳû��Ҫ���ڴ�����������
 */
DWORD increaseSection(IN LPVOID pFileBuffer, IN DWORD size, OUT LPVOID* pNewFileBuffer, OUT PDWORD dwSizeOfFileBuffer);

/**
 * ���ļ���FileBuffer���Ƶ�ImageBuffer ����FileBuffer���촦����ImageBuffer��
 * @param  pFileBuffer  FileBufferָ��
 * @param  pImageBuffer ImageBufferָ��
 * @return              ����ʧ�ܷ���0�����򷵻ظ��ƵĴ�С
 * 
 */
DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer, OUT LPVOID* pImageBuffer);


/**
 * �����û���������ImageBase�޸��ض�λ��
 * @param  pFileBuffer    �û�������޸���FileBuffer��ֱ���޸�ԭʼ��FileBuffer���ò�����Ϊ�������ҲΪ���������
 * @param  newImageBase   �û������ImageBase
 * @return                 1 �޸��ɹ�
 *                        -1 ��FileBuffer���ض�λ��
 */
int repairRelocTable(LPVOID pFileBuffer, IN DWORD newImageBase);


#endif // !defined(AFX_PEOPERATEABOUTSHELL_H__A604767E_7541_44C3_8D80_0A07F25BD8ED__INCLUDED_)
