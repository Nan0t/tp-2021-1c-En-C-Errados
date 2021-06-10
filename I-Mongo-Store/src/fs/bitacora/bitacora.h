#ifndef FS_BITACORA_H
#define FS_BITACORA_H

#include <utils/utils.h>

typedef struct fs_bitacora_t fs_bitacora_t;

fs_bitacora_t* fs_bitacora_create(const char* mount_point, uint32_t tid);
void           fs_bitacora_delete(fs_bitacora_t* bitacora);

void fs_bitacora_add_content(fs_bitacora_t* bitacora, const char* content);

bool fs_bitacora_check_integrity(fs_bitacora_t* bitacora);

uint32_t  fs_bitacora_get_tid(const fs_bitacora_t* bitacora);
uint32_t  fs_bitacora_get_size(const fs_bitacora_t* bitacora);
uint32_t  fs_bitacora_get_block_count(const fs_bitacora_t* bitacora);
char*     fs_bitacora_get_content(const fs_bitacora_t* bitacora);

#endif