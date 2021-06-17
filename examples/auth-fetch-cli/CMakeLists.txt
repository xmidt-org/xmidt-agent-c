# SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

add_executable(auth_fetch_cli)

target_sources(auth_fetch_cli
    PRIVATE
        cli.c
        ../../src/auth_token.c
)



target_link_libraries(auth_fetch_cli ${CURL_LIBRARIES})
target_link_libraries(auth_fetch_cli ${UUID_LIBRARIES})

install(TARGETS auth_fetch_cli RUNTIME DESTINATION bin)

