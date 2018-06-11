cmake_minimum_required(VERSION 2.6)

#output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/\@bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/\@lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/\@lib)

if(APPLE)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/\@bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/\@lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/\@lib)
endforeach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})

#set(CMAKE_XCODE_ATTRIBUTE_WARNING_CFLAGS "-Wno-inconsistent-missing-override -Wno-unused-local-typedef -Wno-deprecated-register -Wnonportable-include-path -Wno-deprecated-declarations")

endif()

if(WIN32)

SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Od /EHsc /DEBUG")
SET(CMAKE_C_FLAGS_DEBUG "/MTd /Zi /Od /EHsc /DEBUG")

# include dirs
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Base
    ${CMAKE_CURRENT_SOURCE_DIR}/Demo
    C:/boost_1_64_0
)

# lib dirs
link_directories(
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    C:/boost_1_64_0/stage/lib
)
add_definitions(
    /D _CRT_SECURE_NO_WARNINGS
    /D _WIN32_WINDOWS
    /D _WINSOCK_DEPRECATED_NO_WARNINGS
    #/D WIN32_LEAN_AND_MEAN
    /D DEBUG
    /D _DEBUG
)

else()

# include dirs
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Base
    ${CMAKE_CURRENT_SOURCE_DIR}/Demo
    /usr/local/include
)

# lib dirs
link_directories(
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    /usr/local/lib64
    /usr/local/lib
    /usr/lib64
    /usr/lib
)

# compile options
add_definitions(
    -g
    -std=c++11
    -DDEBUG
    -D_DEBUG
    -DHAVE_PTHREAD
)

endif()

# solution name
project(tcp)

add_subdirectory(3rd)
add_subdirectory(Base)
add_subdirectory(Demo)
