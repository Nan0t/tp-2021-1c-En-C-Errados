#ifndef INCLUDE_UTILS_PROTOCOL_MESSAGES_BITACORA_H_
#define INCLUDE_UTILS_PROTOCOL_MESSAGES_BITACORA_H_

#include "utils/protocol/paquete.h"

typedef struct
{
	char* contenido;
} u_msg_bitacora_t;

u_msg_bitacora_t* u_msg_bitacora_crear(const char* contenido_bitacora);

u_paquete_t*      u_msg_bitacora_serializar(const u_msg_bitacora_t* this);
u_msg_bitacora_t* u_msg_bitacora_deserializar(const u_buffer_t* buffer);

void u_msg_bitacora_eliminar(u_msg_bitacora_t* this);

char* u_msg_bitacora_to_string(const u_msg_bitacora_t* this);

#endif
