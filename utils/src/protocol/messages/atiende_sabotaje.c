#include "utils/protocol/messages/atiende_sabotaje.h"
#include "utils/memory/allocator.h"

#include <commons/string.h>

u_msg_atender_sabotaje_t* u_msg_atiende_sabotaje_crear(uint32_t _tripulante_id){
    u_msg_atender_sabotaje_t* msg = u_malloc(sizeof(u_msg_atender_sabotaje_t));

    msg->tripulante_id= _tripulante_id;

    return msg;
}

u_paquete_t* u_msg_atiende_sabotaje_serializar(const u_msg_atender_sabotaje_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    u_paquete_t* paquete = u_malloc(sizeof(u_paquete_t));
    paquete->opCodeMsg = ATIENDE_SABOTAJE;
    paquete->buffer    = buffer;


    return paquete; 
}

u_msg_atender_sabotaje_t* u_msg_atiende_sabotaje_deserializar(const u_buffer_t* _buffer){
    u_msg_atender_sabotaje_t* atiende_sabotaje = u_malloc(sizeof( u_msg_atender_sabotaje_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &atiende_sabotaje->tripulante_id, sizeof(uint32_t), offset);

    return atiende_sabotaje;
}


void u_msg_atiende_sabotaje_eliminar(u_msg_atender_sabotaje_t* _msg) {
	u_free(_msg);
}

char* u_msg_atiende_sabotaje_to_string(const u_msg_atender_sabotaje_t* msg) {
    return string_from_format(
        "MSG_ATIENDE_SABOTAJE: { TID: %d }", msg->tripulante_id
    );
}