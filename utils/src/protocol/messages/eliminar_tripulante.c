#include "utils/protocol/messages/eliminar_tripulante.h"
#include "utils/memory/allocator.h"

#include <commons/string.h>

u_msg_eliminar_tripulante_t* u_msg_eliminar_tripulante_crear(uint32_t pid, uint32_t tid){
    u_msg_eliminar_tripulante_t* msg = u_malloc(sizeof(u_msg_eliminar_tripulante_t));

    msg->pid = pid;
    msg->tid = tid;

    return msg;
}

u_buffer_t* u_msg_eliminar_tripulante_serializar(const u_msg_eliminar_tripulante_t* _msg){
    u_buffer_t* buffer = u_buffer_create();
    
    u_buffer_write(buffer, &_msg->pid, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->tid, sizeof(uint32_t));

    return buffer;
}

u_msg_eliminar_tripulante_t* u_msg_eliminar_tripulante_deserializar(const u_buffer_t* _buffer){
    u_msg_eliminar_tripulante_t* eliminar_tripulante = u_malloc(sizeof( u_msg_eliminar_tripulante_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &eliminar_tripulante->pid, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &eliminar_tripulante->tid, sizeof(uint32_t), offset);

    return eliminar_tripulante;
}


void u_msg_eliminar_tripulante_eliminar(u_msg_eliminar_tripulante_t* _msg) {
	u_free(_msg);
}

char* u_msg_eliminar_tripulante_to_string(const u_msg_eliminar_tripulante_t* msg) {
    return string_from_format(
        "MSG_ELIMINAR_TRIPULANTE: { PID: %d | TID: %d }",
        msg->pid, msg->tid
    );
}