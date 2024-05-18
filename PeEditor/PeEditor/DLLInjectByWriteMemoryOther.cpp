// DLLInjectByWriteMemoryOther.cpp: implementation of the DLLInjectByWriteMemoryOther class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DLLInjectByWriteMemoryOther.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//��������
int myGetProcAddressCrossProcess(IN DWORD pid, IN HMODULE hRemoteModule, IN char* funcName, IN DWORD funcOrdinal, OUT PDWORD exportFuncAddr);

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
int injectDllByWriteMemoryWithoutRemoteThread(IN char* dllPath, IN DWORD pid, OUT PDWORD dllAddrInTargetProcess, OUT PDWORD dllSizeOfImage){
    //��ȡ��ע���DLL�ļ�����
    FILEINFOR dllFile = readFile(dllPath);
    if (dllFile.p == NULL){
        printf("dll�ļ���ȡʧ�ܣ�\n");
        return -1;
    }
    //��DLL��FileBuffer����ΪImageBuffer
    LPVOID dllImageBuffer = NULL;
    DWORD dwFlag =  CopyFileBufferToImageBuffer(dllFile.p, &dllImageBuffer);
    if(dwFlag == 0){
        printf("����ʧ��!\n");
        free(dllFile.p);
        return -2;
    }

    //��λImageBuffer�� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)dllImageBuffer;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //��ȡdll ImageBuffer �Ĵ�С
    DWORD SizeOfImageTemp = pOptionalHeader->SizeOfImage;

    //����pid��ȡĿ����̵ľ��
    HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hRemoteProcess == NULL){
        printf("Զ�̽��̴�ʧ��!\n");
        free(dllFile.p);
        free(dllImageBuffer);
        return -3;
    }
    
    //��Ŀ����̵�����λ�������㹻����dll ImageBuffer�Ŀռ�
    LPVOID pVirtualAddr = VirtualAllocEx(hRemoteProcess, NULL, SizeOfImageTemp, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (pVirtualAddr == NULL){
        printf("Ŀ������ڴ�ռ�����ʧ��!\n");
        free(dllFile.p);
        free(dllImageBuffer);
        CloseHandle(hRemoteProcess);
        return -4;
    }

    //============================================ �����ض�λ�޸� ==============================================
    //����dll���ض�λ���޸�ImageBuffer
    int relocFlag = repairRelocTableOfImageBuffer(dllImageBuffer, (DWORD)pVirtualAddr);
    if (relocFlag != 1){
        printf("�ض�λ���޸�ʧ��!\n");
        free(dllFile.p);
        free(dllImageBuffer);
        CloseHandle(hRemoteProcess);
        return -5;
    }
    
    //============================================= �޸�IAT��ı���ֵ =============================================
    //��λ ���ض�λ���޸�֮���ImageBuffer�� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ
    PIMAGE_DOS_HEADER pDosHeaderAfterReloc = (PIMAGE_DOS_HEADER)dllImageBuffer;
    PIMAGE_NT_HEADERS pNtHeaderAfterReloc = (PIMAGE_NT_HEADERS)((DWORD)pDosHeaderAfterReloc + pDosHeaderAfterReloc->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeaderAfterReloc = (PIMAGE_FILE_HEADER)((DWORD)pNtHeaderAfterReloc + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeaderAfterReloc = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeaderAfterReloc + IMAGE_SIZEOF_FILE_HEADER);

    //��ȡ������RVA
    DWORD importTableRva = pOptionalHeaderAfterReloc->DataDirectory[1].VirtualAddress;
    //�� importTableAddr ָ��ָ�������׵�ַ
    PIMAGE_IMPORT_DESCRIPTOR importTableAddr = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)dllImageBuffer + importTableRva);

    //ѭ������dll�����
    while (importTableAddr->Name != 0){
        //�� IatTableAddr ָ��ָ��IAT���׵�ַ
        int* IatTableAddr = (int*)((DWORD)dllImageBuffer + importTableAddr->FirstThunk);
        //�� IntTableAddr ָ��ָ��INT���׵�ַ
        int* IntTableAddr = (int*)((DWORD)dllImageBuffer + importTableAddr->OriginalFirstThunk);
        //�� dllNameAddr ָ��ָ��õ�������Ƶ��׵�ַ
        char* dllNameAddr  = (char*)((DWORD)dllImageBuffer + importTableAddr->Name);
        
        //�����ݴ�Ŀ����̵�ģ����
        HMODULE hRemoteModule = NULL;
        //����pidΪĿ������������
        HANDLE hRemoteSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        //���ڼ�¼Ŀ�������ģ�����Ϣ
        MODULEENTRY32 mem32 = {0};
        mem32.dwSize = sizeof(MODULEENTRY32);
        //��ȡĿ����̵�һ��ģ�����Ϣ
        BOOL isNext = Module32First(hRemoteSnap, &mem32);
        BOOL moduleFlag = FALSE;
        while (isNext){
            //�жϵ�ǰĿ������е�ģ���Ƿ���dll����Ҫ��ģ��
            if (stricmp(dllNameAddr, mem32.szModule) == 0){
                //���ǣ����¼��Ŀ������и�ģ��ľ����������ѭ��
                moduleFlag = TRUE;
                hRemoteModule = mem32.hModule;
                break;
            }
            //��ȡĿ���������һ��ģ��ľ��
            isNext = Module32Next(hRemoteSnap, &mem32);
        }
        if (moduleFlag == FALSE){
            printf("Ŀ�������û�б�dll�����ģ��!\n");
            free(dllFile.p);
            free(dllImageBuffer);
            CloseHandle(hRemoteProcess);
            CloseHandle(hRemoteSnap);
            return -6;
        }
        //ѭ������dll��INT��
        while (*IntTableAddr != 0){
            //�ж�INT����ֵ�����λ�Ƿ�Ϊ1����Ϊ1��˵���ú����ǰ�������ŵ���ģ�����˵���ú����ǰ������������
            if ((*IntTableAddr & 0x80000000) == 0x80000000){
                //���ݵ�����ţ��õ�Ŀ�������ָ��ģ���еĺ����ĺ�����ַ
                DWORD exportFuncAddr = 0;
                int funcAddrFlag = myGetProcAddressCrossProcess(pid, hRemoteModule, NULL, (DWORD)(*IntTableAddr & 0x7fffffff), &exportFuncAddr);
                if (funcAddrFlag != 1){
                    printf("DLL��IAT�����ֵ�޸�ʧ��!\n");
                    free(dllFile.p);
                    free(dllImageBuffer);
                    CloseHandle(hRemoteProcess);
                    CloseHandle(hRemoteSnap);
                    return -7;
                }
                //�޸�dll��IAT����ֵ
                *IatTableAddr = exportFuncAddr;
				
            }else{
                //���ݺ��������õ�Ŀ�������ָ��ģ���еĺ����ĺ�����ַ
                PIMAGE_IMPORT_BY_NAME funcName = (PIMAGE_IMPORT_BY_NAME)((DWORD)dllImageBuffer + *IntTableAddr);
                DWORD exportFuncAddr = 0;
				//printf("func->name %s\n", funcName->Name);
                int funcAddrFlag = myGetProcAddressCrossProcess(pid, hRemoteModule, (char*)funcName->Name, NULL, &exportFuncAddr);
                if (funcAddrFlag != 1){
                    printf("DLL��IAT�����ֵ�޸�ʧ��!\n");
                    free(dllFile.p);
                    free(dllImageBuffer);
                    CloseHandle(hRemoteProcess);
                    CloseHandle(hRemoteSnap);
                    return -7;
                }
				printf("oldExportFuncAddr %x\n ",exportFuncAddr);
                //�޸�dll��IAT����ֵ
                *IatTableAddr = exportFuncAddr;
				printf("funcName %s  exportFuncAddr %x\n\n ",funcName->Name, exportFuncAddr);
            }
            //�� IatTableAddr ָ��ָ����һ��IAT����
            IatTableAddr++;
            //�� IntTableAddr ָ��ָ����һ��INT����
            IntTableAddr++;
        }
        //�� importTableAddr ָ��ָ����һ�������
        importTableAddr++;
        //�رտ���
        CloseHandle(hRemoteSnap);
    }
    //===========================================================================================================

    //�޸�dll��ImageBase
    pOptionalHeaderAfterReloc->ImageBase = (DWORD)pVirtualAddr;

    //���޸���ϵ� dll ImageBuffer д�뵽Ŀ�������
    if (WriteProcessMemory(hRemoteProcess, pVirtualAddr, dllImageBuffer, SizeOfImageTemp, NULL) == 0){
        free(dllFile.p);
        free(dllImageBuffer);
        CloseHandle(hRemoteProcess);
        return -8;
    }
    
    //��dll��Ŀ������е��׵�ַ����
    *dllAddrInTargetProcess = (DWORD)pVirtualAddr;
    //��dll��SizeOfImage����
    *dllSizeOfImage = SizeOfImageTemp;
    
    //�ͷ���Դ
    free(dllFile.p);
    free(dllImageBuffer);
    //�رվ��
    CloseHandle(hRemoteProcess);
    return 1;
}


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
int myGetProcAddressCrossProcess(IN DWORD pid, IN HMODULE hRemoteModule, IN char* funcName, IN DWORD funcOrdinal, OUT PDWORD exportFuncAddr){
    if (funcName != NULL && funcOrdinal != 0){
        printf("�������ͺ�����Ų���ͬʱ����!\n");
        return -1;
    }

    //===============================================��Ŀ��dllģ������ݸ��Ƶ��Լ��Ľ��̿ռ���=========================================================
    //��ȡĿ����̵ľ��
    HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    //���ڴ�ʱ���ǲ���֪��Ŀ��ģ��ImageBuffer�Ĵ�С�����������������㹻���� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ �Ŀռ䣨��Ϊ����ռ�Ŀռ��ǹ̶��ģ���Ȼ���ȡĿ��ģ�飬���ݿ�ѡpeͷ�е�SizeOfImage�ó�Ŀ��ģ���ImgaeBuffer��С
    //����һ��������ǣ���Ȼ dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ�Ĵ�С�ǹ̶��ģ�����dosͷ��ntͷ֮�������������ռ�Ŀռ��ǲ��̶��ģ���������Ҫ��ȡ��dosͷ���ݣ����������������ռ�Ŀռ� 
    //Ϊdosͷ�����ڴ�ռ�
    char* pDosHeaderSizeSpace = (char*)malloc(64);
    if (pDosHeaderSizeSpace == NULL){
        printf("�ڴ�����ʧ��!\n");
        return -2;
    }
    //��ʼ��
    memset(pDosHeaderSizeSpace, 0, 64);
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)hRemoteModule, pDosHeaderSizeSpace, 64, NULL) == 0){
        printf("DOSͷ��ȡʧ��!\n");
        free(pDosHeaderSizeSpace);
        return -3;
    }
    //����dosͷ
    PIMAGE_DOS_HEADER pDosHeaderTemp = (PIMAGE_DOS_HEADER)pDosHeaderSizeSpace;
    //sizeOfRubbishSpare ��¼����������ռ�ռ�Ĵ�С
    DWORD sizeOfRubbishSpare = pDosHeaderTemp->e_lfanew - 0x40;
    //�Ѿ������������������ռ�ռ��С�������ͷŵ�dosͷ������ռ���ڴ���
    free(pDosHeaderSizeSpace);
    
    // sizeOfBeforeSection = dosͷ��С + �������ݴ�С + pe��־��С + ��׼peͷ��С + ��ѡpeͷ��С
    DWORD sizeOfBeforeSection = 64 + sizeOfRubbishSpare + 4 + 20 + 224;
    char* partImageBuffer = (char*)malloc(sizeOfBeforeSection);
    if (partImageBuffer == NULL){
        printf("�ڴ�����ʧ��!\n");
        free(partImageBuffer);
        CloseHandle(hRemoteProcess);
        return -4;
    }
    //��ʼ���ڴ�ռ�
    memset(partImageBuffer, 0, sizeOfBeforeSection);
    //��Ŀ��ģ��� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ ��ȡ���ڴ���
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)hRemoteModule, partImageBuffer, sizeOfBeforeSection, NULL) == 0){
        printf("Զ�̽����ڴ��ȡʧ��!\n");
        free(partImageBuffer);
        CloseHandle(hRemoteProcess);
        return -5;
    }
    
    //����Ŀ��ģ��� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)partImageBuffer;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //���ݿ�ѡpeͷ����ȡĿ��ģ��ImageBuffer�Ĵ�С
    DWORD targetModuleSizeOfImage = pOptionalHeader->SizeOfImage;
    //��Ϊ�����Ѿ���֪Ŀ��ģ��ImageBuffer�Ĵ�С������partImageBuffer�ڴ���Ա��ͷ���
    free(partImageBuffer);
    
    //�����㹻���� Ŀ��ģ��ImageBuffer��С ���ڴ�ռ�
    char* ImageBuffer = (char*)malloc(targetModuleSizeOfImage);
    if(ImageBuffer == NULL){
        printf("�ڴ�����ʧ�ܣ�\n");
        CloseHandle(hRemoteProcess);
        return -6;
    }
    //��Ŀ��ģ���ȡ����������ڴ�ռ���
    if (ReadProcessMemory(hRemoteProcess, (LPVOID)hRemoteModule, ImageBuffer, targetModuleSizeOfImage, NULL) == 0){
        printf("Զ�̽����ڴ��ȡʧ��!\n");
        free(ImageBuffer);
        CloseHandle(hRemoteProcess);
        return -7;
    }

    //======����Ŀ��DLL�ĵ�������ȡĿ�꺯���ĺ�����ַ��RVA�������� ������ַ��RVA �� Ŀ��DLLģ����Ŀ������е��׵�ַ ������ú�����Ŀ������еĺ�����ַ=======
    //���½��� dosͷ��ntͷ����׼peͷ �� ��ѡpeͷ
    pDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;
    pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
    pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
    pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

    //�õ�Ŀ��ģ�鵼�����RVA
    DWORD exportTableRva = pOptionalHeader->DataDirectory[0].VirtualAddress;
    //��pExportTableAddrָ��ָ��Ŀ��ģ�鵼������׵�ַ
    PIMAGE_EXPORT_DIRECTORY pExportTableAddr = (PIMAGE_EXPORT_DIRECTORY)((DWORD)ImageBuffer + exportTableRva);
    //�õ�������� Base ����ֵ
    DWORD exportBase = pExportTableAddr->Base;
	//�õ�������� NumberOfFunctions ����ֵ������������������
	DWORD exportNumberOfFunctions = pExportTableAddr->NumberOfFunctions;
    //�õ�������� NumberOfNames ����ֵ���Ժ������������ĺ���������
    DWORD exportNumberOfNames = pExportTableAddr->NumberOfNames;
    //�õ�������� AddressOfFunctions ����ֵ��������ַ���TVA��
    DWORD exportAddressOfFunctionsRva = pExportTableAddr->AddressOfFunctions;
    //�õ�������� AddressOfNames ����ֵ���������Ʊ��RVA��
    DWORD exportAddressOfNamesRva = pExportTableAddr->AddressOfNames;
    //�õ�������� AddressOfNameOrdinals ����ֵ��������ű��RVA��
    DWORD exportAddressOfNameOrdinalsRva = pExportTableAddr->AddressOfNameOrdinals;

    //�� addressOfFunctions ָ��ָ��Ŀ��ģ��ĺ�����ַ���׵�ַ
    int* addressOfFunctions = (int*)((DWORD)ImageBuffer + exportAddressOfFunctionsRva);
    //�� addressOfNames ָ��ָ��Ŀ��ģ��ĺ������Ʊ��׵�ַ
    int* addressOfNames = (int*)((DWORD)ImageBuffer + exportAddressOfNamesRva);
    //�� addressOfNameOrdinals ָ��ָ��Ŀ��ģ��ĺ�����ű��׵�ַ
    short* addressOfNameOrdinals = (short*)((DWORD)ImageBuffer + exportAddressOfNameOrdinalsRva);
    //���ڴ洢Ŀ�꺯����Ŀ������еĵ�ַ
    DWORD exportFuncAddrTemp = 0;
    if (funcOrdinal != 0){
		if ((funcOrdinal - exportBase) > (exportNumberOfFunctions - 1)){
			printf("��������û����Ҫ���ҵĺ���, ��������ȷ�ĵ������!\n");
			free(ImageBuffer);
			CloseHandle(hRemoteProcess);
			return -8;
		}
        //���ݺ��������������ȡ������ַ
        DWORD exportFuncRva = addressOfFunctions[funcOrdinal - exportBase];
        exportFuncAddrTemp = (DWORD)hRemoteModule + exportFuncRva;
    }else{
		BOOL flag = FALSE;
        //���ݺ���������ȡ������ַ
        for(DWORD j=0; j<exportNumberOfNames; j++){
            char* name = (char*)((DWORD)ImageBuffer + addressOfNames[j]);
            if (stricmp(funcName, name) == 0){
                DWORD exportFuncRva = addressOfFunctions[addressOfNameOrdinals[j]];
                exportFuncAddrTemp = (DWORD)hRemoteModule + exportFuncRva;
				flag = TRUE;
                break;
            }
        }
		if (flag == FALSE){
			printf("��������û����Ҫ���ҵĺ���, ��������ȷ�ĺ�����!");
			free(ImageBuffer);
			CloseHandle(hRemoteProcess);
			return -9;
		}
    }
    //===============================================������õ��ĺ�����ַ����=========================================================
    //��Ŀ�꺯����Ŀ������еĵ�ַ����
    *exportFuncAddr = exportFuncAddrTemp;
    //�ͷŵ�����Ŀռ�
    free(ImageBuffer);
    //�ر�Զ�̽��̵ľ��
    CloseHandle(hRemoteProcess);
    return 1;

}







