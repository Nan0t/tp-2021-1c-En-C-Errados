#include "block_tests.h"

#include <CUnit/Basic.h>

#include <fs/blocks_manager.h>
#include <fs/disk.h>

#include <stdlib.h>
#include <stdio.h>

#define BLOCK_ID_OFFSET(block)          *((uint32_t*)(block))
#define BLOCK_DISK_SIZE_OFFSET(block)   *((uint32_t*)(((void*)block) + sizeof(uint32_t)))
#define BLOCK_SIZE_OFFSET(block)        *((uint32_t*)(((void*)block) + 2 * sizeof(uint32_t)))

void test_block_setup(void)
{
    u_config_init("config/test.conf");
    u_logger_init("BlockTests.log", "BlockTests", false, U_LOG_LEVEL_TRACE);
    system("python3 ../scripts/gen_super_block.py fs_boot 16 16");
    fs_blocks_manager_init();
}

void test_block_tear_down(void)
{
    u_config_terminate();
    u_logger_terminate();
}

void test_block_write_with_no_overflow(void)
{
// Arrange.
//---------
    uint32_t block;

    const char* data_to_write        = "Hello";
    uint32_t expected_bytes_to_write = strlen(data_to_write) + 1;

    char     data_written_in_disk[64] = { 0 };
    uint32_t bytes_written;

// Act.
//-----
    block         = fs_blocks_manager_request_block();
    bytes_written = fs_block_write(block, data_to_write, expected_bytes_to_write, 0);

    fs_physical_disk_flush(0, expected_bytes_to_write);
    fs_physical_disk_read(data_written_in_disk, expected_bytes_to_write, 0);

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(block, 1);
    CU_ASSERT_EQUAL(bytes_written, expected_bytes_to_write);
    CU_ASSERT_STRING_EQUAL(data_written_in_disk, data_to_write);

    fs_blocks_manager_release_block(block);
}

void test_block_write_with_overflow(void)
{
// Arrange.
//---------
    uint32_t block1;
    uint32_t block2;

    const char* data_to_write = "Hello World From FileSystem";
    uint32_t    data_length   = strlen(data_to_write) + 1;

    uint32_t expected_bytes_written_in_block1 = 16;
    uint32_t expected_bytes_written_in_block2 = data_length - 16;

    char     data_written_in_disk[64] = { 0 };
    
    uint32_t bytes_written_in_block1;
    uint32_t bytes_written_in_block2;

// Act.
//-----
    block1                  = fs_blocks_manager_request_block();
    bytes_written_in_block1 = fs_block_write(block1, data_to_write, data_length, 0);

    uint32_t bytes_remained = data_length - bytes_written_in_block1;

    block2                  = fs_blocks_manager_request_block();
    bytes_written_in_block2 = fs_block_write(block2, data_to_write + bytes_written_in_block1, bytes_remained, 0);

    fs_physical_disk_flush(0, data_length);
    fs_physical_disk_read(data_written_in_disk, data_length, 0);

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(block1, 1);
    CU_ASSERT_EQUAL_FATAL(block2, 2);
    CU_ASSERT_EQUAL(bytes_written_in_block1, expected_bytes_written_in_block1);
    CU_ASSERT_EQUAL(bytes_written_in_block2, expected_bytes_written_in_block2);

    CU_ASSERT_STRING_EQUAL(data_written_in_disk, data_to_write);

    fs_blocks_manager_release_block(block1);
    fs_blocks_manager_release_block(block2);
}

void test_block_write_in_non_contiguous_blocks(void)
{
// Arrange.
//---------
    uint32_t block1;
    uint32_t block2;
    uint32_t block3;

    const char* data_to_write = "Hello World From FileSystem";
    uint32_t    data_length   = strlen(data_to_write) + 1;
    
    uint32_t expected_bytes_written_in_block1 = 16;
    uint32_t expected_bytes_written_in_block3 = data_length - 16;

    char     data_written_in_disk[64] = { 0 };
    
    uint32_t bytes_written_in_block1;
    uint32_t bytes_written_in_block3;

// Act.
//-----
    block1                  = fs_blocks_manager_request_block();
    bytes_written_in_block1 = fs_block_write(block1, data_to_write, data_length, 0);

    uint32_t bytes_remained = data_length - bytes_written_in_block1;
    block2 = fs_blocks_manager_request_block();

    block3                  = fs_blocks_manager_request_block();
    bytes_written_in_block3 = fs_block_write(block3, data_to_write + bytes_written_in_block1, bytes_remained, 0);

    fs_physical_disk_flush(0, data_length);
    fs_physical_disk_read(data_written_in_disk, bytes_written_in_block1, 0);
    fs_physical_disk_read(
        data_written_in_disk + bytes_written_in_block1,
        bytes_written_in_block3,
        fs_block_get_disk_offset(block3));

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(block1, 1);
    CU_ASSERT_EQUAL_FATAL(block2, 2);
    CU_ASSERT_EQUAL_FATAL(block3, 3);
    CU_ASSERT_EQUAL(bytes_written_in_block1, expected_bytes_written_in_block1);
    CU_ASSERT_EQUAL(bytes_written_in_block3, expected_bytes_written_in_block3);

    CU_ASSERT_STRING_EQUAL(data_written_in_disk, data_to_write);

    fs_blocks_manager_release_block(block1);
    fs_blocks_manager_release_block(block2);
    fs_blocks_manager_release_block(block3);
}

