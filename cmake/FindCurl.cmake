# SPDX-FileCopyrightText: 2010-2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

# find_curl( [PATH "path"] [VERSION "1.1.2"] [GIT_TAG "v1.2.3"] )
#   PATH    - An alternate path to examine for the curl package
#   VERSION - The expected version of the curl package
#   GIT_TAG - The specific git tag to checkout if it comes to that

# Search Order:
#   1. User specified path
#   2. Existing installed path
#   3. Fetch a working copy & build it

function(find_curl)

    cmake_parse_arguments(CURL "" "PATH;VERSION;GIT_TAG" "" ${ARGN})

    include(LocalInstallPaths)

    #message(STATUS "CURL_PATH    = \"${CURL_PATH}\"")
    #message(STATUS "CURL_VERSION = \"${CURL_VERSION}\"")
    #message(STATUS "CURL_GIT_TAG = \"${CURL_GIT_TAG}\"")

    if (NOT DEFINED CURL_VERSION)
        set(CURL_ENFORCE_VERSION "libcurl")
        if (NOT DEFINED CURL_GIT_TAG)
            set(CURL_GIT_TAG "")
        endif()
    else ()
        set(CURL_ENFORCE_VERSION "libcurl>=${CURL_VERSION}")
        if (NOT DEFINED CURL_GIT_TAG)
            set(CURL_GIT_TAG "v${CURL_VERSION}")
        endif()
    endif()

    find_path(CURL_INCLUDE_DIR
              NAMES "curl/curl.h"
              PATHS "${CMAKE_CURRENT_BINARY_DIR}/${CURL_PATH}"
              PATH_SUFFIXES "include"
              NO_DEFAULT_PATH)

    find_library(CURL_LIBRARY_DIR
                 NAMES "libcurl.so"
                 PATHS "${CMAKE_CURRENT_BINARY_DIR}/${CURL_PATH}"
                 PATH_SUFFIXES "lib" "lib64"
                 NO_DEFAULT_PATH)

    if (NOT (CURL_INCLUDE_DIR MATCHES "-NOTFOUND" OR CURL_LIBRARY_DIR MATCHES "-NOTFOUND"))
        message(STATUS "Found user specified libcurl (at: \"${CURL_PATH}\")")
        include_directories(SYSTEM ${CURL_INCLUDE_DIR})
        set(CURL_LIBRARIES ${CURL_LIBRARY_DIR} PARENT_SCOPE)
    else()
        include(FindPkgConfig)

        pkg_check_modules(CURL QUIET ${CURL_ENFORCE_VERSION})
        if (CURL_FOUND EQUAL 1)
            message(STATUS "Using system provided libcurl (found version \"${CURL_VERSION}\")")
            include_directories(SYSTEM ${CURL_INCLUDE_DIRS})
        else()
            include(ExternalProject)

            message(STATUS "Fetching upstream curl (tag \"${CURL_GIT_TAG}\")")
            ExternalProject_Add(curl
                PREFIX ${LOCAL_PREFIX_DIR}/curl
                GIT_REPOSITORY https://github.com/xmidt-org/curl.git
                GIT_TAG ${CURL_GIT_TAG}
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${LOCAL_INSTALL_DIR} -DBUILD_TESTING=OFF)
            add_library(libcurl STATIC IMPORTED)
            add_dependencies(libcurl curl)
            add_dependencies(${CMAKE_PROJECT_NAME} curl)
            include_directories(SYSTEM ${LOCAL_INCLUDE_DIR})
            set(CURL_LIBRARIES "${LOCAL_LIBRARY_DIR}/libcurl.so" PARENT_SCOPE)
        endif()
    endif()
endfunction()
