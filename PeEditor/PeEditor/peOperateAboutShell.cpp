// peOperateAboutShell.cpp: implementation of the peOperateAboutShell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "peOperateAboutShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * ���û�����Ĵ�Сת��Ϊ�����Ĵ�С
 * @param  realSize           [�û��������ʵ��С]
 * @param  AlignSpecification [������]
 * @return                    �����Ĵ�С
 */
DWORD Align(DWORD realSize, DWORD AlignSpecification){
    return (realSize/AlignSpecification + 1)*AlignSpecification;
}

/**
 * �����û���������ImageBase�޸��ض�λ��
 * @param  pFileBuffer    �û�������޸���FileBuffer��ֱ���޸�ԭʼ��FileBuffer���ò�����Ϊ�������ҲΪ���������
 * @param  newImageBase   �û������ImageBase
 * @return                 1 �޸��ɹ�
 *                        -1 ��FileBuffer���ض�λ��
 */
int repairRelocTable(LPVOID pFileBuffer, IN DWORD newImageBase){
	//��ȡ DOSͷ��NTͷ����׼PEͷ����ѡPEͷ�����һ���ڱ�
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

	//��ȡFileBuffer���ض�λ���RVA
	DWORD reLocationRva = pOptionalHeader->DataDirectory[5].VirtualAddress;
	//�жϸ�File�Ƿ�����ض�λ����û�У��򷵻�-1
	if (reLocationRva == 0){
		return -1;
	}
	//���ض�λ���RVAת��ΪFOA
	DWORD reLocationFoa = RvaToFileOffest(pFileBuffer, reLocationRva);
    //reLocationָ��ָ���һ���ض�λ����׵�ַ��ͨ��pNewFileBufferTempָ����ض�λ���FOA������ض�λ�����ڴ��е��׵�ַ������PIMAGE_BASE_RELOCATION���͵�ָ��ָ������
	PIMAGE_BASE_RELOCATION reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + reLocationFoa);
    //��VirtualAddress��SizeOfBlock��ֵΪ��ʱ����������е��ض�λ�����������Ӧ����ѭ��
	while (!(reLocation->VirtualAddress == 0 && reLocation->SizeOfBlock == 0)){
		//�õ��ض�λ���virtualAddress����ֵ
		DWORD virtualAddress = reLocation->VirtualAddress;
		//�õ��ض�λ���SizeOfBlock����ֵ
		DWORD sizeOfBlock = reLocation->SizeOfBlock;
		//��itemsָ��ָ���������׵�ַ
		short* pItem = (short*)((DWORD)reLocation + 8);
		//���������ĸ���
		DWORD numOfItems = (sizeOfBlock - 8)/2;
		//���������ݿ������еľ����������4λΪ3�ľ����� ������λ�õ����ݽ����޸�
		for (DWORD i=0; i<numOfItems; i++){
			//�жϸþ�����ĸ�4λ��ֵ�Ƿ�Ϊ3����Ϊ3��������޸�
			if ((pItem[i]&0xF000)>>12 == 3){
				//����þ�����������λ�õ�RVA
				DWORD realItemRva = (pItem[i]&0x0FFF) + virtualAddress;
				//��RVAת��ΪFOA
				DWORD realItemFoa = RvaToFileOffest(pFileBuffer, realItemRva);
				//�� realItemAddr ָ��ָ��þ������������λ�õ�ַ
				int* realItemAddr = (int*)((DWORD)pFileBuffer + realItemFoa);
				//�����޸�
				*realItemAddr = *realItemAddr - (pOptionalHeader->ImageBase) + newImageBase;
			}
		}
		//֮����Ҫ�����ƶ�reLocationָ�룬����ΪPIMAGE_BASE_RELOCATION�ṹ���沢�����ǽ�����PIMAGE_BASE_RELOCATION�ṹ�ģ����ҵľ������С�ǲ�ȷ���ġ�
		reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)reLocation + reLocation->SizeOfBlock);
	}
	//�޸�FileBuffer��ImageBase
	pOptionalHeader->ImageBase = newImageBase;
	return 1;
}











