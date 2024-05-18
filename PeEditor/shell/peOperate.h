// peOperate.h: interface for the peOperate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PEOPERATE_H__5C7503D3_A7F9_4BD9_91C4_CEE938F2D504__INCLUDED_)
#define AFX_PEOPERATE_H__5C7503D3_A7F9_4BD9_91C4_CEE938F2D504__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * 将用户输入的大小转换为对齐后的大小
 * @param  realSize           [用户输入的真实大小]
 * @param  AlignSpecification [对齐规格]
 * @return                    对齐后的大小
 */
DWORD Align(DWORD realSize, DWORD AlignSpecification);


/**
 * 新增节
 * @param  pFileBuffer        待处理的FileBuffer
 * @param  size               用户要新增的大小
 * @param  pNewFileBuffer     新增街后的新的FileBuffer
 * @param  dwSizeOfFileBuffer 新的FileBuffer的大小
 * @return                    新增节成功返回新节的FOA（即新节的PointerToRawData)
 *                            -1 最后一个节表的后面无数据的情况，垃圾数据 + 最后一个节后面的空闲空间 仍然不足以容纳一个节表，新增节失败
 *                            -2 最后一个节表的后面有数据的情况，“垃圾数据” 所占的空间不足以容纳一个新节表，新增节失败
 * 
 * 注意：
 *      本节的VirtualAddress = (上一节的VirtualAddress + 上一节的VirtualSize)内存对齐后的值
 *      本节的PointerToRawData = 上一节的PointerToRawData + 上一节的SizeOfRawData
 *      SizeOfRawData必须是文件对齐的整数倍
 *      VirtualSize不必要是文件对齐的整数倍，也没必要是内存对齐的整数倍
 */
DWORD increaseSection(IN LPVOID pFileBuffer, IN DWORD size, OUT LPVOID* pNewFileBuffer, OUT PDWORD dwSizeOfFileBuffer);

/**
 * 将文件从FileBuffer复制到ImageBuffer （将FileBuffer拉伸处理至ImageBuffer）
 * @param  pFileBuffer  FileBuffer指针
 * @param  pImageBuffer ImageBuffer指针
 * @return              操作失败返回0，否则返回复制的大小
 * 
 */
DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer, OUT LPVOID* pImageBuffer);


/**
 * 根据用户传进来的ImageBase修复重定位表
 * @param  pFileBuffer    用户传入待修复的FileBuffer（直接修改原始的FileBuffer，该参数既为输入参数也为输出参数）
 * @param  newImageBase   用户输入的ImageBase
 * @return                 1 修复成功
 *                        -1 该FileBuffer无重定位表
 */
int repairRelocTable(LPVOID pFileBuffer, IN DWORD newImageBase);


#endif // !defined(AFX_PEOPERATE_H__5C7503D3_A7F9_4BD9_91C4_CEE938F2D504__INCLUDED_)
