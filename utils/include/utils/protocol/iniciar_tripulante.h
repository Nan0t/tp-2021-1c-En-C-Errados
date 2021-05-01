#ifndef UTILS_PROTOCOL_INICIAR_TRIPULANTE_H
#define UTILS_PROTOCOL_INICIAR_TRIPULANTE_H

#include "utils/protocol/paquete.h"

typedef struct {
    uint32_t patota_id;
    uint32_t tripulante_id;
} msg_iniciar_tripulante_t;



/**
 * @NAME: u_msg_iniciar_tripulante_crear
 * @DESC: Crea un mensaje msg_iniciar_tripulante_t
 * @PARAMS:
 * 			[in] uint32_t _tripulante_id - id del tripulante.
 * 			[in] uint32_t _patota_id     - id de la patota.
 * @RETURN: Un struct msg_iniciar_tripulante_t.
 * @NOTA: El struct resultante debe ser liberado posteriormente llamanda a u_msg_informar_iniciar_tripulante_eliminar.
 */
msg_iniciar_tripulante_t* u_msg_iniciar_tripulante_crear(uint32_t _patota_id, uint32_t _tripulante_id);

/**
 * @NAME: u_msg_iniciar_tripulante_serializar
 * @DESC: Serializar un msg_iniciar_tripulante_t y lo guarda en un struct paquete_t.
 * @PARAMS:
 * 			[in] const msg_iniciar_tripulante_t* _msg - Mensaje a serializar.
 * @RETURNS: un struct paquete_t que contendra el mensaje serializado.
 */
paquete_t* u_msg_iniciar_tripulante_serializar(const msg_iniciar_tripulante_t* _msg);

/**
 * @NAME: u_msg_informar_iniciar_tripulante_deserializar
 * @DESC: Dado un buffer que contiene el mensaje serializado, deserializa el mensaje.
 * @PARAMS:
 * 			[in] const u_buffer_t* _buffer - buffer con el mensaje serializado
 * @RETURN: Un msg_iniciar_tripulante_t que contiene el mensaje deserializado.
 * 
 * @NOTA: El struct resultante debe ser liberado posteriormente llamando a u_msg_informar_iniciar_tripulante_eliminar.
 */
msg_iniciar_tripulante_t* u_msg_informar_iniciar_tripulante_deserializar(const u_buffer_t* _buffer);

/**
 * @NAME: u_msg_informar_iniciar_tripulante_eliminar
 * @DESC: Elimina un struct msg_iniciar_tripulante_t
 * @PARAMS:
 * 			[in] msg_iniciar_tripulante_t* _msg - mensaje a eliminar.
 */
void u_msg_informar_iniciar_tripulante_eliminar(msg_iniciar_tripulante_t* _msg);


#endif