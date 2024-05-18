// DLLInjectByWriteMemory.h: interface for the DLLInjectByWriteMemory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLINJECTBYWRITEMEMORY_H__DEA40EF2_6105_4CAA_B20F_9331F87C5E71__INCLUDED_)
#define AFX_DLLINJECTBYWRITEMEMORY_H__DEA40EF2_6105_4CAA_B20F_9331F87C5E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/**
 * Զ���߳�ע�� - ���ڴ�д��ķ�ʽ
 * @param  pid  Ŀ����̵�pid
 * @param  flag ���ڼ�¼ע�������˳���
 *              -1 �ڴ�д��ʧ��
 *              -2 Զ���̴߳���ʧ��
 *               1 ע��ɹ�
 * @return      ��װ�й�ע����Ϣ�Ķ���
 */
InjectByWriteMemory RemoteDllInjectMemoryWrite(IN DWORD pid, OUT int* flag);


/**
 * ж��ģ��
 * @param  pid                 Ŀ����̵�pid
 * @param  injectByWriteMemory ��װ��ע��ģ����Ϣ�Ķ����Ա�ж�غ������ݸ���Ϣж��ģ��
 * @return                     -1 �ڴ����Ը���ʧ��
 *                             -2 Զ���߳���ֹʧ��
 *                             -3 ж��ʧ��
 *                              1 ж�سɹ�
 */
int RemoteUninstallModule(IN DWORD pid, IN InjectByWriteMemory injectByWriteMemory);

/**
 * �����û���������ImageBuffer�޸��ض�λ��ע�⣺���ﴫ�������� ImageBuffer�������� FileBuffer��
 * @param  pImageBuffer    �û�������޸���pImageBuffer��ֱ���޸�ԭʼ��pImageBuffer���ò�����Ϊ�������ҲΪ���������
 * @param  newImageBase   �û������ImageBase
 * @return                 1 �޸��ɹ�
 *                        -1 ��ImageBase���ض�λ��
 */
int repairRelocTableOfImageBuffer(LPVOID pImageBuffer, IN DWORD newImageBase);


#endif // !defined(AFX_DLLINJECTBYWRITEMEMORY_H__DEA40EF2_6105_4CAA_B20F_9331F87C5E71__INCLUDED_)
