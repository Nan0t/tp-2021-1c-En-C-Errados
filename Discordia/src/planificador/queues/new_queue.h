#ifndef PLANIFICADOR_NEW_QUEUE_H
#define PLANIFICADOR_NEW_QUEUE_H

#include "tripulante.h"

void ds_new_queue_init(void);

void          ds_new_queue_push(tripulante_t* trip);
tripulante_t* ds_new_queue_pop(void);

#endif