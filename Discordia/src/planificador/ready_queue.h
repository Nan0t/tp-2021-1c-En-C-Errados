#ifndef PLANIFICADOR_READY_QUEUE_H
#define PLANIFICADOR_READY_QUEUE_H

#include "tripulante.h"

void ready_queue_init(void);

void          ready_queue_push(tripulante_t* trip);
tripulante_t* ready_queue_pop(void);

#endif