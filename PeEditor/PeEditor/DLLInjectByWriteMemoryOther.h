// DLLInjectByWriteMemoryOther.h: interface for the DLLInjectByWriteMemoryOther class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLINJECTBYWRITEMEMORYOTHER_H__67F2A0CA_EF7F_4C6E_98F6_9F28196A3258__INCLUDED_)
#define AFX_DLLINJECTBYWRITEMEMORYOTHER_H__67F2A0CA_EF7F_4C6E_98F6_9F28196A3258__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * ��ָ����DLLֱ��д�뵽Ŀ������У���ʹ��LoadLibrary��������������Ľ������޸�IAT����- ���Գɹ�
 * @param  dllPath                DLL��·��������·����
 * @param  pid                    Ŀ����̵�pid
 * @param  dllAddrInTargetProcess д��ɹ���dllģ����Ŀ������е��׵�ַ
 * @param  dllSizeOfImage         д��ɹ���dllģ����Ŀ������е�SizeOfImage
 * @return                        -1 dll�ļ���ȡʧ�ܣ�
 *                                -2 dll��FileBuffer����ʧ��
 *                                -3 Ŀ����̵ľ����ȡʧ��
 *                                -4 Ŀ����̵��ڴ�ռ�����ʧ��
 *                                -5 dll��ImageBuffer�ض�λ���޸�ʧ��
 *                                -6 Ŀ�������û�б����������DLL
 *                                -7 DLL��IAT�����ֵ�޸�ʧ��
 *                                -8 dllд��Ŀ�����ʧ��
 *                                 1 dllע��ɹ�
 * ע�⣺
 *      ���������dll��IAT�����ֵ���޸���������Ŀ������д����޸��ñ���ֵ����Ķ�Ӧģ��
 *    ����������ڸ�dll�У���һ��IAT���������Ӧ�������ú��������ģ��δ��Ŀ����̼��أ���ô��IAT������޸�ʧ�ܣ�
 */
int injectDllByWriteMemoryWithoutRemoteThread(IN char* dllPath, IN DWORD pid, OUT PDWORD dllAddrInTargetProcess, OUT PDWORD dllSizeOfImage);


/**
 * ��ȡĿ�꺯����Ŀ������еĺ�����ַ��GetProcAddress�����Ŀ���̰汾���ԣ����Գɹ���
 * @param  pid            Ŀ����̵�pid
 * @param  hRemoteModule  Ŀ������е�ģ����
 * @param  funcName       ��Ҫ��ȡ�ĺ����ĺ������������ִ�Сд��
 * @param  funcOrdinal    ��Ҫ��ȡ�ĺ����ĵ�����ţ�������10������16���ƣ�
 * @param  exportFuncAddr ��Ҫ��ȡ�ĺ�����Ŀ������еĵ�ַ
 * @return                -1 �������ͺ�����Ų���ͬʱ����
 *                        -2 �ڴ�����ʧ��
 *                        -3 DOSͷ��ȡʧ��
 *                        -4 �ڴ�����ʧ��
 *                        -5 Զ�̽����ڴ��ȡʧ��
 *                        -6 �ڴ�����ʧ��
 *                        -7 Զ�̽����ڴ��ȡʧ��
 *                        -8 ��������û���û����ҵĺ��������������ʽ��
 *                        -9 ��������û���û����ҵĺ�������������ʽ��
 *                         1 ������ַ��ȡ�ɹ�
 * ע�⣺
 *      ����funcName �� ����funcOrdinal ������һ��ΪNULL����������ͬʱ��ֵ��
 *      ����funcName����ʾ���ݺ��������Һ�����ַ
 *      ����funcOrdinal����ʾ���ݺ����ĵ�����Ų��Һ�����ַ
 */
int myGetProcAddressCrossProcess(IN DWORD pid, IN HMODULE hRemoteModule, IN char* funcName, IN DWORD funcOrdinal, OUT PDWORD exportFuncAddr);






#endif // !defined(AFX_DLLINJECTBYWRITEMEMORYOTHER_H__67F2A0CA_EF7F_4C6E_98F6_9F28196A3258__INCLUDED_)
