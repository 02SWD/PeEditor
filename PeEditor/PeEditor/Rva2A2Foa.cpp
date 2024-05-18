// Rva2A2Foa.cpp: implementation of the Rva2A2Foa class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Rva2A2Foa.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/**
* 将RVA转换为FOA
* 返回值               FOA / -1代表转换失败
* 参数pFileBuffer      FileBuffer的首地址
* 参数RVA             RVA（要输入十六进制）
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
    
    //如果RVA在PE头或节表中，则不需要进行转换，直接返回即可(因为这部分不会被拉伸)
    if(dwRva<=pOptionHeader->SizeOfHeaders){
        return dwRva;
    }else{
        //如果rva不在PE头或节表中，那么就判断它在哪个节中，并计算出FOA
        for(int i=0;i<pPEHeader->NumberOfSections;i++){
            pSectionHeaderBak = pSectionHeader + i;
            //判断RVA是否在该节中
            if(dwRva>=pSectionHeaderBak->VirtualAddress && dwRva<=(pSectionHeaderBak->VirtualAddress+pSectionHeaderBak->Misc.VirtualSize)){
                //若rva在该节中，则计算FOA并返回
                return dwRva-pSectionHeaderBak->VirtualAddress+pSectionHeaderBak->PointerToRawData;
            }
        }
    }
    //返回-1代表转换失败
    return -1;
}


/**
* 将FOA转换为RVA
* 返回值              RVA / -1代表转换失败
* 参数pFileBuffer     FileBuffer的首地址
* 参数dwFoa           FOA（要输入十六进制）
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
    
    //如果FOA在PE头或节表中，则不需要进行转换，直接返回即可(因为这部分不会被拉伸)
    if(dwFoa<=pOptionHeader->SizeOfHeaders){
        return dwFoa;
    }else{
        //如果FOA不在PE头或节表中，那么就判断它在哪个节中，并计算出RVA
        for(int i=0;i<pPEHeader->NumberOfSections;i++){
            pSectionHeaderBak = pSectionHeader + i;
            //判断FOA是否在该节中
            if(dwFoa>=pSectionHeaderBak->PointerToRawData && dwFoa<=(pSectionHeaderBak->PointerToRawData+pSectionHeaderBak->SizeOfRawData)){
                //若FOA在该节中，则计算RVA并返回
                return dwFoa-pSectionHeaderBak->PointerToRawData+pSectionHeaderBak->VirtualAddress;
            }
        }
    }
    //返回-1代表转换失败
    return -1;
}
