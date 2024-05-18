// EncryptingShell.h: interface for the EncryptingShell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCRYPTINGSHELL_H__67F6DD84_3CA5_475C_A868_EE8F83492EF6__INCLUDED_)
#define AFX_ENCRYPTINGSHELL_H__67F6DD84_3CA5_475C_A868_EE8F83492EF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * 加壳
 * @param  shellSourcePath 壳源程序路径
 * @param  appPath         待被加壳的应用程序路径
 * @return                  1 - 加壳成功
 *                         -1 - 加壳失败 - 壳源或待加密程序读取失败
 *                         -2 - 加壳失败 - 待加密程序aes加密失败
 *                         -3 - 加壳失败 - 待加密程序加密后，添加到新增节中失败
 *                         -4 - 加壳失败 - 将加壳后的数据写出失败
 */
int EncryptShell(char* shellSourcePath, char* appPath);


#endif // !defined(AFX_ENCRYPTINGSHELL_H__67F6DD84_3CA5_475C_A868_EE8F83492EF6__INCLUDED_)
