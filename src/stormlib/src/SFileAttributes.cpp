/*****************************************************************************/
/* SAttrFile.cpp                          Copyright (c) Ladislav Zezula 2007 */
/*---------------------------------------------------------------------------*/
/* Description:                                                              */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 12.06.04  1.00  Lad  The first version of SAttrFile.cpp                   */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "SCommon.h"

//-----------------------------------------------------------------------------
// Local functions

// This function expects the following order of saving internal files:
//
// 1) Save (listfile)
// 2) Save (attributes)
// 3) Save (signature) (not implemented yet)
// 4) Save MPQ tables

static DWORD DetermineFinalBlockTableSize(TMPQArchive * ha)
{
    TMPQBlock * pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
    TMPQBlock * pBlock;
    DWORD dwExtraEntriesNeeded = 0;

    // Is the (attributes) file in the MPQ already ?
    if(GetHashEntryExact(ha, ATTRIBUTES_NAME, LANG_NEUTRAL) == NULL)
        dwExtraEntriesNeeded++;

    // Is the (signature) file in the MPQ already ?
//  if(GetHashEntryExact(ha, SIGNATURE_NAME, LANG_NEUTRAL) == NULL)
//      dwExtraEntriesNeeded++;

    // Now parse the entire block table and find out if there is enough free entries
    // For each found free entry, we decrement number of extra entries needed
    for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++)
    {
        // Is that block table entry free ?
        if((pBlock->dwFlags & MPQ_FILE_EXISTS) == 0)
        {
            if(--dwExtraEntriesNeeded == 0)
                break;
        }
    }

    // Now get the number of final table entries
    return ha->pHeader->dwBlockTableSize + dwExtraEntriesNeeded;
}

//-----------------------------------------------------------------------------
// Public functions (internal use by StormLib)

