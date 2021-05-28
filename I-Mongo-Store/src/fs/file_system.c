#include "file_system.h"
#include "sabotage_notifier.h"
#include <pthread.h>



private void  file_system_create_bitacora(uint32_t tid);
private void  file_system_remove_bitacora(uint32_t tid);
private void  file_system_update_bitacora(uint32_t tid, const char* content);
private char* file_system_get_bitacora(uint32_t tid);

void file_system_init(const file_system_attr_t* attr)
{
    U_LOG_INFO("I-Mongo-Store File System");
    U_LOG_INFO("Punto de Montaje: %s", attr->mount_point);
    U_LOG_INFO("Tiempo de Sincronizacion: %d", attr->sync_time);

    fs_sabotage_notifier_init(attr->sabotage_positions);
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
    // TODO: inspeccionar en el fs que exista la bitacora del tripulante
    U_LOG_TRACE("Se actualiza bitacora de %d", tid);
    U_LOG_TRACE("Contenido: %s", content);
}

private char* file_system_get_bitacora(uint32_t tid)
{
    U_LOG_TRACE("Se pide bitacora de tripulante %d", tid);
    return string_from_format("Bitacora Tripulante %d", tid);
}
