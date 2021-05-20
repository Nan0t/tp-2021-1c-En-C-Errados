#ifndef PLANIFICADOR_READY_QUEUE_H
#define PLANIFICADOR_READY_QUEUE_H

#include "../tripulante.h"

void ds_ready_queue_init(void);

void          ds_ready_queue_push(tripulante_t* trip);
tripulante_t* ds_ready_queue_pop(void);

#endif