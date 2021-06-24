#ifndef ESQUEMA_PAGINACION_H
#define ESQUEMA_PAGINACION_H

#include "memoria/esquemas/esquema_memoria.h"
#include <stdlib.h>

typedef struct
{
    uint32_t num_frame;
    uint32_t ocupado;
    //uint32_t tiempo_en_memoria;
    //uint32_t uso;
    int32_t ocupantes_frame;
    //uint32_t bit_modificado; //agregado para expulsar tripulante
    //uint32_t pid;    
}p_frame_t;


typedef struct
{
    uint32_t pid;
    t_list* tabla;
    uint32_t tamanio_tareas;
    uint32_t tripulantes_escritos;
    t_list* direcciones_logicas;
}p_patota_y_tabla_t; 


typedef struct
{
    uint32_t num_pagina;
    int32_t frame_memoria; //en caso de -1 no esta en memoria 
    int32_t frame_swap; 
    uint32_t tiempo_en_memoria;
    uint32_t uso;
}p_fila_tabla_de_paginas_t;

typedef struct
{   
    uint32_t tid;
    uint32_t direccion_logica; 
}p_direcciones_logicas_t;

t_list* lista_frames_memoria;
t_list* lista_frames_swap;
int tamanio_pagina;
int tamanio_swap;

void paginacion_memoria_init(void);

bool paginacion_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas);
bool paginacion_memoria_inicializar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);

bool paginacion_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);
bool paginacion_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado);

char*          paginacion_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid);
tripulantes_t* paginacion_memoria_obtener_info_tripulante(uint32_t pid, uint32_t tid);
t_list*        paginacion_memoria_obtener_todos_los_tripulantes(void);

bool           paginacion_memoria_expulsar_tripulante(uint32_t pid, uint32_t tid);

//-----------------------


#endif