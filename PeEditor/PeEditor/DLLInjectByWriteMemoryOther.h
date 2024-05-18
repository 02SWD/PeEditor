// DLLInjectByWriteMemoryOther.h: interface for the DLLInjectByWriteMemoryOther class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLINJECTBYWRITEMEMORYOTHER_H__67F2A0CA_EF7F_4C6E_98F6_9F28196A3258__INCLUDED_)
#define AFX_DLLINJECTBYWRITEMEMORYOTHER_H__67F2A0CA_EF7F_4C6E_98F6_9F28196A3258__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * 将指定的DLL直接写入到目标进程中（不使用LoadLibrary函数，且在自身的进程中修复IAT表表项）- 测试成功
 * @param  dllPath                DLL的路径（绝对路径）
 * @param  pid                    目标进程的pid
 * @param  dllAddrInTargetProcess 写入成功后，dll模块在目标进程中的首地址
 * @param  dllSizeOfImage         写入成功后，dll模块在目标进程中的SizeOfImage
 * @return                        -1 dll文件读取失败！
 *                                -2 dll的FileBuffer拉伸失败
 *                                -3 目标进程的句柄获取失败
 *                                -4 目标进程的内存空间申请失败
 *                                -5 dll的ImageBuffer重定位表修复失败
 *                                -6 目标进程中没有本程序所需的DLL
 *                                -7 DLL的IAT表表项值修复失败
 *                                -8 dll写入目标进程失败
 *                                 1 dll注入成功
 * 注意：
 *      本程序对于dll中IAT表表项值的修复，仅限于目标进程中存在修复该表项值所需的对应模块
 *    （即：如果在该dll中，有一个IAT表表项所对应函数，该函数所需的模块未被目标进程加载，那么该IAT表项将会修复失败）
 */
int injectDllByWriteMemoryWithoutRemoteThread(IN char* dllPath, IN DWORD pid, OUT PDWORD dllAddrInTargetProcess, OUT PDWORD dllSizeOfImage);


/**
 * 获取目标函数在目标进程中的函数地址（GetProcAddress函数的跨进程版本，自，测试成功）
 * @param  pid            目标进程的pid
 * @param  hRemoteModule  目标进程中的模块句柄
 * @param  funcName       想要获取的函数的函数名（不区分大小写）
 * @param  funcOrdinal    想要获取的函数的导出序号（不区分10进制与16进制）
 * @param  exportFuncAddr 想要获取的函数在目标进程中的地址
 * @return                -1 函数名和函数序号不能同时输入
 *                        -2 内存申请失败
 *                        -3 DOS头读取失败
 *                        -4 内存申请失败
 *                        -5 远程进程内存读取失败
 *                        -6 内存申请失败
 *                        -7 远程进程内存读取失败
 *                        -8 导出表中没有用户查找的函数（导出序号形式）
 *                        -9 导出表中没有用户查找的函数（函数名形式）
 *                         1 函数地址获取成功
 * 注意：
 *      参数funcName 和 参数funcOrdinal 必须有一个为NULL（即：不能同时有值）
 *      参数funcName：表示根据函数名查找函数地址
 *      参数funcOrdinal：表示根据函数的导出序号查找函数地址
 */
int myGetProcAddressCrossProcess(IN DWORD pid, IN HMODULE hRemoteModule, IN char* funcName, IN DWORD funcOrdinal, OUT PDWORD exportFuncAddr);






#endif // !defined(AFX_DLLINJECTBYWRITEMEMORYOTHER_H__67F2A0CA_EF7F_4C6E_98F6_9F28196A3258__INCLUDED_)
