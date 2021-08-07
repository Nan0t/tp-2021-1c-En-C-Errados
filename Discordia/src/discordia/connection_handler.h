#ifndef DISCORDIA_CONNECTION_HANDLER_H
#define DISCORDIA_CONNECTION_HANDLER_H

#include <utils/utils.h>

typedef enum
{
    MI_RAM,
    I_MONGO_STORE
} ds_module_e;

void ds_connection_handler_init(void);

void ds_connection_handler_create_connection(uint32_t tid);
void ds_connection_handler_close_connection(uint32_t tid);

bool ds_connection_handler_send(uint32_t tid, ds_module_e module, const void* data, uint64_t data_length);
bool ds_connection_handler_recv(uint32_t tid, ds_module_e module, void* data, uint64_t data_length);

#endif