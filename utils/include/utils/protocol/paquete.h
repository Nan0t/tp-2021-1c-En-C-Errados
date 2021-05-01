#ifndef UTILS_PROTOCOL_PAQUETE_H
#define UTILS_PROTOCOL_PAQUETE_H

#include "utils/protocol/buffer.h"

typedef struct {
    uint32_t  opCodeMsg; 
    u_buffer_t* buffer;
} paquete_t;

#endif
