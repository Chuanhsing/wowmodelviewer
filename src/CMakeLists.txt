project(wowmodelviewer)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
set(wxWidgets_USE_LIBS base core gl net aui xml)
find_package(wxWidgets REQUIRED )
find_package(GLEW REQUIRED)
find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

include(${wxWidgets_USE_FILE})
include(cmake/sources.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
    ${wxWidgets_INCLUDE_DIRS}
    ${BZIP2_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${GLEW_INCLUDE_PATH}
    ${JPEG_INCLUDE_DIR}
    ${PNG_PNG_INCLUDE_DIR}
)

add_definitions(
    -DWotLK
)

add_library(mpq STATIC ${MPQ_SOURCES})
add_library(cximage STATIC ${CXIMAGE_SOURCES})

if (APPLE)
	add_definitions(-D_MAC)
	add_definitions(-D_MAC_INTEL)

	set(CMAKE_OSX_ARCHITECTURES i386)
	set(MACOSX_BUNDLE_ICON_FILE wmv.icns)
	set(MACOSX_BUNDLE_INFO_STRING "World of Warcraft ModelViewer")
	
	set_source_files_properties("${MACOSX_BUNDLE_ICON_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	
	add_executable(wowmodelviewer MACOSX_BUNDLE ${WOWMV_SOURCES} ../bin_support/Icons/${MACOSX_BUNDLE_ICON_FILE})
else()
	add_definitions(-D_LINUX)
	add_definitions(-D__SYS_ZLIB)
	add_definitions(-D_7ZIP_ST)

	add_executable(wowmodelviewer ${WOWMV_SOURCES})
endif()

target_link_libraries(wowmodelviewer
    mpq
    cximage
    ${wxWidgets_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARY}
)
