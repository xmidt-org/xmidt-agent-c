# SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

find_program(LCOV_BIN lcov)
find_program(GENINFO_BIN geninfo)
find_program(GENHTML_BIN genhtml)

if (NOT LCOV_BIN)
    message(STATUS "lcov not found, coverage report not supported.")
    return()
endif (NOT LCOV_BIN)

message(STATUS "Found lcov")

add_custom_target(coverage
                  COMMAND lcov -q --capture
                          --directory ${CMAKE_CURRENT_BINARY_DIR}/
                          --output-file coverage.info
                  COMMAND genhtml coverage.info ${LCOV_DARK_MODE}
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
