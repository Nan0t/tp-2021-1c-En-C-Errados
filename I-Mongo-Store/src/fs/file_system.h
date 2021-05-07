#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <utils/utils.h>

typedef struct
{
    const char* mount_point;
    uint32_t    sync_time;
    t_list*     sabotage_positions;
} file_system_attr_t;

void file_system_init(const file_system_attr_t* attr);

void  file_system_desplazamiento_tripulante(uint32_t tid, const u_pos_t* from, const u_pos_t* to);
void  file_system_inicio_tarea(uint32_t tid, const char* tarea);
void  file_system_finalizacion_tarea(uint32_t tid, const char* tarea);
void  file_system_atiende_sabotaje(uint32_t tid);
void  file_system_resuelve_sabotaje(uint32_t tid);
void  file_system_eliminar_tripulante(uint32_t tid);
char* file_system_obtener_bitacora(uint32_t tid);

#endif