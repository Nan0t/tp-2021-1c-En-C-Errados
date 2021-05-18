#include "block_manager_test.h"
#include "fs/blocks_manager.h"
#include <CUnit/Basic.h>
#include <stdlib.h>

#define BLOCK_ID_OFFSET(block)          *((uint32_t*)(block))
#define BLOCK_DISK_SIZE_OFFSET(block)   *((uint32_t*)(((void*)block) + sizeof(uint32_t)))
#define BLOCK_SIZE_OFFSET(block)        *((uint32_t*)(((void*)block) + 2 * sizeof(uint32_t)))

void test_fs_block_manager_setup(void)
{
    u_config_init("config/test.conf");
    u_logger_init("BlockManagerTest.log", "BlockManagerTest", false, U_LOG_LEVEL_TRACE);
    system("python3 ../scripts/gen_super_block.py fs_boot 16 16");
    fs_blocks_manager_init();
}

void test_fs_block_manager_tear_down(void)
{
    u_config_terminate();
}

void test_fs_block_manager_request_and_release_blocks(void)
{
// Arrange.
//---------
    const fs_block_t* block1 = NULL;
    const fs_block_t* block2 = NULL;
    const fs_block_t* block3 = NULL;
    const fs_block_t* block4 = NULL;

// Act.
//-----
    block1 = fs_blocks_manager_request_block();
    block2 = fs_blocks_manager_request_block();
    block3 = fs_blocks_manager_request_block();
    
    fs_blocks_manager_release_block(block2);

    block4 = fs_blocks_manager_request_block();


// Assert.
//--------
    CU_ASSERT_EQUAL(BLOCK_ID_OFFSET(block1), 1);        // Id
    CU_ASSERT_EQUAL(BLOCK_DISK_SIZE_OFFSET(block1), 0); // Disk_Offset
    CU_ASSERT_EQUAL(BLOCK_SIZE_OFFSET(block1), 16);     // Block_Size

    CU_ASSERT_EQUAL(BLOCK_ID_OFFSET(block4), 2);         // Id
    CU_ASSERT_EQUAL(BLOCK_DISK_SIZE_OFFSET(block4), 16); // Disk_Offset
    CU_ASSERT_EQUAL(BLOCK_SIZE_OFFSET(block4), 16);      // Block_Size

    CU_ASSERT_EQUAL(BLOCK_ID_OFFSET(block3), 3);         // Id
    CU_ASSERT_EQUAL(BLOCK_DISK_SIZE_OFFSET(block3), 32); // Disk_Offset
    CU_ASSERT_EQUAL(BLOCK_SIZE_OFFSET(block3), 16);      // Block_Size
}
