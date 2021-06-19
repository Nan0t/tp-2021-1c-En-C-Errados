#ifndef PLANIFICADOR_TRIPULANTE_H
#define PLANIFICADOR_TRIPULANTE_H

#include <utils/utils.h>
#include <semaphore.h>
#include "tareas.h"

/**
 * @NAME: trip_state_e
 * @DESC: enumerado con los posibles estados de un tripulante
 */
typedef enum
{
    TRIP_STATE_NEW,
    TRIP_STATE_READY,
    TRIP_STATE_EXEC,
    TRIP_STATE_BLOCK_IO,
    TRIP_STATE_BLOCK_SABOTAGE,
    TRIP_STATE_EXIT
} trip_state_e;

/**
 * @NAME: tripulante_t
 * @DESC: estructura con la informacion necesaria de un tripulante para poder ser planificado
 * @ATTR:
 *  uint32_t pid          - id de patota del tripulante.
 *  uint32_t tid          - id del tripulante.
 *  tarea_t* tarea_actual - estructura con la informacion de la tarea actual
 *  u_pos_t  pos          - posicion actual del tripulante.
 *  uint32_t quatum       - quantum actual del tripulante (solo para RR)
 *  bool     terminate    - booleano que indica si el tripulante debe finalizar.
 *  bool     bloquear     - booleano que indica si el tripulante se debe bloquear.
 *  sem_t    sem_sync     - semaforo que permite ejecutar una accion al tripulante.
 *  sem_t    sem_end_exec - semaforo que indica si un tripulante termino de ejecutar una accion.
 */
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

/**
 * @NAME: tripulante_create
 * @DESC: crea una estructura tripulante_t
 * @PARAMS:
 *  [in] uint32_t pid       - id de patota del tripulante.
 *  [in] uint32_t tid       - id del tripulante.
 *  [in] const u_pos_t* pos - posicion del tripulante.
 *  [in] uint32_t quantum   - quantum iniciali del tripulante (solo para RR).
 */
tripulante_t* tripulante_create(uint32_t pid, uint32_t tid, const u_pos_t* pos, uint32_t quantum);

/**
 * @NAME: tripulante_change_state
 * @DESC: cambia el estado de un tripulante.
 * @PARAMS:
 *  [in] tripulante_t* tripulante - tripulante al cual se le cambiara el estado.
 *  [in] trip_state_e  state      - nuevo estado del tripulante.
 */
void tripulante_change_state(tripulante_t* tripulante, trip_state_e state);

/**
 * @NAME: tripulante_obtener_proxima_tarea
 * @DESC: permite que el tripulante solicite una nueva tarea
 * @PARAMS:
 *  [in] tripulante_t* tripulante - tripulante del cual se quiere obtener su proxima tarea.
 * 
 * @RETURNS: true en caso de que se haya podido obtener la proxima tarea. En caso contrario, false.
 */
bool tripulante_obtener_proxima_tarea(tripulante_t* tripulante);

/**
 * @NAME: tripulante_init
 * @DESC: inicializa el hilo de ejecucion de un tripulante
 * @PARAMS:
 *  [in] tripulante_t* tripulante - tripulante que se quiere inicializar.
 */
void tripulante_init(tripulante_t* tripulante);

/**
 * @NAME: tripulante_terminate
 * @DESC: notifica a un tripulante que debe finalizar.
 * @PARAMS:
 *  [in] tripulante_t* tripulante - tripulante que se desea finalizar.
 */
void tripulante_terminate(tripulante_t* tripulante);

#endif