#ifndef ADMIN_MEMORIA_H
#define ADMIN_MEMORIA_H

#include <utils/utils.h>
#include "esquemas/esquema_memoria.h"

void admin_memoria_init(void);

bool    admin_memoria_iniciar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* lista_tareas);
bool    admin_memoria_iniciar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);
bool    admin_memoria_mover_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);
char*   admin_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid);
bool    admin_memoria_tripulante_nuevo_estado(uint32_t pid, uint32_t tid, char estado);
bool    admin_memoria_eliminar_tripulante(uint32_t pid, uint32_t tid);
t_list* admin_memoria_obtener_tripulantes(void);

#endif