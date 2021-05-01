#ifndef UTILS_PROTOCOL_PAQUETE_H
#define UTILS_PROTOCOL_PAQUETE_H

#include "utils/protocol/opcodes.h"
#include "utils/protocol/buffer.h"

typedef struct {
    u_opcode_e  opCodeMsg; 
    u_buffer_t* buffer;
} u_paquete_t;

#endif
