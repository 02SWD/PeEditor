// aesUtils.h: interface for the aesUtils class.
//
///////////////////////////////////////////////////////////////////////////////////////
// ע�⣺����aes���������ĵĳ��ȣ�
//		���������ݳ���Ϊ 16 ��������ʱ,
// 			�����������ݳ��ȵ��� 16n����ʹ�� NoPadding��䷨ʱ ���ܺ����ݳ��ȵ��� 16n��ʹ��������䷽��������¼������ݳ��ȵ��� 16*(n+1)��
//		���������ݳ��Ȳ��� 16 ���������������,
// 			�����������ݳ��ȵ��� 16n+m [���� m С��16]������ NoPadding ���֮����κη�ʽ���������ݳ��ȶ����� 16(n+1)
// �����ε�aes�����㷨�����õ���䷽ʽΪ��PKCS7Padding 
//		���ԣ����������ݳ���Ϊ 16 ��������ʱ�������������ݳ��ȵ��� 16n���������ݳ��ȵ��� 16*(n+1)��
// 			  ���������ݳ��Ȳ��� 16 ��������������£������������ݳ��ȵ��� 16n+m [���� m С��16]���������ݳ��ȶ����� 16(n+1)
///////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AESUTILS_H__BC39B24A_C38E_4093_A2CB_614432DEC2DD__INCLUDED_)
#define AFX_AESUTILS_H__BC39B24A_C38E_4093_A2CB_614432DEC2DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define Nb	4 //�ӽ������ݿ��С���̶�Ϊ4

//�������Ͷ�Ӧ���ܳ׳��ȣ���λbit
typedef enum {
    AES128 = 128,
    AES192 = 192,
    AES256 = 256,
} AESType_t;

//�ӽ���ģʽ
typedef enum {
    AES_MODE_ECB = 0,   // �������뱾ģʽ
    AES_MODE_CBC = 1,   // �����������ģʽ
} AESMode_t;

typedef struct {
    int Nk;  //�û�����Ҫ��䣬��Կ���ȣ���λ�ֽ�, AES128:Nk=16��AES192:Nk=24��AES256:Nr=32
    int Nr;  //�û�����Ҫ��䣬���ܵ����� AES128:Nr=10��AES192:Nr=12��AES256:Nr=14
    int type;//�û�����䣬����AESType_t
    int mode;//�û�����䣬����AESMode_t
    const void *key;//�û�����䣬�ܳ�
    const void *pIV;//�û�����䣬��ʼ������, ��mode=AES_MODE_CBCʱ��Ҫ���ã�ָ��unsigned char IV[4*Nb];
    //AES��չ�ܳ�, �ռ��С AES128:4*Nb*(10+1):4*Nb*(12+1)��AES256:4*Nb*(14+1)
    unsigned char expandKey[4*Nb*(14+1)];//�û�����Ҫ��䣬[4*Nb*(Nr+1)]�����ﰴ����AES256���г�ʼ��
} AESInfo_t;

/*****************************************************************************
*	��������	AESInit
*	����������	��ʼ��
*	���������	aesInfoP -- �û���Ҫ���
*	���������	�ޡ�
*	����ֵ��	�ޡ�
*****************************************************************************/
extern void AESInit(AESInfo_t *aesInfoP);

/*****************************************************************************
*	��������	AESEncrypt
*	����������	��������
*	���������  aesInfoP    -- ����key�����ܷ�ʽ�ȳ�ʼ����Ϣ	
*              pPlainText  -- Ҫ���ܵ����ݣ��䳤��ΪdataLen�ֽڡ�
*			   dataLen	   -- ���ݳ��ȣ����ֽ�Ϊ��λ�����ⳤ�ȵ����ݾ��ɣ�������Զ�������䣩
*	���������	pCipherText	-- ����
*	����ֵ��	���ܺ�����ݳ��ȡ���
*****************************************************************************/
extern unsigned int AESEncrypt(IN AESInfo_t *aesInfoP, IN const unsigned char *pPlainText, OUT unsigned char *pCipherText, IN unsigned int dataLen);

/*****************************************************************************
*	��������	AESDecrypt
*	������		��������
*	���������	aesInfoP    -- ����key�����ܷ�ʽ�ȳ�ʼ����Ϣ	
*              pCipherText  -- Ҫ���ܵ����ݣ��䳤��ΪnDataLen�ֽڡ�
*			   nDataLen	   -- ���ݳ��ȣ����ֽ�Ϊ��λ
*	���������	pPlainText	-- ����
*	����ֵ��	���ؽ��ܺ�����ݳ��ȡ�
*****************************************************************************/
extern unsigned int AESDecrypt(IN AESInfo_t *aesInfoP, OUT unsigned char *pPlainText, IN const unsigned char *pCipherText, IN unsigned int nDataLen);


#endif // !defined(AFX_AESUTILS_H__BC39B24A_C38E_4093_A2CB_614432DEC2DD__INCLUDED_)
