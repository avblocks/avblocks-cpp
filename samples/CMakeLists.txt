cmake_minimum_required(VERSION 3.16)

# meta project that includes all other projects
project(samples)

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

# add projects, each project should be in its own directory
if (OS STREQUAL "darwin")
    add_subdirectory(${OS}/dec_avc_au)
endif()

if(OS STREQUAL "linux")
    add_subdirectory(${OS}/dec_avc_au)
endif()

if(OS STREQUAL "windows")
    add_subdirectory(${OS}/dec_avc_au)

    # copy avblocks
    set(SDK_LIB_PATH ${PROJECT_SOURCE_DIR}/../sdk/lib/${PLATFORM})

    set(BUILD_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM})

    if (CMAKE_GENERATOR MATCHES "Visual Studio.*20.*")
        set(BUILD_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/Debug)
        set(BUILD_OUTPUT_PATH_RELEASE ${PROJECT_SOURCE_DIR}/../bin/Release)
    endif()

    add_custom_target(copy_sdk ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_OUTPUT_PATH}
        COMMAND cmake -E copy_directory ${SDK_LIB_PATH} ${BUILD_OUTPUT_PATH}
    )

    # dependencies
    add_dependencies(dec_avc_au copy_sdk)

    if (CMAKE_GENERATOR MATCHES "Visual Studio.*20.*")
        add_custom_target(copy_sdk_release ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_OUTPUT_PATH_RELEASE}
            COMMAND cmake -E copy_directory ${SDK_LIB_PATH} ${BUILD_OUTPUT_PATH_RELEASE}
        )

        # dependencies
        add_dependencies(dec_avc_au copy_sdk_release)
    endif()    
endif()



