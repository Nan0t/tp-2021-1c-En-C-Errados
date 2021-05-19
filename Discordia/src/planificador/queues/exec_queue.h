#ifndef PLANIFICADOR_EXEC_QUEUE_H
#define PLANIFICADOR_EXEC_QUEUE_H

#include "tripulante.h"

void exec_queue_init(void);

void          exec_queue_push(tripulante_t* trip);
tripulante_t* exec_queue_pop(void);

#endif