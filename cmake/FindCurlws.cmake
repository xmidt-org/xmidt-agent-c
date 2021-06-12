# SPDX-FileCopyrightText: 2010-2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

# find_curlws( [PATH "path"] [VERSION "1.1.2"] [GIT_TAG "v1.2.3"] )
#   PATH    - An alternate path to examine for the curlws package
#   VERSION - The expected version of the curlws package
#   GIT_TAG - The specific git tag to checkout if it comes to that

# Search Order:
#   1. User specified path
#   2. Existing installed path
#   3. Fetch a working copy & build it

function(find_curlws)

    cmake_parse_arguments(CURLWS "" "PATH;VERSION;GIT_TAG" "" ${ARGN})

    include(LocalInstallPaths)

    #message(STATUS "CURLWS_PATH    = \"${CURLWS_PATH}\"")
    #message(STATUS "CURLWS_VERSION = \"${CURLWS_VERSION}\"")
    #message(STATUS "CURLWS_GIT_TAG = \"${CURLWS_GIT_TAG}\"")

    if (NOT DEFINED CURLWS_VERSION)
        set(CURLWS_ENFORCE_VERSION "curlws")
        if (NOT DEFINED CURLWS_GIT_TAG)
            set(CURLWS_GIT_TAG "")
        endif()
    else ()
        set(CURLWS_ENFORCE_VERSION "curlws>=${CURLWS_VERSION}")
        if (NOT DEFINED CURLWS_GIT_TAG)
            set(CURLWS_GIT_TAG "v${CURLWS_VERSION}")
        endif()
    endif()

    find_path(CURLWS_INCLUDE_DIR
              NAMES "curlws/curlws.h"
              PATHS "${CMAKE_CURRENT_BINARY_DIR}/${CURLWS_PATH}"
              PATH_SUFFIXES "include"
              NO_DEFAULT_PATH)

    find_library(CURLWS_LIBRARY_DIR
                 NAMES "libcurlws.so"
                 PATHS "${CMAKE_CURRENT_BINARY_DIR}/${CURLWS_PATH}"
                 PATH_SUFFIXES "lib" "lib64"
                 NO_DEFAULT_PATH)

    if (NOT (CURLWS_INCLUDE_DIR MATCHES "-NOTFOUND" OR CURLWS_LIBRARY_DIR MATCHES "-NOTFOUND"))
        message(STATUS "Found user specified curlws (at: \"${CURLWS_PATH}\")")
        include_directories(SYSTEM ${CURLWS_INCLUDE_DIR})
        set(CURLWS_LIBRARIES ${CURLWS_LIBRARY_DIR} PARENT_SCOPE)
    else()
        include(FindPkgConfig)

        pkg_check_modules(CURLWS QUIET ${CURLWS_ENFORCE_VERSION})
        if (CURLWS_FOUND EQUAL 1)
            message(STATUS "Using system provided curlws (found version \"${CURLWS_VERSION}\")")
            include_directories(SYSTEM ${CURLWS_INCLUDE_DIRS})
        else()
            include(ExternalProject)

            message(STATUS "Fetching upstream curlws (tag \"${CURLWS_GIT_TAG}\")")
            ExternalProject_Add(curlws
                PREFIX ${LOCAL_PREFIX_DIR}/curlws
                GIT_REPOSITORY https://github.com/xmidt-org/curlws.git
                GIT_TAG ${CURLWS_GIT_TAG}
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${LOCAL_INSTALL_DIR} -DBUILD_TESTING=OFF)
            add_library(libcurlws STATIC IMPORTED)
            add_dependencies(libcurlws curlws)
            add_dependencies(${CMAKE_PROJECT_NAME} curlws)
            include_directories(SYSTEM ${LOCAL_INCLUDE_DIR})
            set(CURLWS_LIBRARIES "${LOCAL_LIBRARY_DIR}/libcurlws.so" PARENT_SCOPE)
        endif()
    endif()
endfunction()
