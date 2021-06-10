#ifndef TESTS_FS_FILE_TEST_H
#define TESTS_FS_FILE_TEST_H

void test_file_setup(void);
void test_file_tear_down(void);

void test_file_create(void);
void test_file_add_fill_char_with_no_overflow(void);
void test_file_add_fill_char_with_overflow(void);
void test_file_remove_fill_char_with_no_shrink(void);
void test_file_remove_fill_char_with_shrink(void);

#endif