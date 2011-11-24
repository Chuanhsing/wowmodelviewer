project(CxImage)
cmake_minimum_required(VERSION 2.6)

set(SRC_FILES
    tif_xfile.cpp
    ximabmp.cpp
    ximadsp.cpp
    ximaenc.cpp
    ximaexif.cpp
    ximage.cpp
    ximagif.cpp
    ximahist.cpp
    ximaico.cpp
    ximainfo.cpp
    ximaint.cpp
    ximajas.cpp
    ximajbg.cpp
    ximajpg.cpp
    ximalpha.cpp
    ximalyr.cpp
    ximamng.cpp
    ximapal.cpp
    ximapcx.cpp
    ximapng.cpp
    ximaraw.cpp
    ximasel.cpp
    ximaska.cpp
    ximatga.cpp
    ximath.cpp
    ximatif.cpp
    ximatran.cpp
    ximawbmp.cpp
    ximawmf.cpp
    ximawnd.cpp
    xmemfile.cpp
)

message(STATUS "Using CxImage")

add_definitions(-D_MINGW)
add_definitions(-D_WINDOWS)
add_definitions(-D_WIN32)
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)

add_library(cximage STATIC ${SRC_FILES})


