#ifndef DISCORDIA_PLANIFICADOR_QUEUE_MANAGER_H
#define DISCORDIA_PLANIFICADOR_QUEUE_MANAGER_H

#include "queue_mt.h"

typedef enum
{
    DS_QUEUE_NEW,
    DS_QUEUE_READY,
    DS_QUEUE_EXEC,
    DS_QUEUE_BLOCK,
    DS_QUEUE_SABOTAGE
} ds_queue_type_e;

void ds_queue_manager_init(void);

ds_queue_mt_t* ds_queue_manager_hold(ds_queue_type_e queue_type);
void           ds_queue_manager_release(ds_queue_type_e queue_type);

#endif