#ifndef FS_SERVER_THREAD_POOL_H
#define FS_SERVER_THREAD_POOL_H

#include <utils/utils.h>

void fs_thread_pool_init(uint16_t amount_workers);
void fs_thread_pool_push_task(void* user_data, void(*task_func)(void* user_data));

#endif
