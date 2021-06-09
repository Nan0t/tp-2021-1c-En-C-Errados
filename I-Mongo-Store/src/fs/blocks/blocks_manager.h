#ifndef FS_BLOCKS_MANAGER_H
#define FS_BLOCKS_MANAGER_H

#include "block.h"

void     fs_blocks_manager_init(void);
uint32_t fs_blocks_manager_request_block(void);

void     fs_blocks_manager_release_block(uint32_t block_id);

uint32_t fs_blocks_manager_get_blocks_size(void);

#endif