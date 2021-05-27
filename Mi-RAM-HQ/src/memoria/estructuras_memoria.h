/*
 * estructuras_memoria.h
 *
 *  Created on: 17 may. 2021
 *      Author: utnso
 */

#ifndef SRC_MEMORIA_ESTRUCTURAS_MEMORIA_H_
#define SRC_MEMORIA_ESTRUCTURAS_MEMORIA_H_

#include <utils/utils.h>



/* Estructura de los elementos de la lista de Patotas
   Esta estructura aplica tanto para segmentación como para paginación   */

typedef struct
{
    uint32_t pid;
    void* _tabla;
} patota_t;


/* Estructura de los elementos del vector que representa la tabla de
   páginas de una patota. Aplica para paginación   */

typedef struct
{
    int32_t _frame_memoria;
    int32_t _frame_swap;
} paginas_patota_t;


/* Estructura de los elementos del vector que representa la tabla de
   segmentos de una patota. Aplica para segmentación   */

typedef struct
{
    int32_t _inicio_segmento;
    int32_t _longitud_segmento;
} segmentos_patota_t;


/* Estructura de los elementos de la lista de Tripulantes
   Esta estructura aplica para segmentación  */

typedef struct
{
    uint32_t tid;
    uint32_t pid;
    int32_t _indice_tabla_patota;
} tripulante_t;


/* Estructura de los elementos de la lista de Segmentos
   Esta estructura aplica para segmentación  */

typedef struct
{
    int32_t _posicion_inicio;
    int32_t _posicion_fin;
    uint8_t _tipo_de_segmento;
	uint32_t _id_propietario;
} segmentos_t;


/* Estructura de los elementos del vector que administra los frames
   de memoria. Aplica para paginación   */


typedef struct
{
	uint8_t _presencia;
    int32_t _tiempo_inicial;
    uint8_t _uso;
	uint32_t pid;
} frames_memoria_t;


/* Estructura de los elementos del vector que administra los frames
   de swap. Aplica para paginación   */


typedef struct
{
   uint8_t _asignado;
} frames_swap_t;


int estructuras_memoria_init(void);
bool hay_espacio_para_estructura(uint32_t tam_estrucura);
void* crear_tabla_de_paginas(uint32_t tam_estrucura);
void* crear_tabla_de_segmentos(uint32_t tam_estrucura);
void agregar_a_lista_de_patotas(patota_t* nueva_patota);
void agregar_patota_a_memoria(uint32_t pid, const char* lista_tareas);

#endif /* SRC_MEMORIA_ESTRUCTURAS_MEMORIA_H_ */
