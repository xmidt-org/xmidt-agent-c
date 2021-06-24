# SPDX-FileCopyrightText: 2016-2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

target_sources(xmidt-agent
    PRIVATE
        main.c
        auth_token.c
        dns_token.c
)

configure_file(xmidt-agent-ver.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/xmidt-agent-ver.h)

set_target_properties(xmidt-agent PROPERTIES
        OUTPUT_NAME xmidt_agent)


target_link_libraries(xmidt-agent ${WRPC_LIBRARIES})
target_link_libraries(xmidt-agent ${CJWT_LIBRARIES})
target_link_libraries(xmidt-agent ${CJSON_LIBRARIES})
target_link_libraries(xmidt-agent ${CURL_LIBRARIES})
target_link_libraries(xmidt-agent ${CURLWS_LIBRARIES})
target_link_libraries(xmidt-agent ${UUID_LIBRARIES})
target_link_libraries(xmidt-agent ${MSGPACK_LIBRARIES})
target_link_libraries(xmidt-agent ${TROWER_LIBRARIES})
target_link_libraries(xmidt-agent ${OPENSSL_LIBRARIES})
target_link_libraries(xmidt-agent ${CIMPLOG_LIBRARIES})
target_link_libraries(xmidt-agent resolv)

install(TARGETS xmidt-agent RUNTIME DESTINATION bin)

add_subdirectory(libc)
