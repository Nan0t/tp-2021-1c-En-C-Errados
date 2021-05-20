#ifndef PLANIFICADOR_EXEC_QUEUE_H
#define PLANIFICADOR_EXEC_QUEUE_H

#include "../tripulante.h"

void exec_queue_init(void);

void          ds_exec_queue_push(tripulante_t* trip);
tripulante_t* ds_exec_queue_pop(void);

#endif