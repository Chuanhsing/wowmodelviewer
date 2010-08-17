/*****************************************************************************/
/* SFileOpenArchive.cpp                       Copyright Ladislav Zezula 1999 */
/*                                                                           */
/* Author : Ladislav Zezula                                                  */
/* E-mail : ladik@zezula.net                                                 */
/* WWW    : www.zezula.net                                                   */
/*---------------------------------------------------------------------------*/
/*                       Archive functions of Storm.dll                      */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* xx.xx.xx  1.00  Lad  The first version of SFileOpenArchive.cpp            */
/* 19.11.03  1.01  Dan  Big endian handling                                  */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "SCommon.h"

/*****************************************************************************/
/* Local functions                                                           */
/*****************************************************************************/

#ifdef __STORMLIB_TEST__

#include <vector>
#include <iterator>

using namespace std;

typedef struct _INT128
{
    DWORD dw1;
    DWORD dw2;
    DWORD dw3;
    DWORD dw4;
} INT128, *PINT128;

class BIT_ARRAY
{
    public:

    BIT_ARRAY(DWORD dwSizeInBytes, BYTE bFillPattern);

    void LoadBits(unsigned int nBitPosition,
                  unsigned int nBitLength,
                  void * pbBuffer,
                  int nResultSize);

    LPBYTE pbElements;
    DWORD NumberOfBits;
};

class PARSED_HET_DATA
{
    public:

    PARSED_HET_DATA(DWORD BetBlockOffsets, DWORD dwOpenFlags);

    void CalculateMasks(DWORD arg_0);

    DWORD field_0;
    DWORD field_4;
    DWORD field_8;
    LPBYTE field_C;
    BIT_ARRAY * field_10;
    DWORD field_14;
    DWORD field_18;
    DWORD dwNumberOfBits;
    LARGE_INTEGER AndMask64;
    LARGE_INTEGER OrMask64;
};


class PARSED_BET_DATA
{
    public:

    PARSED_BET_DATA(DWORD BetBlockOffsets, DWORD dwOpenFlags);

    vector<int> field_0;
    DWORD field_18;
    DWORD field_1C;
    DWORD field_20;
    DWORD field_24;
    DWORD field_28;
    DWORD field_2C;
    DWORD field_30;
    DWORD field_34;
    DWORD field_38;
    DWORD field_3C;
    DWORD field_40;
    DWORD field_44;
    DWORD field_48;
    DWORD field_4C;
    BIT_ARRAY * field_50;
    BIT_ARRAY * field_54;
    DWORD field_58;
    vector<INT64>  field_5C;
    vector<DWORD>  field_74;
    vector<INT128> field_8C;
    vector<BYTE>   field_A4;
    DWORD dwOpenFlags;

};

typedef struct _MPQ_FILE_STRUCT_3A
{
    DWORD field_0;
    DWORD field_4;
    DWORD field_8;
    DWORD field_C;
    DWORD field_10;
    DWORD field_14;
    DWORD field_18;
    DWORD result64_lo;
    DWORD result64_hi;
    DWORD result32;
    DWORD result128_1;
    DWORD result128_2;
    DWORD result128_3;
    DWORD result128_4;
    USHORT field_38;

} MPQ_FILE_STRUCT_3A, *PMPQ_FILE_STRUCT_3A;



BIT_ARRAY::BIT_ARRAY(DWORD aNumberOfBits, BYTE bFillPattern)
{
    pbElements = ALLOCMEM(BYTE, (aNumberOfBits + 7) / 8);
    
    memset(pbElements, bFillPattern, (aNumberOfBits + 7) / 8);
    NumberOfBits = aNumberOfBits;
}

void BIT_ARRAY::LoadBits(
    unsigned int nBitPosition,
    unsigned int nBitLength,
    void * pvBuffer,
    int /* nResultByteSize */)
{
    unsigned char * pbBuffer;
    unsigned int nNextBytePosition;                     // EBX
    unsigned int nBytePosition = (nBitPosition / 8);    // ESI
    unsigned int nByteLength = (nBitLength / 8);        // ECX
    unsigned int nBitOffset = (nBitPosition & 0x07);    // AL
    unsigned char BitBuffer;

    // Adjust the buffer pointer
    pbBuffer = (unsigned char *)pvBuffer - nBytePosition;
    nNextBytePosition = nBytePosition + 1;

    // Copy whole bytes, if any
    while(nByteLength > 0)
    {
        if(nBitOffset != 0)
        {
            BitBuffer = (unsigned char)((pbElements[nBytePosition] >> nBitOffset) | (pbElements[nNextBytePosition] << (0x08 - nBitOffset)));
        }
        else
        {
            BitBuffer = pbElements[nBytePosition];
        }

        pbBuffer[nBytePosition++] = BitBuffer;
        nNextBytePosition++;
        nByteLength--;
    }

    // Get the rest of the bits
    nBitLength = (nBitLength & 0x07);
    if(nBitLength != 0)
    {
        BitBuffer = pbElements[nBytePosition];
        pbBuffer[nBytePosition] = (unsigned char)(BitBuffer >> nBitOffset);

        if((8 - nBitOffset) < nBitLength)
            pbBuffer[nBytePosition] = (unsigned char)((pbElements[nNextBytePosition] << (8 - nBitOffset)) | (pbElements[nBytePosition] >> nBitOffset));

        pbBuffer[nBytePosition] &= (0x01 << nBitLength) - 1;
    }
}


PARSED_HET_DATA::PARSED_HET_DATA(DWORD arg_0, DWORD dwBitCount)
{
    LARGE_INTEGER MulResult;
    
    field_0 = 0;
    field_4 = 0;
    field_8 = 0;
    field_C = NULL;
    field_10 = NULL;
    field_14 = arg_0;

    MulResult.QuadPart = 0xAAAAAAAB4 * arg_0;
    field_18 = MulResult.HighPart / 8;

    // Calculate number of bits that is needed for arg_0
    while(arg_0 > 0)
    {
        arg_0 >>= 1;
        field_8++;
    }

    field_0 = field_8;
    field_C = ALLOCMEM(BYTE, field_18);
    CalculateMasks(dwBitCount);
}

