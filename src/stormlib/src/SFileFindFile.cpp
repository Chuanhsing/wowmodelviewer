/*****************************************************************************/
/* SFileFindFile.cpp                      Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* A module for file searching within MPQs                                   */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 25.03.03  1.00  Lad  The first version of SFileFindFile.cpp               */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "SCommon.h"

//-----------------------------------------------------------------------------
// Defines

#define LISTFILE_CACHE_SIZE 0x1000

//-----------------------------------------------------------------------------
// Local functions

static BOOL IsValidSearchHandle(TMPQSearch * hs)
{
    if(hs == NULL || IsBadReadPtr(hs, sizeof(TMPQSearch)))
        return FALSE;

    if(IsValidMpqHandle(hs->ha) == FALSE)
        return FALSE;

    return TRUE;
}

BOOL CheckWildCard(const char * szString, const char * szWildCard)
{
    // Number of following chars in szString that may differ from szWildCard
    unsigned int nDiffCharsAllowed = 0; 

    // When the string is empty, it does not match with any wildcard
    if(*szString == 0)
        return FALSE;

    // When the mask is empty, it matches to every wildcard
    if(szWildCard == NULL || *szWildCard == 0)
        return FALSE;

    // Do normal test
    for(;;)
    {
        // If we allow at least one char that doesn't have to match,
        // Skip them
        if(nDiffCharsAllowed != 0)
        {
            // When end of string, they match
            if(*szString == 0)
                return (*szWildCard == 0) ? TRUE : FALSE;

            // If we found a matching char, clear number of allowed non-matching chars
            if(toupper(*szString) == toupper(*szWildCard))
            {
                nDiffCharsAllowed = 0;
                szWildCard++;
                szString++;
            }
            else
            {
                // When the chars don't match, increment string,
                // but stay at the same pos in wildcard
                nDiffCharsAllowed--;
                szString++;
            }
        }
        else
        {
            while(*szWildCard == '*')
            {
                nDiffCharsAllowed |= 0x80000000;
                szWildCard++;
            }

            while(*szWildCard == '?')
            {
                nDiffCharsAllowed++;
                szWildCard++;
            }

            if(nDiffCharsAllowed == 0)
            {
                // If the current chars don't match, then the strings don't match
                if(toupper(*szString) != toupper(*szWildCard))
                    return FALSE;

                // When end of string, they match
                if(*szString == 0)
                    return TRUE;

                // The chars match. Move both string and wildcard
                szWildCard++;
                szString++;
            }
        }
    }
}

// Performs one MPQ search
static int DoMPQSearch(TMPQSearch * hs, SFILE_FIND_DATA * lpFindFileData)
{
    TMPQArchive * ha = hs->ha;
    TFileNode * pNode = NULL;
    TMPQBlock * pBlock;
    TMPQHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPQHash * pHash = ha->pHashTable + hs->dwNextIndex;
    DWORD dwHashIndex;
    DWORD dwBlockIndex;

    // Do until a file is found or no more files
    while(pHash < pHashEnd)
    {
        pNode = ha->pListFile[hs->dwNextIndex++];

        // Is this entry occupied ?
        // Note don't check the block index to HASH_ENTRY_DELETED.
        // Various maliciously modified MPQs have garbage here
        if(pHash->dwBlockIndex < ha->pHeader->dwBlockTableSize)
        {
            // Check if the file exists
            pBlock = ha->pBlockTable + pHash->dwBlockIndex;
            if((pBlock->dwFlags & MPQ_FILE_EXISTS) && (pNode != NULL))
            {
                // Check the file name.
                if(CheckWildCard(pNode->szFileName, hs->szSearchMask))
                {
                    // Calculate hash index
                    dwHashIndex = (DWORD)(pHash - ha->pHashTable);
                    dwBlockIndex = pHash->dwBlockIndex;

                    // Fill the found entry
                    lpFindFileData->dwHashIndex  = dwHashIndex;
                    lpFindFileData->dwBlockIndex = pHash->dwBlockIndex;
                    lpFindFileData->dwFileSize   = pBlock->dwFSize;
                    lpFindFileData->dwFileFlags  = pBlock->dwFlags;
                    lpFindFileData->dwCompSize   = pBlock->dwCSize;
                    lpFindFileData->lcLocale     = pHash->lcLocale;

                    // Fill the filetime
                    lpFindFileData->dwFileTimeLo = 0;
                    lpFindFileData->dwFileTimeHi = 0;
                    if(ha->pAttributes != NULL && (ha->pAttributes->dwFlags & MPQ_ATTRIBUTE_FILETIME))
                    {
                        lpFindFileData->dwFileTimeLo = ha->pAttributes->pFileTime[dwBlockIndex].dwFileTimeLow;
                        lpFindFileData->dwFileTimeHi = ha->pAttributes->pFileTime[dwBlockIndex].dwFileTimeHigh;
                    }

                    // Fill the file name and plain file name
                    strcpy(lpFindFileData->cFileName, pNode->szFileName);
                    lpFindFileData->szPlainName = (char *)GetPlainMpqFileName(lpFindFileData->cFileName);

                    // Fill the next entry
                    return ERROR_SUCCESS;
                }
            }
        }

        // Move to the next hash table entry
        pHash++;
    }

    // No more files found, return error
    return ERROR_NO_MORE_FILES;
}

