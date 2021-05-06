#ifndef INCLUDE_UTILS_PROTOCOL_MESSAGES_BITACORA_H_
#define INCLUDE_UTILS_PROTOCOL_MESSAGES_BITACORA_H_

#include "utils/protocol/buffer.h"

typedef struct
{
	char* contenido;
} u_msg_bitacora_t;

/**
 * @NAME: u_msg_bitacora_crear
 * @DESC: Crea un mensaje u_msg_bitacora_t
 * @PARAMS:
 * 	[in] const char* contenido_bitacora - contenido de la bitacora.
 * 
 * @RESTURNS: Un struct u_msg_bitacora_t.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_bitacora_eliminar.
 */
u_msg_bitacora_t* u_msg_bitacora_crear(const char* contenido_bitacora);

/**
 * @NAME: u_msg_bitacora_serializar
 * @DESC: Serializar un u_msg_bitacora_t y lo guarda en un struct u_buffer_t.
 * @PARAMS:
 * 			[in] const u_msg_bitacora_t* msg - Mensaje a serializar.
 * @RETURNS: un struct u_buffer_t que contendra el mensaje serializado.
 */
u_buffer_t*      u_msg_bitacora_serializar(const u_msg_bitacora_t* msg);

/**
 * @NAME: u_msg_bitacora_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 * 			[in] const u_buffer_t* _buffer - buffer con el mensaje serializado
 * @RETURN: Un u_msg_bitacora_t* que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_bitacora_eliminar.
 */
u_msg_bitacora_t* u_msg_bitacora_deserializar(const u_buffer_t* buffer);

/**
 * @NAME: u_msg_bitacora_eliminar
 * @DESC: Elimina un struct u_msg_bitacora_t
 * @PARAMS:
 * 			[in] u_msg_bitacora_t* msg - mensaje a eliminar.
 */
void u_msg_bitacora_eliminar(u_msg_bitacora_t* msg);

/**
 * @NAME: u_msg_bitacora_to_string
 * @DESC: Crea un string representativo del mensaje
 * @PARAMS:
 *          [in] const u_msg_bitacora_t* msg - mensaje del cual se quiere obtener una representacion en string.
 * @RETURN: Un string representando el contenido del mensaje.
 * 
 * @NOTA: El string debe ser liberado posteriormente.
 */
char* u_msg_bitacora_to_string(const u_msg_bitacora_t* this);

#endif
