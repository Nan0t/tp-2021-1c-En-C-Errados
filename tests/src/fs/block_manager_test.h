#ifndef TESTS_BLOCK_MANAGER_TEST_H
#define TESTS_BLOCK_MANAGER_TEST_H

void test_fs_block_manager_setup(void);
void test_fs_block_manager_tear_down(void);

void test_fs_block_manager_request_and_release_blocks(void);
void test_fs_block_manager_request_and_get_a_intermidiate_block(void);
void test_fs_block_manager_request_and_reject(void);

#endif