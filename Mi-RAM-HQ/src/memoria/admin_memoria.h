#ifndef ADMIN_MEMORIA_H
#define ADMIN_MEMORIA_H

#include <utils/utils.h>

typedef struct
{
    uint32_t pid;
    uint32_t tid;
    u_pos_t  pos;
    char     estado;
} tripulantes_t;

bool    admin_memoria_iniciar_patota(uint32_t pid, const char* lista_tareas);
bool    admin_memoria_iniciar_tripulante(uint32_t pid, uint32_t tid, const u_pos_t* pos);
void    admin_memoria_mover_tripulante(uint32_t tid, u_pos_t pos);
char*   admin_memoria_obtener_proxima_tarea(uint32_t tid);
void    admin_memoria_tripulante_nuevo_estado(uint32_t tid, char estado);
void    admin_memoria_eliminar_tripulante(uint32_t tid);
t_list* admin_memoria_obtener_tripulantes(void);

#endif