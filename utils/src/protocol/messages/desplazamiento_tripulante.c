#include "utils/protocol/messages/desplazamiento_tripulante.h"
#include "utils/memory/allocator.h"

u_msg_desplazamiento_tripulante_t* u_msg_desplazamiento_tripulante_crear(uint32_t _tripulante_id,uint32_t _pos_x, uint32_t _pos_y){
    u_msg_desplazamiento_tripulante_t* msg = u_malloc(sizeof(u_msg_desplazamiento_tripulante_t));

    msg->tripulante_id = _tripulante_id;
    msg->pos_x         = _pos_x;
    msg->pos_y         = _pos_y;

    return msg;
}

u_paquete_t* u_msg_deplazamiento_tripulante_serializar(const u_msg_desplazamiento_tripulante_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->pos_x, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->pos_y, sizeof(uint32_t));


    u_paquete_t* paquete = u_malloc(sizeof(u_paquete_t));
    paquete->opCodeMsg = DESPLAZAMIENTO_TRIPULANTE;
    paquete->buffer    = buffer;

    return paquete; 
}

u_msg_desplazamiento_tripulante_t* u_msg_desplazamiento_tripulante_deserializar(const u_buffer_t* _buffer){
    u_msg_desplazamiento_tripulante_t* desplazamiento_tripulante = u_malloc(sizeof( u_msg_desplazamiento_tripulante_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &desplazamiento_tripulante->tripulante_id, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &desplazamiento_tripulante->pos_x, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &desplazamiento_tripulante->pos_y, sizeof(uint32_t), offset);

    return desplazamiento_tripulante;
}


void u_msg_desplazamiento_tripulante_eliminar(u_msg_desplazamiento_tripulante_t* _msg) {
	u_free(_msg);
}
