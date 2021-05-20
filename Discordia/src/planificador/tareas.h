#ifndef PLANIFICADOR_TAREAS_H
#define PLANIFICADOR_TAREAS_H
#include <utils/utils.h>

typedef struct
{
    char*    tarea;
    bool     is_blocking; //avisa si debe ejecutar en CPU o pasar a lista bloqueados
    bool     is_finished;
    uint32_t tiempo_bloqueado;
    u_pos_t  pos;
} tarea_t;

tarea_t* parsear_tarea (char* tarea_sin_parsear);
void eliminar_tarea (tarea_t* tarea);



#endif