static void FreeMPQSearch(TMPQSearch *& hs)
{
    if(hs != NULL)
    {
        FREEMEM(hs);
        hs = NULL;
    }
}

//-----------------------------------------------------------------------------
// Public functions

HANDLE WINAPI SFileFindFirstFile(HANDLE hMpq, const char * szMask, SFILE_FIND_DATA * lpFindFileData, const char * szListFile)
{
    TMPQArchive * ha = (TMPQArchive *)hMpq;
    TMPQSearch * hs = NULL;             // Search object handle
    size_t nSize  = 0;
    int nError = ERROR_SUCCESS;

    // Check for the valid parameters
    if(IsValidMpqHandle(ha) == FALSE)
        nError = ERROR_INVALID_HANDLE;
    if(szMask == NULL || lpFindFileData == NULL)
        nError = ERROR_INVALID_PARAMETER;

    // Include the listfile into the MPQ's internal listfile
    // Note that if the listfile name is NULL, do nothing because the
    // internal listfile is always included.
    if(nError == ERROR_SUCCESS && szListFile != NULL && *szListFile != 0)
        nError = SFileAddListFile((HANDLE)ha, szListFile);

    // Allocate the structure for MPQ search
    if(nError == ERROR_SUCCESS)
    {
        nSize = sizeof(TMPQSearch) + strlen(szMask) + 1;
        if((hs = (TMPQSearch *)ALLOCMEM(char, nSize)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Perform the first search
    if(nError == ERROR_SUCCESS)
    {
        memset(hs, 0, sizeof(TMPQSearch));
        hs->ha = ha;
        strcpy(hs->szSearchMask, szMask);
        nError = DoMPQSearch(hs, lpFindFileData);
    }

    // Cleanup
    if(nError != ERROR_SUCCESS)
    {
        FreeMPQSearch(hs);
        SetLastError(nError);
    }
    
    // Return the result value
    return (HANDLE)hs;
}

BOOL WINAPI SFileFindNextFile(HANDLE hFind, SFILE_FIND_DATA * lpFindFileData)
{
    TMPQSearch * hs = (TMPQSearch *)hFind;
    int nError = ERROR_SUCCESS;

    // Check the parameters
    if(IsValidSearchHandle(hs) == FALSE)
        nError = ERROR_INVALID_HANDLE;
    if(lpFindFileData == NULL)
        nError = ERROR_INVALID_PARAMETER;

    if(nError == ERROR_SUCCESS)
        nError = DoMPQSearch(hs, lpFindFileData);

    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

BOOL WINAPI SFileFindClose(HANDLE hFind)
{
    TMPQSearch * hs = (TMPQSearch *)hFind;

    // Check the parameters
    if(IsValidSearchHandle(hs) == FALSE)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    FreeMPQSearch(hs);
    return TRUE;
}
