cmake_minimum_required(VERSION 3.16)

project(dec_g711_ulaw_file)
set (target dec_g711_ulaw_file)

add_executable(${target})

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

# output
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin/${PLATFORM})

if (CMAKE_GENERATOR MATCHES "Visual Studio.*20.*")
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin)
endif()

# debug definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${target} PUBLIC  _DEBUG)
endif()

# release definitions
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${target} PUBLIC NDEBUG)
endif()

# Windows
if(OS STREQUAL "windows")
    set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_compile_options(${target} PRIVATE /EHsc)
    target_compile_definitions(${target} PRIVATE UNICODE _UNICODE)    
    
    if (PLATFORM STREQUAL "x64") 
        target_compile_definitions(${target} PRIVATE WIN64)
    endif()

    if (PLATFORM STREQUAL "x86") 
        target_compile_definitions(${target} PRIVATE WIN32)
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE /Zi /Od)
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE /O2)
    endif()     
endif()

# include dirs
target_include_directories(${target}
    PUBLIC
        ../../../sdk/include
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
    ${PROJECT_SOURCE_DIR}/../../../sdk/lib/${PLATFORM}
)

# libs
if(OS STREQUAL "windows")
    target_link_libraries(
        ${target}
        AVBlocks64.dll
        Shlwapi 
    )
endif()
