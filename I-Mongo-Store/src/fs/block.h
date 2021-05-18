#ifndef FS_BLOCK_H
#define FS_BLOCK_H

#include <utils/utils.h>

typedef struct fs_block_t fs_block_t;

void fs_block_write(fs_block_t* block, const void* data, uint64_t data_size, uint64_t offset);
void fs_block_read(fs_block_t* block, void* data, uint64_t data_size, uint64_t offset);

#endif