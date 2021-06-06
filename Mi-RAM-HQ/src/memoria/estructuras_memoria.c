/*
 * estructuras_memoria.c
 *
 *  Created on: 27 may. 2021
 *      Author: utnso
 */

#include "estructuras.h"
#include <commons/collections/list.h>
#include <stdlib.h>

char* _memoria_principal;
char* _memoria_swap;  // se usa sólo en paginación
t_list* _lista_patotas; // se usa para segmentación y paginación
t_list* _lista_tripulantes; // se usa para segmentación
t_list* _lista_segmentos; // se usa para segmentación
frames_memoria_t* _vector_frames_principal; // se usa para paginación
frames_swap_t* _vector_frames_swap; // se usa para paginación
int32_t _tamanio_principal;
int32_t _tamanio_swap;
int32_t _tamanio_pagina;

int estructuras_memoria_init(void)
{
	_tamanio_principal = 2048; // reemplazar luego hardcode, se toma de archivo de conf.
	_tamanio_swap = 4096 // reemplazar luego hardcode, se toma de archivo de conf.
	_tamanio_pagina = 64 // reemplazar luego hardcode, se toma de archivo de conf.
	_memoria_principal = u_malloc(sizeof(_tamanio_principal));
	_memoria_swap = u_malloc(sizeof(_tamanio_swap));
	_lista_patotas = list_create();
	_lista_tripulantes = list_create();
	_lista_segmentos = list_create();
	segmentos_t* _segmento_inicial;
	_segmento_inicial->_posicion_inicio = 0;
	_segmento_inicial->_posicion_fin = _tamanio_principal - 1;
	_segmento_inicial->_tipo_de_segmento = -1;
	_segmento_inicial->_id_propietario = -1;
	list_add(_lista_segmentos, _segmento_inicial);
	_vector_frames_principal = (frames_memoria_t *) u_malloc(_tamanio_principal/_tamanio_pagina * sizeof(frames_memoria_t));
	_vector_frames_swap = (frames_swap_t *) u_malloc(_tamanio_swap/_tamanio_pagina * sizeof(frames_swap_t));
	return 0;

}

bool hay_espacio_para_estructura(uint32_t tam_estrucura)
{
	// en esta funcion hay que ver el tipo de esquema leyendo el
	// archivo de conf. Luego analizar si hay frames o segmentos para ese tamaño
    return true;
}

void* crear_tabla_de_paginas(uint32_t tam_estrucura)
{
	// esta funcion es para paginación
	// creará el vector donde cada elementos son 2 campos, uno para principal y otro para swap
}

void* crear_tabla_de_segmentos(uint32_t tam_estrucura)
{
	// esta funcion es para segmentación
	// creará el vector donde cada elementos son 2 campos, uno para el inicio del cada segmento y otro para su tamaño
}

void agregar_a_lista_de_patotas(patota_t* nueva_patota)
{
	// esta funcion es para segmentación y paginación
	// deberá verificar qué esquema es y tendrá no sólo agregar la patota a la lista de patotas sino
	// también llamará otras funciones a implementar acá, para actualizar otras estructuras.
}

void agregar_patota_a_memoria(uint32_t pid, const char* lista_tareas)
{
	// esta funcion es para segmentación y paginación
	// deberá verificar qué esquema es y tendrá que hacer el copy de memoria a principal (ambos esquemas) y a swap (si es paginación)
}

