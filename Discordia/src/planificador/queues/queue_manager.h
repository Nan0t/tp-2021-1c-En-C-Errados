#ifndef DISCORDIA_PLANIFICADOR_QUEUE_MANAGER_H
#define DISCORDIA_PLANIFICADOR_QUEUE_MANAGER_H

#include "queue_mt.h"

/**
 * @NAME: ds_queue_type_e
 * @DESC: enumerados con las distintas colas de planificacion.
 */
typedef enum
{
    DS_QUEUE_NEW,
    DS_QUEUE_READY,
    DS_QUEUE_EXEC,
    DS_QUEUE_BLOCK,
    DS_QUEUE_SABOTAGE
} ds_queue_type_e;

/**
 * @NAME: ds_queue_manager_init
 * @DESC: Inicializar el administrador de las colas del planificador.
 */
void ds_queue_manager_init(void);

/**
 * @NAME: ds_queue_manager_hold
 * @DESC: Solicita al administrador de las colas de planificacion la referencia a una cola especifica.
 * @PARAMS:
 *  [in] ds_queue_type_e queue_type - cola de planificacion que se quiere solicitar.
 * 
 * @RETURNS: la cola solicitada. Internamente, el administrador bloqueara la cola de planifiacion hasta que se
 *              libere la cola con ds_queue_manager_release
 */
ds_queue_mt_t* ds_queue_manager_hold(ds_queue_type_e queue_type);

/**
 * @NAME: ds_queue_manager_release
 * @DESC: libera una cola de planificacion solicitada anteriormente por ds_queue_manager_hold
 * @PARAMS:
 *  [in] ds_queue_type_e queue_type - cola que se quiere liberar.
 */
void ds_queue_manager_release(ds_queue_type_e queue_type);

#endif