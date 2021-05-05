#ifndef UTILS_PROTOCOL_MESSAGES_NUEVA_TAREA_H
#define UTILS_PROTOCOL_MESSAGES_NUEVA_TAREA_H

#include "utils/protocol/paquete.h"

typedef struct
{
    char* tarea;
} u_msg_nueva_tarea_t;

u_msg_nueva_tarea_t* u_msg_nueva_tarea_crear(const char* tarea);

u_buffer_t* u_msg_nueva_tarea_serializar(const u_msg_nueva_tarea_t* this);
u_msg_nueva_tarea_t* u_msg_nueva_tarea_deserializar(const u_buffer_t* buffer);

void u_msg_nueva_tarea_eliminar(u_msg_nueva_tarea_t* this);

char* u_msg_nueva_tarea_to_string(const u_msg_nueva_tarea_t* this);

#endif