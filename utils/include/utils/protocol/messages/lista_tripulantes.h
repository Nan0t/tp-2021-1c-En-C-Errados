#ifndef UTILS_PROTOCOL_MESSAGE_LISTA_TRIPULANTES_H
#define UTILS_PROTOCOL_MESSAGE_LISTA_TRIPULANTES_H

#include "utils/protocol/buffer.h"

typedef struct
{
    uint32_t pid;
    uint32_t tid;
    char     estado;
} u_tripulante_info_t;

typedef struct
{
    t_list* tripulantes;
} u_msg_lista_tripulantes_t;

/**
 * @NAME: u_msg_lista_tripulantes_crear
 * @DESC: Crea un mensaje u_msg_lista_tripulantes_t
 * 
 * @RETURN: un mensaje u_msg_lista_tripulantes_t
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_lista_tripulantes_eliminar.
 */
u_msg_lista_tripulantes_t* u_msg_lista_tripulantes_crear(void);

/**
 * @NAME: u_msg_lista_tripulantes_agregar
 * @DESC: agrega una copia del estado de un tripulante al mensaje.
 * 
 * @PARAMS:
 *  [in] u_msg_lista_tripulantes_t* msg  - mensaje al cual se le va a agregar la info del tripulante.
 *  [in] const u_tripulante_info_t* trip - struct con la info del estado del tripulante.
 */
void u_msg_lista_tripulantes_agregar(u_msg_lista_tripulantes_t* msg, u_tripulante_info_t trip);

/**
 * @NAME: u_msg_lista_tripulantes_serialize
 * @DESC: serializa un mensaje u_msg_lista_tripulantes_t
 * 
 * @PARAMS:
 *  [in] const u_msg_lista_tripulantes_t* msg - mensaje a serializar
 * 
 * @RETURN: un buffer con el mensaje serializado.
 */
u_buffer_t*                u_msg_lista_tripulantes_serialize(const u_msg_lista_tripulantes_t* msg);

/**
 * @NAME: u_msg_lista_tripulantes_deserialize
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 * 			[in] const u_buffer_t* _buffer - buffer con el mensaje serializado
 * @RETURN: Un u_msg_lista_tripulantes_t que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_lista_tripulantes_eliminar.
 */
u_msg_lista_tripulantes_t* u_msg_lista_tripulantes_deserialize(const u_buffer_t* buffer);

/**
 * @NAME: u_msg_lista_tripulantes_eliminar
 * @DESC: Elimina un struct u_msg_lista_tripulantes_t
 * @PARAMS:
 * 			[in] u_msg_lista_tripulantes_t* msg - mensaje a eliminar.
 */
void u_msg_lista_tripulantes_eliminar(u_msg_lista_tripulantes_t* msg);

/**
 * @NAME: u_msg_lista_tripulantes_to_string
 * @DESC: Crea un string representativo del mensaje
 * @PARAMS:
 *          [in] const u_msg_lista_tripulantes_t* msg - mensaje del cual se quiere obtener una representacion en string.
 * @RETURN: Un string representando el contenido del mensaje.
 * 
 * @NOTA: El string debe ser liberado posteriormente.
 */
char* u_msg_lista_tripulantes_to_string(const u_msg_lista_tripulantes_t* msg);

#endif