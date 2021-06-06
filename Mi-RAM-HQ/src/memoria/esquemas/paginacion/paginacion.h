#ifndef ESQUEMA_PAGINACION_H
#define ESQUEMA_PAGINACION_H

#include "memoria/esquemas/esquema_memoria.h"

void paginacion_memoria_init(void);

bool paginacion_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas);
void paginacion_memoria_inicializar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);

void paginacion_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);
void paginacion_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado);

char*          paginacion_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid);
tripulantes_t* paginacion_memoria_obtener_info_tripulante(uint32_t pid, uint32_t tid);
t_list*        paginacion_memoria_obtener_todos_los_tripulantes(void);

void           paginacion_memoria_expulsar_tripulante(uint32_t pid, uint32_t tid);

#endif