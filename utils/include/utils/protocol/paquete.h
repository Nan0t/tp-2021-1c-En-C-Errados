#ifndef UTILS_PROTOCOL_PAQUETE_H
#define UTILS_PROTOCOL_PAQUETE_H

#include "utils/protocol/opcodes.h"
#include "utils/protocol/buffer.h"

typedef struct {
    u_opcode_e  opCodeMsg; 
    u_buffer_t* buffer;
} u_paquete_t;

u_paquete_t* u_paquete_crear(u_opcode_e opcode, const u_buffer_t* buffer);
void         u_paquete_delete(u_paquete_t* this);

u_buffer_t* u_paquete_serializar(const u_paquete_t* this);

#endif
