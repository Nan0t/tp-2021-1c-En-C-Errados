#ifndef TESTS_BLOCK_TEST_H
#define TESTS_BLOCK_TESTS_H

void test_block_setup(void);
void test_block_tear_down(void);

void test_block_write_with_no_overflow(void);
void test_block_write_with_overflow(void);
void test_block_write_in_non_contiguous_blocks(void);

void test_block_read_with_no_overflow(void);
void test_block_read_with_overflow(void);
void test_block_read_in_non_contiguous_blocks(void);

#endif