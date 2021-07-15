#ifndef MEMORIA_ESQUEMAS_H
#define MEMORIA_ESQUEMAS_H

#include <utils/utils.h>

typedef struct
{
    uint32_t pid;
    uint32_t tid;
    u_pos_t  pos;
    char     estado;
    uint32_t proxima_tarea;
    uint32_t direccion_pcb;
} tripulantes_t;

int esquema_memoria_tamanio;
void* esquema_memoria_mfisica;
t_list* listado_patotas;

void esquema_memoria_init(void);

bool esquema_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas);
bool esquema_memoria_inicializar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);

bool esquema_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);
bool esquema_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado);

char*          esquema_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid);
tripulantes_t* esquema_memoria_obtener_info_tripulante(uint32_t pid, uint32_t tid);
t_list*        esquema_memoria_obtener_todos_los_tripulantes(void);

bool           esquema_memoria_expulsar_tripulante(uint32_t pid, uint32_t tid);

#endif