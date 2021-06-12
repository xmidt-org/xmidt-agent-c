# SPDX-FileCopyrightText: 2010-2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

project(xmidt-agent VERSION 0.0.1)

include(BitwiseVersion)
include(CTest)
include(FindCimplog)
include(FindcJSON)
include(FindCjwt)
include(FindCurl)
include(FindCurlws)
include(FindLibUuid)
include(FindMsgpack)
include(FindTrowerBase64)
include(FindWrpc)
include(LicenseLinterTarget)
include(Coverage)

# TODO UCRESOLV

################################################################################
# Compile options/flags
################################################################################
add_definitions(-std=c99)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Werror -Wall -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wpedantic")

add_executable(xmidt-agent)

################################################################################
# Find or acquire software dependencies
################################################################################

find_cimplog(      PATH ${CIMPLOG_PATH} VERSION "1.0.3"  GIT_TAG "")
find_cjson(        PATH ${CJSON_PATH}   VERSION "1.7.14" GIT_TAG "")
find_cjwt(         PATH ${CJWT_PATH}    VERSION "2.0.0"  GIT_TAG "")
find_curl(         PATH ${CURL_PATH}    VERSION "7.50.2" GIT_TAG "")
find_curlws(       PATH ${CURLWS_PATH}  VERSION "1.0.3"  GIT_TAG "")
find_libuuid(      PATH ${UUID_PATH})
find_msgpack(      PATH ${MSGPACK_PATH} VERSION "3.0.0"  GIT_TAG "b6803a5fecbe321458faafd6a079dac466614ff9")
find_trower_base64(PATH ${TROWER_PATH}  VERSION "1.2.0"  GIT_TAG "")
find_wrpc(         PATH ${WRPC_PATH}    VERSION "1.1.1"  GIT_TAG "")

find_package(Threads REQUIRED)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIRS})

################################################################################
# Process the subdirectories now
################################################################################

add_subdirectory(src)

if (BUILD_TESTING)
    include(FindCunit)
	add_subdirectory(tests)
endif (BUILD_TESTING)
