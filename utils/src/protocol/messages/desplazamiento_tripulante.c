#include "utils/protocol/messages/desplazamiento_tripulante.h"
#include "utils/memory/allocator.h"

#include <commons/string.h>

u_msg_desplazamiento_tripulante_t* u_msg_desplazamiento_tripulante_crear(uint32_t _tripulante_id, u_pos_t origen, u_pos_t destino) {
    u_msg_desplazamiento_tripulante_t* msg = u_malloc(sizeof(u_msg_desplazamiento_tripulante_t));

    msg->tripulante_id = _tripulante_id;
    msg->origen.x = origen.x;
    msg->origen.y = origen.y;
    msg->destino.x = destino.x;
    msg->destino.y = destino.y;

    return msg;
}

u_paquete_t* u_msg_desplazamiento_tripulante_serializar(const u_msg_desplazamiento_tripulante_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->origen, sizeof(u_pos_t));
    u_buffer_write(buffer, &_msg->destino, sizeof(u_pos_t));


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
    u_buffer_read(_buffer, &desplazamiento_tripulante->origen, sizeof(u_pos_t), offset);
    offset += sizeof(u_pos_t);
    u_buffer_read(_buffer, &desplazamiento_tripulante->destino, sizeof(u_pos_t), offset);

    return desplazamiento_tripulante;
}


void u_msg_desplazamiento_tripulante_eliminar(u_msg_desplazamiento_tripulante_t* _msg) {
	u_free(_msg);
}

char* u_msg_desplazamineto_tripulante_to_string(const u_msg_desplazamiento_tripulante_t* msg) {
    return string_from_format(
        "MSG_DESPALZAMIENTO_TRIPULANTE: { TID: %d | Origen: { Pos_X: %d | Pos_Y: %d } | Destino: { Pos_X: %d | Pos_Y: %d } }",
        msg->tripulante_id, msg->origen.x, msg->origen.y, msg->destino.x, msg->destino.y
    );
}