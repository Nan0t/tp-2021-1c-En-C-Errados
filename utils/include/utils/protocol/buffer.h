#ifndef UTILS_PROTOCOL_BUFFER_H
#define UTILS_PROTOCOL_BUFFER_H

#include "utils/typedefs.h"


typedef struct u_buffer_t u_buffer_t;

/**
 * @NAME: u_buffer_create
 * @DESC: Crea un struct u_buffer_t
 * @RETURNS: Un u_buffer_t*
 */
u_buffer_t* u_buffer_create(void);

/**
 * @NAME: u_buffer_dpulicate
 * @DESC: Crea una copia de un buffer con su propio contenido
 * @PARAMS:
 *  [in] const u_buffer_t* other_buffer - buffer que se quiere copiar.
 * 
 * @RETURNS: Un nuevo buffer con una copia del contenido del buffer
 */
u_buffer_t* u_buffer_duplicate(const u_buffer_t* other_buffer);

/**
 * @NAME: u_buffer_delete
 * @DESC: Eliminar una instancia de u_buffer_t
 * @PARAMS:
 *  [in] u_buffer_t* buffer - buffer a eliminar
 */
void u_buffer_delete(u_buffer_t* buffer);

/**
 * @NAME: u_buffer_write
 * @DESC: copia el contenido de la memoria apuntada por data al final del buffer
 * @PARAMS:
 *  [in] u_buffer_t* buffer - buffer donde se quiere escribir
 *  [in] const void* data   - puntero con la referencia de la memoria que se quiere
 *                            copiar en el buffer.
 *  [in] uint64_t    size   - tamaño de la memoria que se quiere copiar.
 */
void u_buffer_write(u_buffer_t* buffer, const void* data, uint64_t size);

/**
 * @NAME: u_buffer_read
 * @DESC: lee el contenido del buffer.
 * @PARAMS:
 *  [in]  const u_buffer_t* buffer - buffer del cual se quiere leer su contenido.
 *  [out] void*             data   - puntero con la referencia a la memoria donde se quiere
 *                                   copiar cierto contenido del buffer.
 *  [in]  uint64_t          size   - cantidad de bytes que se quieren leer del buffer.
 *  [in]  uint64_t          offset - desplazamiento dentro del buffer desde el cual se va a
 *                                   leer.
 */
void u_buffer_read(const u_buffer_t* buffer, void* data, uint64_t size, uint64_t offset);

/**
 * @NAME: u_buffer_get_size
 * @DESC: devuelve el tamaño del buffer.
 * @PARAMS:
 *  [in] const u_buffer_t* buffer - buffer del cual se quiere conocer su tamaño.
 */
uint32_t    u_buffer_get_size(const u_buffer_t* buffer);

/**
 * @NAME: u_buffer_get_content
 * @DESC: devuelve un puntero que referencia al contenido del buffer
 * @PARAMS:
 *  [in] const u_buffer_t* buffer - buffer del cual se quiere obtener su contenido.
 */
const void* u_buffer_get_content(const u_buffer_t* buffer);

#endif