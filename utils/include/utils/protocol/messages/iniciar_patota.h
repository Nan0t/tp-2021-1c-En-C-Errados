#ifndef UTILS_PROTOCOL_MESSAGES_INICIAR_PATOTA_H
#define UTILS_PROTOCOL_MESSAGES_INICIAR_PATOTA_H

#include "utils/protocol/paquete.h"

typedef struct
{
    uint32_t pid;
    char*    lista_tareas;
} u_msg_iniciar_patota_t;

u_msg_iniciar_patota_t* u_msg_iniciar_patota_crear(uint32_t pid, const char* lista_tareas);

u_buffer_t* u_msg_iniciar_patota_serializar(const u_msg_iniciar_patota_t* this);
u_msg_iniciar_patota_t* u_msg_iniciar_patota_deserializar(const u_buffer_t* buffer);

void u_msg_iniciar_patota_eliminar(u_msg_iniciar_patota_t* this);

char* u_msg_iniciar_patota_to_string(const u_msg_iniciar_patota_t* this);

#endif