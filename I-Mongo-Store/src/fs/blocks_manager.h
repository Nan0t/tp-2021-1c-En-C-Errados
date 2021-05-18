#ifndef FS_BLOCKS_MANAGER_H
#define FS_BLOCKS_MANAGER_H

#include "block.h"

void              fs_blocks_manager_init(void);
const fs_block_t* fs_blocks_manager_request_block(void);
void              fs_blocks_manager_release_block(const fs_block_t* block);

#endif