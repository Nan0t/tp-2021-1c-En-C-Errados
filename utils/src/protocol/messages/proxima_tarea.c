#include "utils/protocol/messages/proxima_tarea.h"
#include "utils/memory/allocator.h"

u_msg_proxima_tarea_t* u_msg_proxima_tarea_crear(uint32_t _tripulante_id){
    u_msg_proxima_tarea_t* msg = u_malloc(sizeof(u_msg_proxima_tarea_t));
    msg->tripulante_id = _tripulante_id;

    return msg;
}

u_buffer_t* u_msg_proxima_tarea_serializar(const u_msg_proxima_tarea_t* _msg){
    u_buffer_t* buffer = u_buffer_create();
    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    return buffer;
}

u_msg_proxima_tarea_t* u_msg_proxima_tarea_deserializar(const u_buffer_t* _buffer){
    u_msg_proxima_tarea_t* proxima_tarea = u_malloc(sizeof( u_msg_proxima_tarea_t));
    u_buffer_read(_buffer, &proxima_tarea->tripulante_id, sizeof(uint32_t), 0);

    return proxima_tarea;
}


void u_msg_proxima_tarea_eliminar(u_msg_proxima_tarea_t* _msg) {
	u_free(_msg);
}

char* u_msg_proxima_tarea_to_string(const u_msg_proxima_tarea_t* msg) {
    return string_from_format(
        "MSG_PROXIMA_TAREA: { TID: %d }",
        msg->tripulante_id
    );
}