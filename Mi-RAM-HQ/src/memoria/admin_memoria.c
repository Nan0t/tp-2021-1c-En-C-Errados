#include "admin_memoria.h"
#include "esquemas/esquema_memoria.h"

void admin_memoria_init(void)
{
    esquema_memoria_init();
}

bool admin_memoria_iniciar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* lista_tareas)
{
    return esquema_memoria_inicializar_patota(pid, cant_tripulantes, lista_tareas);
}

void admin_memoria_iniciar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
    esquema_memoria_inicializar_tripulante(pid, tid, pos);
}

void admin_memoria_mover_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
    esquema_memoria_actualizar_posicion_tripulante(pid, tid, pos);
}

char* admin_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid)
{
    return esquema_memoria_obtener_proxima_tarea(pid, tid);
}

void admin_memoria_tripulante_nuevo_estado(uint32_t pid, uint32_t tid, char estado)
{
    esquema_memoria_actualizar_estado_tripulante(pid, tid, estado);
}

void admin_memoria_eliminar_tripulante(uint32_t pid, uint32_t tid)
{
    esquema_memoria_expulsar_tripulante(pid, tid);
}

t_list* admin_memoria_obtener_tripulantes(void)
{
    return esquema_memoria_obtener_todos_los_tripulantes();
}
