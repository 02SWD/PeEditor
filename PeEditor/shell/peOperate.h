// peOperate.h: interface for the peOperate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PEOPERATE_H__5C7503D3_A7F9_4BD9_91C4_CEE938F2D504__INCLUDED_)
#define AFX_PEOPERATE_H__5C7503D3_A7F9_4BD9_91C4_CEE938F2D504__INCLUDED_

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


#endif // !defined(AFX_PEOPERATE_H__5C7503D3_A7F9_4BD9_91C4_CEE938F2D504__INCLUDED_)
