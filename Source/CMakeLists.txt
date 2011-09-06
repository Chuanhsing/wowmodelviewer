project(wowmodelviewer)
cmake_minimum_required(VERSION 2.6)

######### CUSTOMIZABLE PART #########

add_definitions(-D_WINDOWS )
add_definitions(-D_WIN64 )

set(WMV_BASE_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR})

#####################################



######### DON'T CHANGE THIS PART  !!! #########
set(QT_USE_QTCORE 1)
set(QT_USE_QTGUI 1)

find_package( Qt4 REQUIRED )
include(${QT_USE_FILE})


# -fpermissive is needed for math part compilation => gcc stop on an error where VS is OK...
add_definitions(
    -Wall -g -fpermissive
)

# for source headers
include_directories(.)

# for preprocessed ui
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB wmv_src *.cpp)
file(GLOB wmv_h *.h)
file(GLOB wmv_ui ui/*.ui)

QT4_WRAP_CPP(wmv_moc ${wmv_h})
QT4_WRAP_UI(wmv_ui_h ${wmv_ui})

# for building exporters
add_subdirectory("Exporters")

# for building StormLib
add_subdirectory("StormLib")

add_executable(wowmodelviewer ${wmv_src} ${wmv_moc} ${wmv_ui_h})
add_dependencies(wowmodelviewer exporters StormLib)
target_link_libraries(wowmodelviewer exporters StormLib ${QT_LIBRARIES})
install(TARGETS wowmodelviewer RUNTIME DESTINATION ${WMV_BASE_DEST_DIR}/bin)