/**
 * ������
 * @param  pFileBuffer        �������FileBuffer
 * @param  size               �û�Ҫ�����Ĵ�С
 * @param  pNewFileBuffer     �����ֺ���µ�FileBuffer
 * @param  dwSizeOfFileBuffer �µ�FileBuffer�Ĵ�С
 * @return                    �����ڳɹ������½ڵ�FOA�����½ڵ�PointerToRawData)
 *                            -1 ���һ���ڱ�ĺ��������ݵ�������������� + ���һ���ں���Ŀ��пռ� ��Ȼ����������һ���ڱ�������ʧ��
 *                            -2 ���һ���ڱ�ĺ��������ݵ���������������ݡ� ��ռ�Ŀռ䲻��������һ���½ڱ�������ʧ��
 * 
 * ע�⣺
 *      ���ڵ�VirtualAddress = (��һ�ڵ�VirtualAddress + ��һ�ڵ�VirtualSize)�ڴ������ֵ
 *      ���ڵ�PointerToRawData = ��һ�ڵ�PointerToRawData + ��һ�ڵ�SizeOfRawData
 *      SizeOfRawData�������ļ������������
 *      VirtualSize����Ҫ���ļ��������������Ҳû��Ҫ���ڴ�����������
 */
DWORD increaseSection(IN LPVOID pFileBuffer, IN DWORD size, OUT LPVOID* pNewFileBuffer, OUT PDWORD dwSizeOfFileBuffer){
	//��pDosHeaderָ��ָ��FileBuffer��DOSͷ
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    //��pNTHeaderָ��ָ��FileBuffer��NTͷ
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    //��pPEHeaderָ��ָ��FileBuffer�ı�׼PEͷ
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
    //��pOptionHeaderָ��ָ��FileBuffer�Ŀ�ѡPEͷ
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    //��pSectionHeaderָ��ָ��FileBuffer�ĵ�һ���ڱ�
    PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
    //��pSectionHeaderBakָ��ָ��FileBuffer�����һ���ڱ�
    PIMAGE_SECTION_HEADER pSectionHeaderBak = pSectionHeader + pPEHeader->NumberOfSections - 1;
    
    //����������֮���µ�FileBuffer���ܴ�С
	DWORD totalSizeOfNewFileBuffer = pSectionHeaderBak->PointerToRawData + pSectionHeaderBak->SizeOfRawData + Align(size,pOptionHeader->FileAlignment);
	//��������������FileBuffer���ܴ�С�����ڴ�ռ�
    char* pTempFileBuffer = (char*) malloc(totalSizeOfNewFileBuffer);
    //��ʼ����������ڴ�ռ�
	memset(pTempFileBuffer, 0, totalSizeOfNewFileBuffer);
	//��FileBufferȫ�����Ƶ�NewFileBuffer��
    memcpy(pTempFileBuffer, pFileBuffer, pSectionHeaderBak->PointerToRawData + pSectionHeaderBak->SizeOfRawData);
    
    //�� pTempDosHeader ָ��ָ�� NewFileBuffer ��DOSͷ
    PIMAGE_DOS_HEADER pTempDosHeader = (PIMAGE_DOS_HEADER)pTempFileBuffer;
    //�� pTempNTHeader ָ��ָ�� NewFileBuffer ��NTͷ
    PIMAGE_NT_HEADERS pTempNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pTempFileBuffer + pTempDosHeader->e_lfanew);
    //�� pTempPEHeader ָ��ָ�� NewFileBuffer �ı�׼PEͷ
    PIMAGE_FILE_HEADER pTempPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pTempNTHeader + 4);
    //�� pTempOptionHeader ָ��ָ�� NewFileBuffer �Ŀ�ѡPEͷ
    PIMAGE_OPTIONAL_HEADER32 pTempOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pTempPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    //�� pTempSectionHeader ָ��ָ�� NewFileBuffer �ĵ�һ���ڱ�ͷ
    PIMAGE_SECTION_HEADER pTempSectionHeader =  (PIMAGE_SECTION_HEADER)((DWORD)pTempOptionHeader + pTempPEHeader->SizeOfOptionalHeader);
    //�� pTempLastSection ָ��ָ�� NewFileBuffer �����һ���ڱ�ͷ
    PIMAGE_SECTION_HEADER pTempLastSection = pTempSectionHeader + pTempPEHeader->NumberOfSections - 1;

    //����DOSͷ�Ĵ�С
	DWORD sizeOfDosHeader = sizeof(IMAGE_DOS_HEADER);
	//����DOSͷ���� ���������ݡ� �Ĵ�С
	DWORD sizeOfRubbishSpare = pDosHeader->e_lfanew - 0x40;
	//����NTͷ��PE��־�Ĵ�С
	DWORD sizeOfPeSign = 4;
	//�����׼PEͷ�Ĵ�С
	DWORD sizeOfPeHeader = sizeof(IMAGE_FILE_HEADER);
	//�����ѡPEͷ�Ĵ�С
	DWORD sizeOfOptionalHeader = pPEHeader->SizeOfOptionalHeader;
	//�������нڱ���ܴ�С
	DWORD sizeOfTotalSections = (pPEHeader->NumberOfSections)*sizeof(IMAGE_SECTION_HEADER);
	//ȥ��ѡPEͷ��SizeOfHeaders���Ե�ֵ
	DWORD sizeOfHeaders = pOptionHeader->SizeOfHeaders;

	//�� pInt ָ��ָ�����һ���ڱ�ĩβ���λ��
	int* pInt = (int*)(pTempSectionHeader + pTempPEHeader->NumberOfSections);
	//�ж����һ���ڱ�ĺ����Ƿ������ݣ������жϽ��������һ���ڱ�ĺ����Ƿ������ݣ�
	if(*pInt == 0){
		//1. ��û�����ݣ���ִ�����������ڵķ���
		//�� useByteCount ��¼���һ���ڱ�ĺ��滹�ж��ٿ��пռ�
		DWORD useByteCount = sizeOfHeaders - sizeOfDosHeader - sizeOfRubbishSpare - sizeOfPeSign - sizeOfPeHeader - sizeOfOptionalHeader - sizeOfTotalSections ;
		//�ж����һ���ڱ�ĺ��������еĿ��пռ��Ƿ��㹻���һ���ڱ�
		if(useByteCount < 80){
			//1.1 =====================����һ����� ���һ���ڱ�ĺ���û�����ݣ���������пռ䲻��������һ���ڱ�������======================== 
			//totalSpareSize �������ڼ�¼ ������������ռ�ռ�Ĵ�С��+useByteCount ���ܴ�С
			DWORD totalSpareSize = useByteCount + sizeOfRubbishSpare;
			//�� totalSpareSize ��Ȼ���������һ���½ڱ���ֱ�ӷ���-1��������ʧ��
			if(totalSpareSize < 80){
				return -1;
			}
			//�� NTͷ+��׼PEͷ+��ѡPEͷ+���нڱ� ���Ƶ� DOSͷ�ĺ���
			memcpy((char*)((DWORD)pTempFileBuffer + 0x40), (char*)((DWORD)pTempFileBuffer + pDosHeader->e_lfanew),sizeOfPeSign+sizeOfPeHeader+sizeOfOptionalHeader+sizeOfTotalSections);
			//��DOSͷ��e_lfanew����ָ���µ�NTͷ��λ��
			pTempDosHeader->e_lfanew = 0x40;
        	
        	//���� pTempDosHeader��pTempNTHeader��pTempPEHeader��pTempOptionHeader��pTempSectionHeader��pTempLastSection ָ�루��Ϊ�ƶ��� NTͷ ��׼PEͷ ��ѡPEͷ ���нڱ�
			PIMAGE_DOS_HEADER pTempDosHeader = (PIMAGE_DOS_HEADER)pTempFileBuffer;
			PIMAGE_NT_HEADERS pTempNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pTempFileBuffer + pTempDosHeader->e_lfanew);
			PIMAGE_FILE_HEADER pTempPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pTempNTHeader + 4);
			PIMAGE_OPTIONAL_HEADER32 pTempOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pTempPEHeader + IMAGE_SIZEOF_FILE_HEADER);
			PIMAGE_SECTION_HEADER pTempSectionHeader =  (PIMAGE_SECTION_HEADER)((DWORD)pTempOptionHeader + pTempPEHeader->SizeOfOptionalHeader);
			PIMAGE_SECTION_HEADER pTempLastSection = pTempSectionHeader + pTempPEHeader->NumberOfSections - 1;
			
        	//�� pBtye ָ��ָ��NewFileBuffer�ƶ�֮������һ���ڱ����Ŀռ��׵�ַ
			char* pBtye = (char*)(pTempSectionHeader + pTempPEHeader->NumberOfSections);
			//��ʼ���ƶ�������һ���ڱ����Ŀռ�
			memset(pBtye, 0, totalSpareSize);
        	
        	//����һ���µĽڱ��������½ڱ������
			IMAGE_SECTION_HEADER newSectionTable;
			memcpy(newSectionTable.Name, ".yishen", 8);
			newSectionTable.Misc.VirtualSize = size;
			newSectionTable.VirtualAddress = Align(pTempLastSection->VirtualAddress + pTempLastSection->Misc.VirtualSize, pTempOptionHeader->SectionAlignment);
			newSectionTable.SizeOfRawData = Align(size, pTempOptionHeader->FileAlignment);
			newSectionTable.PointerToRawData = pTempLastSection->PointerToRawData + pTempLastSection->SizeOfRawData;
			newSectionTable.PointerToRelocations = 0;
			newSectionTable.PointerToLinenumbers = 0;
			newSectionTable.NumberOfRelocations = 0;
			newSectionTable.NumberOfLinenumbers = 0;
			newSectionTable.Characteristics = 0x60000020;
        	
        	//�� �µĽڱ� ���Ƶ� NewFileBuffer�ƶ�֮�����һ���ڱ�ĺ���
			PIMAGE_SECTION_HEADER newSectionTableBak = &newSectionTable;
			memcpy(pBtye, (char*)newSectionTableBak, sizeof(newSectionTable));
        	
        	//�޸� NewFileBuffer ��SizeOfImage����
			pTempOptionHeader->SizeOfImage = newSectionTable.VirtualAddress + newSectionTable.SizeOfRawData;
			//�� NewFileBuffer �� pTempPEHeader->NumberOfSections ��1
			pTempPEHeader->NumberOfSections = pTempPEHeader->NumberOfSections + 1;
			//�� NewFileBuffer ���׵�ַ����
			*pNewFileBuffer = pTempFileBuffer;
			//�� NewFileBuffer �Ĵ�С����
			*dwSizeOfFileBuffer = totalSizeOfNewFileBuffer;
			//���������ڵ�FOA
			return newSectionTable.PointerToRawData;
		}else{
			//1.2 =====================���ڶ������ ���һ���ڱ�ĺ���û�����ݣ�������пռ��㹻����һ���ڱ�������======================== 
			//����һ���µĽڱ��������½ڱ������
			IMAGE_SECTION_HEADER newSectionTable;
			memcpy(newSectionTable.Name, ".yishen", 8);
			newSectionTable.Misc.VirtualSize = size;
			newSectionTable.VirtualAddress = Align(pTempLastSection->VirtualAddress + pTempLastSection->Misc.VirtualSize, pTempOptionHeader->SectionAlignment);
			newSectionTable.SizeOfRawData = Align(size, pTempOptionHeader->FileAlignment);
			newSectionTable.PointerToRawData = pTempLastSection->PointerToRawData + pTempLastSection->SizeOfRawData;
			newSectionTable.PointerToRelocations = 0;
			newSectionTable.PointerToLinenumbers = 0;
			newSectionTable.NumberOfRelocations = 0;
			newSectionTable.NumberOfLinenumbers = 0;
			newSectionTable.Characteristics = 0x60000020;
        	
        	//�� pBtye ָ��ָ��NewFileBuffer���һ���ڱ����Ŀռ��׵�ַ
			char* pBtye = (char*)(pTempSectionHeader + pTempPEHeader->NumberOfSections);
			//�� �µĽڱ� ���Ƶ� NewFileBuffer���һ���ڱ�ĺ���
			PIMAGE_SECTION_HEADER newSectionTableBak = &newSectionTable;
			memcpy(pBtye, (char*)newSectionTableBak, sizeof(newSectionTable));
			memset((char*)((DWORD)pTempFileBuffer + newSectionTable.PointerToRawData), 0, newSectionTable.SizeOfRawData);
        
        	//�޸� NewFileBuffer ��SizeOfImage����
			pTempOptionHeader->SizeOfImage = newSectionTable.VirtualAddress + newSectionTable.SizeOfRawData;
			//�� NewFileBuffer �� pTempPEHeader->NumberOfSections ��1
			pTempPEHeader->NumberOfSections = pTempPEHeader->NumberOfSections + 1;
			//�� NewFileBuffer ���׵�ַ����
			*pNewFileBuffer = pTempFileBuffer;
			//�� NewFileBuffer �Ĵ�С����
			*dwSizeOfFileBuffer = totalSizeOfNewFileBuffer;
			//���������ڵ�FOA
			return newSectionTable.PointerToRawData;
		}
	}else{
		//2. =====================����������� ���һ���ڱ�ĺ��������ݵ������======================== 
		//�ж�DOSͷ����� ���������ݡ� ��ռ�Ŀռ��Ƿ���������һ���½ڱ����������򷵻�-1
		if (sizeOfRubbishSpare < 80){
			return -2;
		}
		//���㹻������ totalSpareSize ���� sizeOfRubbishSpare��Ȼ��ִ���������һ�������ͬ�Ĵ��뼴��
		DWORD totalSpareSize = sizeOfRubbishSpare;
		memcpy((char*)((DWORD)pTempFileBuffer + 0x40), (char*)((DWORD)pTempFileBuffer + pDosHeader->e_lfanew),sizeOfPeSign+sizeOfPeHeader+sizeOfOptionalHeader+sizeOfTotalSections);
		pTempDosHeader->e_lfanew = 0x40;
    
		PIMAGE_DOS_HEADER pTempDosHeader = (PIMAGE_DOS_HEADER)pTempFileBuffer;
		PIMAGE_NT_HEADERS pTempNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pTempFileBuffer + pTempDosHeader->e_lfanew);
		PIMAGE_FILE_HEADER pTempPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pTempNTHeader + 4);
		PIMAGE_OPTIONAL_HEADER32 pTempOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pTempPEHeader + IMAGE_SIZEOF_FILE_HEADER);
		PIMAGE_SECTION_HEADER pTempSectionHeader =  (PIMAGE_SECTION_HEADER)((DWORD)pTempOptionHeader + pTempPEHeader->SizeOfOptionalHeader);
		PIMAGE_SECTION_HEADER pTempLastSection = pTempSectionHeader + pTempPEHeader->NumberOfSections - 1;
    
		char* pBtye = (char*)(pTempSectionHeader + pTempPEHeader->NumberOfSections);
		memset(pBtye, 0, totalSpareSize);
    
		IMAGE_SECTION_HEADER newSectionTable;
		memcpy(newSectionTable.Name, ".yishen", 8);
		newSectionTable.Misc.VirtualSize = size;
		newSectionTable.VirtualAddress = Align(pTempLastSection->VirtualAddress + pTempLastSection->Misc.VirtualSize, pTempOptionHeader->SectionAlignment);
		newSectionTable.SizeOfRawData = Align(size, pTempOptionHeader->FileAlignment);
		newSectionTable.PointerToRawData = pTempLastSection->PointerToRawData + pTempLastSection->SizeOfRawData;
		newSectionTable.PointerToRelocations = 0;
		newSectionTable.PointerToLinenumbers = 0;
		newSectionTable.NumberOfRelocations = 0;
		newSectionTable.NumberOfLinenumbers = 0;
		newSectionTable.Characteristics = 0x60000020;
    
		PIMAGE_SECTION_HEADER newSectionTableBak = &newSectionTable;
		memcpy(pBtye, (char*)newSectionTableBak, sizeof(newSectionTable));
    
		pTempOptionHeader->SizeOfImage = newSectionTable.VirtualAddress + newSectionTable.SizeOfRawData;
		pTempPEHeader->NumberOfSections = pTempPEHeader->NumberOfSections + 1;
		*pNewFileBuffer = pTempFileBuffer;
		*dwSizeOfFileBuffer = totalSizeOfNewFileBuffer;
		return newSectionTable.PointerToRawData;
	}
}


