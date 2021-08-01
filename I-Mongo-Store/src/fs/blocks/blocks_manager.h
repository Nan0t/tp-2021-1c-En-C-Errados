#ifndef FS_BLOCKS_MANAGER_H
#define FS_BLOCKS_MANAGER_H

#include "block.h"

/**
 * @NAME: fs_blocks_manager_init
 * @DESC: inicializa el administrador de bloques.
 * @PARAMS:
 *  [in] const char* mount_point - string con el punto de montaje.
 */
void fs_blocks_manager_init(const char* mount_point, bool is_clean_initialization);

/**
 * @NAME: fs_blocks_manager_request_block
 * @DESC: permite obtener un bloque disponible del disco.
 * @RETURNS: el numero de bloque asignado. La numeracion de los bloques arranca desde
 *             1, por lo que si la funcion retorna 0, es indicio de un error.
 */
uint32_t fs_blocks_manager_request_block(void);

void fs_blocks_manager_sync(void);

/**
 * @NAME: fs_blocks_manager_release_block
 * @DESC: libera un bloque del disco.
 * @PARAMS:
 *  [in] uint32_t block_id - numero de bloque a liberar.
 */
void fs_blocks_manager_release_block(uint32_t block_id);

/**
 * @NAME: fs_blocks_manager_get_blocks_size
 * @DESC: devuelve el tamaño de los bloques.
 * @RETURNS: el tamaño que poseen los bloques en el sistema.
 */
uint32_t fs_blocks_manager_get_blocks_size(void);

bool fs_blocks_manager_check_integrity(void);

#endif