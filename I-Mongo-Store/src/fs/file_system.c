#include "file_system.h"
#include <pthread.h>

// typedef struct
// {
//     uint32_t tid;
//     char*    content;
// } fs_bitacora_t;

// pthread_mutex_t p_bitacora_tripulantes_mx = PTHREAD_MUTEX_INITIALIZER;
// t_list*         p_bitacora_tripulantes    = NULL;

private void  file_system_create_bitacora(uint32_t tid);
private void  file_system_remove_bitacora(uint32_t tid);
private void  file_system_update_bitacora(uint32_t tid, const char* content);
private char* file_system_get_bitacora(uint32_t tid);

void file_system_init(const file_system_attr_t* attr)
{
    U_LOG_INFO("I-Mongo-Store File System");
    U_LOG_INFO("Punto de Montaje: %s", attr->mount_point);
    U_LOG_INFO("Tiempo de Sincronizacion: %d", attr->sync_time);
    
    char* positions = string_new();
    bool first_iteration = true;

    void _add_positions_to_log(const u_pos_t* pos) {
        if(first_iteration)
        {
            string_append_with_format(&positions, "{ x: %d | y: %d }", pos->x, pos->y);
            first_iteration = false;
        }
        else
            string_append_with_format(&positions, ", { x: %d | y: %d }", pos->x, pos->y);
    };

    list_iterate(attr->sabotage_positions, (void*)_add_positions_to_log);

    U_LOG_INFO("Posiciones de Sabotaje: %s", positions);
    u_free(positions);
}

void  file_system_desplazamiento_tripulante(uint32_t tid, const u_pos_t* from, const u_pos_t* to)
{
    char* desplazamiento =
        string_from_format("Tripulante %d se desplaza desde { %d, %d } hasta { %d, %d }",
        tid, from->x, from->y, to->x, to->y);

    file_system_update_bitacora(tid, desplazamiento);
}

void  file_system_inicio_tarea(uint32_t tid, const char* tarea)
{
    char* inicio_tarea =
        string_from_format("Tripulante %d inicia tarea %s", tid, tarea);

    file_system_update_bitacora(tid, inicio_tarea);
}

void  file_system_finalizacion_tarea(uint32_t tid, const char* tarea)
{
    char* finaliza_tarea =
        string_from_format("Tripulante %d finalizo tarea %s",
        tid, tarea);

    file_system_update_bitacora(tid, finaliza_tarea);
}

void  file_system_atiende_sabotaje(uint32_t tid)
{
    char* atiende_sabotaje =
        string_from_format("Tripulante %d atiende sabotaje", tid);

    file_system_update_bitacora(tid, atiende_sabotaje);
}

void  file_system_resuelve_sabotaje(uint32_t tid)
{
    char* resuelve_sabotaje =
        string_from_format("Tripulante %d resuelve sabotaje",
        tid);

    file_system_update_bitacora(tid, resuelve_sabotaje);
}

void  file_system_eliminar_tripulante(uint32_t tid)
{
    file_system_remove_bitacora(tid);
}

char* file_system_obtener_bitacora(uint32_t tid)
{
    return file_system_get_bitacora(tid);
}

// ========================================================
//             *** Private Functions ***
// ========================================================

private void  file_system_create_bitacora(uint32_t tid)
{
    U_LOG_TRACE("Se crea bitacora para tripulante %d", tid);
}

private void  file_system_remove_bitacora(uint32_t tid)
{
    U_LOG_TRACE("Se elimina bitacora para tripulante %d", tid);
}

private void  file_system_update_bitacora(uint32_t tid, const char* content)
{
    U_LOG_TRACE("Se actualiza bitacora de %d", tid);
    U_LOG_TRACE("Contenido: %s", content);
}

private char* file_system_get_bitacora(uint32_t tid)
{
    U_LOG_TRACE("Se pide bitacora de tripulante %d", tid);
    return string_from_format("Bitacora Tripulante %d", tid);
}
