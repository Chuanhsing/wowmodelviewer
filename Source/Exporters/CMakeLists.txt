project(exporters)
cmake_minimum_required(VERSION 2.6)

set(QT_USE_QTCORE 1)

find_package( Qt4 REQUIRED )
include(${QT_USE_FILE})


# -fpermissive is needed for math part compilation => gcc stop on an error where VS is OK...
add_definitions(
    -Wall -g -fpermissive -DEXPORTERS_LIB
)

# for source headers
include_directories(.)

# for preprocessed ui
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB exporters_src *.cpp ../Qs*.cpp)
file(GLOB exporters_h *.h ../Qs*.h)

QT4_WRAP_CPP(exporters_moc ${exporters_h})

add_library(exporters SHARED ${exporters_src} ${exporters_moc})

target_link_libraries(exporters ${QT_LIBRARIES})
install(TARGETS exporters RUNTIME DESTINATION ${WMV_BASE_DEST_DIR}/bin)
