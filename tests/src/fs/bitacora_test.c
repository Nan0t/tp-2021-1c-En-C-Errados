#include "bitacora_test.h"

#include <CUnit/Basic.h>

#include <utils/utils.h>

#include <fs/bitacora/bitacora.h>
#include <fs/blocks/blocks_manager.h>

#include <stdlib.h>

void test_bitacora_setup(void)
{
    u_config_init("config/test.conf");
    u_logger_init("BlockManagerTest.log", "BlockManagerTest", false, U_LOG_LEVEL_TRACE);
    system("python3 ../scripts/gen_super_block.py fs_boot 16 16");
    fs_blocks_manager_init(u_config_get_string_value("PUNTO_MONTAJE"));
}

void test_bitacora_tear_down(void)
{
    u_logger_terminate();
    u_config_terminate();
}

void test_bitacora_create(void)
{
// Arrange.
//---------
    fs_bitacora_t* bitacora = NULL;
// Act.
//-----
    bitacora = fs_bitacora_create(u_config_get_string_value("PUNTO_MONTAJE"), 1);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(bitacora);

    CU_ASSERT_EQUAL(fs_bitacora_get_tid(bitacora), 1);
    CU_ASSERT_EQUAL(fs_bitacora_get_size(bitacora), 0);
    CU_ASSERT_EQUAL(fs_bitacora_get_block_count(bitacora), 1);

    char* content = fs_bitacora_get_content(bitacora);

    CU_ASSERT_PTR_NOT_NULL(content);

    if(content)
    {
        CU_ASSERT_STRING_EQUAL(content, "");
        u_free(content);
    }

    fs_bitacora_delete(bitacora);
}

void test_bitacora_add_content_with_no_overflow(void)
{
// Arrange.
//---------
    const char* content_expected = "Hello World";
    uint32_t content_expected_length = strlen(content_expected);

    fs_bitacora_t* bitacora = NULL;

// Act.
//-----
    bitacora = fs_bitacora_create(u_config_get_string_value("PUNTO_MONTAJE"), 1);
    fs_bitacora_add_content(bitacora, content_expected);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(bitacora);

    CU_ASSERT_EQUAL(fs_bitacora_get_tid(bitacora), 1);
    CU_ASSERT_EQUAL(fs_bitacora_get_size(bitacora), content_expected_length);
    CU_ASSERT_EQUAL(fs_bitacora_get_block_count(bitacora), 1);
    
    char* content = fs_bitacora_get_content(bitacora);

    CU_ASSERT_PTR_NOT_NULL(content);

    if(content)
    {
        CU_ASSERT_STRING_EQUAL(content, content_expected);
        u_free(content);
    }

    fs_bitacora_delete(bitacora);
}

void test_bitacora_add_content_with_overflow(void)
{
// Arrange.
//---------
    const char* content_expected = "Hello World From File System";
    uint32_t content_expected_length = strlen(content_expected);

    fs_bitacora_t* bitacora = NULL;

// Act.
//-----
    bitacora = fs_bitacora_create(u_config_get_string_value("PUNTO_MONTAJE"), 1);
    fs_bitacora_add_content(bitacora, content_expected);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(bitacora);

    CU_ASSERT_EQUAL(fs_bitacora_get_tid(bitacora), 1);
    CU_ASSERT_EQUAL(fs_bitacora_get_size(bitacora), content_expected_length);
    CU_ASSERT_EQUAL(fs_bitacora_get_block_count(bitacora), 2);
    
    char* content = fs_bitacora_get_content(bitacora);

    CU_ASSERT_PTR_NOT_NULL(content);

    if(content)
    {
        CU_ASSERT_STRING_EQUAL(content, content_expected);
        u_free(content);
    }

    fs_bitacora_delete(bitacora);
}
