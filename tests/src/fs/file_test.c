#include "file_test.h"

#include <CUnit/Basic.h>

#include <utils/utils.h>

#include <fs/file/file.h>
#include <fs/blocks/blocks_manager.h>

#include <stdio.h>
#include <stdlib.h>

void test_file_setup(void)
{
    u_config_init("config/test.conf");
    u_logger_init("BlockManagerTest.log", "BlockManagerTest", false, U_LOG_LEVEL_TRACE);
    system("python3 ../scripts/gen_super_block.py fs_boot 16 16");
    fs_blocks_manager_init(u_config_get_string_value("PUNTO_MONTAJE"), true);
}

void test_file_tear_down(void)
{
    u_logger_terminate();
    u_config_terminate();
}

void test_file_create(void)
{
// Arrange.
//---------
    fs_file_t* file = NULL;

    char* file_path = string_from_format("%sOxigeno.ims", u_config_get_string_value("PUNTO_MONTAJE"));
    char file_fill[2] = "O";

    fclose(fopen(file_path, "w"));
// Act.
//-----
    file = fs_file_create(file_path, file_fill);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(file);

    CU_ASSERT_EQUAL(fs_file_get_name(file), file_path);
    CU_ASSERT_EQUAL(fs_file_get_size(file), 0);
    CU_ASSERT_STRING_EQUAL(fs_file_get_fill_char(file), "O");
    CU_ASSERT_EQUAL(fs_file_get_blocks_count(file), 1);

    fs_file_delete(file);
}

void test_file_add_fill_char_with_no_overflow(void)
{
// Arrange.
//---------
    fs_file_t* file = NULL;

    char* path = string_from_format("%sOxigeno.ims", u_config_get_string_value("PUNTO_MONTAJE"));
    char fill[2] = "O";

    fclose(fopen(path, "w"));
// Act.
//-----
    file = fs_file_create(path, fill);

    if(file)
        fs_file_add_fill_char(file, 5);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(file);

    CU_ASSERT_EQUAL(fs_file_get_name(file), path);
    CU_ASSERT_EQUAL(fs_file_get_size(file), 5);
    CU_ASSERT_STRING_EQUAL(fs_file_get_fill_char(file), "O");
    CU_ASSERT_EQUAL(fs_file_get_blocks_count(file), 1);

    fs_file_delete(file);
    u_free(path);
}

void test_file_add_fill_char_with_overflow(void)
{
// Arrange.
//---------
    fs_file_t* file_overflow_1 = NULL;
    fs_file_t* file_overflow_2 = NULL;

    char* file1_path = string_from_format("%sOxigeno.ims", u_config_get_string_value("PUNTO_MONTAJE"));
    char* file2_path = string_from_format("%sBasura.ims", u_config_get_string_value("PUNTO_MONTAJE"));

    char file1_fill[2] = "O";
    char file2_fill[2] = "B";

    fclose(fopen(file1_path, "w"));
    fclose(fopen(file2_path, "w"));

// Act.
//-----
    file_overflow_1 = fs_file_create(file1_path, file1_fill);
    file_overflow_2 = fs_file_create(file2_path, file2_fill);

    if(file_overflow_1)
        fs_file_add_fill_char(file_overflow_1, 20);

    if(file_overflow_2)
        fs_file_add_fill_char(file_overflow_2, 32);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(file_overflow_1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(file_overflow_2);

    CU_ASSERT_EQUAL(fs_file_get_name(file_overflow_1), file1_path);
    CU_ASSERT_EQUAL(fs_file_get_size(file_overflow_1), 20);
    CU_ASSERT_STRING_EQUAL(fs_file_get_fill_char(file_overflow_1), "O");
    CU_ASSERT_EQUAL(fs_file_get_blocks_count(file_overflow_1), 2);

    CU_ASSERT_EQUAL(fs_file_get_name(file_overflow_2), file2_path);
    CU_ASSERT_EQUAL(fs_file_get_size(file_overflow_2), 40);
    CU_ASSERT_STRING_EQUAL(fs_file_get_fill_char(file_overflow_2), "B");
    CU_ASSERT_EQUAL(fs_file_get_blocks_count(file_overflow_2), 3);

    fs_file_delete(file_overflow_1);
    fs_file_delete(file_overflow_2);
}

void test_file_remove_fill_char_with_no_shrink(void)
{
// Arrange.
//---------
    fs_file_t* file = NULL;

    char* file_path = string_from_format("%sOxigeno.ims", u_config_get_string_value("PUNTO_MONTAJE"));
    char file_fill[2] = "O";

    fclose(fopen(file_path, "w"));

// Act.
//-----
    file = fs_file_create(file_path, file_fill);

    if(file)
    {
        fs_file_add_fill_char(file, 26);
        fs_file_remove_fill_char(file, 5);
    }

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(file);

    CU_ASSERT_EQUAL(fs_file_get_name(file), file_path);
    CU_ASSERT_EQUAL(fs_file_get_size(file), 21);
    CU_ASSERT_STRING_EQUAL(fs_file_get_fill_char(file), "O");
    CU_ASSERT_EQUAL(fs_file_get_blocks_count(file), 2);

    fs_file_delete(file);
}

void test_file_remove_fill_char_with_shrink(void)
{
// Arrange.
//---------
    fs_file_t* file = NULL;

    char* file_path = string_from_format("%sOxigeno.ims", u_config_get_string_value("PUNTO_MONTAJE"));
    char file_fill[2] = "O";

    fclose(fopen(file_path, "w"));

// Act.
//-----
    file = fs_file_create(file_path, file_fill);

    if(file)
    {
        fs_file_add_fill_char(file, 26);
        fs_file_remove_fill_char(file, 10);
    }

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(file);

    CU_ASSERT_EQUAL(fs_file_get_name(file), file_path);
    CU_ASSERT_EQUAL(fs_file_get_size(file), 14);
    CU_ASSERT_STRING_EQUAL(fs_file_get_fill_char(file), "O");
    CU_ASSERT_EQUAL(fs_file_get_blocks_count(file), 1);

    fs_file_delete(file);
}