void PARSED_HET_DATA::CalculateMasks(DWORD dwBitCount)
{
    dwNumberOfBits = dwBitCount;
    if(dwNumberOfBits == 0x40)
    {
        AndMask64.QuadPart = 0;
    }
    else
    {
        AndMask64.QuadPart = (LONGLONG)1 << dwNumberOfBits;
    }

    AndMask64.QuadPart--;
    OrMask64.QuadPart = (LONGLONG)1 << (dwNumberOfBits - 1);
}

PARSED_BET_DATA::PARSED_BET_DATA(DWORD BetBlockOffsets, DWORD dwOpenFlags)
{
    field_18 = 0;
    field_1C = 0;
    field_20 = 0;
    field_24 = 0;
    field_28 = 0;
    field_2C = 0;
    field_30 = 0;
    field_34 = 0;
    field_38 = 0;
    field_3C = 0;
    field_40 = 0;
    field_44 = 0;
    field_48 = 0;
    field_4C = 0;
    field_50 = NULL;
    field_54 = NULL;
    field_58 = BetBlockOffsets;
    dwOpenFlags = dwOpenFlags;

    if(dwOpenFlags & 0x80)
        field_74.resize(field_58);

    if(dwOpenFlags & 0x40)
        field_8C.resize(field_58);

    if(dwOpenFlags & 0x100)
        field_5C.resize(field_58);

    if(dwOpenFlags & 0x1000)
        field_A4.resize((field_58 + 7) >> 0x03);
}

static PARSED_HET_DATA * ParseHetBlock(LPVOID pvHetBlockData, DWORD cbHetBlock)
{
    PARSED_HET_DATA * pParsedHetData = NULL;
    LPBYTE pbSrcData = (LPBYTE)pvHetBlockData + 0x20;
    DWORD HetBlockOffsets[0x08];

    if(pvHetBlockData == NULL || cbHetBlock >= 0x20)
    {
        memcpy(HetBlockOffsets, pvHetBlockData, sizeof(HetBlockOffsets));
        BSWAP_ARRAY32_UNSIGNED(HetBlockOffsets, sizeof(HetBlockOffsets));
        if(HetBlockOffsets[0] > cbHetBlock)
            return NULL;

        pParsedHetData = new PARSED_HET_DATA(HetBlockOffsets[0x01], HetBlockOffsets[0x03]);
        if(pParsedHetData != NULL)
        {
            memcpy(&pParsedHetData->field_0, &HetBlockOffsets[0x04], 0x0C);
            
            memcpy(pParsedHetData->field_C, pbSrcData, HetBlockOffsets[0x02]);
            pbSrcData += HetBlockOffsets[0x02];

            pParsedHetData->field_10 = new BIT_ARRAY(pParsedHetData->field_0 * pParsedHetData->field_18, 0xFF);
            memcpy(pParsedHetData->field_10->pbElements, pbSrcData, HetBlockOffsets[0x07]);
        }
    }

    return pParsedHetData;
}

static PARSED_BET_DATA * ParseBetBlock(LPVOID pvBetBlockData, DWORD cbBetBlock, DWORD dwOpenFlags)
{
    PARSED_BET_DATA * pParsedBetData = NULL;
    LPBYTE pbSrcData = (LPBYTE)pvBetBlockData + 0x4C;
    DWORD BetBlockOffsets[0x13];
    DWORD LengthInBytes;

    if(pvBetBlockData != NULL && cbBetBlock >= 0x4C)
    {
        // Copy the numbers
        memcpy(BetBlockOffsets, pvBetBlockData, sizeof(BetBlockOffsets));
        BSWAP_ARRAY32_UNSIGNED(BetBlockOffsets, sizeof(BetBlockOffsets));
        if(BetBlockOffsets[0] > cbBetBlock)
            return NULL;

        // Operator new
        pParsedBetData = new PARSED_BET_DATA(BetBlockOffsets[1], dwOpenFlags);
        memcpy(&pParsedBetData->field_18, &BetBlockOffsets[0x03], 0x2C);

        pParsedBetData->field_0.resize(BetBlockOffsets[0x12]);
        if(BetBlockOffsets[0x12] > 0)
        {
            PDWORD pdwBetData10 = (PDWORD)pbSrcData;

            for(DWORD i = 0; i < BetBlockOffsets[0x12]; i++)
            {
                pParsedBetData->field_0[i] = *pdwBetData10++;
            }

            pbSrcData = (LPBYTE)pdwBetData10;
        }

        pParsedBetData->field_54 = new BIT_ARRAY(pParsedBetData->field_18 * pParsedBetData->field_58, 0);
        LengthInBytes = pParsedBetData->field_54 ? (pParsedBetData->field_54->NumberOfBits + 7) / 8 : 0;
        memcpy(pParsedBetData->field_54->pbElements, pbSrcData, LengthInBytes);
        pbSrcData += LengthInBytes;

        memcpy(&pParsedBetData->field_44, &BetBlockOffsets[0x0E], 0x0C);
        
        pParsedBetData->field_50 = new BIT_ARRAY(pParsedBetData->field_44 * pParsedBetData->field_58, 0);
        LengthInBytes = pParsedBetData->field_50 ? (pParsedBetData->field_50->NumberOfBits + 7) / 8 : 0;
        memcpy(pParsedBetData->field_50->pbElements, pbSrcData, LengthInBytes);
        pbSrcData += LengthInBytes;
    }

    return pParsedBetData;
}

