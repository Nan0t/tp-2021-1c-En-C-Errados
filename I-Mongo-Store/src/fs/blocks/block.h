#ifndef FS_BLOCK_H
#define FS_BLOCK_H

#include <utils/utils.h>

/**
 * @NAME: fs_block_write
 * @DESC: escribe el contenido apuntado por "data" dentro del bloque.
 * @PARAMS:
 *  [in] uint32_t    block_id  - numero de bloque.
 *  [in] const void* data      - direccion de memoria con el valor a guardar en el disco.
 *  [in] uint64_t    data_size - tamaño del dato a guardar en bytes.
 *  [in] uint64_t    offset    - offset dentro del bloque donde se debe empezar a escribir.
 * 
 * @RETURNS: la cantidad de bytes escritos en el bloque. Si se intenta escribir mas haya de los
 *              limites del bloque, el bloque deja de escribir y devuelve la cantidad de bytes
 *              que se pudieron escribir (que pueden resultar menor a la cantidad de bytes que
 *              se deseaban escribir).
 */
uint64_t fs_block_write(uint32_t block_id, const void* data, uint64_t data_size, uint64_t offset);

/**
 * @NAME: fs_block_read
 * @DESC: lee y guarda del disco el contenido que se encuentra a "offset" bytes del inicio del bloque en
 *          la memoria apuntada por "data".
 * @PARAMS:
 *  [in] uint32_t block_id  - numero de bloque.
 *  [in] void*    data      - direccion de memoria en donde se quiere guardar el contenido del disco.
 *  [in] uint64_t data_size - tamaño del dato a leer del disco en bytes.
 *  [in] uint64_t offset    - offset dentro del bloque donde se quiere empezar a leer.
 * 
 * @RETURNS: la cantidad de bytes leidos del bloque. Si se intenta leer mas haya de los
 *              limites del bloque, el bloque deja leer y devuelve la cantidad de bytes
 *              que se pudieron leer (que pueden resultar menor a la cantidad de bytes que
 *              se deseaban leer).
 */
uint64_t fs_block_read(uint32_t block, void* data, uint64_t data_size, uint64_t offset);

/**
 * @NAME: fs_block_get_disk_offset
 * @DESC: permite consultar el offset dentro del disco en el que se encuentra el bloque.
 * @PARAMS:
 *  [in] uint32_t block_id - numero de bloque.
 * 
 * @RETURNS: el offset dentro del disco del bloque
 */
//para testeo
uint32_t fs_block_get_disk_offset(uint32_t block_id);

#endif
