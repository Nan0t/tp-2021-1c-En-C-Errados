#ifndef PLANIFICADOR_EXEC_QUEUE_H
#define PLANIFICADOR_EXEC_QUEUE_H

#include "planificador/tripulante/tripulante.h"

void ds_exec_queue_init(uint32_t max_size);

bool ds_exec_is_full(void);

void          ds_exec_queue_push(tripulante_t* trip);
tripulante_t* ds_exec_queue_pop(void);

uint64_t ds_exec_queue_size(void);

#endif