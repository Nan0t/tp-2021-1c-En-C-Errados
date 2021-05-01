#include "utils/protocol/inicio_tarea.h"
#include <stdint.h>


u_msg_inicio_tarea_t* u_msg_inicio_tarea_crear(uint32_t _tripulante_id, const char* _tarea){
    u_msg_inicio_tarea_t* msg = malloc(sizeof(u_msg_inicio_tarea_t));

    msg->tripulante_id      = _tripulante_id;
    msg->largo_nombre_tarea = strlen(_tarea) + 1;
    msg->tarea              = strdup(_tarea);

    return msg;
}

u_paquete_t* u_msg_inicio_tarea_serializar(const u_msg_inicio_tarea_t* _msg){
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &_msg->tripulante_id, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->largo_nombre_tarea, sizeof(uint32_t));
    u_buffer_write(buffer, &_msg->tarea, _msg->largo_nombre_tarea);

    u_paquete_t* paquete = malloc(sizeof(u_paquete_t));
    paquete->opCodeMsg = INICIO_TAREA;
    paquete->buffer    = buffer;

    return paquete; 
}

u_msg_inicio_tarea_t* u_msg_inicio_tarea_deserializar(const u_buffer_t* _buffer){
    u_msg_inicio_tarea_t* inicio_tarea = malloc(sizeof( u_msg_inicio_tarea_t));
    uint32_t offset = 0;

    u_buffer_read(_buffer, &inicio_tarea->tripulante_id, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    u_buffer_read(_buffer, &inicio_tarea->largo_nombre_tarea, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);
    inicio_tarea->tarea = malloc(sizeof(inicio_tarea->largo_nombre_tarea));
    u_buffer_read(_buffer, inicio_tarea->tarea, inicio_tarea->largo_nombre_tarea, offset);

    return inicio_tarea;
}


void u_msg_inicio_tarea_eliminar(u_msg_inicio_tarea_t* _msg) {
    free((void*)_msg->tarea);
	free(_msg);
}
