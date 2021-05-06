#include "utils/protocol/messages/iniciar_tripulante.h"
#include "utils/memory/allocator.h"

#include <commons/string.h>

u_msg_iniciar_tripulante_t* u_msg_iniciar_tripulante_crear(uint32_t _patota_id, uint32_t _tripulante_id){
    u_msg_iniciar_tripulante_t* msg = u_malloc(sizeof(u_msg_iniciar_tripulante_t));

    msg->patota_id = _patota_id;
    msg->tripulante_id = _tripulante_id;

    return msg;
}

u_buffer_t* u_msg_iniciar_tripulante_serializar(const u_msg_iniciar_tripulante_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->patota_id, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    return buffer;
}

u_msg_iniciar_tripulante_t* u_msg_iniciar_tripulante_deserializar(const u_buffer_t* _buffer){
    u_msg_iniciar_tripulante_t* iniciar_tripulante = u_malloc(sizeof( u_msg_iniciar_tripulante_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &iniciar_tripulante->patota_id, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &iniciar_tripulante->tripulante_id, sizeof(uint32_t), offset);

    return iniciar_tripulante;
}


void u_msg_iniciar_tripulante_eliminar(u_msg_iniciar_tripulante_t* _msg) {
	u_free(_msg);
}

char* u_msg_iniciar_tripulante_to_string(const u_msg_iniciar_tripulante_t* msg) {
    return string_from_format(
        "MSG_INICIAR_TRIPULANTE: { PID: %d | TID: %d}",
        msg->patota_id, msg->tripulante_id
    );
}