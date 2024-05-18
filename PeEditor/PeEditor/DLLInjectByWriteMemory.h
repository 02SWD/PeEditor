// DLLInjectByWriteMemory.h: interface for the DLLInjectByWriteMemory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLINJECTBYWRITEMEMORY_H__DEA40EF2_6105_4CAA_B20F_9331F87C5E71__INCLUDED_)
#define AFX_DLLINJECTBYWRITEMEMORY_H__DEA40EF2_6105_4CAA_B20F_9331F87C5E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/**
 * 远程线程注入 - 以内存写入的方式
 * @param  pid  目标进程的pid
 * @param  flag 用于记录注入程序的退出码
 *              -1 内存写入失败
 *              -2 远程线程创建失败
 *               1 注入成功
 * @return      封装有关注入信息的对象
 */
InjectByWriteMemory RemoteDllInjectMemoryWrite(IN DWORD pid, OUT int* flag);


/**
 * 卸载模块
 * @param  pid                 目标进程的pid
 * @param  injectByWriteMemory 封装有注入模块信息的对象，以便卸载函数根据该信息卸载模块
 * @return                     -1 内存属性更改失败
 *                             -2 远程线程终止失败
 *                             -3 卸载失败
 *                              1 卸载成功
 */
int RemoteUninstallModule(IN DWORD pid, IN InjectByWriteMemory injectByWriteMemory);

/**
 * 根据用户传进来的ImageBuffer修复重定位表（注意：这里传进来的是 ImageBuffer，而不是 FileBuffer）
 * @param  pImageBuffer    用户传入待修复的pImageBuffer（直接修改原始的pImageBuffer，该参数既为输入参数也为输出参数）
 * @param  newImageBase   用户输入的ImageBase
 * @return                 1 修复成功
 *                        -1 该ImageBase无重定位表
 */
int repairRelocTableOfImageBuffer(LPVOID pImageBuffer, IN DWORD newImageBase);


#endif // !defined(AFX_DLLINJECTBYWRITEMEMORY_H__DEA40EF2_6105_4CAA_B20F_9331F87C5E71__INCLUDED_)
