#include "utils/protocol/informar_sabotaje.h"
#include <stdint.h>




msg_posicion_sabotaje_t* u_msg_sabotaje_crear(uint32_t _pos_x, uint32_t _pos_y){
    msg_posicion_sabotaje_t* msg = malloc(sizeof(msg_posicion_sabotaje_t));

    msg->pos_x = _pos_x;
    msg->pos_y = _pos_y;

    return msg;
}

paquete_t* u_msg_sabotaje_serializar(const msg_posicion_sabotaje_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->pos_x, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->pos_y, sizeof(uint32_t));

    paquete_t* paquete = malloc(sizeof(paquete_t));
    paquete->opCodeMsg = INFORMAR_SABOTAJE;
    paquete->buffer    = buffer;


    return paquete; 
}

msg_posicion_sabotaje_t* u_msg_informar_sabotaje_deserializar(const u_buffer_t* _buffer){
    msg_posicion_sabotaje_t* posicion_sabotaje = u_malloc(sizeof( msg_posicion_sabotaje_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &posicion_sabotaje->pos_x, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &posicion_sabotaje->pos_y, sizeof(uint32_t), offset);

    return posicion_sabotaje;
}


void u_msg_informar_sabotaje_eliminar(msg_posicion_sabotaje_t* _msg) {
	free(_msg);
}

