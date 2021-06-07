#ifndef ESQUEMA_PAGINACION_H
#define ESQUEMA_PAGINACION_H

#include "memoria/esquemas/esquema_memoria.h"

typedef struct
{
    uint32_t num_frame;
    uint32_t ocupado;
    uint32_t tiempo_en_memoria;
    uint32_t uso;
    //uint32_t pid;
}m_frame_t;

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

private void paginacion_inicializar_frames_memoria(int);
private void paginacion_mostrar_frames(int);

#endif