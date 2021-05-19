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

uint32_t fs_block_write(fs_block_t* this, const void* data, uint64_t data_size, uint64_t offset)
{
    uint32_t bytes_to_write =
        (data_size + offset < this->block_size) ?
            data_size :
            data_size - (data_size + offset - this->block_size);

    fs_physical_disk_write(data, bytes_to_write, this->disk_offset + offset);

    return bytes_to_write;
}

uint32_t fs_block_read(fs_block_t* this, void* data, uint64_t data_size, uint64_t offset)
{
    uint32_t bytes_to_read =
        (data_size + offset < this->block_size) ?
            data_size :
            data_size - (data_size + offset - this->block_size);

    fs_physical_disk_read(data, bytes_to_read, this->disk_offset + offset);

    return bytes_to_read;
}
