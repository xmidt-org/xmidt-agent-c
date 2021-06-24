# SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

add_executable(dns_token_cli)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

target_sources(dns_token_cli
    PRIVATE
        cli.c
        ../../src/dns_token.c
        ../../src/libc/xa_file.c
        ../../src/libc/xa_time.c
        ../../src/libc/xa_utils.c
        ../../src/libc/xa_xxd.c
)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

target_link_libraries(dns_token_cli ${CJWT_LIBRARIES})
target_link_libraries(dns_token_cli ${CJSON_LIBRARIES})
target_link_libraries(dns_token_cli ${OPENSSL_LIBRARIES})
target_link_libraries(dns_token_cli ${TROWER_LIBRARIES})
target_link_libraries(dns_token_cli resolv)

install(TARGETS dns_token_cli RUNTIME DESTINATION bin)

