#ifndef UTILS_PROTOCOL_PAQUETE_H
#define UTILS_PROTOCOL_PAQUETE_H

#include "utils/protocol/opcodes.h"
#include "utils/protocol/buffer.h"

typedef struct {
    u_opcode_e  opCodeMsg; 
    u_buffer_t* buffer;
} u_package_t;

/**
 * @NAME: u_package_create.
 * @DESC: Crea un struct u_package_t.
 * @PARAMS:
 *  [in] u_opcode_e opcode        - opcode del mensaje.
 *  [in] const u_buffer_t* buffer - el mensaje serializado.
 * 
 * @RETURNS: un struct u_package_t con el contenido del mensaje serializado y su opcode.
 */
u_package_t* u_package_create(u_opcode_e opcode, const u_buffer_t* buffer);

/**
 * @NAME: u_package_delete
 * @DESC: Eliminar un struct u_package_t
 * @PARAMS:
 *  [in] u_package_t* package - paquete que se quiere eliminar.
 */
void u_package_delete(u_package_t* package);

/**
 * @NAME: u_package_serialize
 * @DESC: serializa un struct u_package_t
 * @PARAMS:
 *  [in] const u_package_t* package - paquete que se quiere serializar.
 * 
 * @RETURNS: un struct u_buffer_t con el paquete serializado.
 */
u_buffer_t* u_package_serialize(const u_package_t* package);

#endif
