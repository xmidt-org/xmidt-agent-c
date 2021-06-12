# SPDX-FileCopyrightText: 2016-2021 Comcast Cable Communications Management, LLC
# SPDX-License-Identifier: Apache-2.0

target_sources(xmidt-agent
    PRIVATE
        main.c
)

configure_file(xmidt-agent-ver.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/xmidt-agent-ver.h)

set_target_properties(xmidt-agent PROPERTIES
        OUTPUT_NAME xmidt_agent)

install(TARGETS xmidt-agent RUNTIME DESTINATION bin)

