#ifndef TRIPULANTE_QUEUE_H
#define TRIPULANTE_QUEUE_H

#include "tripulante.h"
#include <pthread.h>

typedef struct tripulante_queue_t tripulante_queue_t;

tripulante_queue_t* tripulante_queue_create(void);
void                tripulante_queue_delete(tripulante_queue_t* queue);

bool tripulante_queue_is_empty(tripulante_queue_t* queue);

void                tripulante_queue_push(tripulante_queue_t* queue, tripulante_t* trip_info);
tripulante_queue_t* tripulante_queue_pop(tripulante_queue_t* queue);

#endif