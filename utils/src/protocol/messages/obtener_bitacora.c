#include "utils/protocol/messages/obtener_bitacora.h"
#include "utils/memory/allocator.h"

u_msg_obtener_bitacora_t* u_msg_obtener_bitacora_crear(uint32_t _tripulante_id){
    u_msg_obtener_bitacora_t* msg = u_malloc(sizeof(u_msg_obtener_bitacora_t));

    msg->tripulante_id= _tripulante_id;

    return msg;
}

u_paquete_t* u_msg_obtener_bitacora_serializar(const u_msg_obtener_bitacora_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    u_paquete_t* paquete = u_malloc(sizeof(u_paquete_t));
    paquete->opCodeMsg = OBTENER_BITACORA;
    paquete->buffer    = buffer;


    return paquete; 
}

u_msg_obtener_bitacora_t* u_msg_obtener_bitacora_deserializar(const u_buffer_t* _buffer){
    u_msg_obtener_bitacora_t* obtener_bitacora = u_malloc(sizeof( u_msg_obtener_bitacora_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &obtener_bitacora->tripulante_id, sizeof(uint32_t), offset);

    return obtener_bitacora;
}


void u_msg_obtener_bitacora_eliminar(u_msg_obtener_bitacora_t* _msg) {
	u_free(_msg);
}

char* u_msg_obtener_bitacora_to_string(const u_msg_obtener_bitacora_t* msg) {
    string_from_format(
        "MSG_OBTENER_BITACORA: { TID: %d }",
        msg->tripulante_id
    );
}