# SPDX-FileCopyrightText: 2010-2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

# find_curl( [PATH "path"] [VERSION "1.1.2"] [GIT_TAG "v1.2.3"] )
#   VERSION - The expected version of the curl package

# Search Order:
#   1. Existing installed path

function(find_curl)

    cmake_parse_arguments(CURL "" "VERSION" "" ${ARGN})

    include(FindCURL)

    if (NOT (CURL_FOUND MATCHES TRUE))
        message(FATAL_ERROR "Unable to find libcurl.")
    endif()

    if (VERSION VERSION_GREATER CURL_VERSION_STRING)
        message(FATAL_ERROR "libcurl version: ${CURL_VERSION_STRING} older than needed: ${VERSION}")
    endif()

    message(STATUS "Using system provided libcurl (found version \"${CURL_VERSION_STRING}\")")
    include_directories(SYSTEM ${CURL_INCLUDE_DIRS})
    set(CURL_LIBRARIES ${CURL_LIBRARY} PARENT_SCOPE)
endfunction()
