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

#include "../src/libc/xa_utils.h"


void test_xa_strnlen()
{
    CU_ASSERT(4 == xa_strnlen("1234", 10));
    CU_ASSERT(4 == xa_strnlen("12345678", 4));
}


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


void test_xa_tocase()
{
    struct test {
        int in;
        int upper;
        int lower;
    } vector[] = {
        { .in = 0, .upper = 0, .lower = 0 },
        { .in = 10, .upper = 10, .lower = 10 },
        { .in = '0', .upper = '0', .lower = '0' },
        { .in = '@', .upper = '@', .lower = '@' },
        { .in = 'A', .upper = 'A', .lower = 'a' },
        { .in = 'B', .upper = 'B', .lower = 'b' },
        { .in = 'I', .upper = 'I', .lower = 'i' },
        { .in = 'Z', .upper = 'Z', .lower = 'z' },
        { .in = '[', .upper = '[', .lower = '[' },
        { .in = '`', .upper = '`', .lower = '`' },
        { .in = 'a', .upper = 'A', .lower = 'a' },
        { .in = 'c', .upper = 'C', .lower = 'c' },
        { .in = 'i', .upper = 'I', .lower = 'i' },
        { .in = 'z', .upper = 'Z', .lower = 'z' },
        { .in = '{', .upper = '{', .lower = '{' },
        { .in = 1000, .upper = 1000, .lower = 1000 },
    };

    for (size_t i = 0; i < sizeof(vector) / sizeof(struct test); i++) {
        CU_ASSERT(vector[i].upper == xa_toupper(vector[i].in));
        CU_ASSERT(vector[i].lower == xa_tolower(vector[i].in));
    }
}


void test_xa_strncasecmp()
{
    const char foo[] = "foo";
    CU_ASSERT(0 == xa_strncasecmp("a", "b", 0));
    CU_ASSERT(0 == xa_strncasecmp("a", "a", 1));
    CU_ASSERT(0 == xa_strncasecmp(foo, foo, 3));
    CU_ASSERT(0 == xa_strncasecmp(foo, "FOO", 1));
    CU_ASSERT(0 == xa_strncasecmp(foo, "FOO", 2));
    CU_ASSERT(0 == xa_strncasecmp(foo, "FOO", 3));
    CU_ASSERT(0 == xa_strncasecmp(foo, "FOO", 4));
    CU_ASSERT(0 > xa_strncasecmp("a", "bOO", 4));
    CU_ASSERT(0 < xa_strncasecmp("c", "bOO", 4));
    CU_ASSERT(0 > xa_strncasecmp("foo", "FOOd", 4));
    CU_ASSERT(0 == xa_strncasecmp("foo", "FOOd", 3));
    CU_ASSERT(0 != xa_strncasecmp("foo", "food", 4));
    CU_ASSERT(0 != xa_strncasecmp("caduceus", "caduceus stick", 14));
}


void test_xa_saferealloc()
{
    void *p = NULL;

    CU_ASSERT_FATAL(NULL == xa_saferealloc(NULL, 0));

    /* alloc a new buffer */
    p = xa_saferealloc(NULL, 10);
    CU_ASSERT_FATAL(NULL != p);

    /* increase the size of the buffer */
    p = xa_saferealloc(p, 20);
    CU_ASSERT_FATAL(NULL != p);

    /* free the buffer */
    CU_ASSERT_FATAL(NULL == xa_saferealloc(p, 0));
}


void test_xa_memdup()
{
    void *p = NULL;
    const char *in = "foobar";

    CU_ASSERT_FATAL(NULL == xa_memdup(in, 0));
    CU_ASSERT_FATAL(NULL == xa_memdup(NULL, 0));

    p = xa_memdup(in, 6);
    CU_ASSERT_FATAL(NULL != p);

    CU_ASSERT_NSTRING_EQUAL("foobar", p, 6);

    free(p);
}


void test_xa_memappend()
{
    void *p = NULL;
    size_t len = 0;

    CU_ASSERT_FATAL(0 != xa_memappend(&p, &len, NULL, 0));

    CU_ASSERT_FATAL(0 == xa_memappend(&p, &len, "hello", 5));
    CU_ASSERT_FATAL(5 == len);
    CU_ASSERT_NSTRING_EQUAL("hello", p, 5);

    CU_ASSERT_FATAL(0 == xa_memappend(&p, &len, ", world", 7));
    CU_ASSERT_FATAL(12 == len);
    CU_ASSERT_NSTRING_EQUAL("hello, world", p, 12);

    free(p);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("utils.c tests", NULL, NULL);
    CU_add_test(*suite, "xa_strnlen() Tests", test_xa_strnlen);
    CU_add_test(*suite, "xa_strdup() Tests", test_xa_strdup);
    CU_add_test(*suite, "xa_strndup() Tests", test_xa_strndup);
    CU_add_test(*suite, "xa_toupper/lower() Tests", test_xa_tocase);
    CU_add_test(*suite, "xa_strncasecmp() Tests", test_xa_strncasecmp);
    CU_add_test(*suite, "xa_saferealloc() Tests", test_xa_saferealloc);
    CU_add_test(*suite, "xa_memdup() Tests", test_xa_memdup);
    CU_add_test(*suite, "xa_memappend() Tests", test_xa_memappend);
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
