/*****************************************************************************/
/* SFilePatchArchives.cpp                 Copyright (c) Ladislav Zezula 2010 */
/*---------------------------------------------------------------------------*/
/* Description:                                                              */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 18.08.10  1.00  Lad  The first version of SFilePatchArchives.cpp          */
/*****************************************************************************/

#define __STORMLIB_SELF__
#define __INCLUDE_CRYPTOGRAPHY__
#include "StormLib.h"
#include "SCommon.h"

//-----------------------------------------------------------------------------
// Local structures

typedef struct _BLIZZARD_BSDIFF40_FILE
{
    ULONGLONG Signature;
    ULONGLONG CtrlBlockSize;
    ULONGLONG DataBlockSize;
    ULONGLONG NewFileSize;
} BLIZZARD_BSDIFF40_FILE, *PBLIZZARD_BSDIFF40_FILE;

//-----------------------------------------------------------------------------
// Local functions

static void Decompress_RLE(LPBYTE pbDecompressed, DWORD cbDecompressed, LPBYTE pbCompressed, DWORD cbCompressed)
{
    LPBYTE pbDecompressedEnd = pbDecompressed + cbDecompressed;
    LPBYTE pbCompressedEnd = pbCompressed + cbCompressed;
    BYTE RepeatCount; 
    BYTE OneByte;

    // Pre-fill decompressed buffer with zeros
    memset(pbDecompressed, 0, cbDecompressed);

    // Unpack
    while(pbCompressed < pbCompressedEnd)
    {
        OneByte = *pbCompressed++;
        
        // Is it a repetition byte ?
        if(OneByte & 0x80)
        {
            RepeatCount = (OneByte & 0x7F) + 1;
            for(BYTE i = 0; i < RepeatCount; i++)
            {
                if(pbDecompressed == pbDecompressedEnd || pbCompressed == pbCompressedEnd)
                    break;

                *pbDecompressed++ = *pbCompressed++;
            }
        }
        else
        {
            pbDecompressed += (OneByte + 1);
        }
    }
}

