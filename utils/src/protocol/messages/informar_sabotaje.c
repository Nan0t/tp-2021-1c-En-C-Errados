#include "utils/protocol/messages/informar_sabotaje.h"
#include "utils/memory/allocator.h"

#include <commons/string.h>

u_msg_posicion_sabotaje_t* u_msg_sabotaje_crear(uint32_t _pos_x, uint32_t _pos_y){
    u_msg_posicion_sabotaje_t* msg = u_malloc(sizeof(u_msg_posicion_sabotaje_t));

    msg->pos_x = _pos_x;
    msg->pos_y = _pos_y;

    return msg;
}

u_buffer_t* u_msg_sabotaje_serializar(const u_msg_posicion_sabotaje_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->pos_x, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->pos_y, sizeof(uint32_t));

    return buffer;
}

u_msg_posicion_sabotaje_t* u_msg_informar_sabotaje_deserializar(const u_buffer_t* _buffer){
    u_msg_posicion_sabotaje_t* posicion_sabotaje = u_malloc(sizeof( u_msg_posicion_sabotaje_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &posicion_sabotaje->pos_x, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &posicion_sabotaje->pos_y, sizeof(uint32_t), offset);

    return posicion_sabotaje;
}


void u_msg_informar_sabotaje_eliminar(u_msg_posicion_sabotaje_t* _msg) {
	u_free(_msg);
}

char* u_msg_informar_sabotaje_to_string(const u_msg_posicion_sabotaje_t* msg) {
    return string_from_format(
        "MSG_INFORMAR_SABOTAJE: { Pos_X: %d | Pos_Y: %d }",
        msg->pos_x, msg->pos_y
    );
}