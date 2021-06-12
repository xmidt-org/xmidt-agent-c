# SPDX-FileCopyrightText: 2010-2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

# find_wrp-c( [PATH "path"] [VERSION "1.1.2"] [GIT_TAG "v1.2.3"] )
#   PATH    - An alternate path to examine for the wrp-c package
#   VERSION - The expected version of the wrp-c package
#   GIT_TAG - The specific git tag to checkout if it comes to that

# Search Order:
#   1. User specified path
#   2. Existing installed path
#   3. Fetch a working copy & build it

function(find_wrpc)

    cmake_parse_arguments(WRPC "" "PATH;VERSION;GIT_TAG" "" ${ARGN})

    include(LocalInstallPaths)

    #message(STATUS "WRPC_PATH    = \"${WRPC_PATH}\"")
    #message(STATUS "WRPC_VERSION = \"${WRPC_VERSION}\"")
    #message(STATUS "WRPC_GIT_TAG = \"${WRPC_GIT_TAG}\"")

    if (NOT DEFINED WRPC_VERSION)
        set(WRPC_ENFORCE_VERSION "wrp-c")
        if (NOT DEFINED WRPC_GIT_TAG)
            set(WRPC_GIT_TAG "")
        endif()
    else ()
        set(WRPC_ENFORCE_VERSION "wrp-c>=${WRPC_VERSION}")
        if (NOT DEFINED WRPC_GIT_TAG)
            set(WRPC_GIT_TAG "v${WRPC_VERSION}")
        endif()
    endif()

    find_path(WRPC_INCLUDE_DIR
              NAMES "wrp-c/wrp-c.h"
              PATHS "${CMAKE_CURRENT_BINARY_DIR}/${WRPC_PATH}"
              PATH_SUFFIXES "include"
              NO_DEFAULT_PATH)

    find_library(WRPC_LIBRARY_DIR
                 NAMES "libwrp-c.so"
                 PATHS "${CMAKE_CURRENT_BINARY_DIR}/${WRPC_PATH}"
                 PATH_SUFFIXES "lib" "lib64"
                 NO_DEFAULT_PATH)

    if (NOT (WRPC_INCLUDE_DIR MATCHES "-NOTFOUND" OR WRPC_LIBRARY_DIR MATCHES "-NOTFOUND"))
        message(STATUS "Found user specified wrp-c (at: \"${WRPC_PATH}\")")
        include_directories(SYSTEM ${WRPC_INCLUDE_DIR})
        set(WRPC_LIBRARIES ${WRPC_LIBRARY_DIR} PARENT_SCOPE)
    else()
        include(FindPkgConfig)

        pkg_check_modules(WRPC QUIET ${WRPC_ENFORCE_VERSION})
        if (WRPC_FOUND EQUAL 1)
            message(STATUS "Using system provided wrp-c (found version \"${WRPC_VERSION}\")")
            include_directories(SYSTEM ${WRPC_INCLUDE_DIRS})
        else()
            include(ExternalProject)

            message(STATUS "Fetching upstream wrp-c (tag \"${WRPC_GIT_TAG}\")")
            ExternalProject_Add(wrp-c
                PREFIX ${LOCAL_PREFIX_DIR}/wrp-c
                GIT_REPOSITORY https://github.com/xmidt-org/wrp-c.git
                GIT_TAG ${WRPC_GIT_TAG}
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${LOCAL_INSTALL_DIR} -DBUILD_TESTING=OFF)
            add_library(libwrp-c STATIC IMPORTED)
            add_dependencies(libwrp-c wrp-c)
            add_dependencies(${CMAKE_PROJECT_NAME} wrp-c)
            include_directories(SYSTEM ${LOCAL_INCLUDE_DIR})
            set(WRPC_LIBRARIES "${LOCAL_LIBRARY_DIR}/libwrp-c.so" PARENT_SCOPE)
        endif()
    endif()
endfunction()
