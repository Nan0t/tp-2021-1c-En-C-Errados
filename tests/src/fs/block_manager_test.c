#include "block_manager_test.h"
#include "fs/blocks/blocks_manager.h"
#include <CUnit/Basic.h>
#include <stdlib.h>

#define BLOCK_ID_OFFSET(block)          *((uint32_t*)(block))
#define BLOCK_DISK_SIZE_OFFSET(block)   *((uint32_t*)(((void*)block) + sizeof(uint32_t)))
#define BLOCK_SIZE_OFFSET(block)        *((uint32_t*)(((void*)block) + 2 * sizeof(uint32_t)))

#define ARRAY_LENGTH(arr)   (sizeof(arr) / sizeof(*arr))

void test_fs_block_manager_setup(void)
{
    u_config_init("config/test.conf");
    u_logger_init("BlockManagerTest.log", "BlockManagerTest", false, U_LOG_LEVEL_TRACE);
    system("python3 ../scripts/gen_super_block.py fs_boot 16 16");
    fs_blocks_manager_init(u_config_get_string_value("PUNTO_MONTAJE"));
}

void test_fs_block_manager_tear_down(void)
{
    u_logger_terminate();
    u_config_terminate();
}

void test_fs_block_manager_request_and_release_blocks(void)
{
// Arrange.
//---------
    uint32_t blocks[16] = { 0 };

// Act.
//-----
    for(uint32_t i = 0; i < ARRAY_LENGTH(blocks); i ++)
        blocks[i] = fs_blocks_manager_request_block();

// Assert.
//--------
    for(uint32_t i = 0; i < ARRAY_LENGTH(blocks); i ++)
    {
        CU_ASSERT_EQUAL(blocks[i], i + 1);
        CU_ASSERT_EQUAL(fs_block_get_disk_offset(blocks[i]), 16 * i); // Disk_Offset
    }
}

void test_fs_block_manager_request_and_get_a_intermidiate_block(void)
{
// Arrange.
//---------
    uint32_t blocks[16] = { 0 };
    uint32_t intermediate_block;

    uint32_t expected_block_number = 9;
    uint32_t expected_block_disk_offset = 8 * 16;

// Act.
//-----
    for(uint32_t i = 0; i < ARRAY_LENGTH(blocks); i ++)
        blocks[i] = fs_blocks_manager_request_block();

    fs_blocks_manager_release_block(blocks[8]);

    intermediate_block = fs_blocks_manager_request_block();

// Assert.
//--------
    for(uint32_t i = 0; i < ARRAY_LENGTH(blocks); i ++)
    {
        if(i == 8)
        {
            CU_ASSERT_EQUAL(intermediate_block, expected_block_number);
            CU_ASSERT_EQUAL(fs_block_get_disk_offset(intermediate_block), expected_block_disk_offset);
        }
        CU_ASSERT_EQUAL(blocks[i], i + 1);
        CU_ASSERT_EQUAL(fs_block_get_disk_offset(blocks[i]), 16 * i); // Disk_Offset
    }
}

void test_fs_block_manager_request_and_reject(void)
{
// Arrange.
//---------
    uint32_t blocks[16] = { 0 };
    uint32_t invalid_block;

// Act.
//-----
    for(uint64_t i = 0; i < ARRAY_LENGTH(blocks); i ++)
        blocks[i] = fs_blocks_manager_request_block();

    invalid_block = fs_blocks_manager_request_block();

// Assert.
//--------
    CU_ASSERT_EQUAL(invalid_block, 0);

    for(uint64_t i = 0; i < ARRAY_LENGTH(blocks); i ++)
        fs_blocks_manager_release_block(blocks[i]);
}
