#ifndef UTILS_PROTOCOL_TRIPULANTE_NUEVO_ESTADO_H
#define UTILS_PROTOCOL_TRIPULANTE_NUEVO_ESTADO_H

#include "utils/protocol/paquete.h"

typedef struct {
    uint32_t tripulante_id;
    uint32_t nuevo_estado;
    
} msg_tripulante_nuevo_estado_t;



/**
 * @NAME: u_msg_tripulante_nuevo_estado_crear
 * @DESC: Crea un mensaje msg_tripulante_nuevo_estado_t
 * @PARAMS:
 * 			[in] uint32_t _tripulante_id - id del tripulante.
 * @RETURN: Un struct msg_tripulante_nuevo_estado_t.
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_tripulante_nuevo_estado_eliminar.
 */
msg_tripulante_nuevo_estado_t* u_msg_tripulante_nuevo_estado_crear(uint32_t _tripulante_id, uint32_t _nuevo_estado);

/**
 * @NAME: u_msg_tripulante_nuevo_estado_serializar
 * @DESC: Serializar un msg_tripulante_nuevo_estado_t y lo guarda en un struct paquete_t.
 * @PARAMS:
 * 			[in] const msg_tripulante_nuevo_estado_t* _msg - Mensaje a serializar.
 * @RETURNS: un struct paquete_t que contendra el mensaje serializado.
 */
paquete_t* u_msg_tripulante_nuevo_estado_serializar(const msg_tripulante_nuevo_estado_t* _msg);

/**
 * @NAME: u_msg_tripulante_nuevo_estado_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 * 			[in] const u_buffer_t* _buffer - buffer con el mensaje serializado
 * @RETURN: Un msg_tripulante_nuevo_estado_t que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_tripulante_nuevo_estado_eliminar.
 */
msg_tripulante_nuevo_estado_t* u_msg_tripulante_nuevo_estado_deserializar(const u_buffer_t* _buffer);

/**
 * @NAME: u_msg_tripulante_nuevo_estado_eliminar
 * @DESC: Elimina un struct msg_tripulante_nuevo_estado_t
 * @PARAMS:
 * 			[in] msg_tripulante_nuevo_estado_t* _msg - mensaje a eliminar.
 */
void u_msg_tripulante_nuevo_estado_eliminar(msg_tripulante_nuevo_estado_t* _msg);


#endif