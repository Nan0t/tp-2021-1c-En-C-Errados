#ifndef ESQUEMA_PAGINACION_H
#define ESQUEMA_PAGINACION_H

#include "memoria/esquemas/esquema_memoria.h"
#include <stdlib.h>

typedef struct
{
    uint32_t num_frame;
    uint32_t ocupado;
}p_frame_t;


typedef struct
{
    uint32_t pid;

    t_list* tabla;

    uint32_t tamanio_tareas;
    uint32_t tripulantes_escritos;

    pthread_mutex_t direcciones_logicas_mx;
    t_list* direcciones_logicas; //perteneciente a los tcb

    t_list* direcciones_tareas; 
}p_patota_y_tabla_t;


typedef struct
{
    pthread_mutex_t mx;
    uint32_t num_pagina;
    int32_t frame_memoria; //en caso de -1 no esta en memoria 
    int32_t frame_swap; 
    int32_t ingreso_en_memoria;
    int32_t uso;
    int32_t ocupantes_pagina;
}p_fila_tabla_de_paginas_t;

typedef struct
{   
    uint32_t tid;
    uint32_t direccion_logica; 
}p_direcciones_logicas_t;


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