#ifndef FS_BITACORA_H
#define FS_BITACORA_H

#include <utils/utils.h>

typedef struct fs_bitacora_t fs_bitacora_t;

/**
 * @NAME: fs_bitacora_create
 * @DESC: crea una instancia de bitacora
 * @PARAMS:
 *  [in] const char* path - ruta de la bitacora
 *  [in] uint32_t    tid  - numero del tripulante.
 * 
 * @RETURNS: una instancia fs_bitacora_t
 */
fs_bitacora_t* fs_bitacora_create(const char* path, uint32_t tid);

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
 * @NAME: fs_bitacora_get_content
 * @DESC: devuelve el contenido de una bitacora.
 * @PARAMS:
 *  [in] const fs_bitacora_t* bitacora - bitacora de la cual se quiere obtener su contenido.
 * 
 * @RETURNS: un copia del contenido de la bitacora. Esta debe ser liberada posteriormente con u_free.
 */
char* fs_bitacora_get_content(const fs_bitacora_t* bitacora);


/**
 * @NAME: fs_bitacora_get_blocks
 * @DESC: devuelve la t_list con los bloques utilizados por la bitacora.
 * @PARAMS:
 *  [in] const fs_bitacora_t* bitacora - bitacora de la cual se quiere obtener su contenido.
 * 
 * @RETURNS: devuelve la t_list. Esta debe ser liberada posteriormente con list_destroy_and_destroy_elements
 */
t_list* fs_bitacora_get_blocks(const fs_bitacora_t* this);

#endif