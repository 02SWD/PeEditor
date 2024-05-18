// peOperateAboutShell.cpp: implementation of the peOperateAboutShell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "peOperateAboutShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * 将用户输入的大小转换为对齐后的大小
 * @param  realSize           [用户输入的真实大小]
 * @param  AlignSpecification [对齐规格]
 * @return                    对齐后的大小
 */
DWORD Align(DWORD realSize, DWORD AlignSpecification){
    return (realSize/AlignSpecification + 1)*AlignSpecification;
}

/**
 * 根据用户传进来的ImageBase修复重定位表
 * @param  pFileBuffer    用户传入待修复的FileBuffer（直接修改原始的FileBuffer，该参数既为输入参数也为输出参数）
 * @param  newImageBase   用户输入的ImageBase
 * @return                 1 修复成功
 *                        -1 该FileBuffer无重定位表
 */
int repairRelocTable(LPVOID pFileBuffer, IN DWORD newImageBase){
	//获取 DOS头、NT头、标准PE头、可选PE头、最后一个节表
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPeHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPeHeader + IMAGE_SIZEOF_FILE_HEADER);

	//获取FileBuffer的重定位表的RVA
	DWORD reLocationRva = pOptionalHeader->DataDirectory[5].VirtualAddress;
	//判断该File是否具有重定位表，若没有，则返回-1
	if (reLocationRva == 0){
		return -1;
	}
	//将重定位表的RVA转换为FOA
	DWORD reLocationFoa = RvaToFileOffest(pFileBuffer, reLocationRva);
    //reLocation指针指向第一个重定位表的首地址（通过pNewFileBufferTemp指针和重定位表的FOA计算出重定位表在内存中的首地址，并令PIMAGE_BASE_RELOCATION类型的指针指向它）
	PIMAGE_BASE_RELOCATION reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + reLocationFoa);
    //当VirtualAddress与SizeOfBlock的值为零时，则代表所有的重定位表遍历结束，应跳出循环
	while (!(reLocation->VirtualAddress == 0 && reLocation->SizeOfBlock == 0)){
		//得到重定位表的virtualAddress属性值
		DWORD virtualAddress = reLocation->VirtualAddress;
		//得到重定位表的SizeOfBlock属性值
		DWORD sizeOfBlock = reLocation->SizeOfBlock;
		//将items指针指向具体项的首地址
		short* pItem = (short*)((DWORD)reLocation + 8);
		//计算具体项的个数
		DWORD numOfItems = (sizeOfBlock - 8)/2;
		//遍历该数据块中所有的具体项，并将高4位为3的具体项 所代表位置的数据进行修复
		for (DWORD i=0; i<numOfItems; i++){
			//判断该具体项的高4位的值是否为3，若为3，则进行修复
			if ((pItem[i]&0xF000)>>12 == 3){
				//计算该具体项所代表位置的RVA
				DWORD realItemRva = (pItem[i]&0x0FFF) + virtualAddress;
				//将RVA转换为FOA
				DWORD realItemFoa = RvaToFileOffest(pFileBuffer, realItemRva);
				//令 realItemAddr 指针指向该具体项所代表的位置地址
				int* realItemAddr = (int*)((DWORD)pFileBuffer + realItemFoa);
				//进行修复
				*realItemAddr = *realItemAddr - (pOptionalHeader->ImageBase) + newImageBase;
			}
		}
		//之所以要这样移动reLocation指针，是因为PIMAGE_BASE_RELOCATION结构后面并不还是紧跟着PIMAGE_BASE_RELOCATION结构的，并且的具体项大小是不确定的。
		reLocation = (PIMAGE_BASE_RELOCATION)((DWORD)reLocation + reLocation->SizeOfBlock);
	}
	//修改FileBuffer的ImageBase
	pOptionalHeader->ImageBase = newImageBase;
	return 1;
}