//-----------------------------------------------------------------------------
// 

extern "C" void sub_6CAB90(const char * szFileName, size_t nLength, PDWORD pdwValue1, PDWORD pdwValue2);
extern "C" ULONGLONG _stdcall sub_417540(DWORD, DWORD, DWORD, DWORD);

extern "C" ULONGLONG wow_HashStringJenkins(const char * szFileName);

/*
static void sub_6CAB90(
    const char * szLocFileName,
    size_t cchLength,
    PDWORD pdwValue2,
    PDWORD pdwValue1)
{
    DWORD dwValue1 = *pdwValue1;    // ECX
    DWORD dwValue2;

    ESI = ECX = EDI = dwValue1 + cchLength + 0x21524111;
    EDI += *pdwValue2;





}
*/

/*
static LONGLONG sub_8F8FC0(LONGLONG MaskedHash64, DWORD field_18, DWORD arg_C)
{
    if(arg_C == 0)
        return MaskedValue64 / field_18;


}
*/

extern "C" DWORD wow_CalculateExtBlockIndex(
    PARSED_HET_DATA * pParsedHetData, 
    PARSED_BET_DATA * pParsedBetData,
    PULARGE_INTEGER pFileNameHash);

static DWORD CalculateExtBlockIndex(
    PARSED_HET_DATA * pParsedHetData, 
    PARSED_BET_DATA * pParsedBetData,
    PULARGE_INTEGER pFileNameHash)
{
    ULONGLONG MaskedHash64;
    ULONGLONG AndMask64 = pParsedHetData->AndMask64.QuadPart;
    ULONGLONG OrMask64 = pParsedHetData->OrMask64.QuadPart;
    ULONGLONG SearchValue_2;
    DWORD StartIndex;
    DWORD CurrIndex;
    BYTE SearchValue_1;

    // Change the file name hash, so it only contains required
    // number of bits, and always has the highest bit set
    MaskedHash64 = (pFileNameHash->QuadPart & AndMask64) | OrMask64;
    
    // Calculate the starting index to the field_C table
    CurrIndex = StartIndex = (DWORD)(MaskedHash64 % pParsedHetData->field_18);

    // Highest 8 bits of the masked name hash is the primary identification value
    SearchValue_1 = (BYTE)(MaskedHash64 >> (pParsedHetData->dwNumberOfBits - 8));

    // Remaining lower bits are the secondary identification value
    SearchValue_2 = MaskedHash64 & (AndMask64 >> 0x08);

    // Go through hash table until we find a terminator
    while(pParsedHetData->field_C[CurrIndex] != 0)
    {
        // Did we find match ?
        if(pParsedHetData->field_C[CurrIndex] == SearchValue_1)
        {
            ULONGLONG Result64 = 0;
            DWORD dwExtBlockIndex = 0;

            // field_10 contains array of indexes to the BET table
            pParsedHetData->field_10->LoadBits(pParsedHetData->field_0 * CurrIndex, // BitPosition
                                               pParsedHetData->field_8,             // BitLength
                                              &dwExtBlockIndex,                     // Target pointer
                                               4);

            // field_50 is a "confirmation" value to the BET table index found
            pParsedBetData->field_50->LoadBits(pParsedBetData->field_44 * dwExtBlockIndex, // BitPosition
                                               pParsedBetData->field_4C,            // BitLength
                                              &Result64,                            // Target pointer
                                               8);
            if(Result64 == SearchValue_2)
                return dwExtBlockIndex;
        }

        // Move to the next entry in the primary search table
        CurrIndex = (CurrIndex + 1) % pParsedHetData->field_18;
        
        // If we came to the start index again, we are done
        if(CurrIndex == StartIndex)
            break;
    }

    return 0xFFFFFFFF;
}

static bool sub_6D0B60(
    PARSED_BET_DATA * pParsedBetData,
    DWORD dwExtBlockIndex,
    PMPQ_FILE_STRUCT_3A pResult)
{
    if(pParsedBetData->field_54 == 0)
        return false;

    memset(pResult, 0, sizeof(MPQ_FILE_STRUCT_3A));

    pParsedBetData->field_54->LoadBits(pParsedBetData->field_1C + pParsedBetData->field_18 * dwExtBlockIndex,
                                       pParsedBetData->field_30,
                                      &pResult->field_0,
                                       8);

    pParsedBetData->field_54->LoadBits(pParsedBetData->field_20 + pParsedBetData->field_18 * dwExtBlockIndex,
                                       pParsedBetData->field_34,
                                      &pResult->field_10,
                                       8);

    pParsedBetData->field_54->LoadBits(pParsedBetData->field_24 + pParsedBetData->field_18 * dwExtBlockIndex,
                                       pParsedBetData->field_38,
                                      &pResult->field_8,
                                       8);

    if(pParsedBetData->field_0.size() == 0)
        return false;

    if(pParsedBetData->field_0.size() != 1)
    {
        pParsedBetData->field_54->LoadBits(pParsedBetData->field_24 + pParsedBetData->field_18 * dwExtBlockIndex,
                                           pParsedBetData->field_3C,
                                          &pResult->field_18,
                                           4);
        
        if(pParsedBetData->field_0.size() > pResult->field_18)
            return false;
        pResult->field_18 = pParsedBetData->field_0[pResult->field_18];
    }
    else
    {
        pResult->field_18 = pParsedBetData->field_0[0];
    }

    if(pParsedBetData->dwOpenFlags & 0x200)
    {
        BYTE OneByte = 0;

        pParsedBetData->field_54->LoadBits(pParsedBetData->field_24 + pParsedBetData->field_18 * dwExtBlockIndex,
                                           pParsedBetData->field_40,
                                          &OneByte,
                                           1);
        pResult->field_38 = (OneByte > 0) ? 1 : 0;
    }

    return true;
}

