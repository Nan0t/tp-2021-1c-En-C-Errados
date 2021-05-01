#ifndef UTILS_PROTOCOL_OBTENER_BITACORA_H
#define UTILS_PROTOCOL_OBTENER_BITACORA_H

#include "utils/protocol/paquete.h"

typedef struct {
    uint32_t tripulante_id;
    
} msg_obtener_bitacora_t;



/**
 * @NAME: u_msg_obtener_bitacora_crear
 * @DESC: Crea un mensaje msg_obtener_bitacora_t
 * @PARAMS:
 * 			[in] uint32_t _tripulante_id - id del tripulante.
 * @RETURN: Un struct msg_obtener_bitacora_t.
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_obtener_bitacora_eliminar.
 */
msg_obtener_bitacora_t* u_msg_obtener_bitacora_crear(uint32_t _tripulante_id);

/**
 * @NAME: u_msg_obtener_bitacora_serializar
 * @DESC: Serializar un msg_obtener_bitacora_t y lo guarda en un struct paquete_t.
 * @PARAMS:
 * 			[in] const msg_obtener_bitacora_t* _msg - Mensaje a serializar.
 * @RETURNS: un struct paquete_t que contendra el mensaje serializado.
 */
paquete_t* u_msg_obtener_bitacora_serializar(const msg_obtener_bitacora_t* _msg);

/**
 * @NAME: u_msg_obtener_bitacora_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 * 			[in] const u_buffer_t* _buffer - buffer con el mensaje serializado
 * @RETURN: Un msg_obtener_bitacora_t que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_obtener_bitacora_eliminar.
 */
msg_obtener_bitacora_t* u_msg_obtener_bitacora_deserializar(const u_buffer_t* _buffer);

/**
 * @NAME: u_msg_obtener_bitacora_eliminar
 * @DESC: Elimina un struct msg_obtener_bitacora_t
 * @PARAMS:
 * 			[in] msg_obtener_bitacora_t* _msg - mensaje a eliminar.
 */
void u_msg_obtener_bitacora_eliminar(msg_obtener_bitacora_t* _msg);


#endif