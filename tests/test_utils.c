/*
 * SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <CUnit/Basic.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/utils.h"


void test_xa_strdup()
{
    char *tmp;
    char *empty = "e";

    CU_ASSERT(NULL == xa_strdup(NULL));

    tmp = xa_strdup(&empty[1]);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT('\0' == *tmp);
    free(tmp);

    tmp = xa_strdup("asdf");
    CU_ASSERT(NULL != tmp);
    CU_ASSERT_STRING_EQUAL(tmp, "asdf");
    free(tmp);
}


void test_xa_strndup()
{
    char *tmp;
    char *empty = "e";
    CU_ASSERT(NULL == xa_strndup(NULL, 0));

    tmp = xa_strndup(&empty[1], 5);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT('\0' == *tmp);
    free(tmp);

    tmp = xa_strndup(&empty[1], 0);
    CU_ASSERT(NULL == tmp);

    tmp = xa_strndup("foo", 0);
    CU_ASSERT(NULL == tmp);

    tmp = xa_strndup("asdf", 12);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT_STRING_EQUAL(tmp, "asdf");
    free(tmp);

    tmp = xa_strndup("asdf", 2);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT_STRING_EQUAL(tmp, "as");
    free(tmp);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("utils.c tests", NULL, NULL);
    CU_add_test(*suite, "xa_strdup() Tests", test_xa_strdup);
    CU_add_test(*suite, "xa_strndup() Tests", test_xa_strndup);
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
