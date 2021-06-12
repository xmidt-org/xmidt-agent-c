# SPDX-FileCopyrightText: 2010-2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

# find_uuid( [PATH "path"] [VERSION "1.1.2"] [GIT_TAG "v1.2.3"] )
#   PATH    - An alternate path to examine for the libuuid package
#   VERSION - The expected version of the libuuid package
#   GIT_TAG - The specific git tag to checkout if it comes to that

# Search Order:
#   1. User specified path
#   2. Existing installed path
#   3. Fetch a working copy & build it

function(find_libuuid)

    cmake_parse_arguments(UUID "" "PATH;VERSION;GIT_TAG" "" ${ARGN})

    include(LocalInstallPaths)

    #message(STATUS "UUID_PATH    = \"${UUID_PATH}\"")
    #message(STATUS "UUID_VERSION = \"${UUID_VERSION}\"")
    #message(STATUS "UUID_GIT_TAG = \"${UUID_GIT_TAG}\"")

    if (NOT DEFINED UUID_VERSION)
        set(UUID_ENFORCE_VERSION "uuid")
        if (NOT DEFINED UUID_GIT_TAG)
            set(UUID_GIT_TAG "")
        endif()
    else ()
        set(UUID_ENFORCE_VERSION "uuid>=${UUID_VERSION}")
        if (NOT DEFINED UUID_GIT_TAG)
            set(UUID_GIT_TAG "v${UUID_VERSION}")
        endif()
    endif()

    find_path(UUID_INCLUDE_DIR
              NAMES "uuid/uuid.h"
              PATHS "${CMAKE_CURRENT_BINARY_DIR}/${UUID_PATH}"
              PATH_SUFFIXES "include"
              NO_DEFAULT_PATH)

    find_library(UUID_LIBRARY_DIR
                 NAMES "libuuid.so"
                 PATHS "${CMAKE_CURRENT_BINARY_DIR}/${UUID_PATH}"
                 PATH_SUFFIXES "lib" "lib64"
                 NO_DEFAULT_PATH)

    if (NOT (UUID_INCLUDE_DIR MATCHES "-NOTFOUND" OR UUID_LIBRARY_DIR MATCHES "-NOTFOUND"))
        message(STATUS "Found user specified libuuid (at: \"${UUID_PATH}\")")
        include_directories(SYSTEM ${UUID_INCLUDE_DIR})
        set(UUID_LIBRARIES ${UUID_LIBRARY_DIR} PARENT_SCOPE)
    else()
        include(FindPkgConfig)

        pkg_check_modules(UUID QUIET ${UUID_ENFORCE_VERSION})
        if (UUID_FOUND EQUAL 1)
            message(STATUS "Using system provided libuuid (found version \"${UUID_VERSION}\")")
            include_directories(SYSTEM ${UUID_INCLUDE_DIRS})
        else()
            message(FATAL_ERROR "Need system provided libuuid.")
        endif()
    endif()
endfunction()
