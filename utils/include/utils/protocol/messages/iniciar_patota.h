#ifndef UTILS_PROTOCOL_MESSAGES_INICIAR_PATOTA_H
#define UTILS_PROTOCOL_MESSAGES_INICIAR_PATOTA_H

#include "utils/protocol/buffer.h"

typedef struct
{
    uint32_t pid;
    uint32_t cant_trips;
    char*    lista_tareas;
} u_msg_iniciar_patota_t;

/**
 * @NAME: u_msg_iniciar_patota_crear
 * @DESC: Crea un mensaje u_msg_iniciar_patota_t
 * @PARAMS:
 *  [in] uint32_t    pid          - id de la patota.
 *  [in] uint32_t    cant_trips   - cantidad de tripulantes de la patota.
 *  [in] const char* lista_tareas - string con la lista de tareas.
 * 
 * @RETURN: Un struct u_msg_iniciar_patota_t*.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_iniciar_patota_eliminar.
 */
u_msg_iniciar_patota_t* u_msg_iniciar_patota_crear(uint32_t pid, uint32_t cant_trips, const char* lista_tareas);

/**
 * @NAME: u_msg_iniciar_patota_serializar
 * @DESC: Serializar un u_msg_iniciar_patota_t y lo guarda en un struct u_buffer_t.
 * @PARAMS:
 *  [in] const u_msg_iniciar_patota_t* msg - Mensaje a serializar.
 * @RETURNS: un struct u_buffer_t que contendra el mensaje serializado.
 */
u_buffer_t* u_msg_iniciar_patota_serializar(const u_msg_iniciar_patota_t* msg);

/**
 * @NAME: u_msg_iniciar_patota_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 *  [in] const u_buffer_t* _buffer - buffer con el mensaje serializado
 * @RETURN: Un u_msg_iniciar_patota_t que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_iniciar_patota_eliminar.
 */
u_msg_iniciar_patota_t* u_msg_iniciar_patota_deserializar(const u_buffer_t* buffer);

/**
 * @NAME: u_msg_iniciar_patota_eliminar
 * @DESC: Elimina un struct u_msg_iniciar_patota_t
 * @PARAMS:
 *  [in] u_msg_iniciar_patota_t* msg - mensaje a eliminar.
 */
void u_msg_iniciar_patota_eliminar(u_msg_iniciar_patota_t* msg);

/**
 * @NAME: u_msg_iniciar_patota_to_string
 * @DESC: Crea un string representativo del mensaje
 * @PARAMS:
 *  [in] const u_msg_iniciar_patota_t* msg - mensaje del cual se quiere obtener una representacion en string.
 * @RETURN: Un string representando el contenido del mensaje.
 * 
 * @NOTA: El string debe ser liberado posteriormente.
 */
char* u_msg_iniciar_patota_to_string(const u_msg_iniciar_patota_t* msg);

#endif