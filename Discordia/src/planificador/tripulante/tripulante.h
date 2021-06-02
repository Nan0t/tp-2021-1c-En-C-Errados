#ifndef PLANIFICADOR_TRIPULANTE_H
#define PLANIFICADOR_TRIPULANTE_H

#include <utils/utils.h>
#include <semaphore.h>
#include "tareas.h"


typedef struct
{
    uint32_t pid;
    uint32_t tid;
    tarea_t* tarea_actual;
    u_pos_t  pos;

    uint32_t quatum;

    bool bloquear; //avisa al CPU si debe bloquearse

    sem_t sem_sync; // espera a que un CPU lo ejecute
    sem_t sem_end_exec;  // notifica al CPU que terminó de ejecutar y este pueda sincronizar con otros cpus
} tripulante_t;

tripulante_t* tripulante_create(uint32_t pid, uint32_t tid, const u_pos_t* pos, uint32_t quantum);
void          tripulante_delete(tripulante_t* tripulante);

bool tripulante_obtener_proxima_tarea(tripulante_t* tripulante);

void tripulante_init(tripulante_t* tripulante);

#endif