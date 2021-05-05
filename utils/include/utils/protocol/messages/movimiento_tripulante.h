#ifndef UTILS_PROTOCOL_MESSAGES_MOVIMIENTO_TRIPULANTE_H
#define UTILS_PROTOCOL_MESSAGES_MOVIMIENTO_TRIPULANTE_H

#include "utils/protocol/paquete.h"

typedef struct
{
    uint32_t tid;
    u_pos_t  pos;
} u_msg_movimiento_tripulante_t;

u_msg_movimiento_tripulante_t* u_msg_movimiento_tripulante_crear(uint32_t tid, u_pos_t pos);

u_paquete_t* u_msg_movimiento_tripulante_serializar(const u_msg_movimiento_tripulante_t* this);
u_msg_movimiento_tripulante_t* u_msg_movimiento_tripulante_deserializar(const u_buffer_t* buffer);

void u_msg_movimiento_tripulante_eliminar(u_msg_movimiento_tripulante_t* this);

char* u_msg_movimiento_tripulante_to_string(const u_msg_movimiento_tripulante_t* this);

#endif