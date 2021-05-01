#include "utils/protocol/iniciar_tripulante.h"
#include <stdint.h>




msg_iniciar_tripulante_t* u_msg_iniciar_tripulante_crear(uint32_t _patota_id, uint32_t _tripulante_id){
    msg_iniciar_tripulante_t* msg = malloc(sizeof(msg_iniciar_tripulante_t));

    msg->patota_id = _patota_id;
    msg->tripulante_id = _tripulante_id;

    return msg;
}

paquete_t* u_msg_iniciar_tripulante_serializar(const msg_iniciar_tripulante_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->patota_id, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    paquete_t* paquete = malloc(sizeof(paquete_t));
    paquete->opCodeMsg = INICIAR_TRIPULANTE;
    paquete->buffer    = buffer;


    return paquete; 
}

msg_iniciar_tripulante_t* u_msg_iniciar_tripulante_deserializar(const u_buffer_t* _buffer){
    msg_iniciar_tripulante_t* iniciar_tripulante = u_malloc(sizeof( msg_iniciar_tripulante_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &iniciar_tripulante->patota_id, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &iniciar_tripulante->tripulante_id, sizeof(uint32_t), offset);

    return iniciar_tripulante;
}


void u_msg_iniciar_tripulante_eliminar(msg_iniciar_tripulante_t* _msg) {
	free(_msg);
}

