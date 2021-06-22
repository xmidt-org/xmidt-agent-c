# SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

if(NOT DISABLE_VALGRIND)
set (MEMORY_CHECK valgrind --leak-check=full --show-reachable=yes  --error-exitcode=1 -v)
endif ()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage -O0 -finstrument-functions")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage -O0 -finstrument-functions")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -O0 -finstrument-functions")


# ---------------------------------------------------------------------------- #
add_test(NAME test_auth_token COMMAND ${MEMORY_CHECK} ./test_auth_token)

add_executable(test_auth_token test_auth_token.c
                               ../src/auth_token.c
                               ../src/libc/xa_utils.c)

target_link_libraries(test_auth_token gcov)
target_link_libraries(test_auth_token ${CUNIT_LIBRARIES})
target_link_libraries(test_auth_token ${CURL_LIBRARIES})
# ---------------------------------------------------------------------------- #


# ---------------------------------------------------------------------------- #
add_test(NAME test_utils COMMAND ${MEMORY_CHECK} ./test_utils)

add_executable(test_utils test_utils.c ../src/libc/xa_utils.c)

target_link_libraries(test_utils gcov)
target_link_libraries(test_utils ${CUNIT_LIBRARIES})
# ---------------------------------------------------------------------------- #


# ---------------------------------------------------------------------------- #
add_test(NAME test_time COMMAND ${MEMORY_CHECK} ./test_time)

add_executable(test_time test_time.c ../src/libc/xa_time.c)

target_link_libraries(test_time gcov)
target_link_libraries(test_time ${CUNIT_LIBRARIES})
# ---------------------------------------------------------------------------- #


# ---------------------------------------------------------------------------- #
add_test(NAME test_xxd COMMAND ${MEMORY_CHECK} ./test_xxd)

add_executable(test_xxd test_xxd.c ../src/libc/xa_xxd.c)

target_link_libraries(test_xxd gcov)
target_link_libraries(test_xxd ${CUNIT_LIBRARIES})
# ---------------------------------------------------------------------------- #
