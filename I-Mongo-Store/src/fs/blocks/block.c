#include "block.h"

extern uint64_t fs_blocks_manager_write_in_block(uint32_t block_id, const void* data, uint64_t data_size, uint64_t offset);
extern uint64_t fs_blocks_manager_read_in_block(uint32_t block_id, void* data, uint64_t data_size, uint64_t offset);
extern uint32_t fs_blocks_manager_get_disk_offset_of_block(uint32_t block_id);

uint64_t fs_block_write(uint32_t block_id, const void* data, uint64_t data_size, uint64_t offset)
{
    return fs_blocks_manager_write_in_block(block_id, data, data_size, offset);
}

uint64_t fs_block_read(uint32_t block_id, void* data, uint64_t data_size, uint64_t offset)
{
    return fs_blocks_manager_read_in_block(block_id, data, data_size, offset);
}

uint32_t fs_block_get_disk_offset(uint32_t block_id)
{
    return fs_blocks_manager_get_disk_offset_of_block(block_id);
}