int SAttrFileCreate(TMPQArchive * ha)
{
    TMPQAttributes * pNewAttr;
    int nError = ERROR_SUCCESS;

    // There should NOT be any attributes
    assert(ha->pAttributes == NULL);

    pNewAttr = ALLOCMEM(TMPQAttributes, 1);
    if(pNewAttr != NULL)
    {
        // Pre-set the structure
        pNewAttr->dwVersion = MPQ_ATTRIBUTES_V1;
        pNewAttr->dwFlags = 0;

        // Allocate array for CRC32
        pNewAttr->pCrc32 = ALLOCMEM(DWORD, ha->dwBlockTableMax);
        if(pNewAttr->pCrc32 != NULL)
        {
            memset(pNewAttr->pCrc32, 0, sizeof(DWORD) * ha->dwBlockTableMax);
            pNewAttr->dwFlags |= MPQ_ATTRIBUTE_CRC32;
        }
        else
            nError = ERROR_NOT_ENOUGH_MEMORY;

        // Allocate array for FILETIME
        pNewAttr->pFileTime = ALLOCMEM(TMPQFileTime, ha->dwBlockTableMax);
        if(pNewAttr->pFileTime != NULL)
        {
            memset(pNewAttr->pFileTime, 0, sizeof(TMPQFileTime) * ha->dwBlockTableMax);
            pNewAttr->dwFlags |= MPQ_ATTRIBUTE_FILETIME;
        }
        else
            nError = ERROR_NOT_ENOUGH_MEMORY;

        // Allocate array for MD5
        pNewAttr->pMd5 = ALLOCMEM(TMPQMD5, ha->dwBlockTableMax);
        if(pNewAttr->pMd5 != NULL)
        {
            memset(pNewAttr->pMd5, 0, sizeof(TMPQMD5) * ha->dwBlockTableMax);
            pNewAttr->dwFlags |= MPQ_ATTRIBUTE_MD5;
        }
        else
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // If something failed, then free the attributes structure
    if(nError != ERROR_SUCCESS)
    {
        FreeMPQAttributes(pNewAttr);
        pNewAttr = NULL;
    }

    ha->pAttributes = pNewAttr;
    return nError;
}


int SAttrFileLoad(TMPQArchive * ha)
{
    TMPQAttributes * pAttr = ha->pAttributes;
    HANDLE hFile = NULL;
    DWORD dwBytesRead;
    DWORD dwToRead;
    int nError = ERROR_SUCCESS;

    // The attributes must be already created
    assert(ha->pAttributes != NULL);
    assert(ha->pHeader->dwBlockTableSize <= ha->dwBlockTableMax);

    // Attempt to open the "(attributes)" file.
    // If it's not there, we don't support attributes
    if(!SFileOpenFileEx((HANDLE)ha, ATTRIBUTES_NAME, SFILE_OPEN_ANY_LOCALE, &hFile))
        nError = GetLastError();

    // Load the content of the attributes file
    if(nError == ERROR_SUCCESS)
    {
        dwToRead = sizeof(DWORD) + sizeof(DWORD);
        SFileReadFile(hFile, pAttr, dwToRead, &dwBytesRead, NULL);
        if(dwBytesRead != dwToRead)
            nError = ERROR_FILE_CORRUPT;
    }

    // Verify format of the attributes
    if(nError == ERROR_SUCCESS)
    {
        if(pAttr->dwVersion > MPQ_ATTRIBUTES_V1)
            nError = ERROR_BAD_FORMAT;
    }

    // Load the CRC32 (if any)
    if(nError == ERROR_SUCCESS && (pAttr->dwFlags & MPQ_ATTRIBUTE_CRC32))
    {
        dwToRead = ha->pHeader->dwBlockTableSize * sizeof(DWORD);
        SFileReadFile(hFile, pAttr->pCrc32, dwToRead, &dwBytesRead, NULL);
        if(dwBytesRead != dwToRead)
            nError = ERROR_FILE_CORRUPT;
    }

    // Read the FILETIMEs (if any)
    if(nError == ERROR_SUCCESS && (pAttr->dwFlags & MPQ_ATTRIBUTE_FILETIME))
    {
        dwToRead = ha->pHeader->dwBlockTableSize * sizeof(TMPQFileTime);
        SFileReadFile(hFile, pAttr->pFileTime, dwToRead, &dwBytesRead, NULL);
        if(dwBytesRead != dwToRead)
            nError = ERROR_FILE_CORRUPT;
    }

    // Read the MD5 (if any)
    if(nError == ERROR_SUCCESS && (pAttr->dwFlags & MPQ_ATTRIBUTE_MD5))
    {
        dwToRead = ha->pHeader->dwBlockTableSize * sizeof(TMPQMD5);
        SFileReadFile(hFile, pAttr->pMd5, dwToRead, &dwBytesRead, NULL);
        if(dwBytesRead != dwToRead)
            nError = ERROR_FILE_CORRUPT;
    }

    // If any error happened, we clear the attributes' flags
    if(nError != ERROR_SUCCESS)
        pAttr->dwFlags = 0;

    // Cleanup & exit
    SFileCloseFile(hFile);
    return nError;
}

int SAttrFileSaveToMpq(TMPQArchive * ha)
{
    TMPQAttributes * pAttr = ha->pAttributes;
    DWORD dwFinalBlockTableSize;
    DWORD dwFileSize = 0;
    DWORD dwToWrite;
    void * pvAddHandle = NULL;
    int nError = ERROR_SUCCESS;

    // If there are no attributes, do nothing
    if(ha->pAttributes == NULL)
        return ERROR_SUCCESS;

    // If there are attributes, all three buffers must be valid
    assert(pAttr->pCrc32 != NULL);
    assert(pAttr->pFileTime != NULL);
    assert(pAttr->pMd5 != NULL);

    // We have to determine the block table size that AFTER adding all special MPQ files
    // This is because after attributes file gets saved, the block table size will increase.
    dwFinalBlockTableSize = DetermineFinalBlockTableSize(ha);
    if(dwFinalBlockTableSize > ha->dwBlockTableMax)
        return ERROR_DISK_FULL;

    // Calculate the size of the attributes file
    dwFileSize += sizeof(DWORD) + sizeof(DWORD);     // Header
    if(pAttr->dwFlags & MPQ_ATTRIBUTE_CRC32)
        dwFileSize += dwFinalBlockTableSize * sizeof(DWORD);
    if(pAttr->dwFlags & MPQ_ATTRIBUTE_FILETIME)
        dwFileSize += dwFinalBlockTableSize * sizeof(TMPQFileTime);
    if(pAttr->dwFlags & MPQ_ATTRIBUTE_MD5)
        dwFileSize += dwFinalBlockTableSize * sizeof(TMPQMD5);

    // Create the attributes file in the MPQ
    nError = SFileAddFile_Init(ha, ATTRIBUTES_NAME,
                                   NULL,
                                   dwFileSize,
                                   LANG_NEUTRAL,
                                   MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING,
                                  &pvAddHandle);
    
    // Write all parts of the (attributes) file
    if(nError == ERROR_SUCCESS)
    {
        dwToWrite = sizeof(DWORD) + sizeof(DWORD);
        nError = SFileAddFile_Write(pvAddHandle, pAttr, dwToWrite, MPQ_COMPRESSION_ZLIB);
    }

    // Write the array of CRC32
    if(nError == ERROR_SUCCESS && (pAttr->dwFlags & MPQ_ATTRIBUTE_CRC32))
    {
        dwToWrite = dwFinalBlockTableSize * sizeof(DWORD);
        nError = SFileAddFile_Write(pvAddHandle, pAttr->pCrc32, dwToWrite, MPQ_COMPRESSION_ZLIB);
    }

    // Write the array of FILETIMEs
    if(nError == ERROR_SUCCESS && (pAttr->dwFlags & MPQ_ATTRIBUTE_FILETIME))
    {
        dwToWrite = dwFinalBlockTableSize * sizeof(TMPQFileTime);
        nError = SFileAddFile_Write(pvAddHandle, pAttr->pFileTime, dwToWrite, MPQ_COMPRESSION_ZLIB);
    }

    // Write the array of MD5s
    if(nError == ERROR_SUCCESS && (pAttr->dwFlags & MPQ_ATTRIBUTE_MD5))
    {
        dwToWrite = dwFinalBlockTableSize * sizeof(TMPQMD5);
        nError = SFileAddFile_Write(pvAddHandle, pAttr->pMd5, dwToWrite, MPQ_COMPRESSION_ZLIB);
    }

    // Finalize the file in the archive
    if(pvAddHandle != NULL)
        SFileAddFile_Finish(pvAddHandle, nError);

    return nError;
}

void FreeMPQAttributes(TMPQAttributes * pAttr)
{
    if(pAttr != NULL)
    {
        if(pAttr->pCrc32 != NULL)
            FREEMEM(pAttr->pCrc32);
        if(pAttr->pFileTime != NULL)
            FREEMEM(pAttr->pFileTime);
        if(pAttr->pMd5 != NULL)
            FREEMEM(pAttr->pMd5);

        FREEMEM(pAttr);
    }
}

