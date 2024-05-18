// Rva2A2Foa.cpp: implementation of the Rva2A2Foa class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Rva2A2Foa.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/**
* ��RVAת��ΪFOA
* ����ֵ               FOA / -1����ת��ʧ��
* ����pFileBuffer      FileBuffer���׵�ַ
* ����RVA             RVA��Ҫ����ʮ�����ƣ�
*/
DWORD RvaToFileOffest(IN LPVOID pFileBuffer,IN DWORD dwRva){
    PIMAGE_DOS_HEADER pDosHeader = NULL;
    PIMAGE_NT_HEADERS pNTHeader = NULL;
    PIMAGE_FILE_HEADER pPEHeader = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;
    PIMAGE_SECTION_HEADER pSectionHeaderBak = NULL;
    
    pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader+4);
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER);
    pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader+pPEHeader->SizeOfOptionalHeader);
    
    //���RVA��PEͷ��ڱ��У�����Ҫ����ת����ֱ�ӷ��ؼ���(��Ϊ�ⲿ�ֲ��ᱻ����)
    if(dwRva<=pOptionHeader->SizeOfHeaders){
        return dwRva;
    }else{
        //���rva����PEͷ��ڱ��У���ô���ж������ĸ����У��������FOA
        for(int i=0;i<pPEHeader->NumberOfSections;i++){
            pSectionHeaderBak = pSectionHeader + i;
            //�ж�RVA�Ƿ��ڸý���
            if(dwRva>=pSectionHeaderBak->VirtualAddress && dwRva<=(pSectionHeaderBak->VirtualAddress+pSectionHeaderBak->Misc.VirtualSize)){
                //��rva�ڸý��У������FOA������
                return dwRva-pSectionHeaderBak->VirtualAddress+pSectionHeaderBak->PointerToRawData;
            }
        }
    }
    //����-1����ת��ʧ��
    return -1;
}


/**
* ��FOAת��ΪRVA
* ����ֵ              RVA / -1����ת��ʧ��
* ����pFileBuffer     FileBuffer���׵�ַ
* ����dwFoa           FOA��Ҫ����ʮ�����ƣ�
*/
DWORD FoaToRva(IN LPVOID pFileBuffer,IN DWORD dwFoa){
    PIMAGE_DOS_HEADER pDosHeader = NULL;
    PIMAGE_NT_HEADERS pNTHeader = NULL;
    PIMAGE_FILE_HEADER pPEHeader = NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
    PIMAGE_SECTION_HEADER pSectionHeader = NULL;
    PIMAGE_SECTION_HEADER pSectionHeaderBak = NULL;
    
    pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
    pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader+4);
    pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER);
    pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader+pPEHeader->SizeOfOptionalHeader);
    
    //���FOA��PEͷ��ڱ��У�����Ҫ����ת����ֱ�ӷ��ؼ���(��Ϊ�ⲿ�ֲ��ᱻ����)
    if(dwFoa<=pOptionHeader->SizeOfHeaders){
        return dwFoa;
    }else{
        //���FOA����PEͷ��ڱ��У���ô���ж������ĸ����У��������RVA
        for(int i=0;i<pPEHeader->NumberOfSections;i++){
            pSectionHeaderBak = pSectionHeader + i;
            //�ж�FOA�Ƿ��ڸý���
            if(dwFoa>=pSectionHeaderBak->PointerToRawData && dwFoa<=(pSectionHeaderBak->PointerToRawData+pSectionHeaderBak->SizeOfRawData)){
                //��FOA�ڸý��У������RVA������
                return dwFoa-pSectionHeaderBak->PointerToRawData+pSectionHeaderBak->VirtualAddress;
            }
        }
    }
    //����-1����ת��ʧ��
    return -1;
}
