file(GLOB WOWMV_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)
list(REMOVE_ITEM WOWMV_SOURCES particle_test.cpp)

if (UNIX)
	list(REMOVE_ITEM WOWMV_SOURCES RenderTexture.cpp AVIGenerator.cpp)
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
