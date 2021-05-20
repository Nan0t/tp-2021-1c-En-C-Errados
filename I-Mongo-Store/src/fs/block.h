#ifndef FS_BLOCK_H
#define FS_BLOCK_H

#include <utils/utils.h>

uint64_t fs_block_write(uint32_t block_id, const void* data, uint64_t data_size, uint64_t offset);
uint64_t fs_block_read(uint32_t block, void* data, uint64_t data_size, uint64_t offset);

uint32_t fs_block_get_disk_offset(uint32_t block_id);

#endif