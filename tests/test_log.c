/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <CUnit/Basic.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/logging/log.h"


void test_most(void)
{
    /* Basically don't crash or leak memory */
    log_trace("Hello, world.");
    log_debug("%s", "Hello, world.");
    log_info("Hello, world.");
    log_warn("Hello, world.");
    log_error("Hello, world.");
    log_fatal("Hello, world.");
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("log.c tests", NULL, NULL);
    CU_add_test(*suite, "log_*() Tests", test_most);
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
