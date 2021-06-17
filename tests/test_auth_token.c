/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <CUnit/Basic.h>
#include <curl/curl.h>

#include "../src/auth_token.h"

#include "curl_mocks.c"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define validate_and_reset(...)                       \
    do {                                              \
        const char *test[] = { __VA_ARGS__ };         \
        curl_slist_compare(__curl_easy_setopt, test); \
        curl_slist_free_all(__curl_easy_setopt);      \
        __curl_easy_setopt = NULL;                    \
    } while (0)

#define reset_setopt()                           \
    do {                                         \
        curl_slist_free_all(__curl_easy_setopt); \
        __curl_easy_setopt = NULL;               \
    } while (0)

#define str(s) #s
#define xstr(s) str(s)

/* The latest version */
#define MY_CURL_SSLVERSION_TLS 65536

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

void test_inputs_00() /* All the options basically */
{
    int brw = 123;
    const struct auth_info in = {
        .url = "https://example.com",
        .interface = "eth0",
        .timeout = 12,
        .ip_resolve = CURL_IPRESOLVE_V4,
        .verbose_stream = stdout,
        .max_redirects = 50,
        .client_cert_path = "/cert/path",
        .private_key_path = "/key/path",
        .ca_bundle_path = NULL,
        .tls_version = 0,
        .mac_address = "mac:112233445566",
        .serial_number = "sn:abcdef",
        .uuid = "1bbb60c8-22ef-4321-a5ca-7216b716f807",
        .partner_id = "partner",
        .hardware_model = "model 1",
        .hardware_manufacturer = "maker 0",
        .firmware_name = "firmware 12",
        .protocol = "xmidt-proto 1.3",
        .last_reboot_reason = "power loss",
        .last_reconnect_reason = "connection broken",
        .boot_retry_wait = &brw,
    };
    struct curl_easy_perform_data write[2] = {
        {
            .next = NULL,
            .payload = "abcde",
            .len = 5,
        },
        {
            .next = NULL,
            .payload = "12345",
            .len = 5,
        },
    };
    struct auth_response out;

    write[0].next = &write[1];
    __curl_easy_perform = write;

    CU_ASSERT(CURLE_OK == auth_token_req(&in, &out));

    validate_and_reset("CURLOPT_URL              : https://example.com",
                       "CURLOPT_SSLVERSION       : " xstr(MY_CURL_SSLVERSION_TLS),
                       "CURLOPT_WRITEFUNCTION    : pointer",
                       "CURLOPT_WRITEDATA        : pointer",
                       "CURLOPT_FOLLOWLOCATION   : 1",
                       "CURLOPT_MAXREDIRS        : 50",
                       "CURLOPT_TIMEOUT          : 12",
                       "CURLOPT_INTERFACE        : eth0",
                       "CURLOPT_IPRESOLVE        : " xstr(CURL_IPRESOLVE_V4),
                       "CURLOPT_DNS_CACHE_TIMEOUT: 0",
                       "CURLOPT_FORBID_REUSE     : 1",
                       "CURLOPT_FRESH_CONNECT    : 1",
                       "CURLOPT_SSL_VERIFYHOST   : 2",
                       "CURLOPT_SSL_VERIFYPEER   : 1",
                       "CURLOPT_SSLCERT          : /cert/path",
                       "CURLOPT_SSLKEY           : /key/path",
                       "CURLOPT_VERBOSE          : 1",
                       "CURLOPT_STDERR           : pointer",
                       "X-Midt-Boot-Retry-Wait: 123",
                       "X-Midt-Mac-Address: mac:112233445566",
                       "X-Midt-Serial-Number: sn:abcdef",
                       "X-Midt-Uuid: 1bbb60c8-22ef-4321-a5ca-7216b716f807",
                       "X-Midt-Partner-Id: partner",
                       "X-Midt-Hardware-Model: model 1",
                       "X-Midt-Hardware-Manufacturer: maker 0",
                       "X-Midt-Firmware-Name: firmware 12",
                       "X-Midt-Protocol: xmidt-proto 1.3",
                       "X-Midt-Interface-Used: eth0",
                       "X-Midt-Last-Reboot-Reason: power loss",
                       "X-Midt-Last-Reconnect-Reason: connection broken");

    CU_ASSERT_FATAL(NULL != out.payload);
    CU_ASSERT_FATAL(10 == out.len);
    CU_ASSERT_FATAL(REQ_STATE__COMPLETED == out.state);
    CU_ASSERT_FATAL(CURLE_OK == out.curl_rv);
    CU_ASSERT_NSTRING_EQUAL(out.payload, "abcde12345", 10);
    free(out.payload);
}


void test_inputs_01() /* Let some options be missing & set tls to something else */
{
    const struct auth_info in = {
        .url = "https://example.com",
        .interface = NULL,
        .timeout = 0,
        .ip_resolve = CURL_IPRESOLVE_V6,
        .verbose_stream = NULL,
        .max_redirects = 0,
        .client_cert_path = NULL,
        .private_key_path = NULL,
        .ca_bundle_path = NULL,
        .tls_version = 12,
        .mac_address = NULL,
        .serial_number = NULL,
        .uuid = NULL,
        .partner_id = NULL,
        .hardware_model = NULL,
        .hardware_manufacturer = NULL,
        .firmware_name = NULL,
        .protocol = NULL,
        .last_reboot_reason = NULL,
        .last_reconnect_reason = NULL,
        .boot_retry_wait = NULL,
    };
    struct auth_response out;

    auth_token_req(&in, &out);

    validate_and_reset("CURLOPT_URL              : https://example.com",
                       "CURLOPT_SSLVERSION       : 12",
                       "CURLOPT_WRITEFUNCTION    : pointer",
                       "CURLOPT_WRITEDATA        : pointer",
                       "CURLOPT_FOLLOWLOCATION   : 1",
                       "CURLOPT_MAXREDIRS        : 0",
                       "CURLOPT_TIMEOUT          : 0",
                       "CURLOPT_IPRESOLVE        : " xstr(CURL_IPRESOLVE_V6),
                       "CURLOPT_DNS_CACHE_TIMEOUT: 0",
                       "CURLOPT_FORBID_REUSE     : 1",
                       "CURLOPT_FRESH_CONNECT    : 1",
                       "CURLOPT_SSL_VERIFYHOST   : 2",
                       "CURLOPT_SSL_VERIFYPEER   : 1");
}

void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("utils.c tests", NULL, NULL);
    CU_add_test(*suite, "inputs_00 Tests", test_inputs_00);
    CU_add_test(*suite, "inputs_01 Tests", test_inputs_01);
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(void)
{
    unsigned rv = 1;
    CU_pSuite suite = NULL;

    if (CUE_SUCCESS == CU_initialize_registry()) {
        add_suites(&suite);

        if (NULL != suite) {
            CU_basic_set_mode(CU_BRM_VERBOSE);
            CU_basic_run_tests();
            printf("\n");
            CU_basic_show_failures(CU_get_failure_list());
            printf("\n\n");
            rv = CU_get_number_of_tests_failed();
        }

        CU_cleanup_registry();
    }

    if (0 != rv) {
        return 1;
    }
    return 0;
}
