#ifndef UTILS_MEMORY_ALLOCATOR_H
#define UTILS_MEMORY_ALLOCATOR_H

#include "utils/typedefs.h"

/**
 * @File: allocator.h
 * 
 * @Desc: Header que contiene wrappers al rededor de las funciones comunes para
 * reservar memoria de la libreria estandar de C.
 */

/**
 * @Func: u_malloc
 * 
 * @Desc: reserva memoria dinamica del tamaño especificado por size
 * 
 * @Params:
 *  uint64_t size   --> cantidad de memoria a reservar.
 * 
 * @Returns: un puntero a la direccion de memoria reservada si se pudo reservar.
 * En caso contrario NULL.
 * 
 * @Assert: en caso de que el malloc falle.
 */
void* u_malloc(uint64_t size);

/**
 * @Func: u_realloc
 * 
 * @Desc: redimensiona la memoria apuntada por ptr en size cantidad.
 * 
 * @Params:
 *  void*     ptr   --> espacio de memoria a redimensionar.
 *  uint64_t  size  --> nuevo tamaño de memoria.
 * 
 * @Returns: un puntero con el nuevo espacio de memoria redimensionado, o NULL
 * en caso de que no se haya podido redimensionar la memoria.
 * 
 * @Assert: en caso de que no se haya podido redimensionar la memoria, o en caso
 * de que ptr sea NULL.
 */
void* u_realloc(void* ptr, uint64_t size);

/**
 * @Func: u_free
 * 
 * @Desc: libera memoria previamente reservada por u_malloc o u_realloc.
 * 
 * @Params:
 *  void* ptr    --> memoria a liberar.
 * 
 * @Assert: en caso de que ptr sea NULL.
 */
void  u_free(void* ptr);

#endif
