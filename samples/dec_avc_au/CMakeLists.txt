cmake_minimum_required(VERSION 3.16)

project(dec_avc_au)
set (target dec_avc_au)

add_executable(${target})

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

# output
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/${PLATFORM})

if (CMAKE_GENERATOR STREQUAL "Xcode")
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin)
endif()

# debug definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${target} PUBLIC  _DEBUG)
endif()

# release definitions
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${target} PUBLIC NDEBUG)
endif()

# macOS
if(OS STREQUAL "darwin") 
    # common compile options
    target_compile_options(${target} PRIVATE -std=c++17 -stdlib=libc++)

    # x64 compile options
    if (PLATFORM STREQUAL "x64") 
        target_compile_options(${target} PRIVATE -m64 -fPIC)
    endif()

    # x86 compile options
    if (PLATFORM STREQUAL "x86") 
    target_compile_options(${target} PRIVATE -m32)
    endif()

    # debug compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE -g)
    endif()

    # release compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE -Os)
    endif()
endif()

# Linux
if(OS STREQUAL "linux") 
    # common compile options
    target_compile_options(${target} PRIVATE -std=c++17 -MMD -MP -MF)

    # x64 compile options
    if (PLATFORM STREQUAL "x64") 
        target_compile_options(${target} PRIVATE -m64 -fPIC)
    endif()

    # x86 compile options
    if (PLATFORM STREQUAL "x86") 
    target_compile_options(${target} PRIVATE -m32)
    endif()

    # debug compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE -g)
    endif()

    # release compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE -O2 -s)
    endif()
endif()

# Windows
if(OS STREQUAL "windows")
    # add /MTd or /MT compiler option 
    set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # enable C++ 17 standard features
    target_compile_features(${target} PRIVATE cxx_std_17)

    # assume extern "C" functions do not throw. 
    # see: https://docs.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=vs-2019
    target_compile_options(${target} PRIVATE /EHsc)
    
    # x64 compile options
    if (PLATFORM STREQUAL "x64") 
        target_compile_definitions(${target} PRIVATE WIN64)
    endif()

    # x86 compile options
    if (PLATFORM STREQUAL "x86") 
        target_compile_definitions(${target} PRIVATE WIN32)
    endif()

    # debug compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE /Zi /Od)
    endif()

    # release compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE /O2)
    endif()     
endif()

# include dirs
target_include_directories(${target}
    PUBLIC
        ../../sdk/include
)

# sources
file(GLOB source "./*.cpp")

target_sources(${target}
PRIVATE
    ${source} 
)

# lib dirs
target_link_directories(${target}
PRIVATE
    # avblocks
    ${PROJECT_SOURCE_DIR}/../../sdk/lib/${PLATFORM}
)

# libs
if (OS STREQUAL "darwin")
    target_link_libraries(
        ${target}

        # primo-avblocks
        libAVBlocks.dylib

        # os
        "-framework CoreFoundation" 
        "-framework ApplicationServices" 
    )
endif()

if(OS STREQUAL "linux")
    target_link_libraries(
        ${target}

        # primo-avblocks
        libAVBlocks.so

        # os
        # pthread 
        # rt
    )
endif()

if(OS STREQUAL "windows")
    target_link_libraries(
        ${target}
        
        # primo-avblocks
        AVBlocks.dll

        # os
        # Winmm 
        # Pdh
    )
endif()

