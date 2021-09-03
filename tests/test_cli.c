/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <CUnit/Basic.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/config/config.h"
#include "../src/config/internal.h"

config_t *config_read(const char *path, XAcode *rv)
{
    static config_t h;
    (void)path;
    (void)rv;
    return &h;
}

void test_good1(void)
{
    const char *argv[] = { "program", "--config.dir", "path" };
    XAcode rv = XA_OK;

    CU_ASSERT(NULL != config_from_cli(3, argv, &rv));
    CU_ASSERT(XA_OK == rv);
}


void test_good2(void)
{
    const char *argv[] = { "program", "--config.dir=path" };
    XAcode rv = XA_OK;

    CU_ASSERT(NULL != config_from_cli(2, argv, &rv));
    CU_ASSERT(XA_OK == rv);
}


void test_bad1(void)
{
    const char *argv[] = { "program" };
    XAcode rv = XA_OK;

    CU_ASSERT(NULL == config_from_cli(1, argv, &rv));
    CU_ASSERT(XA_CLI_ERROR == rv);
}


void test_bad2(void)
{
    const char *argv[] = { "program", "--not_legit", "path" };
    XAcode rv = XA_OK;

    CU_ASSERT(NULL == config_from_cli(3, argv, &rv));
    CU_ASSERT(XA_CLI_ERROR == rv);
}


void test_bad3(void)
{
    const char *argv[] = { "program", "--not_legit" };
    XAcode rv = XA_OK;

    CU_ASSERT(NULL == config_from_cli(2, argv, &rv));
    CU_ASSERT(XA_CLI_ERROR == rv);
}


void test_bad4(void)
{
    const char *argv[] = { "program", "--config.dir=" };
    XAcode rv = XA_OK;

    CU_ASSERT(NULL == config_from_cli(2, argv, &rv));
    CU_ASSERT(XA_CLI_ERROR == rv);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("cli.c tests", NULL, NULL);
    CU_add_test(*suite, "Good 3 arg Test", test_good1);
    CU_add_test(*suite, "Good 2 arg Test", test_good2);
    CU_add_test(*suite, "Bad arg count Test", test_bad1);
    CU_add_test(*suite, "Bad arg value Test", test_bad2);
    CU_add_test(*suite, "Bad arg value Test", test_bad3);
    CU_add_test(*suite, "Bad arg value Test", test_bad4);
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
