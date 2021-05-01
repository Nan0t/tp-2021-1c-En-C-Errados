#include "utils/protocol/eliminar_tripulante.h"
#include <stdint.h>




u_msg_eliminar_tripulante_t* u_msg_eliminar_tripulante_crear(uint32_t _tripulante_id){
    u_msg_eliminar_tripulante_t* msg = malloc(sizeof(u_msg_eliminar_tripulante_t));

    msg->tripulante_id= _tripulante_id;

    return msg;
}

u_paquete_t* u_msg_eliminar_tripulante_serializar(const u_msg_eliminar_tripulante_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    u_paquete_t* paquete = malloc(sizeof(u_paquete_t));
    paquete->opCodeMsg = ELIMINAR_TRIPULANTE;
    paquete->buffer    = buffer;


    return paquete; 
}

u_msg_eliminar_tripulante_t* u_msg_eliminar_tripulante_deserializar(const u_buffer_t* _buffer){
    u_msg_eliminar_tripulante_t* eliminar_tripulante = u_malloc(sizeof( u_msg_eliminar_tripulante_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &eliminar_tripulante->tripulante_id, sizeof(uint32_t), offset);

    return eliminar_tripulante;
}


void u_msg_eliminar_tripulante_eliminar(u_msg_eliminar_tripulante_t* _msg) {
	free(_msg);
}

