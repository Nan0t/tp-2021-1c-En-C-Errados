#ifndef PLANIFICADOR_TRIPULANTE_H
#define PLANIFICADOR_TRIPULANTE_H

#include <utils/utils.h>
#include <semaphore.h>
#include "tareas.h"

typedef enum
{
    TRIP_STATE_NEW,
    TRIP_STATE_READY,
    TRIP_STATE_EXEC,
    TRIP_STATE_BLOCK_IO,
    TRIP_STATE_BLOCK_SABOTAGE,
    TRIP_STATE_EXIT
} trip_state_e;

typedef struct
{
    uint32_t pid;
    uint32_t tid;
    tarea_t* tarea_actual;
    u_pos_t  pos;

    uint32_t quatum;

    trip_state_e curr_state;

    bool terminate;
    bool bloquear; //avisa al CPU si debe bloquearse

    sem_t sem_sync; // espera a que un CPU lo ejecute
    sem_t sem_end_exec;  // notifica al CPU que terminó de ejecutar y este pueda sincronizar con otros cpus
} tripulante_t;

tripulante_t* tripulante_create(uint32_t pid, uint32_t tid, const u_pos_t* pos, uint32_t quantum);

void tripulante_change_state(tripulante_t* tripulante, trip_state_e state);
bool tripulante_obtener_proxima_tarea(tripulante_t* tripulante);

void tripulante_init(tripulante_t* tripulante);
void tripulante_terminate(tripulante_t* tripulante);

#endif