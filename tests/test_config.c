/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <CUnit/Basic.h>
#include <cutils/memory.h>
#include <cutils/printf.h>
#include <cutils/strings.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/config/internal.h"

static char *base_dir;

void test_config_file_insert(void)
{
    struct cfg_file *list = NULL;
    struct cfg_file *p    = NULL;

    const char *vector[] = {
        "foo",
        "bar",
        "goo",
        "add",
        "zoo",
        "aba",
        "zoo",
        "zap",
    };
    const char *want[] = {
        "aba",
        "add",
        "bar",
        "foo",
        "goo",
        "zap",
        "zoo",
        "zoo"
    };

    for (size_t i = 0; i < sizeof(vector) / sizeof(const char *); i++) {
        list = cfg_file_insert(list, cu_must_strdup(vector[i]));
        CU_ASSERT(list != NULL);
    }

    p = list;
    for (size_t i = 0; i < sizeof(want) / sizeof(const char *); i++, p = p->next) {
        CU_ASSERT(p != NULL);
        CU_ASSERT_STRING_EQUAL(p->filename, want[i]);
    }

    cfg_file_list_destroy(list);
}

void test_not_a_dir(void)
{
    char *path  = must_maprintf("%s/cfg/missing", base_dir);
    config_t *c = NULL;
    XAcode rv;

    rv = XA_OK;
    c  = config_read(path, &rv);

    CU_ASSERT(NULL == c);
    CU_ASSERT(rv == XA_NOT_A_DIR);

    free(path);

    path = must_maprintf("%s/cfg/file", base_dir);
    rv   = XA_OK;
    c    = config_read(path, &rv);

    CU_ASSERT(NULL == c);
    CU_ASSERT(rv == XA_NOT_A_DIR);

    free(path);
}


void test_file_too_long(void)
{
    char *long_string = NULL;
    char *path        = NULL;
    config_t *c       = NULL;
    XAcode rv;

    long_string = must_calloc(sizeof(char), 5000);
    memset(long_string, 'a', 4999);
    path = must_maprintf("%s/%s", base_dir, long_string);

    rv = XA_OK;
    c  = config_read(path, &rv);

    CU_ASSERT(NULL == c);
    CU_ASSERT(rv == XA_PATH_TOO_LONG);

    free(path);
    free(long_string);
}


void test_simple(const char *dir)
{
    char *path  = must_maprintf("%s/cfg/%s", base_dir, dir);
    config_t *c = NULL;
    XAcode rv;

    rv = XA_OK;
    c  = config_read(path, &rv);
    CU_ASSERT_FATAL(NULL != c);
    CU_ASSERT_FATAL(XA_OK == rv);

    CU_ASSERT_STRING_EQUAL(c->identity.device_id.s, "mac:112233445566");
    CU_ASSERT_STRING_EQUAL(c->identity.partner_id.s, "my_friend");

    CU_ASSERT_STRING_EQUAL(c->hardware.model.s, "MODEL_NAME");
    CU_ASSERT_STRING_EQUAL(c->hardware.serial_number.s, "SERIAL_NUMBER");
    CU_ASSERT_STRING_EQUAL(c->hardware.manufacturer.s, "MANUFACTURER_NAME");
    CU_ASSERT_STRING_EQUAL(c->hardware.last_reboot_reason.s, "REASON");

    CU_ASSERT_STRING_EQUAL(c->firmware.name.s, "NAME");

    CU_ASSERT_STRING_EQUAL(c->behavior.url.s, "URL");
    CU_ASSERT(c->behavior.ping_timeout == 90);
    CU_ASSERT(c->behavior.backoff_max == 250);
    CU_ASSERT(c->behavior.force_ip == 4);
    CU_ASSERT(c->behavior.interface_count == 2);

    CU_ASSERT_STRING_EQUAL(c->behavior.dns_txt.base_fqdn.s, "xmidt.example.com");
    CU_ASSERT_STRING_EQUAL(c->behavior.dns_txt.jwt.keys_dir.s, "keys_dir");
    CU_ASSERT_STRING_EQUAL(c->behavior.issuer.url.s, "issuer.example.com");

    config_destroy(c);
    free(path);
}

void simple(void)
{
    test_simple("test_01");
    test_simple("test_01/");
    test_simple("test_02");
    test_simple("test_03");
    test_simple("test_04");
}

void invalid_test(const char *dir, XAcode want)
{
    char *path  = must_maprintf("%s/cfg/%s", base_dir, dir);
    config_t *c = NULL;
    XAcode rv;

    rv = XA_OK;
    c  = config_read(path, &rv);
    printf("path: %s\n", path);
    CU_ASSERT_FATAL(NULL == c);
    CU_ASSERT_FATAL(want == rv);

    free(path);
}

void invalid(void)
{
    invalid_test("invalid_01", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_02", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_03", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_04", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_05", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_06", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_07", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_08", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_09", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_10", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_11", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_12", XA_CONFIG_FILE_ERROR);
    invalid_test("invalid_13", XA_CONFIG_FILE_ERROR);
}


void verify_print_does_not_crash(void)
{
    char *path  = must_maprintf("%s/cfg/test_01", base_dir);
    config_t *c = NULL;
    XAcode rv;

    rv = XA_OK;
    c  = config_read(path, &rv);
    CU_ASSERT_FATAL(NULL != c);
    CU_ASSERT_FATAL(XA_OK == rv);

    config_print(c);

    config_destroy(c);
    free(path);
}

void check_passing_NULL(void)
{
    config_print(NULL);
    config_destroy(NULL);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("config file reading tests", NULL, NULL);
    CU_add_test(*suite, "not a dir test", test_not_a_dir);
    CU_add_test(*suite, "long filename test", test_file_too_long);
    CU_add_test(*suite, "valid tests", simple);
    CU_add_test(*suite, "invalid tests", invalid);
    CU_add_test(*suite, "print tests", verify_print_does_not_crash);
    CU_add_test(*suite, "NULL tests", check_passing_NULL);
    CU_add_test(*suite, "config_file_insert test", test_config_file_insert);
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    unsigned rv     = 1;
    CU_pSuite suite = NULL;

    if (2 != argc) {
        printf("usage: %s path_to_tests_dir\n", argv[0]);
        return 1;
    }

    base_dir = argv[1];
    printf("base_dir: %s\n", base_dir);

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
