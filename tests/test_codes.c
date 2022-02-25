/* SPDX-FileCopyrightText: 2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <CUnit/Basic.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/error/codes.h"

#define TEST(a) CU_ASSERT_STRING_EQUAL(xa_error_to_string(a), #a)


void test_error_to_string(void)
{
    TEST(XA_OK);
    TEST(XA_OUT_OF_MEMORY);
    TEST(XA_DNS_FORMAT_ERROR);
    TEST(XA_DNS_SERVER_ERROR);
    TEST(XA_DNS_NAME_ERROR);
    TEST(XA_DNS_NOT_IMPLEMENTED);
    TEST(XA_DNS_REFUSED);
    TEST(XA_DNS_UNKNOWN_VALUE);
    TEST(XA_DNS_TOO_FEW_ANSWERS);
    TEST(XA_DNS_RESOLVER_ERROR);
    TEST(XA_JWT_DECODE_ERROR);
    TEST(XA_INVALID_INPUT);
    TEST(XA_DNS_RECORD_TOO_SHORT);
    TEST(XA_DNS_RECORD_INVALID);
    TEST(XA_DNS_TOKEN_NOT_PRESENT);
    TEST(XA_NOT_A_DIR);
    TEST(XA_PATH_TOO_LONG);
    TEST(XA_FAILED_TO_OPEN_FILE);
    TEST(XA_FAILED_TO_READ_FILE);
    TEST(XA_CLI_ERROR);
    TEST(XA_CONFIG_FILE_ERROR);
    TEST(XA_INSUFFICIENT_RESOURCES);

    CU_ASSERT_STRING_EQUAL(xa_error_to_string((XAcode) -1), "XAcode is out of bounds");
    CU_ASSERT_STRING_EQUAL(xa_error_to_string((XAcode) 1000), "XAcode is out of bounds");
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("codes.c tests", NULL, NULL);
    CU_add_test(*suite, "xa_error_to_string() Tests", test_error_to_string);
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(void)
{
    unsigned rv     = 1;
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
