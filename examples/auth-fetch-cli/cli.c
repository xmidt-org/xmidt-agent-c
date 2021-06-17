/*
 * SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/auth_token.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

/* A simple options parser helper. */
static bool is_opt(const char *in, const char *s1, const char *s2)
{
    if (s1 && s2) {
        return ((0 == strcmp(in, s1)) || (0 == strcmp(in, s2))) ? true : false;
    } else if (s2) {
        return (0 == strcmp(in, s2)) ? true : false;
    } else if (s1) {
        return (0 == strcmp(in, s1)) ? true : false;
    }
    return false;
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    struct auth_info cfg;
    struct auth_response res;
    long max_redirs = 0;
    int brw = 0;
    CURLcode rv;

    /* Select the defaults & overwrite them as needed. */
    memset(&cfg, 0, sizeof(struct auth_info));

    /* Very simple args parser. */
    for (int i = 1; i < argc; i++) {
        if (is_opt(argv[i], "-4", NULL)) {
            cfg.ip_resolve = CURL_IPRESOLVE_V4;
        } else if (is_opt(argv[i], "-6", NULL)) {
            cfg.ip_resolve = CURL_IPRESOLVE_V6;
        } else if (is_opt(argv[i], "-h", "--help")) {
            printf("Usage: %s [options...] <url>\n"
                   " -4                               Resolve names to IPv4 addresses\n"
                   " -6                               Resolve names to IPv6 addresses\n"
                   " -h, --help                       This help text\n"
                   "     --interface       <name>     Use network INTERFACE (or address)\n"
                   " -L, --location                   Follow redirects\n"
                   "     --max-redirs      <num>      Maximum number of redirects allowed\n"
                   "     --tlsv1.2                    Set the maximum TLS version (useful since Wireshare can only decode tls1.2\n"
                   " -v  --verbose                    Verbose debugging is enabled, repeat for more\n"
                   " -m  --max-time        <seconds>  Maximum time in seconds that you allow the whole operation to take.\n"
                   " -E  --cert            <file>     Tells the program to use the specified client certificate file.\n"
                   "     --capath          <path>     Tells the program to use the specified CA bundle file.\n"
                   "     --key             <file>     Private key file name.\n"
                   "     --mac             <string>   The MAC address to use.\n"
                   "     --sn              <string>   The serial number to use.\n"
                   "     --uuid            <uuid>     The uuid to use for the transaction.\n"
                   "     --partner-id      <string>   The partner id to use.\n"
                   "     --hw-model        <string>   The hardware model to use.\n"
                   "     --hw-manuf        <string>   The hardware manufacturer to use.\n"
                   "     --fw-name         <string>   The firmware name to use.\n"
                   "     --xmidt-proto     <string>   The xmidt protocol name to use.\n"
                   "     --last-reboot     <string>   The last reboot reason to use.\n"
                   "     --last-reconn     <string>   The last reconnect reason to use.\n"
                   "     --boot-retry-wait <num>   The last reconnect reason to use.\n",
                   argv[0]);
            return 0;
        } else if (is_opt(argv[i], NULL, "--interface")) {
            i++;
            cfg.interface = argv[i];
        } else if (is_opt(argv[i], "-L", "--location")) {
            cfg.max_redirects = -1;
        } else if (is_opt(argv[i], NULL, "--max-redirs")) {
            i++;
            max_redirs = atol(argv[i]);
        } else if (is_opt(argv[i], NULL, "--tlsv1.2")) {
            cfg.tls_version = CURL_SSLVERSION_MAX_TLSv1_2;
        } else if (is_opt(argv[i], "-v", "--verbose")) {
            cfg.verbose_stream = stdout;
        } else if (is_opt(argv[i], "-m", "--max-time")) {
            cfg.timeout = atol(argv[i]);
        } else if (is_opt(argv[i], NULL, "--capath")) {
            i++;
            cfg.private_key_path = argv[i];
        } else if (is_opt(argv[i], "-E", "--cert")) {
            i++;
            cfg.client_cert_path = argv[i];
        } else if (is_opt(argv[i], NULL, "--key")) {
            i++;
            cfg.private_key_path = argv[i];
        } else if (is_opt(argv[i], NULL, "--mac")) {
            i++;
            cfg.mac_address = argv[i];
        } else if (is_opt(argv[i], NULL, "--sn")) {
            i++;
            cfg.serial_number = argv[i];
        } else if (is_opt(argv[i], NULL, "--uuid")) {
            i++;
            cfg.uuid = argv[i];
        } else if (is_opt(argv[i], NULL, "--partner-id")) {
            i++;
            cfg.partner_id = argv[i];
        } else if (is_opt(argv[i], NULL, "--hw-model")) {
            i++;
            cfg.hardware_model = argv[i];
        } else if (is_opt(argv[i], NULL, "--hw-manuf")) {
            i++;
            cfg.hardware_manufacturer = argv[i];
        } else if (is_opt(argv[i], NULL, "--fw-name")) {
            i++;
            cfg.firmware_name = argv[i];
        } else if (is_opt(argv[i], NULL, "--xmidt-proto")) {
            i++;
            cfg.protocol = argv[i];
        } else if (is_opt(argv[i], NULL, "--last-reboot")) {
            i++;
            cfg.last_reboot_reason = argv[i];
        } else if (is_opt(argv[i], NULL, "--last-reconn")) {
            i++;
            cfg.last_reconnect_reason = argv[i];
        } else if (is_opt(argv[i], NULL, "--boot-retry-wait")) {
            i++;
            brw = atoi(argv[i]);
            cfg.boot_retry_wait = &brw;
        } else {
            cfg.url = argv[i];
            break;
        }
    }

    if (-1 == cfg.max_redirects) {
        if (0 != max_redirs) {
            cfg.max_redirects = max_redirs;
        }
    }

    /* Create the curl global context. */
    curl_global_init(CURL_GLOBAL_DEFAULT);

    rv = auth_token_req(&cfg, &res);

    printf("auth_token_req() rv = %d\n", rv);
    printf("overall state: %d\n", res.state);
    printf("      curl rv: %d\n", res.curl_rv);
    printf("  http status: %ld\n", res.http_status);
    printf("  payload len: %zd bytes\n", res.len);
    printf("  retry after: %ld s\n", res.retry_after);
    printf("   namelookup: %f s\n", res.namelookup);
    printf("      connect: %f s\n", res.connect);
    printf("   appconnect: %f s\n", res.appconnect);
    printf("  pretransfer: %f s\n", res.pretransfer);
    printf("starttransfer: %f s\n", res.starttransfer);
    printf("        total: %f s\n", res.total);
    printf("     redirect: %f s\n", res.redirect);
    printf("     payload:\n%.*s\n", (int)res.len, res.payload);

    if (res.payload) {
        free(res.payload);
    }

    return 0;
}

