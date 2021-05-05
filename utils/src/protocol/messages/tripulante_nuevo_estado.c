#include "utils/protocol/messages/tripulante_nuevo_estado.h"
#include "utils/memory/allocator.h"

u_msg_tripulante_nuevo_estado_t* u_msg_tripulante_nuevo_estado_crear(uint32_t _tripulante_id, uint32_t _nuevo_estado){
    u_msg_tripulante_nuevo_estado_t* msg = u_malloc(sizeof(u_msg_tripulante_nuevo_estado_t));

    msg->tripulante_id = _tripulante_id;
    msg->nuevo_estado  = _nuevo_estado;

    return msg;
}

u_buffer_t* u_msg_tripulante_nuevo_estado_serializar(const u_msg_tripulante_nuevo_estado_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->nuevo_estado, sizeof(uint32_t));

    return buffer;
}

u_msg_tripulante_nuevo_estado_t* u_msg_tripulante_nuevo_estado_deserializar(const u_buffer_t* _buffer){
    u_msg_tripulante_nuevo_estado_t* tripulante_nuevo_estado = u_malloc(sizeof( u_msg_tripulante_nuevo_estado_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &tripulante_nuevo_estado->tripulante_id, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &tripulante_nuevo_estado->nuevo_estado, sizeof(uint32_t), offset);

    return tripulante_nuevo_estado;
}


void u_msg_tripulante_nuevo_estado_eliminar(u_msg_tripulante_nuevo_estado_t* _msg) {
	u_free(_msg);
}

char* u_msg_tripulante_nuevo_estado_to_string(const u_msg_tripulante_nuevo_estado_t* msg) {
    return string_from_format(
        "MSG_TRIPULANTE_NUEVO_ESTADO: { TID: %d | Estado: %d }",
        msg->tripulante_id, msg->nuevo_estado
    );
}