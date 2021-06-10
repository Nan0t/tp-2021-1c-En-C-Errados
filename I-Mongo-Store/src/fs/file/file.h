#ifndef FS_FILE_H
#define FS_FILE_H

#include <utils/utils.h>

typedef struct fs_file_t fs_file_t;

fs_file_t* fs_file_create(const char* mount_point, const char* file_name, char fill_char);
void       fs_file_delete(fs_file_t* file);

void fs_file_add_fill_char(fs_file_t* file, uint32_t amount);
void fs_file_remove_fill_char(fs_file_t* file, uint32_t amount);

bool fs_file_check_integrity(fs_file_t* file);

const char* fs_file_get_name(const fs_file_t* file);
uint32_t    fs_file_get_size(const fs_file_t* file);
char        fs_file_get_fill_char(const fs_file_t* file);
const char* fs_file_get_md5(const fs_file_t* file);
uint32_t    fs_file_get_blocks_count(const fs_file_t* file);

#endif