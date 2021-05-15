#ifndef UTILS_MEMORY_ALLOCATOR_H
#define UTILS_MEMORY_ALLOCATOR_H

#include "utils/typedefs.h"

/**
 * @FILE: allocator.h
 * @DESC: Header que contiene wrappers al rededor de las funciones comunes para
 * reservar memoria de la libreria estandar de C.
 */

/**
 * @NAME: u_malloc
 * @DESC: reserva memoria dinamica del tamaño especificado por size
 * @PARAMS:
 *  [in] uint64_t size - cantidad de memoria a reservar.
 * 
 * @RETURNS: un puntero a la direccion de memoria reservada si se pudo reservar.
 * En caso contrario NULL.
 * 
 * @NOTA: en caso de que el malloc falle el programa aborta.
 */
void* u_malloc(uint64_t size);

/**
 * @NAME: u_realloc
 * @DESC: redimensiona la memoria apuntada por ptr en size cantidad.
 * @PARAMS:
 *  [in] void*     ptr  - espacio de memoria a redimensionar.
 *  [in] uint64_t  size - nuevo tamaño de memoria.
 * 
 * @RETURNS: un puntero con el nuevo espacio de memoria redimensionado, o NULL
 * en caso de que no se haya podido redimensionar la memoria.
 * 
 * @NOTA: en caso de que no se haya podido redimensionar el programa aborta.
 */
void* u_realloc(void* ptr, uint64_t size);

/**
 * @NAME: u_free
 * @DESC: libera memoria previamente reservada por u_malloc o u_realloc.
 * @PARAMS:
 *  [in] void* ptr - memoria a liberar.
 * 
 * @NOTA: en caso de que ptr sea NULL.
 */
void  u_free(void* ptr);

#endif
