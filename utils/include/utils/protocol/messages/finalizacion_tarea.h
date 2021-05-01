#ifndef UTILS_PROTOCOL_FINALIZACION_TAREA_H
#define UTILS_PROTOCOL_FINALIZACION_TAREA_H

#include "utils/protocol/paquete.h"

typedef struct {
    uint32_t tripulante_id;
    uint32_t largo_nombre_tarea;
    char*    tarea;
} u_msg_finalizacion_tarea_t;


/**
 * @NAME: u_msg_finalizacion_tarea_crear
 * @DESC: Crea un mensaje u_msg_finalizacion_tarea_t
 * @PARAMS:
 * 			[in] uint32_t _tripulante_id - ID del tripulante.
 *          [in] const char* _tarea -  tarea que se inicia.
 * @RETURN: Un struct u_msg_finalizacion_tarea_t.
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_finalizacion_tarea_eliminar.
 */
u_msg_finalizacion_tarea_t* u_msg_finalizacion_tarea_crear(uint32_t _tripulante_id, const char* _tarea);

/**
 * @NAME: u_msg_finalizacion_tarea_serializar
 * @DESC: Serializar un u_msg_finalizacion_tarea_t y lo guarda en un struct u_paquete_t.
 * @PARAMS:
 * 			[in] const u_msg_finalizacion_tarea_t* _msg - Mensaje a serializar.
 * @RETURNS: un struct u_paquete_t que contendra el mensaje serializado.
 */
u_paquete_t* u_msg_finalizacion_tarea_serializar(const u_msg_finalizacion_tarea_t* _msg);

/**
 * @NAME: u_msg_finalizacion_tarea_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 * 			[in] const u_buffer_t* _buffer - buffer con el mensaje serializado
 * @RETURN: Un u_msg_finalizacion_tarea_t que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_finalizacion_tarea_eliminar.
 */
u_msg_finalizacion_tarea_t* u_msg_finalizacion_tarea_deserializar(const u_buffer_t* _buffer);

/**
 * @NAME: u_msg_finalizacion_tarea_eliminar
 * @DESC: Elimina un struct u_msg_finalizacion_tarea_t
 * @PARAMS:
 * 			[in] u_msg_finalizacion_tarea_t* _msg - mensaje a eliminar.
 */
void u_msg_finalizacion_tarea_eliminar(u_msg_finalizacion_tarea_t* _msg);


#endif