static void sub_6D1100(TMPQArchive * ha, PARSED_BET_DATA * pParsedBetData, PMPQ_FILE_STRUCT_3A pResult, DWORD dwExtBlockIndex)
{
    MPQ_FILE_STRUCT_3A result;    
    LARGE_INTEGER temp_li;
    LPBYTE pointer_to_struct_3A;
    BYTE byte_A9E170[] = {0x80, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

    DWORD dwOpenFlags = 0;


    if(pParsedBetData != NULL)
    {
        sub_6D0B60(pParsedBetData, dwExtBlockIndex, &result);

        if(dwOpenFlags & 0x80)
        {
            result.result32 = pParsedBetData->field_74[dwExtBlockIndex];
        }

        if(dwOpenFlags & 0x40)
        {
            INT128 result128 = pParsedBetData->field_8C[dwExtBlockIndex];

            result.result128_1 = result128.dw1;
            result.result128_2 = result128.dw2;
            result.result128_3 = result128.dw3;
            result.result128_4 = result128.dw4;
        }

        if(dwOpenFlags & 0x100)
        {         
            INT64 result64 = pParsedBetData->field_5C[dwExtBlockIndex];

            result.result64_hi = (DWORD)(result64 >> 0x20);
            result.result64_lo = (DWORD)(result64 & 0xFFFFFFFF);
        }

        if(dwOpenFlags & 0x1000)
        {
            BYTE BitMask = byte_A9E170[dwExtBlockIndex & 0x07];

            result.field_38 = (pParsedBetData->field_A4[dwExtBlockIndex / 8] & BitMask) ? 0x100 : 0;
        }

        // 0x3A bytes !!!
        memcpy(pResult, &result, sizeof(MPQ_FILE_STRUCT_3A));
    }
    else
    {
/*
        // pArchive->field_26C is apparently a vector of MPQ_FILE_STRUCT_3A
        temp_li.QuadPart = (ULONGLONG)0x8D3DCB09 * (pArchive->field_270 - pArchive->field_26C);
        temp_li.HighPart = (temp_li.HighPart + (pArchive->field_270 - pArchive->field_26C)) >> 0x05;
        eax = (temp_li.HighPart >> 0x05) + temp_li.HighPart;
        assert(dwExtBlockIndex < eax);

        memcpy(pResult, &pArchive->field_26C[dwExtBlockIndex], sizeof(MPQ_FILE_STRUCT_3A));
*/
    }
}

#endif

static bool IsAviFile(TMPQHeader * pHeader)
{
    DWORD * AviHdr = (DWORD *)pHeader;

#ifdef PLATFORM_LITTLE_ENDIAN
    // Test for 'RIFF', 'AVI ' or 'LIST'
    return (AviHdr[0] == 0x46464952 && AviHdr[2] == 0x20495641 && AviHdr[3] == 0x5453494C);
#else
    return (AviHdr[0] == 0x52494646 && AviHdr[2] == 0x41564920 && AviHdr[3] == 0x4C495354);
#endif
}

// This function gets the right positions of the hash table and the block table.
static void RelocateMpqTablePositions(TMPQArchive * ha)
{
    TMPQHeader * pHeader = ha->pHeader;

    // Calculate the file position of the HET block
    if(pHeader->dwHETBlockPosHigh || pHeader->dwHETBlockPosLow)
    {
        ha->HETBlockPos.HighPart = pHeader->dwHETBlockPosHigh;
        ha->HETBlockPos.LowPart = pHeader->dwHETBlockPosLow;
        ha->HETBlockPos.QuadPart += ha->MpqPos.QuadPart;
    }

    // Calculate the file position of the BET block
    if(pHeader->dwBETBlockPosHigh || pHeader->dwBETBlockPosLow)
    {
        ha->BETBlockPos.HighPart = pHeader->dwBETBlockPosHigh;
        ha->BETBlockPos.LowPart = pHeader->dwBETBlockPosLow;
        ha->BETBlockPos.QuadPart += ha->MpqPos.QuadPart;
    }

    // Calculate the file position of the hash table
    ha->HashTablePos.HighPart = pHeader->wHashTablePosHigh;
    ha->HashTablePos.LowPart = pHeader->dwHashTablePos;
    ha->HashTablePos.QuadPart += ha->MpqPos.QuadPart;

    // Calculate the file position of the block table
    ha->BlockTablePos.HighPart = pHeader->wBlockTablePosHigh;
    ha->BlockTablePos.LowPart = pHeader->dwBlockTablePos;
    ha->BlockTablePos.QuadPart += ha->MpqPos.QuadPart;

    // Calculate the file position of the extended block table
    if(pHeader->dwExtBlockTablePosHigh || pHeader->dwExtBlockTablePosLow)
    {
        ha->ExtBlockTablePos.HighPart = pHeader->dwExtBlockTablePosHigh;
        ha->ExtBlockTablePos.LowPart = pHeader->dwExtBlockTablePosLow;
        ha->ExtBlockTablePos.QuadPart += ha->MpqPos.QuadPart;
    }

    // Calculate the size of the archive from MPQ header to the end of the archive
    if(pHeader->dwHeaderSize >= MPQ_HEADER_SIZE_V2 + sizeof(DWORD) * 2)
    {
        ha->ArchiveSize.HighPart = pHeader->dwArchiveSizeHigh;
        ha->ArchiveSize.LowPart = pHeader->dwArchiveSizeLow;
    }
    else if(pHeader->dwExtBlockTablePosHigh || pHeader->dwExtBlockTablePosLow)
    {
        ha->ArchiveSize.HighPart = pHeader->dwExtBlockTablePosHigh;
        ha->ArchiveSize.LowPart = pHeader->dwExtBlockTablePosLow;
        ha->ArchiveSize.QuadPart += (pHeader->dwBlockTableSize * sizeof(TMPQBlockEx));
    }
    else
    {
        ha->ArchiveSize.HighPart = 0;
        ha->ArchiveSize.LowPart = pHeader->dwArchiveSize;
    }
}


/*****************************************************************************/
/* Public functions                                                          */
/*****************************************************************************/

//-----------------------------------------------------------------------------
// SFileGetLocale and SFileSetLocale
// Set the locale for all neewly opened archives and files

LCID WINAPI SFileGetLocale()
{
    return lcFileLocale;
}

LCID WINAPI SFileSetLocale(LCID lcNewLocale)
{
    lcFileLocale = lcNewLocale;
    return lcFileLocale;
}

//-----------------------------------------------------------------------------
// SFileOpenArchive
//
//   szFileName - MPQ archive file name to open
//   dwPriority - When SFileOpenFileEx called, this contains the search priority for searched archives
//   dwFlags    - See MPQ_OPEN_XXX in StormLib.h
//   phMpq      - Pointer to store open archive handle

bool WINAPI SFileOpenArchive(
    const char * szMpqName,
    DWORD dwPriority,
    DWORD dwFlags,
    HANDLE * phMpq)
{
    LARGE_INTEGER FileSize = {0};
    TFileStream * pStream = NULL;       // Open file stream
    TMPQArchive * ha = NULL;            // Archive handle
    DWORD dwCompressedTableSize = 0;    // Size of compressed block/hash table
    DWORD dwTableSize = 0;              // Size of block/hash table
    DWORD dwBytes = 0;                  // Number of bytes to read
    bool bMpqHeaderTrimmed = false;
    int nError = ERROR_SUCCESS;   

    // Verify the parameters
    if(szMpqName == NULL || *szMpqName == 0 || phMpq == NULL)
        nError = ERROR_INVALID_PARAMETER;

    // One time initialization of MPQ cryptography
    InitializeMpqCryptography();
    dwPriority = dwPriority;

    // Open the MPQ archive file
    if(nError == ERROR_SUCCESS)
    {
        if(!(dwFlags & MPQ_OPEN_ENCRYPTED))
        {
            pStream = FileStream_OpenFile(szMpqName, (dwFlags & MPQ_OPEN_READ_ONLY) ? false : true);
            if(pStream == NULL)
                nError = GetLastError();
        }
        else
        {
            pStream = FileStream_OpenEncrypted(szMpqName);
            if(pStream == NULL)
                nError = GetLastError();
        }
    }
    
    // Allocate the MPQhandle
    if(nError == ERROR_SUCCESS)
    {
        FileStream_GetSize(pStream, &FileSize);
        if((ha = ALLOCMEM(TMPQArchive, 1)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Initialize handle structure and allocate structure for MPQ header
    if(nError == ERROR_SUCCESS)
    {
        memset(ha, 0, sizeof(TMPQArchive));
        ha->pStream    = pStream;
        ha->pHeader    = (TMPQHeader *)ha->HeaderData;
        ha->pListFile  = NULL;
        pStream = NULL;

        // Remember if the archive is open for write
        if(ha->pStream->StreamFlags & (STREAM_FLAG_READ_ONLY | STREAM_FLAG_ENCRYPTED_FILE))
            ha->dwFlags |= MPQ_FLAG_READ_ONLY;

        // Also remember if we shall check sector CRCs when reading file
        if(dwFlags & MPQ_OPEN_CHECK_SECTOR_CRC)
            ha->dwFlags |= MPQ_FLAG_CHECK_SECTOR_CRC;
    }

    // Find the offset of MPQ header within the file
    if(nError == ERROR_SUCCESS)
    {
        LARGE_INTEGER SearchPos = {0};
        DWORD dwHeaderID;

        for(;;)
        {
            // Read the eventual MPQ header
            if(!FileStream_Read(ha->pStream, &SearchPos, ha->HeaderData, MPQ_HEADER_SIZE_V1))
            {
                nError = GetLastError();
                break;
            }

            // Special check : Some MPQs are actually AVI files, only with
            // changed extension.
            if(SearchPos.QuadPart == 0 && IsAviFile(ha->pHeader))
            {
                nError = ERROR_AVI_FILE;
                break;
            }

            // If there is the MPQ user data signature, process it
            dwHeaderID = BSWAP_INT32_UNSIGNED(ha->pHeader->dwID);
            if(dwHeaderID == ID_MPQ_USERDATA && ha->pUserData == NULL)
            {
                // Ignore the MPQ user data completely if the caller wants to open the MPQ as V1.0
                if((dwFlags & MPQ_OPEN_FORCE_MPQ_V1) == 0)
                {
                    // Fill the user data header
                    ha->pUserData = &ha->UserData;
                    memcpy(ha->pUserData, ha->pHeader, sizeof(TMPQUserData));
                    BSWAP_TMPQUSERDATA(ha->pUserData);

                    // Remember the position of the user data and continue search
                    ha->UserDataPos.QuadPart = SearchPos.QuadPart;
                    SearchPos.QuadPart += ha->pUserData->dwHeaderOffs;
                    continue;
                }
            }

            // There must be MPQ header signature
            if(dwHeaderID == ID_MPQ)
            {
                DWORD  dwHeaderSize;
                USHORT wFormatVersion;
                bool   bFormatOK = false;

                // Save the position where the MPQ header has been found
                if(ha->pUserData == NULL)
                    ha->UserDataPos = SearchPos;
                ha->MpqPos = SearchPos;

                // If valid header version 1 has been found, break the loop
                wFormatVersion = BSWAP_INT16_UNSIGNED(ha->pHeader->wFormatVersion);
                dwHeaderSize = BSWAP_INT32_UNSIGNED(ha->pHeader->dwHeaderSize);

                // Check for MPQ format 1.0
                switch(wFormatVersion)
                {
                    case MPQ_FORMAT_VERSION_1:

                        if(dwHeaderSize != MPQ_HEADER_SIZE_V1)
                        {
                            dwHeaderSize = MPQ_HEADER_SIZE_V1;
                            ha->dwFlags |= MPQ_FLAG_PROTECTED;
                        }
                        bFormatOK = true;
                        break;

                    case MPQ_FORMAT_VERSION_2:

                        if(dwHeaderSize == MPQ_HEADER_SIZE_V2)
                            bFormatOK = true;
                        break;

                    case MPQ_FORMAT_VERSION_3:
                        
                        // For MPQ format version 3, it either has to be 0x2C or 0x44
                        if(dwHeaderSize == MPQ_HEADER_SIZE_V2 || dwHeaderSize == MPQ_HEADER_SIZE_V3)
                            bFormatOK = true;
                        break;

                    case MPQ_FORMAT_VERSION_4:
                        if(dwHeaderSize >= MPQ_HEADER_SIZE_V4)
                            bFormatOK = true;
                        break;
                }

                // If the format is OK, read the rest (if needed), BSWAP it and go on.
                if(bFormatOK)
                {
                    // Read the rest of the file header, if needed
                    if(dwHeaderSize > MPQ_HEADER_SIZE_V1)
                    {
                        if(!FileStream_Read(ha->pStream, &SearchPos, ha->HeaderData, dwHeaderSize))
                            nError = GetLastError();
                    }

                    // Swap the header and fix the header size
                    BSWAP_TMPQHEADER(ha->pHeader);
                    ha->pHeader->dwHeaderSize = dwHeaderSize;
					break;
                }

				//
				// Note: the "dwArchiveSize" member in the MPQ header is ignored by Storm.dll
				// and can contain garbage value ("w3xmaster" protector)
				// 
                
                nError = ERROR_NOT_SUPPORTED;
                break;
            }

            // Move to the next possible offset
            SearchPos.QuadPart += 0x200;
        }
    }

    // Fix table positions according to format
    if(nError == ERROR_SUCCESS)
    {
        // W3x Map Protectors use the fact that War3's Storm.dll ignores the MPQ user data,
        // and probably ignores the MPQ format version as well. The trick is to
        // fake MPQ format 2, with an improper hi-word position of hash table and block table
        // We can overcome such protectors by forcing opening the archive as MPQ v 1.0
        if(dwFlags & MPQ_OPEN_FORCE_MPQ_V1)
        {
            ha->pHeader->wFormatVersion = MPQ_FORMAT_VERSION_1;
            ha->pHeader->dwHeaderSize = MPQ_HEADER_SIZE_V1;
            ha->dwFlags |= MPQ_FLAG_READ_ONLY;
            ha->pUserData = NULL;
        }

        // Both MPQ_OPEN_NO_LISTFILE or MPQ_OPEN_NO_ATTRIBUTES trigger read only mode
        if(dwFlags & (MPQ_OPEN_NO_LISTFILE | MPQ_OPEN_NO_ATTRIBUTES))
            ha->dwFlags |= MPQ_FLAG_READ_ONLY;

        // If we had to trim the MPQ header, force read-only access
        if(bMpqHeaderTrimmed)
            ha->dwFlags |= MPQ_FLAG_READ_ONLY;

        // Clear the rest of the header
        if(ha->pHeader->dwHeaderSize < sizeof(ha->HeaderData))
        {
            LPBYTE pbHeader = (LPBYTE)ha->pHeader;
            size_t nLength = sizeof(ha->HeaderData) - ha->pHeader->dwHeaderSize;

            memset(pbHeader + ha->pHeader->dwHeaderSize, 0, nLength);
        }

        ha->dwSectorSize = (0x200 << ha->pHeader->wSectorSize);
        RelocateMpqTablePositions(ha);

        // Verify if neither table is outside the file
        if(ha->HETBlockPos.QuadPart > FileSize.QuadPart)
            nError = ERROR_BAD_FORMAT;
        if(ha->BETBlockPos.QuadPart > FileSize.QuadPart)
            nError = ERROR_BAD_FORMAT;
        if(ha->HashTablePos.QuadPart > FileSize.QuadPart)
            nError = ERROR_BAD_FORMAT;
        if(ha->BlockTablePos.QuadPart > FileSize.QuadPart)
            nError = ERROR_BAD_FORMAT;
        if(ha->ExtBlockTablePos.QuadPart > FileSize.QuadPart)
            nError = ERROR_BAD_FORMAT;
    }

    // Allocate buffers
    if(nError == ERROR_SUCCESS)
    {
        DWORD dwBlockTableSize = ha->pHeader->dwBlockTableSize;
        DWORD dwHashTableSize = ha->pHeader->dwHashTableSize;

        //
        // Note: It is allowed that either of the tables sizes is zero.
        // If this is the case, we increase the table size to 1
        // to prevent unpredictable results from allocating 0 bytes of memory
        // 

        if(dwHashTableSize == 0)
            dwHashTableSize++;
        if(dwBlockTableSize == 0)
            dwBlockTableSize++;

        //
        // Note that the block table should be at least as large 
        // as the hash table (for later file additions).
        //
        // Note that the block table *can* be bigger than the hash table.
        // We have to deal with this properly.
        //
        
        ha->dwBlockTableMax = STORMLIB_MAX(dwHashTableSize, dwBlockTableSize);
        
        // Allocate all three MPQ tables
        ha->pHashTable     = ALLOCMEM(TMPQHash, dwHashTableSize);
        ha->pBlockTable    = ALLOCMEM(TMPQBlock, ha->dwBlockTableMax);
        ha->pExtBlockTable = ALLOCMEM(TMPQBlockEx, ha->dwBlockTableMax);
        if(!ha->pHashTable || !ha->pBlockTable || !ha->pExtBlockTable)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Read the HET block, if present
    if(nError == ERROR_SUCCESS && ha->HETBlockPos.QuadPart && ha->BETBlockPos.QuadPart > ha->HETBlockPos.QuadPart)
    {
        ha->dwHETBlockSize = (DWORD)(ha->BETBlockPos.QuadPart - ha->HETBlockPos.QuadPart);
        if(ha->dwHETBlockSize != 0)
        {
            nError = LoadXXXBlock(ha, &ha->pHETBlock, ha->HETBlockPos, ha->dwHETBlockSize, "(hash table)");
        }
    }

    // Read the BET block, if present
    if(nError == ERROR_SUCCESS && ha->BETBlockPos.QuadPart != 0 && ha->HashTablePos.QuadPart > ha->BETBlockPos.QuadPart)
    {
        ha->dwBETBlockSize = (DWORD)(ha->HashTablePos.QuadPart - ha->BETBlockPos.QuadPart);
        if(ha->dwBETBlockSize != 0)
        {
            nError = LoadXXXBlock(ha, &ha->pBETBlock, ha->BETBlockPos, ha->dwBETBlockSize, "(block table)");
        }
    }

    // Read the hash table.
    // "interface.MPQ.part" in trial version of World of Warcraft
    // has compressed block table and hash table.
    if(nError == ERROR_SUCCESS)
    {
        dwCompressedTableSize = ha->pHeader->dwHashTableSize * sizeof(TMPQHash);
        dwTableSize = dwCompressedTableSize;

        // If the block table follows the hash table
        // and if position of the block table is lower
        // than uncompressed size of hash table,
        // it means that the hash table is compressed
        if(ha->BlockTablePos.QuadPart > ha->HashTablePos.QuadPart)
        {
            if((ha->HashTablePos.QuadPart + dwTableSize) > ha->BlockTablePos.QuadPart)
            {
                dwCompressedTableSize = (DWORD)(ha->BlockTablePos.QuadPart - ha->HashTablePos.QuadPart);
//              bHashTableCompressed = true;
            }
        }

        //
        // Note: We will not check if the hash table is properly decrypted.
        // Some MPQ protectors corrupt the hash table by rewriting part of it.
        // The way how MPQ tables work allows arbitrary values for unused entries.
        // 

        // Read the hash table into memory
        nError = LoadMpqTable(ha, &ha->HashTablePos, ha->pHashTable, dwCompressedTableSize, dwTableSize, "(hash table)");
    }

    // Now, read the block table
    if(nError == ERROR_SUCCESS)
    {
        dwCompressedTableSize = ha->pHeader->dwBlockTableSize * sizeof(TMPQBlock);
        dwTableSize = dwCompressedTableSize;

        // Pre-zero the entire block table
        memset(ha->pBlockTable, 0, (ha->dwBlockTableMax * sizeof(TMPQBlock)));

        // Check whether block table is compressed
        if(ha->ExtBlockTablePos.QuadPart != 0)
        {
            if(ha->ExtBlockTablePos.QuadPart > ha->BlockTablePos.QuadPart)
            {
                if((ha->BlockTablePos.QuadPart + dwTableSize) > ha->ExtBlockTablePos.QuadPart)
                {
                    dwCompressedTableSize = (DWORD)(ha->ExtBlockTablePos.QuadPart - ha->BlockTablePos.QuadPart);
//                  bBlockTableCompressed = true;
                }
            }
        }
        else
        {
            LARGE_INTEGER ArchiveSize;

            ArchiveSize.QuadPart = ha->ArchiveSize.QuadPart + ha->MpqPos.QuadPart;
            if(ArchiveSize.QuadPart > ha->BlockTablePos.QuadPart)
            {
                dwCompressedTableSize = (DWORD)(ArchiveSize.QuadPart - ha->BlockTablePos.QuadPart);
//              bBlockTableCompressed = true;
            }
        }

        // I have found a MPQ which claimed 0x200 entries in the block table,
        // but the file was cut and there was only 0x1A0 entries.
        // We will handle this case properly.
        if((ha->BlockTablePos.QuadPart + dwCompressedTableSize) > FileSize.QuadPart)
        {
            dwCompressedTableSize = (DWORD)(FileSize.QuadPart - ha->BlockTablePos.QuadPart);
            dwTableSize = dwCompressedTableSize;
        }

        //
        // One of the first cracked versions of Diablo had block table unencrypted 
        // StormLib does NOT supports such MPQs anymore, as they are incompatible
        // with compressed block table feature
        //

        // Read the block table
        nError = LoadMpqTable(ha, &ha->BlockTablePos, ha->pBlockTable, dwCompressedTableSize, dwTableSize, "(block table)");
    }

    // Now, read the extended block table.
    // For V1 archives, we still will maintain the extended block table
    // (it will be filled with zeros)
    if(nError == ERROR_SUCCESS)
    {
        memset(ha->pExtBlockTable, 0, ha->dwBlockTableMax * sizeof(TMPQBlockEx));

        if(ha->pHeader->dwExtBlockTablePosHigh || ha->pHeader->dwExtBlockTablePosLow)
        {
            dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TMPQBlockEx);
            if(!FileStream_Read(ha->pStream, &ha->ExtBlockTablePos, ha->pExtBlockTable, dwBytes))
                nError = GetLastError();

            // We have to convert every USHORT in ext block table from LittleEndian
            BSWAP_ARRAY16_UNSIGNED(ha->pExtBlockTable, dwBytes);
        }
    }

    // Verify both block tables (if the MPQ file is not protected)
    if(nError == ERROR_SUCCESS && (ha->dwFlags & MPQ_FLAG_PROTECTED) == 0)
    {
        LARGE_INTEGER RawFilePos;
        TMPQBlockEx * pBlockEx;
        TMPQBlock * pBlock;
        TMPQHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
        TMPQHash * pHash;

        // Parse the hash table and the block table
        for(pHash = ha->pHashTable; pHash < pHashEnd; pHash++)
        {
            if(pHash->dwBlockIndex < ha->pHeader->dwBlockTableSize)
            {
                // Get both block table entries
                pBlockEx = ha->pExtBlockTable + pHash->dwBlockIndex;
                pBlock = ha->pBlockTable + pHash->dwBlockIndex;

                // If the file exists, check if it doesn't go beyond the end of the file
                if(pBlock->dwFlags & MPQ_FILE_EXISTS)
                {
                    // Get the 64-bit file position
                    RawFilePos.HighPart = pBlockEx->wFilePosHigh;
                    RawFilePos.LowPart = pBlock->dwFilePos;

                    // Begin of the file must be within range
                    RawFilePos.QuadPart += ha->MpqPos.QuadPart;
                    if(RawFilePos.QuadPart > FileSize.QuadPart)
                    {
                        nError = ERROR_FILE_CORRUPT;
                        break;
                    }

                    // End of the file must be within range
                    RawFilePos.QuadPart += pBlock->dwCSize;
                    if(RawFilePos.QuadPart > FileSize.QuadPart)
                    {
                        nError = ERROR_FILE_CORRUPT;
                        break;
                    }
                }
            }
        }
    }

    // If the caller didn't specified otherwise, 
    // include the internal listfile to the TMPQArchive structure
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_OPEN_NO_LISTFILE) == 0)
    {
        // Create listfile and load it from the MPQ
        nError = SListFileCreateListFile(ha);
        if(nError == ERROR_SUCCESS)
            SFileAddListFile((HANDLE)ha, NULL);
    }

    // If the caller didn't specified otherwise, 
    // load the "(attributes)" file
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_OPEN_NO_ATTRIBUTES) == 0)
    {
        // Create the attributes file and load it from the MPQ
        nError = SAttrCreateAttributes(ha, 0);
        if(nError == ERROR_SUCCESS)
            SAttrLoadAttributes(ha);
    }

#ifdef __STORMLIB_TEST__
    if(ha->pHETBlock != NULL && ha->pBETBlock != NULL)
    {
        SFILE_FIND_DATA sf;
        PARSED_HET_DATA * pParsedHetData;
        PARSED_BET_DATA * pParsedBetData;
        HANDLE hFind;
        BOOL bResult = 1;

        pParsedHetData = ParseHetBlock(&ha->pHETBlock->Data, ha->pHETBlock->dwDataSize);
        pParsedBetData = ParseBetBlock(&ha->pBETBlock->Data, ha->pBETBlock->dwDataSize, 0);
        if(pParsedHetData && pParsedBetData)
        {
            hFind = SFileFindFirstFile((HANDLE)ha, "*", &sf, NULL);
            while(hFind && bResult)
            {
                MPQ_FILE_STRUCT_3A Result3A;
                TMPQHash * pHash;
                ULARGE_INTEGER FileNameHash;
                DWORD dwExtBlockIndex;

                FileNameHash.QuadPart = HashStringJenkins(sf.cFileName);
                dwExtBlockIndex = CalculateExtBlockIndex(pParsedHetData, pParsedBetData, &FileNameHash);
                pHash = GetHashEntryAny(ha, sf.cFileName);
//              dwExtBlockIndex = wow_CalculateExtBlockIndex(pParsedHetData, pParsedBetData, &FileNameHash);
                
                if(dwExtBlockIndex != 0xFFFFFFFF)
                {
                    sub_6D1100(ha, pParsedBetData, &Result3A, dwExtBlockIndex);
                }

                bResult = SFileFindNextFile(hFind, &sf);
            }
        }

        delete pParsedBetData;
        delete pParsedHetData;
    }
#endif // __STORMLIB_TEST__

    // Cleanup and exit
    if(nError != ERROR_SUCCESS)
    {
        FileStream_Close(pStream);
        FreeMPQArchive(ha);
        SetLastError(nError);
        ha = NULL;
    }

    *phMpq = ha;
    return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// bool SFileFlushArchive(HANDLE hMpq)
//
// Saves all dirty data into MPQ archive.
// Has similar effect like SFileCLoseArchive, but the archive is not closed.
// Use on clients who keep MPQ archive open even for write operations,
// and terminating without calling SFileCloseArchive might corrupt the archive.
//

bool WINAPI SFileFlushArchive(HANDLE hMpq)
{
    TMPQArchive * ha = (TMPQArchive *)hMpq;
    int nResultError = ERROR_SUCCESS;
    int nError;

    // Do nothing if 'hMpq' is bad parameter
    if(!IsValidMpqHandle(ha))
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return false;
    }

    // If the archive has been changed, update the changes on the disk drive.
    // Save listfile (if created), attributes (if created) and also save MPQ tables.
    if(ha->dwFlags & MPQ_FLAG_CHANGED)
    {
        nError = SListFileSaveToMpq(ha);
        if(nError != ERROR_SUCCESS)
            nResultError = nError;

        nError = SAttrFileSaveToMpq(ha);
        if(nError != ERROR_SUCCESS)
            nResultError = nError;

        nError = SaveMPQTables(ha);
        if(nError != ERROR_SUCCESS)
            nResultError = nError;
    }

    // Return the error
    if(nResultError != ERROR_SUCCESS)
        SetLastError(nResultError);
    return (nResultError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// bool SFileCloseArchive(HANDLE hMpq);
//

bool WINAPI SFileCloseArchive(HANDLE hMpq)
{
    TMPQArchive * ha = (TMPQArchive *)hMpq;
    bool bResult;

    // Flush all unsaved data to the storage
    bResult = SFileFlushArchive(hMpq);

    // Free all memory used by MPQ archive
    FreeMPQArchive(ha);
    return bResult;
}