void test_block_read_with_no_overflow(void)
{
// Arrange.
//---------
    uint32_t block;
    uint64_t data_read_from_block;

    const char* data_to_write = "Hello";
    uint32_t    data_length   = strlen(data_to_write) + 1;

    char buffer[64] = { 0 };
// Act.
//-----
    block = fs_blocks_manager_request_block();
    data_read_from_block = fs_block_write(block, data_to_write, data_length, 0);
    fs_block_read(block, buffer, data_length, 0);

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(block, 1);
    CU_ASSERT_EQUAL_FATAL(data_read_from_block, data_length);
    CU_ASSERT_STRING_EQUAL(buffer, data_to_write);

    fs_blocks_manager_release_block(block);
}
void test_block_read_with_overflow(void)
{
// Arrange.
//---------
    const char* data_to_write = "Hello World From FileSystem";
    uint32_t    data_length   = strlen(data_to_write) + 1;

    uint32_t block1;
    uint32_t block2;

    uint64_t expected_data_read_from_block1 = 16;
    uint64_t expected_data_read_from_block2 = data_length - 16;

    uint64_t data_read_from_block1;
    uint64_t data_read_from_block2;

    char buffer[64] = { 0 };
// Act.
//-----
    block1 = fs_blocks_manager_request_block();
    block2 = fs_blocks_manager_request_block();

    fs_block_write(block1, data_to_write, data_length, 0);
    fs_block_write(block2, data_to_write + 16, data_length - 16, 0);

    data_read_from_block1 = fs_block_read(block1, buffer, data_length, 0);
    data_read_from_block2 = fs_block_read(
        block2,
        buffer + data_read_from_block1,
        data_length - data_read_from_block1,
        0);

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(block1, 1);
    CU_ASSERT_EQUAL_FATAL(block2, 2);

    CU_ASSERT_EQUAL_FATAL(data_read_from_block1, expected_data_read_from_block1);
    CU_ASSERT_EQUAL_FATAL(data_read_from_block2, expected_data_read_from_block2);

    CU_ASSERT_STRING_EQUAL(buffer, data_to_write);

    fs_blocks_manager_release_block(block1);
    fs_blocks_manager_release_block(block2);
}
void test_block_read_in_non_contiguous_blocks(void)
{
// Arrange.
//---------
    const char* data_to_write = "Hello World From FileSystem";
    uint32_t    data_length   = strlen(data_to_write) + 1;

    uint32_t block1;
    uint32_t block2;
    uint32_t block3;

    uint64_t expected_data_read_from_block1 = 16;
    uint64_t expected_data_read_from_block3 = data_length - 16;

    uint64_t data_read_from_block1;
    uint64_t data_read_from_block3;

    char buffer[64] = { 0 };
// Act.
//-----
    block1 = fs_blocks_manager_request_block();
    block2 = fs_blocks_manager_request_block();
    block3 = fs_blocks_manager_request_block();

    fs_block_write(block1, data_to_write, data_length, 0);
    fs_block_write(block3, data_to_write + 16, data_length - 16, 0);

    data_read_from_block1 = fs_block_read(block1, buffer, data_length, 0);
    data_read_from_block3 = fs_block_read(
        block3,
        buffer + data_read_from_block1,
        data_length - data_read_from_block1,
        0);

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(block1, 1);
    CU_ASSERT_EQUAL_FATAL(block2, 2);
    CU_ASSERT_EQUAL_FATAL(block3, 3);

    CU_ASSERT_EQUAL_FATAL(data_read_from_block1, expected_data_read_from_block1);
    CU_ASSERT_EQUAL_FATAL(data_read_from_block3, expected_data_read_from_block3);

    CU_ASSERT_STRING_EQUAL(buffer, data_to_write);

    fs_blocks_manager_release_block(block1);
    fs_blocks_manager_release_block(block2);
    fs_blocks_manager_release_block(block3);
}
