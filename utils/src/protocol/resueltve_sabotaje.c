#include "utils/protocol/resuelve_sabotaje.h"
#include <stdint.h>




msg_resuelve_sabotaje_t* u_msg_resuelve_sabotaje_crear(uint32_t _tripulante_id){
    msg_resuelve_sabotaje_t* msg = malloc(sizeof(msg_resuelve_sabotaje_t));

    msg->tripulante_id= _tripulante_id;

    return msg;
}

paquete_t* u_msg_resuelve_sabotaje_serializar(const msg_resuelve_sabotaje_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    paquete_t* paquete = malloc(sizeof(paquete_t));
    paquete->opCodeMsg = RESUELVE_SABOTAJE;
    paquete->buffer    = buffer;


    return paquete; 
}

msg_resuelve_sabotaje_t* u_msg_resuelve_sabotaje_deserializar(const u_buffer_t* _buffer){
    msg_resuelve_sabotaje_t* resuelve_sabotaje = u_malloc(sizeof( msg_resuelve_sabotaje_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &resuelve_sabotaje->tripulante_id, sizeof(uint32_t), offset);

    return resuelve_sabotaje;
}


void u_msg_resuelve_sabotaje_eliminar(msg_resuelve_sabotaje_t* _msg) {
	free(_msg);
}

