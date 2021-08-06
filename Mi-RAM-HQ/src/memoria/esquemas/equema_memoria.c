#include "esquema_memoria.h"
#include "paginacion/paginacion.h"
#include "segmentacion/segmentacion.h"

typedef struct
{
    bool(*inicializar_patota)(uint32_t, uint32_t, const char*);
    bool(*inicializar_tripulante)(uint32_t, uint32_t, u_pos_t);
    bool(*actualizar_posicion_tripulante)(uint32_t, uint32_t, u_pos_t);
    bool(*actualizar_estado_tripulante)(uint32_t, uint32_t, char);
    char*(*obtener_proxima_tarea)(uint32_t, uint32_t);
    tripulantes_t*(*obtener_tripulante)(uint32_t, uint32_t);
    t_list*(*obtener_todos_los_tripulantes)(void);
    bool(*expulsar_tripulante)(uint32_t, uint32_t);
} esquema_memoria_t;

private esquema_memoria_t* p_esquema_memoria_instance = NULL;

private void init_paginacion(void);
private void init_segmentacion(void);

void esquema_memoria_init(void)
{
    const char* esquema_memoria = u_config_get_string_value("ESQUEMA_MEMORIA");
    p_esquema_memoria_instance = u_malloc(sizeof(esquema_memoria_t));

    if(!strcmp(esquema_memoria, "PAGINACION"))
        init_paginacion();
    else
        init_segmentacion();
}

bool esquema_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas)
{
    return p_esquema_memoria_instance->inicializar_patota(pid, cant_tripulantes, tareas);
}

bool esquema_memoria_inicializar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
    return p_esquema_memoria_instance->inicializar_tripulante(pid, tid, pos);
}

bool esquema_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos)
{
    return p_esquema_memoria_instance->actualizar_posicion_tripulante(pid, tid, pos);
}

bool esquema_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado)
{
    return p_esquema_memoria_instance->actualizar_estado_tripulante(pid, tid, estado);
}

char* esquema_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid)
{
    return p_esquema_memoria_instance->obtener_proxima_tarea(pid, tid);
}

tripulantes_t* esquema_memoria_obtener_info_tripulante(uint32_t pid, uint32_t tid)
{
    return p_esquema_memoria_instance->obtener_tripulante(pid, tid);
}

t_list* esquema_memoria_obtener_todos_los_tripulantes(void)
{
    return p_esquema_memoria_instance->obtener_todos_los_tripulantes();
}

bool esquema_memoria_expulsar_tripulante(uint32_t pid, uint32_t tid)
{
    return p_esquema_memoria_instance->expulsar_tripulante(pid, tid);
}

private void init_paginacion(void)
{
    paginacion_memoria_init();

    p_esquema_memoria_instance->inicializar_patota             = paginacion_memoria_inicializar_patota;
    p_esquema_memoria_instance->inicializar_tripulante         = paginacion_memoria_inicializar_tripulante;
    p_esquema_memoria_instance->actualizar_posicion_tripulante = paginacion_memoria_actualizar_posicion_tripulante;
    p_esquema_memoria_instance->actualizar_estado_tripulante   = paginacion_memoria_actualizar_estado_tripulante;
    p_esquema_memoria_instance->obtener_proxima_tarea          = paginacion_memoria_obtener_proxima_tarea;
    p_esquema_memoria_instance->obtener_tripulante             = paginacion_memoria_obtener_info_tripulante;
    p_esquema_memoria_instance->obtener_todos_los_tripulantes  = paginacion_memoria_obtener_todos_los_tripulantes;
    p_esquema_memoria_instance->expulsar_tripulante            = paginacion_memoria_expulsar_tripulante;
}

private void init_segmentacion(void)
{
    segmentacion_memoria_init();

    p_esquema_memoria_instance->inicializar_patota             = segmentacion_memoria_inicializar_patota;
    p_esquema_memoria_instance->inicializar_tripulante         = segmentacion_memoria_inicializar_tripulante;
    p_esquema_memoria_instance->actualizar_posicion_tripulante = segmentacion_memoria_actualizar_posicion_tripulante;
    p_esquema_memoria_instance->actualizar_estado_tripulante   = segmentacion_memoria_actualizar_estado_tripulante;
    p_esquema_memoria_instance->obtener_proxima_tarea          = segmentacion_memoria_obtener_proxima_tarea;
    p_esquema_memoria_instance->obtener_tripulante             = segmentacion_memoria_obtener_info_tripulante;
    p_esquema_memoria_instance->obtener_todos_los_tripulantes  = segmentacion_memoria_obtener_todos_los_tripulantes;
    p_esquema_memoria_instance->expulsar_tripulante            = segmentacion_memoria_expulsar_tripulante;
}