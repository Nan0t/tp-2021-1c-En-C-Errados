#ifndef FS_BITACORA_BITACORAS_MANAGER_H
#define FS_BITACORA_BITACORAS_MANAGER_H

#include "bitacora.h"

void fs_bitacoras_manager_init(const char* mount_point);
void fs_bitacoras_manager_create_bitacora(uint32_t tid);

fs_bitacora_t* fs_bitacoras_manager_hold_bitacora(uint32_t tid);
void           fs_bitacoras_manager_release_bitacora(uint32_t tid);

void fs_bitacoras_manager_delete_bitacora(uint32_t tid);

#endif