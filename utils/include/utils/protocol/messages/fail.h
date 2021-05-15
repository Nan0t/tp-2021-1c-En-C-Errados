#ifndef UTILS_PROTOCOL_MESSAGE_FAIL_H
#define UTILS_PROTOCOL_MESSAGE_FAIL_H

#include "utils/protocol/buffer.h"

typedef struct
{
    char* description;
} u_msg_fail_t;

/**
 * @NAME: u_msg_fail_crear
 * @DESC: Crea un mensaje u_msg_fail_t
 * @PARAMS:
 *  [in] const char* message - mensaje con formato printf que se va a enviar como mensaje de error.
 *              ...          - argumentos que correspondan al formato del message.
 * 
 * @RETURN: Un struct u_msg_fail_crear.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_fail_eliminar.
 */
u_msg_fail_t* u_msg_fail_crear(const char* format, ...) UTILS_PRINT_FORMAT(1, 2);

/**
 * @NAME: u_msg_fail_serializar
 * @DESC: Serializar un u_msg_fail_t y lo guarda en un struct u_buffer_t.
 * @PARAMS:
 *  [in] const u_msg_fail_t* msg - Mensaje a serializar.
 * 
 * @RETURNS: un struct u_buffer_t que contendra el mensaje serializado.
 */
u_buffer_t* u_msg_fail_serializar(const u_msg_fail_t* msg);

/**
 * @NAME: u_msg_fail_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 *  [in] const u_buffer_t* buffer - buffer con el mensaje serializado
 * 
 * @RETURN: Un u_msg_fail_t* que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_fail_eliminar.
 */
u_msg_fail_t* u_msg_fail_deserializar(const u_buffer_t* buffer);

/**
 * @NAME: u_msg_fail_eliminar
 * @DESC: Elimina un struct u_msg_fail_t
 * @PARAMS:
 *  [in] u_msg_atender_sabotaje_t* msg - mensaje a eliminar.
 */
void u_msg_fail_eliminar(u_msg_fail_t* msg);

/**
 * @NAME: u_msg_fail_to_string
 * @DESC: Crea un string representativo del mensaje
 * @PARAMS:
 *          [in] const u_msg_fail_t* msg - mensaje del cual se quiere obtener una representacion en string.
 * 
 * @RETURN: Un string representando el contenido del mensaje.
 * 
 * @NOTA: El string debe ser liberado posteriormente.
 */
char* u_msg_fail_to_string(const u_msg_fail_t* this);

#endif