static int LoadMpqPatchFile(TMPQFile * hf)
{
    TPatchHeader PatchHeader;
    LPBYTE pbCompressed = NULL;
    DWORD cbCompressed = 0;
    DWORD dwBytesRead;
    int nError = ERROR_SUCCESS;

    // Read the patch header
    SFileReadFile((HANDLE)hf, &PatchHeader, sizeof(TPatchHeader), &dwBytesRead);
    if(dwBytesRead != sizeof(TPatchHeader))
        nError = ERROR_FILE_CORRUPT;

    // Verify the signatures in the patch header
    if(nError == ERROR_SUCCESS)
    {
        // BSWAP the entire header, if needed
        BSWAP_ARRAY32_UNSIGNED(&PatchHeader, sizeof(DWORD) * 6);
        BSWAP_ARRAY32_UNSIGNED(&PatchHeader->dwXFRM, sizeof(DWORD) * 4);

        if(PatchHeader.dwSignature != 0x48435450 || PatchHeader.dwXFRM != 0x4d524658 || PatchHeader.dwBSD0 != 0x30445342)
            nError = ERROR_FILE_CORRUPT;
    }

    // Allocate space for patch header and compressed data
    if(nError == ERROR_SUCCESS)
    {
        cbCompressed = PatchHeader.dwXfrmBlockSize - SIZE_OF_XFRM_HEADER;
        pbCompressed = ALLOCMEM(BYTE, cbCompressed);
        hf->pPatchHeader = (TPatchHeader *)ALLOCMEM(BYTE, sizeof(TPatchHeader) + PatchHeader.dwUnpackedSize);
        if(hf->pPatchHeader == NULL || pbCompressed == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Load the patch data and decide if they are compressed or not
    if(nError == ERROR_SUCCESS)
    {
        // Pre-fill the patch with zeros
        memset(hf->pPatchHeader, 0, sizeof(TPatchHeader) + PatchHeader.dwUnpackedSize);

        // Load the rest of the header
        SFileReadFile((HANDLE)hf, pbCompressed, cbCompressed, &dwBytesRead);
        if(dwBytesRead != cbCompressed)
            nError = ERROR_FILE_CORRUPT;
    }

    // Construct the uncompressed block
    if(nError == ERROR_SUCCESS)
    {
        LPBYTE pbDecompressed = (LPBYTE)(hf->pPatchHeader + 1);

        // Copy the patch header
        memcpy(hf->pPatchHeader, &PatchHeader, sizeof(TPatchHeader));

        // Decompress or copy the patch data
        assert(cbCompressed < PatchHeader.dwUnpackedSize);
        Decompress_RLE(pbDecompressed, PatchHeader.dwUnpackedSize, pbCompressed, cbCompressed);
    }

    // Free buffers and exit
    if(pbCompressed != NULL)
        FREEMEM(pbCompressed);
    return nError;
}

static LPBYTE PatchFileData(
    LPBYTE pbPatchData,
    LPVOID pvOldData,
    DWORD  cbOldData,
    PDWORD pcbNewData)
{
    PBLIZZARD_BSDIFF40_FILE pBsdiff = (PBLIZZARD_BSDIFF40_FILE)pbPatchData;
    PDWORD pCtrlBlock;
    LPBYTE pDataBlock;
    LPBYTE pExtraBlock;
    LPBYTE pbNewData = NULL;
    LPBYTE pbOldData = (PBYTE)pvOldData;
    DWORD dwNewOffset = 0;                          // Current position to patch
    DWORD dwOldOffset = 0;                          // Current source position
    DWORD dwNewSize;                                // Patched file size

    // Get pointer to the patch header
    // Format of BSDIFF header corresponds to original BSDIFF, which is:
    // 0000   8 bytes   signature "BSDIFF40"
    // 0008   8 bytes   size of the control block
    // 0010   8 bytes   size of the data block
    // 0018   8 bytes   new size of the patched file
    pBsdiff = (PBLIZZARD_BSDIFF40_FILE)pbPatchData;
    pbPatchData += sizeof(BLIZZARD_BSDIFF40_FILE);

    // Get pointer to the 32-bit BSDIFF control block
    // The control block follows immediately after the BSDIFF header
    // and consists of three 32-bit integers
    // 0000   4 bytes   Length to copy from the BSDIFF data block the new file
    // 0004   4 bytes   Length to copy from the BSDIFF extra block
    // 0008   4 bytes   Size to increment source file offset
    pCtrlBlock = (PDWORD)pbPatchData;
    pbPatchData += (size_t)pBsdiff->CtrlBlockSize;

    // Get the pointer to the data block
    pDataBlock = (LPBYTE)pbPatchData;
    pbPatchData += (size_t)pBsdiff->DataBlockSize;

    // Get the pointer to the extra block
    pExtraBlock = (LPBYTE)pbPatchData;
    dwNewSize = (DWORD)pBsdiff->NewFileSize;

    // Allocate new buffer
    pbNewData = ALLOCMEM(BYTE, dwNewSize);
    if(pbNewData == NULL)
        return NULL;

    // Now patch the file
    // TODO: Optimization, so it's not that memory consuming
    while(dwNewOffset < dwNewSize)
    {
        DWORD dwAddDataLength = pCtrlBlock[0];
        DWORD dwMovDataLength = pCtrlBlock[1];
        DWORD dwOldMoveLength = pCtrlBlock[2];
        DWORD i;

        // Sanity check
        if((dwNewOffset + dwAddDataLength) > dwNewSize)
        {
            FREEMEM(pbNewData);
            return NULL;
        }

        // Read the diff string to the target buffer
        memcpy(pbNewData + dwNewOffset, pDataBlock, dwAddDataLength);
        pDataBlock += dwAddDataLength;

        // Now combine the patch data with the original file
        for(i = 0; i < dwAddDataLength; i++)
        {
            if(dwOldOffset < cbOldData)
                pbNewData[dwNewOffset] = pbNewData[dwNewOffset] + pbOldData[dwOldOffset];

            dwNewOffset++;
            dwOldOffset++;
        }

        // Sanity check
        if((dwNewOffset + dwMovDataLength) > dwNewSize)
        {
            FREEMEM(pbNewData);
            return NULL;
        }

        // Copy the data from the extra block in BSDIFF patch
        memcpy(pbNewData + dwNewOffset, pExtraBlock, dwMovDataLength);
        pExtraBlock += dwMovDataLength;
        dwNewOffset += dwMovDataLength;

        // Move the old offset
        if(dwOldMoveLength & 0x80000000)
            dwOldMoveLength = 0x80000000 - dwOldMoveLength;
        dwOldOffset += dwOldMoveLength;
        pCtrlBlock += 3;
    }

    // Give the new size to the caller
    if(pcbNewData != NULL)
        *pcbNewData = dwNewSize;
    return pbNewData;
}

//-----------------------------------------------------------------------------
// Public functions (StormLib internals)

int PatchFileData(TMPQFile * hf)
{
    unsigned char md5_digest[MD5_DIGEST_SIZE];
    hash_state md5_state;
    TMPQFile * hfBase = hf;
    LPBYTE pbNewData = NULL;
    LPBYTE pbOldData = NULL;
    DWORD cbOldSize = 0;
    DWORD cbNewSize = 0;
    int nError = ERROR_SUCCESS;

    // Get the original file data
    cbOldSize = hf->dwPatchedSize;
    pbOldData = hf->pbFileData;
    assert(pbOldData != NULL);

    // Move to the first patch
    hf = hf->hfPatchFile;

    // Now go through all patches and patch the original data
    while(hf != NULL)
    {
        // This must be true
        assert(hf->pBlock->dwFlags & MPQ_FILE_PATCH_FILE);

        // Load the patch data
        nError = LoadMpqPatchFile(hf);
        if(nError != ERROR_SUCCESS)
            break;

        // Verify the original file before patching
        md5_init(&md5_state);
        md5_process(&md5_state, pbOldData, cbOldSize);
        md5_done(&md5_state, md5_digest);
        if(memcmp(hf->pPatchHeader->md5_before_patch, md5_digest, MD5_DIGEST_SIZE))
        {
            nError = ERROR_FILE_CORRUPT;
            break;
        }

        // Create block with patched data
        pbNewData = PatchFileData((LPBYTE)(hf->pPatchHeader + 1), pbOldData, cbOldSize, &cbNewSize);
        if(pbNewData == NULL)
        {
            nError = ERROR_FILE_CORRUPT;
            break;
        }

        // Verify the patched file
        md5_init(&md5_state);
        md5_process(&md5_state, pbNewData, cbNewSize);
        md5_done(&md5_state, md5_digest);
        if(memcmp(hf->pPatchHeader->md5_after_patch, md5_digest, MD5_DIGEST_SIZE))
        {
            FREEMEM(pbNewData);
            nError = ERROR_FILE_CORRUPT;
            break;
        }

        // Move old data to new data
        if(pbOldData != NULL)
            FREEMEM(pbOldData);
        pbOldData = pbNewData;
        cbOldSize = cbNewSize;

        // Move to the next patch
        hf = hf->hfPatchFile;
    }

    // Now remember the patched datain the file structure
    hfBase->pbFileData = pbOldData;
    hfBase->dwPatchedSize = cbOldSize;
    return nError;
}

//-----------------------------------------------------------------------------
// Public functions

//
// Patch prefix is the path subdirectory where the patched files are within MPQ.
//
// Example 1:
// Main MPQ:  locale-enGB.MPQ
// Patch MPQ: wow-update-12694.MPQ
// File in main MPQ: DBFilesClient\Achievement.dbc
// File in patch MPQ: enGB\DBFilesClient\Achievement.dbc
// Path prefix: enGB
//
// Example 2:
// Main MPQ:  expansion1.MPQ
// Patch MPQ: wow-update-12694.MPQ
// File in main MPQ: DBFilesClient\Achievement.dbc
// File in patch MPQ: Base\DBFilesClient\Achievement.dbc
// Path prefix: Base
//

bool WINAPI SFileOpenPatchArchive(
    HANDLE hMpq,
    const char * szPatchMpqName,
    const char * szPatchPathPrefix,
    DWORD dwFlags)
{
    TMPQArchive * haPatch;
    TMPQArchive * ha = (TMPQArchive *)hMpq;
    HANDLE hPatchMpq = NULL;
    size_t nLength = 0;
    int nError = ERROR_SUCCESS;

    // Keep compiler happy
    dwFlags = dwFlags;

    // Verify input parameters
    if(!IsValidMpqHandle(ha))
        nError = ERROR_INVALID_HANDLE;
    if(szPatchMpqName == NULL || *szPatchMpqName == 0)
        nError = ERROR_INVALID_PARAMETER;

    // Check the path prefix for patches
    if(szPatchPathPrefix != NULL)
    {
        nLength = strlen(szPatchPathPrefix);
        if(nLength > MPQ_PATCH_PREFIX_LEN - 2)
            nError = ERROR_INVALID_PARAMETER;
    }

    //
    // We don't allow adding patches to archives that have been open for write
    //
    // Error scenario:
    //
    // 1) Open archive for writing
    // 2) Modify or replace a file
    // 3) Add patch archive to the opened MPQ
    // 4) Read patched file
    // 5) Now what ?
    //

    if(nError == ERROR_SUCCESS)
    {
        if((ha->pStream->StreamFlags & STREAM_FLAG_READ_ONLY) == 0)
            nError = ERROR_ACCESS_DENIED;
    }

    // Open the archive like it is normal archive
    if(nError == ERROR_SUCCESS)
    {
        if(!SFileOpenArchive(szPatchMpqName, 0, MPQ_OPEN_READ_ONLY, &hPatchMpq))
            return false;
        haPatch = (TMPQArchive *)hPatchMpq;

        // Save the prefix for patch file names.
        // Make sure that there is backslash after it
        if(nLength > 0)
        {
            strcpy(haPatch->szPatchPrefix, szPatchPathPrefix);
            if(haPatch->szPatchPrefix[nLength - 1] != '\\')
            {
                haPatch->szPatchPrefix[nLength++] = '\\';
                haPatch->szPatchPrefix[nLength] = 0;
            }
        }

        // Now add the patch archive to the list of patches to the original MPQ
        while(ha != NULL)
        {
            if(ha->haPatch == NULL)
            {
                ha->haPatch = haPatch;
                return true;
            }

            // Move to the next archive
            ha = ha->haPatch;
        }

        // Should never happen
        nError = ERROR_CAN_NOT_COMPLETE;
    }

    SetLastError(nError);
    return false;
}

bool WINAPI SFileIsPatchedArchive(HANDLE hMpq)
{
    TMPQArchive * ha = (TMPQArchive *)hMpq;

    // Verify input parameters
    if(!IsValidMpqHandle(ha))
        return false;

    return (ha->haPatch != NULL);
}
