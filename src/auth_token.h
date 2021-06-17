/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __AUTH_TOKEN_H__
#define __AUTH_TOKEN_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <curl/curl.h>

struct auth_info {
    /* The FQDN URL to query. */
    const char *url;

    /* The local network interface to use for the request. */
    const char *interface;

    /* The timeout in seconds to wait until we completely give up. */
    long timeout;

    /* The IP version to use for the request.
     *
     * CURL_IPRESOLVE_WHATEVER = it's up to curl (default = 0)
     * CURL_IPRESOLVE_V4       = IPv4
     * CURL_IPRESOLVE_V6       = IPv6
     */
    long ip_resolve;

    /* If set to non-NULL curl debug information and sends it here. */
    FILE *verbose_stream;

    /* Set to -1 for unlimited, 0 for none, 1+ for a normal max. */
    long max_redirects;

    /* The mTLS cert PEM file path. */
    const char *client_cert_path;

    /* The cert private key path. */
    const char *private_key_path;

    /* The CA bundle to use if not the standard one. */
    const char *ca_bundle_path;

    /* The TLS version to use in place of the default if not 0.
     * The default is CURL_SSLVERSION_MAX_DEFAULT. */
    long tls_version;

    /* The metadata headers to send to the issuer. */
    const char *mac_address;
    const char *serial_number;
    const char *uuid;
    const char *partner_id;
    const char *hardware_model;
    const char *hardware_manufacturer;
    const char *firmware_name;
    const char *protocol;
    const char *last_reboot_reason;
    const char *last_reconnect_reason;
    int *boot_retry_wait;
};


enum req_state {
    /* The call didn't complete the initialization phase due to an error. */
    REQ_STATE__INIT = 0,

    /* The call performed the call to the server, but there were issues. */
    REQ_STATE__PERFORMED,

    /* The call made the call and got a response.  It is up to the caller
     * to check to see if the results are good. */
    REQ_STATE__COMPLETED
};


struct auth_response {
    /* The payload or NULL. */
    uint8_t *payload;

    size_t len;

    /* The number of seconds to wait before retrying if the server sent a
     * 429 http status code, or 0. */
    curl_off_t retry_after;

    /* The HTTP status code or 0. */
    long http_status;

    /* The resulting curl return code. */
    CURLcode curl_rv;

    /* The furthest state achieved before success/failure. */
    enum req_state state;

    /* See https://curl.se/libcurl/c/curl_easy_getinfo.html section
     * TIMES for full details. */
    double namelookup;
    double connect;
    double appconnect;
    double pretransfer;
    double starttransfer;
    double total;
    double redirect;
};


/**
 *  Based on the inputs this makes a request for the auth credentials needed to
 *  connect to the rest of the Xmidt cluster.  The response is the summarized
 *  output that could include a payload if successful, or details about the
 *  error encountered.
 *
 *  @note A success means the HTTP request was made & a response was received.
 *        Make sure to check the https_status to make sure you got what you
 *        wanted.
 *
 *  @param in the input information needed
 *  @param r  the resulting information (memory must be provided by the caller)
 *
 *  @return CURLE_OK on success, error otherwise
 */
CURLcode auth_token_req(const struct auth_info *in, struct auth_response *r);

#endif

