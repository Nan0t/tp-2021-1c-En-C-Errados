#ifndef FS_PHYSICAL_DISK_H
#define FS_PHYSICAL_DISK_H

#include <utils/utils.h>

void     fs_physical_disk_init(uint64_t size, bool is_clean_initialization);
void     fs_physical_disk_write(const void* data, uint64_t size, uint64_t offset);
void     fs_physical_disk_read(void* data, uint64_t size, uint64_t offset);
void     fs_physical_disk_flush(uint64_t offset, uint64_t size);
uint32_t fs_physical_disk_get_size(void);

#endif