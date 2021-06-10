#ifndef TESTS_FS_BITACORA_TEST_H
#define TESTS_FS_BITACORA_TEST_H

void test_bitacora_setup(void);
void test_bitacora_tear_down(void);

void test_bitacora_create(void);
void test_bitacora_add_content_with_no_overflow(void);
void test_bitacora_add_content_with_overflow(void);

#endif