project(wowmodelviewer)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE MinSizeRel)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
find_package(wxWidgets REQUIRED net gl aui xml adv core base)
#find_package(BZip2 REQUIRED)
#find_package(ZLIB REQUIRED)
set(JPEG_INCLUDE_DIR c:/MinGW/include)
#find_package(JPEG REQUIRED)
#find_package(PNG REQUIRED)



set(USE_STORM 1)
if (APPLE)
	message(STATUS "Using Mac OS X port")
	add_definitions(-D_MAC)
	add_definitions(-D_MAC_INTEL)

	set(CMAKE_OSX_ARCHITECTURES i386)
	set(MACOSX_BUNDLE_ICON_FILE wmv.icns)
	set(MACOSX_BUNDLE_INFO_STRING "World of Warcraft ModelViewer")
	
	set_source_files_properties("${MACOSX_BUNDLE_ICON_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

	set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> cr <TARGET> <LINK_FLAGS> <OBJECTS> ;<CMAKE_RANLIB> -c <TARGET> ")
	set(CMAKE_CXX_CREATE_STATIC_LIBRARY "<CMAKE_AR> cr <TARGET> <LINK_FLAGS> <OBJECTS> ;<CMAKE_RANLIB> -c <TARGET> ")
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	message(STATUS "Using Linux port")
	add_definitions(-D_LINUX)
	add_definitions(-D_LINUX32)
endif()

if (WIN32)
	message(STATUS "Using Windows MinGW version")
	add_subdirectory(CxImage)
	
	if (USE_STORM)
    	message(STATUS "Using Stormlib")
    	if (WIN32)
    		add_subdirectory(stormlib)
    	else()
    		add_library(storm STATIC ${STORM_SOURCES})
    		set (EXTRA_LIBS ${EXTRA_LIBS} storm)
    		add_definitions(-D__SYS_ZLIB)
    		add_definitions(-D_7ZIP_ST)
    		add_definitions(-DUSE_STORM)
    	endif()
	else ()
    	message("Using libmpq")
    	add_library(mpq STATIC ${MPQ_SOURCES})
    	set (EXTRA_LIBS ${EXTRA_LIBS} mpq)
	endif()
endif()


include(${wxWidgets_USE_FILE})
include(cmake/sources.cmake)


add_definitions(
    -DWotLK -Wall -Wno-unknown-pragmas -g
)

if(WIN32)
	message(STATUS "Using glew")
	include(glew/glew.cmake)
	add_definitions(-DGLEW_STATIC)
	set(WOWMV_SOURCES ${WOWMV_SOURCES} ${GLEW_SRC})
	message("glew src : ${GLEW_SRC}")
	message("glew include : ${GLEW_INCLUDE_DIR}")
endif()

include_directories(${CMAKE_SOURCE_DIR}
    ${wxWidgets_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIR}
)


if (APPLE)
	add_executable(wowmodelviewer MACOSX_BUNDLE ${WOWMV_SOURCES} ../bin_support/Icons/${MACOSX_BUNDLE_ICON_FILE})
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	add_executable(wowmodelviewer ${WOWMV_SOURCES})
endif()

if(WIN32)
    add_definitions(-D_WINDOWS)
	add_definitions(-D_MINGW)
	set(CMAKE_EXE_LINKER_FLAGS "-s") # strip exe
	
    add_executable(wowmodelviewer ${WOWMV_SOURCES})
    add_dependencies(wowmodelviewer CxImage StormLib)   
endif()

target_link_libraries(wowmodelviewer
    cximage
    ${wxWidgets_LIBRARIES}
    stormlib
)


install(TARGETS wowmodelviewer 
        RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
        
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/../bin_support/MinGW/*.dll")
install(FILES ${files} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
        