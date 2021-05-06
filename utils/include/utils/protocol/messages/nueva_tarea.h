#ifndef UTILS_PROTOCOL_MESSAGES_NUEVA_TAREA_H
#define UTILS_PROTOCOL_MESSAGES_NUEVA_TAREA_H

#include "utils/protocol/buffer.h"

typedef struct
{
    char* tarea;
} u_msg_nueva_tarea_t;

/**
 * @NAME: u_msg_nueva_tarea_crear
 * @DESC: Crea un mensaje u_msg_nueva_tarea_t
 * @PARAMS:
 *  [in] const char* tarea - contenido de la tarea.
 * 
 * @RETURN: Un struct u_msg_nueva_tarea_t*.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_nueva_tarea_eliminar.
 */
u_msg_nueva_tarea_t* u_msg_nueva_tarea_crear(const char* tarea);

/**
 * @NAME: u_msg_nueva_tarea_serializar
 * @DESC: Serializar un u_msg_nueva_tarea_t y lo guarda en un struct u_buffer_t.
 * @PARAMS:
 *  [in] const u_msg_nueva_tarea_t* msg - Mensaje a serializar.
 * 
 * @RETURNS: un struct u_buffer_t que contendra el mensaje serializado.
 */
u_buffer_t* u_msg_nueva_tarea_serializar(const u_msg_nueva_tarea_t* this);

/**
 * @NAME: u_msg_nueva_tarea_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 *  [in] const u_buffer_t* buffer - buffer con el mensaje serializado
 * 
 * @RETURN: Un u_msg_nueva_tarea_t que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_nueva_tarea_eliminar.
 */
u_msg_nueva_tarea_t* u_msg_nueva_tarea_deserializar(const u_buffer_t* buffer);

/**
 * @NAME: u_msg_nueva_tarea_eliminar
 * @DESC: Elimina un struct u_msg_nueva_tarea_t
 * @PARAMS:
 *  [in] u_msg_nueva_tarea_t* msg - mensaje a eliminar.
 */
void u_msg_nueva_tarea_eliminar(u_msg_nueva_tarea_t* this);

/**
 * @NAME: u_msg_nueva_tarea_to_string
 * @DESC: Crea un string representativo del mensaje
 * @PARAMS:
 *  [in] const u_msg_nueva_tarea_t* msg - mensaje del cual se quiere obtener una representacion en string.
 * @RETURN: Un string representando el contenido del mensaje.
 * 
 * @NOTA: El string debe ser liberado posteriormente.
 */
char* u_msg_nueva_tarea_to_string(const u_msg_nueva_tarea_t* this);

#endif