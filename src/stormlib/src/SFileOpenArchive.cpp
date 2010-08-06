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

DWORD TryDecryptHetBlock(TMPQXXXBlock * pData);

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

                        if(dwHeaderSize != MPQ_HEADER_SIZE_V2)
                        {
                            dwHeaderSize = MPQ_HEADER_SIZE_V3;
                            ha->dwFlags |= MPQ_FLAG_PROTECTED;
                        }
                        bFormatOK = true;
                        break;

                    default:
                        
                        // Wow.exe doesn't really care about format number if it's > 1,
                        // as long as the header size is greater or equal to 0x2C
                        if(dwHeaderSize > MPQ_HEADER_SIZE_V3)
                        {
                            dwHeaderSize = MPQ_HEADER_SIZE_V3;
                            bMpqHeaderTrimmed = true;
                        }
                        if(dwHeaderSize >= MPQ_HEADER_SIZE_V2)
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
        
        // Allocate space for HET block
        if(ha->HETBlockPos.QuadPart && ha->BETBlockPos.QuadPart > ha->HETBlockPos.QuadPart)
        {
            ha->dwHETBlockSize = (DWORD)(ha->BETBlockPos.QuadPart - ha->HETBlockPos.QuadPart);
            if(ha->dwHETBlockSize != 0)
            {
                ha->pHETBlock = (TMPQXXXBlock *)ALLOCMEM(BYTE, ha->dwHETBlockSize);
                if(ha->pHETBlock == NULL)
                    nError = ERROR_NOT_ENOUGH_MEMORY;
            }
        }

        // Allocate space for BET block
        if(ha->BETBlockPos.QuadPart != 0 && ha->HashTablePos.QuadPart > ha->BETBlockPos.QuadPart)
        {
            ha->dwBETBlockSize = (DWORD)(ha->HashTablePos.QuadPart - ha->BETBlockPos.QuadPart);
            if(ha->dwBETBlockSize != 0)
            {
                ha->pBETBlock = (TMPQXXXBlock *)ALLOCMEM(BYTE, ha->dwBETBlockSize);
                if(ha->pBETBlock == NULL)
                    nError = ERROR_NOT_ENOUGH_MEMORY;
            }
        }

        // Allocate all three MPQ tables
        ha->pHashTable     = ALLOCMEM(TMPQHash, dwHashTableSize);
        ha->pBlockTable    = ALLOCMEM(TMPQBlock, ha->dwBlockTableMax);
        ha->pExtBlockTable = ALLOCMEM(TMPQBlockEx, ha->dwBlockTableMax);
        if(!ha->pHashTable || !ha->pBlockTable || !ha->pExtBlockTable)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Read the HET block, if present
    if(nError == ERROR_SUCCESS && ha->dwHETBlockSize)
    {
        if(!FileStream_Read(ha->pStream, &ha->HETBlockPos, ha->pHETBlock, ha->dwHETBlockSize))
            nError = GetLastError();
        BSWAP_ARRAY32_UNSIGNED(ha->pHETBlock, sizeof(TMPQXXXBlock));

        // The purpose of HET and BET blocks is unknown.
        // The data don't appear to be compressed
        // (or at least my attempts to decrypt them by brute-force failed)
        // Wow-Cataclysm itself doesn't care about them.
//      TryDecryptHetBlock(ha->pHETBlock);
    }

    // Read the BET block, if present
    if(nError == ERROR_SUCCESS && ha->dwBETBlockSize)
    {
        if(!FileStream_Read(ha->pStream, &ha->BETBlockPos, ha->pBETBlock, ha->dwBETBlockSize))
            nError = GetLastError();
        BSWAP_ARRAY32_UNSIGNED(ha->pBETBlock, sizeof(TMPQXXXBlock));
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

