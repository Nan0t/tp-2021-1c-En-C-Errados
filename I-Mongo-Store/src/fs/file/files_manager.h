#ifndef FS_FILE_FILES_MANAGER_H
#define FS_FILE_FILES_MANAGER_H

#include "file.h"

void fs_files_manager_init(const char* mount_point);
void fs_files_manager_create_file(const char* name, char fill_char);

fs_file_t* fs_files_manager_hold_file(const char* name);
void           fs_files_manager_release_file(const char* name);

void fs_files_manager_delete_file(const char* name);

#endif