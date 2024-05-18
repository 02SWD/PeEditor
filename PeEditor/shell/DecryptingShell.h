// DecryptingShell.h: interface for the DecryptingShell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DECRYPTINGSHELL_H__0A16B20A_D37B_4088_800F_CD26C9BDEAEF__INCLUDED_)
#define AFX_DECRYPTINGSHELL_H__0A16B20A_D37B_4088_800F_CD26C9BDEAEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * 脱壳，并运行真正要执行的程序
 * @return 		1 脱壳并运行成功
 *             -1 待脱壳程序读取失败
 *             -2 aes解密失败
 *             -3 解密出来的明文不是PE文件的FileBuffer
 *             -4 解密出来的明文不是PE文件的FileBuffer
 *             -5 FileBuffer 拉伸为 ImageBuffer 失败
 *             -6 ntdll.dll 加载失败
 *             -7 ZwUnmapViewOfSection()函数地址获取失败
 *             -9 真正的待运行程序的ImageBuffer写入内存失败
 *            -10 ImageBase修改失败
 *            -11 没有在指定的位置申请到内存，且该FileBuffer没有重定位表
 *            -12 没有在指定的位置申请到内存，该FileBuffer存在重定位表，但是在任意位置申请内存时，仍然申请失败
 *            -13 重定位表修复失败
 *             
 */
int decryptShell();

/**
 * AES解密
 * @param  cipherText       密文
 * @param  sizeOfCipherText 密文大小
 * @param  plainText        明文
 * @param  sizeOfPlainText  明文大小
 * @return                  1 解密成功
 *                         -1 明文空间申请失败
 */
int aesDecryptApp(IN LPVOID cipherText, IN DWORD sizeOfCipherText, OUT LPVOID* plainText, OUT PDWORD sizeOfPlainText);

#endif // !defined(AFX_DECRYPTINGSHELL_H__0A16B20A_D37B_4088_800F_CD26C9BDEAEF__INCLUDED_)
