#include "config_test.h"
#include <utils/config/config.h>
#include <CUnit/Basic.h>
#include <stdlib.h>

void test_utils_config_setup(void)
{
    if(!u_config_init("config/test.conf"))
    {
        fprintf(stderr, "Couldn't load the config file\n");
        exit(-1);
    }
}

void test_utils_config_tear_down(void)
{
    u_config_terminate();
}

void test_utils_config_get_string_value(void)
{
    const char* str = u_config_get_string_value("STRING_VALUE");
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_STRING_EQUAL(str, "StringTest");
}

void test_utils_config_get_array_value(void)
{
    const char* expect[] =
    {
        "ArrayValue1",
        "ArrayValue2",
        "ArrayValue3",
        "ArrayValue4",
    };

    char** values = u_config_get_array_value("ARRAY_VALUES");
    CU_ASSERT_PTR_NOT_NULL(values);

    for(int i = 0; values[i] != NULL; i ++)
    {
        CU_ASSERT_STRING_EQUAL(values[i], expect[i]);
        free(values[i]);
    }

    free(values);
}

void test_utils_config_get_int_value(void)
{
    int value = u_config_get_int_value("INT_VALUE");
    CU_ASSERT_EQUAL(value, 100);
}

void test_utils_config_get_float_value(void)
{
    float value = u_config_get_float_value("FLOAT_VALUE");
    CU_ASSERT_EQUAL(value, 3.14f);
}

void test_utils_config_get_double_value(void)
{
    double value = u_config_get_double_value("DOUBLE_VALUE");
    CU_ASSERT_EQUAL(value, 2.71);
}
