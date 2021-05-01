#include "utils/protocol/proxima_tarea.h"
#include <stdint.h>




msg_proxima_tarea_t* u_msg_proxima_tarea_crear(uint32_t _patota_id, uint32_t _tripulante_id){
    msg_proxima_tarea_t* msg = malloc(sizeof(msg_proxima_tarea_t));

    msg->patota_id = _patota_id;
    msg->tripulante_id = _tripulante_id;

    return msg;
}

paquete_t* u_msg_proxima_tarea_serializar(const msg_proxima_tarea_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->patota_id, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));

    paquete_t* paquete = malloc(sizeof(paquete_t));
    paquete->opCodeMsg = PROXIMA_TAREA;
    paquete->buffer    = buffer;


    return paquete; 
}

msg_proxima_tarea_t* u_msg_proxima_tarea_deserializar(const u_buffer_t* _buffer){
    msg_proxima_tarea_t* proxima_tarea = u_malloc(sizeof( msg_proxima_tarea_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &proxima_tarea->patota_id, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &proxima_tarea->tripulante_id, sizeof(uint32_t), offset);

    return proxima_tarea;
}


void u_msg_proxima_tarea_eliminar(msg_proxima_tarea_t* _msg) {
	free(_msg);
}

