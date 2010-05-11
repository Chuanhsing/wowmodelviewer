file(GLOB WOWMV_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)
list(REMOVE_ITEM WOWMV_SOURCES particle_test.cpp)

if (UNIX)
        list(REMOVE_ITEM WOWMV_SOURCES RenderTexture.cpp AVIGenerator.cpp mpq_stormlib.cpp)
endif()

if (MAC)
        list(REMOVE_ITEM WOWMV_SOURCES mpq_stormlib.cpp)
endif()

set(STORM_SOURCES
    stormlib/src/SCommon.cpp
    stormlib/src/SCompression.cpp
    stormlib/src/SFileAddFile.cpp
    stormlib/src/SFileAttributes.cpp
    stormlib/src/SFileCompactArchive.cpp
    stormlib/src/SFileCreateArchiveEx.cpp
    stormlib/src/SFileExtractFile.cpp
    stormlib/src/SFileFindFile.cpp
    stormlib/src/SFileListFile.cpp
    stormlib/src/SFileOpenArchive.cpp
    stormlib/src/SFileOpenFileEx.cpp
    stormlib/src/SFileReadFile.cpp
    stormlib/src/SFileVerify.cpp
    stormlib/src/huffman/huff.cpp
    stormlib/src/adpcm/adpcm.cpp
    stormlib/src/pklib/crc32.c
    stormlib/src/pklib/explode.c
    stormlib/src/pklib/implode.c
    stormlib/src/lzma/C/LzFind.c
    stormlib/src/lzma/C/LzmaDec.c
    stormlib/src/lzma/C/LzmaEnc.c
)

if (UNIX)
	list(APPEND STORM_SOURCES stormlib/src/StormPortLinux.cpp)
endif()

if (MAC)
	list(APPEND STORM_SOURCES stormlib/src/StormPortMac.cpp)
endif()

set(MPQ_SOURCES
    libmpq/common.cpp
    libmpq/explode.cpp
    libmpq/extract.cpp
    libmpq/huffman.cpp
    libmpq/mpq.cpp
    libmpq/wave.cpp
)

set(CXIMAGE_SOURCES
    CxImage/tif_xfile.cpp
    CxImage/ximabmp.cpp
    CxImage/ximadsp.cpp
    CxImage/ximaenc.cpp
    CxImage/ximaexif.cpp
    CxImage/ximage.cpp
    CxImage/ximagif.cpp
    CxImage/ximahist.cpp
    CxImage/ximaico.cpp
    CxImage/ximainfo.cpp
    CxImage/ximaint.cpp
    CxImage/ximajas.cpp
    CxImage/ximajbg.cpp
    CxImage/ximajpg.cpp
    CxImage/ximalpha.cpp
    CxImage/ximalyr.cpp
    CxImage/ximamng.cpp
    CxImage/ximapal.cpp
    CxImage/ximapcx.cpp
    CxImage/ximapng.cpp
    CxImage/ximaraw.cpp
    CxImage/ximasel.cpp
    CxImage/ximaska.cpp
    CxImage/ximatga.cpp
    CxImage/ximath.cpp
    CxImage/ximatif.cpp
    CxImage/ximatran.cpp
    CxImage/ximawbmp.cpp
    CxImage/ximawmf.cpp
    CxImage/ximawnd.cpp
    CxImage/xmemfile.cpp
)