/**
 * 新增节
 * @param  pFileBuffer        待处理的FileBuffer
 * @param  size               用户要新增的大小
 * @param  pNewFileBuffer     新增街后的新的FileBuffer
 * @param  dwSizeOfFileBuffer 新的FileBuffer的大小
 * @return                    新增节成功返回新节的FOA（即新节的PointerToRawData)
 *                            -1 最后一个节表的后面无数据的情况，垃圾数据 + 最后一个节后面的空闲空间 仍然不足以容纳一个节表，新增节失败
 *                            -2 最后一个节表的后面有数据的情况，“垃圾数据” 所占的空间不足以容纳一个新节表，新增节失败
 * 
 * 注意：
 *      本节的VirtualAddress = (上一节的VirtualAddress + 上一节的VirtualSize)内存对齐后的值
 *      本节的PointerToRawData = 上一节的PointerToRawData + 上一节的SizeOfRawData
 *      SizeOfRawData必须是文件对齐的整数倍
 *      VirtualSize不必要是文件对齐的整数倍，也没必要是内存对齐的整数倍
 */
DWORD increaseSection(IN LPVOID pFileBuffer, IN DWORD size, OUT LPVOID* pNewFileBuffer, OUT PDWORD dwSizeOfFileBuffer){
	//令pDosHeader指针指向FileBuffer的DOS头
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
    //令pNTHeader指针指向FileBuffer的NT头
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
    //令pPEHeader指针指向FileBuffer的标准PE头
    PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4);
    //令pOptionHeader指针指向FileBuffer的可选PE头
    PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    //令pSectionHeader指针指向FileBuffer的第一个节表
    PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
    //令pSectionHeaderBak指针指向FileBuffer的最后一个节表
    PIMAGE_SECTION_HEADER pSectionHeaderBak = pSectionHeader + pPEHeader->NumberOfSections - 1;
    
    //计算新增节之后，新的FileBuffer的总大小
	DWORD totalSizeOfNewFileBuffer = pSectionHeaderBak->PointerToRawData + pSectionHeaderBak->SizeOfRawData + Align(size,pOptionHeader->FileAlignment);
	//根据上面计算的新FileBuffer的总大小申请内存空间
    char* pTempFileBuffer = (char*) malloc(totalSizeOfNewFileBuffer);
    //初始化新申请的内存空间
	memset(pTempFileBuffer, 0, totalSizeOfNewFileBuffer);
	//将FileBuffer全部复制到NewFileBuffer中
    memcpy(pTempFileBuffer, pFileBuffer, pSectionHeaderBak->PointerToRawData + pSectionHeaderBak->SizeOfRawData);
    
    //令 pTempDosHeader 指针指向 NewFileBuffer 的DOS头
    PIMAGE_DOS_HEADER pTempDosHeader = (PIMAGE_DOS_HEADER)pTempFileBuffer;
    //令 pTempNTHeader 指针指向 NewFileBuffer 的NT头
    PIMAGE_NT_HEADERS pTempNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pTempFileBuffer + pTempDosHeader->e_lfanew);
    //令 pTempPEHeader 指针指向 NewFileBuffer 的标准PE头
    PIMAGE_FILE_HEADER pTempPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pTempNTHeader + 4);
    //令 pTempOptionHeader 指针指向 NewFileBuffer 的可选PE头
    PIMAGE_OPTIONAL_HEADER32 pTempOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pTempPEHeader + IMAGE_SIZEOF_FILE_HEADER);
    //令 pTempSectionHeader 指针指向 NewFileBuffer 的第一个节表头
    PIMAGE_SECTION_HEADER pTempSectionHeader =  (PIMAGE_SECTION_HEADER)((DWORD)pTempOptionHeader + pTempPEHeader->SizeOfOptionalHeader);
    //令 pTempLastSection 指针指向 NewFileBuffer 的最后一个节表头
    PIMAGE_SECTION_HEADER pTempLastSection = pTempSectionHeader + pTempPEHeader->NumberOfSections - 1;

    //计算DOS头的大小
	DWORD sizeOfDosHeader = sizeof(IMAGE_DOS_HEADER);
	//计算DOS头后面 “垃圾数据” 的大小
	DWORD sizeOfRubbishSpare = pDosHeader->e_lfanew - 0x40;
	//计算NT头中PE标志的大小
	DWORD sizeOfPeSign = 4;
	//计算标准PE头的大小
	DWORD sizeOfPeHeader = sizeof(IMAGE_FILE_HEADER);
	//计算可选PE头的大小
	DWORD sizeOfOptionalHeader = pPEHeader->SizeOfOptionalHeader;
	//计算所有节表的总大小
	DWORD sizeOfTotalSections = (pPEHeader->NumberOfSections)*sizeof(IMAGE_SECTION_HEADER);
	//去可选PE头中SizeOfHeaders属性的值
	DWORD sizeOfHeaders = pOptionHeader->SizeOfHeaders;

	//令 pInt 指针指向最后一个节表末尾后的位置
	int* pInt = (int*)(pTempSectionHeader + pTempPEHeader->NumberOfSections);
	//判断最后一个节表的后面是否还有数据（即：判断紧接着最后一个节表的后面是否还有数据）
	if(*pInt == 0){
		//1. 若没有数据，则执行下面新增节的方法
		//令 useByteCount 记录最后一个节表的后面还有多少空闲空间
		DWORD useByteCount = sizeOfHeaders - sizeOfDosHeader - sizeOfRubbishSpare - sizeOfPeSign - sizeOfPeHeader - sizeOfOptionalHeader - sizeOfTotalSections ;
		//判断最后一个节表的后面所具有的空闲空间是否足够添加一个节表
		if(useByteCount < 80){
			//1.1 =====================【第一种情况 最后一个节表的后面没有数据，但是其空闲空间不足以容纳一个节表的情况】======================== 
			//totalSpareSize 变量用于记录 “垃圾数据所占空间的大小”+useByteCount 的总大小
			DWORD totalSpareSize = useByteCount + sizeOfRubbishSpare;
			//若 totalSpareSize 仍然不足以添加一个新节表，则直接返回-1，新增节失败
			if(totalSpareSize < 80){
				return -1;
			}
			//将 NT头+标准PE头+可选PE头+所有节表 复制到 DOS头的后面
			memcpy((char*)((DWORD)pTempFileBuffer + 0x40), (char*)((DWORD)pTempFileBuffer + pDosHeader->e_lfanew),sizeOfPeSign+sizeOfPeHeader+sizeOfOptionalHeader+sizeOfTotalSections);
			//令DOS头的e_lfanew属性指向新的NT头的位置
			pTempDosHeader->e_lfanew = 0x40;
        	
        	//更新 pTempDosHeader、pTempNTHeader、pTempPEHeader、pTempOptionHeader、pTempSectionHeader、pTempLastSection 指针（因为移动了 NT头 标准PE头 可选PE头 所有节表）
			PIMAGE_DOS_HEADER pTempDosHeader = (PIMAGE_DOS_HEADER)pTempFileBuffer;
			PIMAGE_NT_HEADERS pTempNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pTempFileBuffer + pTempDosHeader->e_lfanew);
			PIMAGE_FILE_HEADER pTempPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pTempNTHeader + 4);
			PIMAGE_OPTIONAL_HEADER32 pTempOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pTempPEHeader + IMAGE_SIZEOF_FILE_HEADER);
			PIMAGE_SECTION_HEADER pTempSectionHeader =  (PIMAGE_SECTION_HEADER)((DWORD)pTempOptionHeader + pTempPEHeader->SizeOfOptionalHeader);
			PIMAGE_SECTION_HEADER pTempLastSection = pTempSectionHeader + pTempPEHeader->NumberOfSections - 1;
			
        	//令 pBtye 指针指向NewFileBuffer移动之后的最后一个节表后面的空间首地址
			char* pBtye = (char*)(pTempSectionHeader + pTempPEHeader->NumberOfSections);
			//初始化移动后的最后一个节表后面的空间
			memset(pBtye, 0, totalSpareSize);
        	
        	//创建一个新的节表、并设置新节表的属性
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
        	
        	//将 新的节表 复制到 NewFileBuffer移动之后最后一个节表的后面
			PIMAGE_SECTION_HEADER newSectionTableBak = &newSectionTable;
			memcpy(pBtye, (char*)newSectionTableBak, sizeof(newSectionTable));
        	
        	//修改 NewFileBuffer 的SizeOfImage属性
			pTempOptionHeader->SizeOfImage = newSectionTable.VirtualAddress + newSectionTable.SizeOfRawData;
			//令 NewFileBuffer 的 pTempPEHeader->NumberOfSections 加1
			pTempPEHeader->NumberOfSections = pTempPEHeader->NumberOfSections + 1;
			//将 NewFileBuffer 的首地址传出
			*pNewFileBuffer = pTempFileBuffer;
			//将 NewFileBuffer 的大小传出
			*dwSizeOfFileBuffer = totalSizeOfNewFileBuffer;
			//返回新增节的FOA
			return newSectionTable.PointerToRawData;
		}else{
			//1.2 =====================【第二种情况 最后一个节表的后面没有数据，且其空闲空间足够容纳一个节表的情况】======================== 
			//创建一个新的节表、并设置新节表的属性
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
        	
        	//令 pBtye 指针指向NewFileBuffer最后一个节表后面的空间首地址
			char* pBtye = (char*)(pTempSectionHeader + pTempPEHeader->NumberOfSections);
			//将 新的节表 复制到 NewFileBuffer最后一个节表的后面
			PIMAGE_SECTION_HEADER newSectionTableBak = &newSectionTable;
			memcpy(pBtye, (char*)newSectionTableBak, sizeof(newSectionTable));
			memset((char*)((DWORD)pTempFileBuffer + newSectionTable.PointerToRawData), 0, newSectionTable.SizeOfRawData);
        
        	//修改 NewFileBuffer 的SizeOfImage属性
			pTempOptionHeader->SizeOfImage = newSectionTable.VirtualAddress + newSectionTable.SizeOfRawData;
			//令 NewFileBuffer 的 pTempPEHeader->NumberOfSections 加1
			pTempPEHeader->NumberOfSections = pTempPEHeader->NumberOfSections + 1;
			//将 NewFileBuffer 的首地址传出
			*pNewFileBuffer = pTempFileBuffer;
			//将 NewFileBuffer 的大小传出
			*dwSizeOfFileBuffer = totalSizeOfNewFileBuffer;
			//返回新增节的FOA
			return newSectionTable.PointerToRawData;
		}
	}else{
		//2. =====================【第三种情况 最后一个节表的后面有数据的情况】======================== 
		//判断DOS头后面的 “垃圾数据” 所占的空间是否足以容纳一个新节表，若不够，则返回-1
		if (sizeOfRubbishSpare < 80){
			return -2;
		}
		//若足够，则令 totalSpareSize 等于 sizeOfRubbishSpare，然后执行与上面第一种情况相同的代码即可
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
 * 将文件从FileBuffer复制到ImageBuffer （将FileBuffer拉伸处理至ImageBuffer）
 * @param  pFileBuffer  FileBuffer指针
 * @param  pImageBuffer ImageBuffer指针
 * @return              操作失败返回0，否则返回ImageBuffer的大小
 * 
 */
DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer, OUT LPVOID* pImageBuffer){
	//DOS头结构体指针
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	//NT头结构体指针
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	//标准PE头的结构体指针
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	//可选PE头的结构体指针
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	//节表的结构体指针
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	//令DOS结构体指针指向FileBuffer的DOS头数据首地址
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	//令NT结构体指针指向FileBuffer的NT头数据首地址
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer+pDosHeader->e_lfanew);
	//令标准PE头指针指向FileBuffer的标准PE头数据首地址
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader+4);
	//令可选PE头指针指向FileBuffer的可选PE头数据首地址
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader+IMAGE_SIZEOF_FILE_HEADER);
	//令节表结构体指针指向FileBuffer的节表数据首地址
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader+pPEHeader->SizeOfOptionalHeader);
	
	//根据可选PE头中SizeOfImage属性的值向堆申请足够的内存空间
	*(pImageBuffer) = malloc(pOptionHeader->SizeOfImage);
	if ((*pImageBuffer) == NULL) {
		printf("内存空间申请失败！\n");
		return 0;
	}
	//将申请到的内存空间初始化为0
	memset(*(pImageBuffer),0,pOptionHeader->SizeOfImage);
	
	//根据可选PE头中的SizeOfHeaders属性的值，将FileBuffer中的 所有头+节表的数据 复制到ImageBuffer之中
	//srcHeader指针指向FileBuffer的首地址
	char* srcHeader = (char*)pFileBuffer;
	//desHeader指针指向ImageBuffer的首地址
	char* desHeader = (char*)(*(pImageBuffer));
	//根据SizeOfHeaders一字节一字节的将所有头数据和节表数据复制ImageBuffer中
	for (DWORD i=0;i<pOptionHeader->SizeOfHeaders;i++) {
		*(desHeader+i) = *(srcHeader+i);
	}
	
	//根据标准PE头的NumberOfSection属性的值获取节表的数量
	DWORD sectionNum = (DWORD)pPEHeader->NumberOfSections;
	//为避免pSectionHeader指针在后续进行数据操作时发生丢失，在此声明一个专门用于操作节表的pSectionHeaderBak指针
	PIMAGE_SECTION_HEADER pSectionHeaderBak = NULL;
	//用于记录节表中PointerToRawData属性的值（即：节在FileBuffer中的偏移地址值）
	DWORD srcSection = 0;
	//用于记录节表中VirtualAddress属性的值（即：节在ImageBuffer中的偏移地址值）
	DWORD desSection = 0;
	//用于指向节在FileBuffer中的首地址（注意：不是偏移地址了）
	char* pSrcSection = NULL;
	//用于指向节的ImageBuffer中的首地址（注意：不是偏移地址了）
	char* pDesSection = NULL;
	//用于记录节表中Misc.VirtualSize或SizeOfRawData属性的最小值（即：节表所对应节的数据大小）
	DWORD sizeSection = 0;
	//将节的数据从FileBuffer复制到ImageBuffer中对应的位置上
	for (DWORD k=0;k<sectionNum;k++) {
		//令pSectionHeaderBak结构体指针指向第k+1个节表的首地址
		pSectionHeaderBak = pSectionHeader + k;
		//记录第k+1个节表中的PointerToRawData属性值（即第k+1个节表所对应的节在FileBuffer中的偏移地址值）
		srcSection = pSectionHeaderBak->PointerToRawData;
		//记录第k+1个节表中的VirtualAddress属性值（即第k+1个节表所对应的节在ImageBuffer中的偏移地址值）
		desSection = pSectionHeaderBak->VirtualAddress;
		//令pSrcSection指针指向第k+1个节表所对应的节在FileBuffer中的首地址（注意：不是偏移地址）
		pSrcSection = (char*)((DWORD)pFileBuffer+srcSection);
		//令pDesSection指针指向第k+1个节表所对应的节在FileBuffer中的首地址（注意：不是偏移地址）
		pDesSection = (char*)((DWORD)(*(pImageBuffer))+desSection);
		//记录第k+1个节表所对应的节的数据大小
		sizeSection = (pSectionHeaderBak->Misc.VirtualSize)>(pSectionHeaderBak->SizeOfRawData)?(pSectionHeaderBak->SizeOfRawData):(pSectionHeaderBak->Misc.VirtualSize);
		//将第k+1个节表所对应的节数据从FileBuffer中复制到ImageBuffer中
		for (DWORD j=0;j<sizeSection;j++) {
			*(pDesSection+j) = *(pSrcSection+j);
		}
	}
	//复制成功返回ImageBuffer的大小
	return pOptionHeader->SizeOfImage;	
}