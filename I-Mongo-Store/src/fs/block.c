#include "block.h"
#include "disk.h"

struct fs_block_t
{
    uint32_t number;
    uint32_t disk_offset;
    uint32_t block_size;
};

void fs_block_init(fs_block_t* this, uint32_t number, uint32_t disk_offset, uint32_t block_size)
{
    this->number      = number;
    this->disk_offset = disk_offset;
    this->block_size  = block_size;
}

void fs_block_delete(fs_block_t* this)
{
    u_free(this);
}

void fs_block_write(fs_block_t* this, const void* data, uint64_t data_size, uint64_t offset)
{
    U_ASSERT(data_size + offset < this->block_size, "Trying to write OutOfBound block location");
    fs_physical_disk_write(data, data_size, this->disk_offset + offset);
}

void fs_block_read(fs_block_t* this, void* data, uint64_t data_size, uint64_t offset)
{
    U_ASSERT(data_size + offset < this->block_size, "Trying to read OutOfBound block location");
    fs_physical_disk_read(data, data_size, this->disk_offset + offset);
}
