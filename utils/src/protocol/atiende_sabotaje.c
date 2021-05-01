#include "utils/protocol/atiende_sabotaje.h"
#include <stdint.h>




msg_atiende_sabotaje_t* u_msg_atiende_sabotaje_crear(uint32_t _tripulante_id){
    msg_atiende_sabotaje_t* msg = malloc(sizeof(msg_atiende_sabotaje_t));

    msg->tripulante_id= _tripulante_id;

    return msg;
}

paquete_t* u_msg_atiende_sabotaje_serializar(const msg_atiende_sabotaje_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    paquete_t* paquete = malloc(sizeof(paquete_t));
    paquete->opCodeMsg = ATIENDE_SABOTAJE;
    paquete->buffer    = buffer;


    return paquete; 
}

msg_atiende_sabotaje_t* u_msg_atiende_sabotaje_deserializar(const u_buffer_t* _buffer){
    msg_atiende_sabotaje_t* atiende_sabotaje = u_malloc(sizeof( msg_atiende_sabotaje_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &atiende_sabotaje->tripulante_id, sizeof(uint32_t), offset);

    return atiende_sabotaje;
}


void u_msg_atiende_sabotaje_eliminar(msg_atiende_sabotaje_t* _msg) {
	free(_msg);
}

