#ifndef UTILS_PROTOCOL_RESUELVE_SABOTAJE_H
#define UTILS_PROTOCOL_RESUELVE_SABOTAJE_H

#include "utils/protocol/paquete.h"

typedef struct {
    uint32_t tripulante_id;
    
} u_msg_resuelve_sabotaje_t;



/**
 * @NAME: u_msg_resuelve_sabotaje_crear
 * @DESC: Crea un mensaje u_msg_resuelve_sabotaje_t
 * @PARAMS:
 * 			[in] uint32_t _tripulante_id - id del tripulante.
 * @RETURN: Un struct u_msg_resuelve_sabotaje_t.
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_resuelve_sabotaje_eliminar.
 */
u_msg_resuelve_sabotaje_t* u_msg_resuelve_sabotaje_crear(uint32_t _tripulante_id);

/**
 * @NAME: u_msg_resuelve_sabotaje_serializar
 * @DESC: Serializar un u_msg_resuelve_sabotaje_t y lo guarda en un struct u_paquete_t.
 * @PARAMS:
 * 			[in] const u_msg_resuelve_sabotaje_t* _msg - Mensaje a serializar.
 * @RETURNS: un struct u_paquete_t que contendra el mensaje serializado.
 */
u_paquete_t* u_msg_resuelve_sabotaje_serializar(const u_msg_resuelve_sabotaje_t* _msg);

/**
 * @NAME: u_msg_resuelve_sabotaje_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 * 			[in] const u_buffer_t* _buffer - buffer con el mensaje serializado
 * @RETURN: Un u_msg_resuelve_sabotaje_t que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_resuelve_sabotaje_eliminar.
 */
u_msg_resuelve_sabotaje_t* u_msg_resuelve_sabotaje_deserializar(const u_buffer_t* _buffer);

/**
 * @NAME: u_msg_resuelve_sabotaje_eliminar
 * @DESC: Elimina un struct u_msg_resuelve_sabotaje_t
 * @PARAMS:
 * 			[in] u_msg_resuelve_sabotaje_t* _msg - mensaje a eliminar.
 */
void u_msg_resuelve_sabotaje_eliminar(u_msg_resuelve_sabotaje_t* _msg);


#endif