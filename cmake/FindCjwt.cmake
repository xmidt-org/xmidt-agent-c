# SPDX-FileCopyrightText: 2010-2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

# find_cjwt( [PATH "path"] [VERSION "1.1.2"] [GIT_TAG "v1.2.3"] )
#   PATH    - An alternate path to examine for the cjwt package
#   VERSION - The expected version of the cjwt package
#   GIT_TAG - The specific git tag to checkout if it comes to that

# Search Order:
#   1. User specified path
#   2. Existing installed path
#   3. Fetch a working copy & build it

function(find_cjwt)

    cmake_parse_arguments(CJWT "" "PATH;VERSION;GIT_TAG" "" ${ARGN})

    include(LocalInstallPaths)

    #message(STATUS "CJWT_PATH    = \"${CJWT_PATH}\"")
    #message(STATUS "CJWT_VERSION = \"${CJWT_VERSION}\"")
    #message(STATUS "CJWT_GIT_TAG = \"${CJWT_GIT_TAG}\"")

    if (NOT DEFINED CJWT_VERSION)
        set(CJWT_ENFORCE_VERSION "cjwt")
        if (NOT DEFINED CJWT_GIT_TAG)
            set(CJWT_GIT_TAG "")
        endif()
    else ()
        set(CJWT_ENFORCE_VERSION "cjwt>=${CJWT_VERSION}")
        if (NOT DEFINED CJWT_GIT_TAG)
            set(CJWT_GIT_TAG "v${CJWT_VERSION}")
        endif()
    endif()

    find_path(CJWT_INCLUDE_DIR
              NAMES "cjwt/cjwt.h"
              PATHS "${CMAKE_CURRENT_BINARY_DIR}/${CJWT_PATH}"
              PATH_SUFFIXES "include"
              NO_DEFAULT_PATH)

    find_library(CJWT_LIBRARY_DIR
                 NAMES "libcjwt.so"
                 PATHS "${CMAKE_CURRENT_BINARY_DIR}/${CJWT_PATH}"
                 PATH_SUFFIXES "lib" "lib64"
                 NO_DEFAULT_PATH)

    if (NOT (CJWT_INCLUDE_DIR MATCHES "-NOTFOUND" OR CJWT_LIBRARY_DIR MATCHES "-NOTFOUND"))
        message(STATUS "Found user specified cjwt (at: \"${CJWT_PATH}\")")
        include_directories(SYSTEM ${CJWT_INCLUDE_DIR})
        set(CJWT_LIBRARIES ${CJWT_LIBRARY_DIR} PARENT_SCOPE)
    else()
        include(FindPkgConfig)

        pkg_check_modules(CJWT QUIET ${CJWT_ENFORCE_VERSION})
        if (CJWT_FOUND EQUAL 1)
            message(STATUS "Using system provided cjwt (found version \"${CJWT_VERSION}\")")
            include_directories(SYSTEM ${CJWT_INCLUDE_DIRS})
        else()
            include(ExternalProject)

            message(STATUS "Fetching upstream cjwt (tag \"${CJWT_GIT_TAG}\")")
            ExternalProject_Add(cjwt
                PREFIX ${LOCAL_PREFIX_DIR}/cjwt
                GIT_REPOSITORY https://github.com/xmidt-org/cjwt.git
                GIT_TAG ${CJWT_GIT_TAG}
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${LOCAL_INSTALL_DIR} -DBUILD_TESTING=OFF)
            add_library(libcjwt STATIC IMPORTED)
            add_dependencies(libcjwt cjwt)
            add_dependencies(${CMAKE_PROJECT_NAME} cjwt)
            include_directories(SYSTEM ${LOCAL_INCLUDE_DIR})
            set(CJWT_LIBRARIES "${LOCAL_LIBRARY_DIR}/libcjwt.so" PARENT_SCOPE)
        endif()
    endif()
endfunction()
