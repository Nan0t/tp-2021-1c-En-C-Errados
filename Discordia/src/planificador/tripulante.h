#ifndef PLANIFICADOR_TRIPULANTE_H
#define PLANIFICADOR_TRIPULANTE_H

#include <utils/utils.h>
#include <semaphore.h>

typedef struct
{
    char*    tarea;
    bool     is_blocking; //avisa si debe ejecutar en CPU o pasar a lista bloqueados
    bool     is_finished;
    uint32_t tiempo_bloqueado;
    u_pos_t  pos;
} tarea_t;

typedef struct
{
    uint32_t tid;
    tarea_t* tarea_actual;
    u_pos_t  pos;

    bool bloquear; //avisa al CPU si debe bloquearse

    sem_t sem_sync; // espera a que un CPU lo ejecute
    sem_t sem_end_exec;  // notifica al CPU que terminó de ejecutar y este pueda sincronizar con otros cpus
} tripulante_t;

void tripulante_init(tripulante_t* trip_info);
tarea_t* parsear_tarea (char* tarea_sin_parsear);

#endif