/**
 * ���ļ���FileBuffer���Ƶ�ImageBuffer ����FileBuffer���촦����ImageBuffer��
 * @param  pFileBuffer  FileBufferָ��
 * @param  pImageBuffer ImageBufferָ��
 * @return              ����ʧ�ܷ���0�����򷵻�ImageBuffer�Ĵ�С
 * 
 */
DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer, OUT LPVOID* pImageBuffer){
	//DOSͷ�ṹ��ָ��
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	//NTͷ�ṹ��ָ��
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	//��׼PEͷ�Ľṹ��ָ��
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	//��ѡPEͷ�Ľṹ��ָ��
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	//�ڱ�Ľṹ��ָ��
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	//��DOS�ṹ��ָ��ָ��FileBuffer��DOSͷ�����׵�ַ
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	//��NT�ṹ��ָ��ָ��FileBuffer��NTͷ�����׵�ַ
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
	//���׼PEͷָ��ָ��FileBuffer�ı�׼PEͷ�����׵�ַ
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader+4);
	//���ѡPEͷָ��ָ��FileBuffer�Ŀ�ѡPEͷ�����׵�ַ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER);
	//��ڱ�ṹ��ָ��ָ��FileBuffer�Ľڱ������׵�ַ
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader+pPEHeader->SizeOfOptionalHeader);
	
	//���ݿ�ѡPEͷ��SizeOfImage���Ե�ֵ��������㹻���ڴ�ռ�
	*(pImageBuffer) = malloc(pOptionHeader->SizeOfImage);
	if ((*pImageBuffer) == NULL) {
		printf("�ڴ�ռ�����ʧ�ܣ�\n");
		return 0;
	}
	//�����뵽���ڴ�ռ��ʼ��Ϊ0
	memset(*(pImageBuffer),0,pOptionHeader->SizeOfImage);
	
	//���ݿ�ѡPEͷ�е�SizeOfHeaders���Ե�ֵ����FileBuffer�е� ����ͷ+�ڱ������ ���Ƶ�ImageBuffer֮��
	//srcHeaderָ��ָ��FileBuffer���׵�ַ
	char* srcHeader = (char*)pFileBuffer;
	//desHeaderָ��ָ��ImageBuffer���׵�ַ
	char* desHeader = (char*)(*(pImageBuffer));
	//����SizeOfHeadersһ�ֽ�һ�ֽڵĽ�����ͷ���ݺͽڱ����ݸ���ImageBuffer��
	for (DWORD i=0;i<pOptionHeader->SizeOfHeaders;i++) {
		*(desHeader+i) = *(srcHeader+i);
	}
	
	//���ݱ�׼PEͷ��NumberOfSection���Ե�ֵ��ȡ�ڱ������
	DWORD sectionNum = (DWORD)pPEHeader->NumberOfSections;
	//Ϊ����pSectionHeaderָ���ں����������ݲ���ʱ������ʧ���ڴ�����һ��ר�����ڲ����ڱ��pSectionHeaderBakָ��
	PIMAGE_SECTION_HEADER pSectionHeaderBak = NULL;
	//���ڼ�¼�ڱ���PointerToRawData���Ե�ֵ����������FileBuffer�е�ƫ�Ƶ�ֵַ��
	DWORD srcSection = 0;
	//���ڼ�¼�ڱ���VirtualAddress���Ե�ֵ����������ImageBuffer�е�ƫ�Ƶ�ֵַ��
	DWORD desSection = 0;
	//����ָ�����FileBuffer�е��׵�ַ��ע�⣺����ƫ�Ƶ�ַ�ˣ�
	char* pSrcSection = NULL;
	//����ָ��ڵ�ImageBuffer�е��׵�ַ��ע�⣺����ƫ�Ƶ�ַ�ˣ�
	char* pDesSection = NULL;
	//���ڼ�¼�ڱ���Misc.VirtualSize��SizeOfRawData���Ե���Сֵ�������ڱ�����Ӧ�ڵ����ݴ�С��
	DWORD sizeSection = 0;
	//���ڵ����ݴ�FileBuffer���Ƶ�ImageBuffer�ж�Ӧ��λ����
	for (DWORD k=0;k<sectionNum;k++) {
		//��pSectionHeaderBak�ṹ��ָ��ָ���k+1���ڱ���׵�ַ
		pSectionHeaderBak = pSectionHeader + k;
		//��¼��k+1���ڱ��е�PointerToRawData����ֵ������k+1���ڱ�����Ӧ�Ľ���FileBuffer�е�ƫ�Ƶ�ֵַ��
		srcSection = pSectionHeaderBak->PointerToRawData;
		//��¼��k+1���ڱ��е�VirtualAddress����ֵ������k+1���ڱ�����Ӧ�Ľ���ImageBuffer�е�ƫ�Ƶ�ֵַ��
		desSection = pSectionHeaderBak->VirtualAddress;
		//��pSrcSectionָ��ָ���k+1���ڱ�����Ӧ�Ľ���FileBuffer�е��׵�ַ��ע�⣺����ƫ�Ƶ�ַ��
		pSrcSection = (char*)((DWORD)pFileBuffer+srcSection);
		//��pDesSectionָ��ָ���k+1���ڱ�����Ӧ�Ľ���FileBuffer�е��׵�ַ��ע�⣺����ƫ�Ƶ�ַ��
		pDesSection = (char*)((DWORD)(*(pImageBuffer))+desSection);
		//��¼��k+1���ڱ�����Ӧ�Ľڵ����ݴ�С
		sizeSection = (pSectionHeaderBak->Misc.VirtualSize)>(pSectionHeaderBak->SizeOfRawData)?(pSectionHeaderBak->SizeOfRawData):(pSectionHeaderBak->Misc.VirtualSize);
		//����k+1���ڱ�����Ӧ�Ľ����ݴ�FileBuffer�и��Ƶ�ImageBuffer��
		for (DWORD j=0;j<sizeSection;j++) {
			*(pDesSection+j) = *(pSrcSection+j);
		}
	}
	//���Ƴɹ�����ImageBuffer�Ĵ�С
	return pOptionHeader->SizeOfImage;	
}