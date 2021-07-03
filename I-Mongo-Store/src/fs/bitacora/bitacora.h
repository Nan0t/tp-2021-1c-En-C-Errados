#ifndef FS_BITACORA_H
#define FS_BITACORA_H

#include <utils/utils.h>

typedef struct fs_bitacora_t fs_bitacora_t;

/**
 * @NAME: fs_bitacora_create
 * @DESC: crea una instancia de bitacora
 * @PARAMS:
 *  [in] const char* mount_point - punto de montaje del sistema
 *  [in] uint32_t    tid         - numero del tripulante.
 * 
 * @RETURNS: una instancia fs_bitacora_t
 */
fs_bitacora_t* fs_bitacora_create(const char* mount_point, uint32_t tid);

/**
 * @NAME: fs_bitacora_delete
 * @DESC: elimina una instancia de bitacora.
 * @PARAMS:
 *  [in] fs_bitacora_t* bitacora - instancia de la bitacora que se quiere eliminar.
 */
void fs_bitacora_delete(fs_bitacora_t* bitacora);

/**
 * @NAME: fs_bitacora_add_content
 * @DESC: escribe informacion al final de la bitacora.
 * @PARAMS:
 *  [in] fs_bitacora_t* bitacora - bitacora en la cual se va a escribir el contenido.
 *  [in] const char*    content  - contenido que se quiere escribir al final de la bitacora.
 */
void fs_bitacora_add_content(fs_bitacora_t* bitacora, const char* content);

/**
 * @NAME: fs_bitacora_get_tid
 * @DESC: devuelve el numero de tripulante al que pertenece la bitacora.
 * @PARAMS:
 *  [in] const fs_bitacora_t* bitacora - bitacora del cual se quiere conocer el numero de tripulante.
 * 
 * @RETURNS: el numero de tripulante.
 */
uint32_t  fs_bitacora_get_tid(const fs_bitacora_t* bitacora);

/**
 * @NAME: fs_bitacora_get_size
 * @DESC: devuelve el tamaño de la bitacora.
 * @PARAMS:
 *  [in] const fs_bitacora_t* bitacora - bitacora del cual se quiere conocer su tamaño.
 * 
 * @RETURNS: el tamaño de la bitacora.
 */
uint32_t  fs_bitacora_get_size(const fs_bitacora_t* bitacora);

/**
 * @NAME: fs_bitacora_get_block_count
 * @DESC: devuelve la cantidad de bloques que ocupa una bitacora.
 * @PARAMS:
 *  [in] const fs_bitacora_t* bitacora - bitacora de la cual se quiere conocer la cantidad de bloques que ocupa.
 * 
 * @RETURNS: el numero de bloques ocupados por la bitacora.
 */
uint32_t  fs_bitacora_get_block_count(const fs_bitacora_t* bitacora);

/**
 * @NAME: fs_bitacora_get_content
 * @DESC: devuelve el contenido de una bitacora.
 * @PARAMS:
 *  [in] const fs_bitacora_t* bitacora - bitacora de la cual se quiere obtener su contenido.
 * 
 * @RETURNS: un copia del contenido de la bitacora. Esta debe ser liberada posteriormente con u_free.
 */
char* fs_bitacora_get_content(const fs_bitacora_t* bitacora);

#endif