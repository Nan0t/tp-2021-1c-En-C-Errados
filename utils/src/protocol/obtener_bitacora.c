#include "utils/protocol/obtener_bitacora.h"
#include <stdint.h>




msg_obtener_bitacora_t* u_msg_obtener_bitacora_crear(uint32_t _tripulante_id){
    msg_obtener_bitacora_t* msg = malloc(sizeof(msg_obtener_bitacora_t));

    msg->tripulante_id= _tripulante_id;

    return msg;
}

paquete_t* u_msg_obtener_bitacora_serializar(const msg_obtener_bitacora_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    paquete_t* paquete = malloc(sizeof(paquete_t));
    paquete->opCodeMsg = OBTENER_BITACORA;
    paquete->buffer    = buffer;


    return paquete; 
}

msg_obtener_bitacora_t* u_msg_obtener_bitacora_deserializar(const u_buffer_t* _buffer){
    msg_obtener_bitacora_t* obtener_bitacora = u_malloc(sizeof( msg_obtener_bitacora_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &obtener_bitacora->tripulante_id, sizeof(uint32_t), offset);

    return obtener_bitacora;
}


void u_msg_obtener_bitacora_eliminar(msg_obtener_bitacora_t* _msg) {
	free(_msg);
}

