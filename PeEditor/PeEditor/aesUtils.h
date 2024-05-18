// aesUtils.h: interface for the aesUtils class.
//
///////////////////////////////////////////////////////////////////////////////////////
// 注意：关于aes明文与密文的长度：
//		在明文数据长度为 16 的整数倍时,
// 			假如明文数据长度等于 16n，则使用 NoPadding填充法时 加密后数据长度等于 16n，使用其它填充方法的情况下加密数据长度等于 16*(n+1)。
//		在明文数据长度不足 16 的整数倍的情况下,
// 			假如明文数据长度等于 16n+m [其中 m 小于16]，除了 NoPadding 填充之外的任何方式，加密数据长度都等于 16(n+1)
// 而本次的aes加密算法所采用的填充方式为：PKCS7Padding 
//		所以：在明文数据长度为 16 的整数倍时，假如明文数据长度等于 16n，加密数据长度等于 16*(n+1)；
// 			  在明文数据长度不足 16 的整数倍的情况下，假如明文数据长度等于 16n+m [其中 m 小于16]，加密数据长度都等于 16(n+1)
///////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AESUTILS_H__BC39B24A_C38E_4093_A2CB_614432DEC2DD__INCLUDED_)
#define AFX_AESUTILS_H__BC39B24A_C38E_4093_A2CB_614432DEC2DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define Nb	4 //加解密数据块大小，固定为4

//加密类型对应的密匙长度，单位bit
typedef enum {
    AES128 = 128,
    AES192 = 192,
    AES256 = 256,
} AESType_t;

//加解密模式
typedef enum {
    AES_MODE_ECB = 0,   // 电子密码本模式
    AES_MODE_CBC = 1,   // 密码分组链接模式
} AESMode_t;

typedef struct {
    int Nk;  //用户不需要填充，密钥长度，单位字节, AES128:Nk=16、AES192:Nk=24、AES256:Nr=32
    int Nr;  //用户不需要填充，加密的轮数 AES128:Nr=10、AES192:Nr=12、AES256:Nr=14
    int type;//用户需填充，关联AESType_t
    int mode;//用户需填充，关联AESMode_t
    const void *key;//用户需填充，密匙
    const void *pIV;//用户需填充，初始化向量, 当mode=AES_MODE_CBC时需要设置，指向unsigned char IV[4*Nb];
    //AES拓展密匙, 空间大小 AES128:4*Nb*(10+1):4*Nb*(12+1)、AES256:4*Nb*(14+1)
    unsigned char expandKey[4*Nb*(14+1)];//用户不需要填充，[4*Nb*(Nr+1)]、这里按最大的AES256进行初始化
} AESInfo_t;

/*****************************************************************************
*	函数名：	AESInit
*	功能描述：	初始化
*	输入参数：	aesInfoP -- 用户需要填充
*	输出参数：	无。
*	返回值：	无。
*****************************************************************************/
extern void AESInit(AESInfo_t *aesInfoP);

/*****************************************************************************
*	函数名：	AESEncrypt
*	功能描述：	加密数据
*	输入参数：  aesInfoP    -- 包含key、加密方式等初始化信息	
*              pPlainText  -- 要加密的数据，其长度为dataLen字节。
*			   dataLen	   -- 数据长度，以字节为单位（任意长度的数据均可，程序会自动帮你填充）
*	输出参数：	pCipherText	-- 密文
*	返回值：	解密后的数据长度。。
*****************************************************************************/
extern unsigned int AESEncrypt(IN AESInfo_t *aesInfoP, IN const unsigned char *pPlainText, OUT unsigned char *pCipherText, IN unsigned int dataLen);

/*****************************************************************************
*	函数名：	AESDecrypt
*	描述：		解密数据
*	输入参数：	aesInfoP    -- 包含key、加密方式等初始化信息	
*              pCipherText  -- 要解密的数据，其长度为nDataLen字节。
*			   nDataLen	   -- 数据长度，以字节为单位
*	输出参数：	pPlainText	-- 明文
*	返回值：	返回解密后的数据长度。
*****************************************************************************/
extern unsigned int AESDecrypt(IN AESInfo_t *aesInfoP, OUT unsigned char *pPlainText, IN const unsigned char *pCipherText, IN unsigned int nDataLen);


#endif // !defined(AFX_AESUTILS_H__BC39B24A_C38E_4093_A2CB_614432DEC2DD__INCLUDED_)
