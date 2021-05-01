#ifndef UTILS_PROTOCOL_INICIO_TAREA_H
#define UTILS_PROTOCOL_INICIO_TAREA_H

#include "utils/protocol/paquete.h"

typedef struct {
    uint32_t tripulante_id;
    uint32_t largo_nombre_tarea;
    char*    tarea;
} msg_inicio_tarea_t;


/**
 * @NAME: u_msg_inicio_tarea_crear
 * @DESC: Crea un mensaje msg_inicio_tarea_t
 * @PARAMS:
 * 			[in] uint32_t _tripulante_id - ID del tripulante.
 *          [in] const char* _tarea -  tarea que se inicia.
 * @RETURN: Un struct msg_inicio_tarea_t.
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_inicio_tarea_eliminar.
 */
msg_inicio_tarea_t* u_msg_inicio_tarea_crear(uint32_t _tripulante_id, const char* _tarea);

/**
 * @NAME: u_msg_inicio_tarea_serializar
 * @DESC: Serializar un msg_inicio_tarea_t y lo guarda en un struct paquete_t.
 * @PARAMS:
 * 			[in] const msg_inicio_tarea_t* _msg - Mensaje a serializar.
 * @RETURNS: un struct paquete_t que contendra el mensaje serializado.
 */
paquete_t* u_msg_inicio_tarea_serializar(const msg_inicio_tarea_t* _msg);

/**
 * @NAME: u_msg_inicio_tarea_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 * 			[in] const u_buffer_t* _buffer - buffer con el mensaje serializado
 * @RETURN: Un msg_inicio_tarea_t que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_inicio_tarea_eliminar.
 */
msg_inicio_tarea_t* u_msg_inicio_tarea_deserializar(const u_buffer_t* _buffer);

/**
 * @NAME: u_msg_inicio_tarea_eliminar
 * @DESC: Elimina un struct msg_inicio_tarea_t
 * @PARAMS:
 * 			[in] msg_inicio_tarea_t* _msg - mensaje a eliminar.
 */
void u_msg_inicio_tarea_eliminar(msg_inicio_tarea_t* _msg);


#endif