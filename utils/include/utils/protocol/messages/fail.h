#ifndef UTILS_PROTOCOL_MESSAGE_FAIL_H
#define UTILS_PROTOCOL_MESSAGE_FAIL_H

#include "utils/protocol/paquete.h"

typedef struct
{
    char* description;
} u_msg_fail_t;

u_msg_fail_t* u_msg_fail_crear(const char* format, ...) UTILS_PRINT_FORMAT(1, 2);

u_buffer_t* u_msg_fail_serializar(const u_msg_fail_t* this);
u_msg_fail_t* u_msg_fail_deserializar(const u_buffer_t* buffer);

void u_msg_fail_eliminar(u_msg_fail_t* this);

char* u_msg_fail_to_string(const u_msg_fail_t* this);

#endif