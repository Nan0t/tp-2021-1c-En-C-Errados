#ifndef PLANIFICADOR_BLOCK_TAREAS_H
#define PLANIFICADOR_BLOCK_TAREAS_H

#include "tripulante.h"

void block_tareas_init(void);

void          block_tareas_push(tripulante_t* trip);
tripulante_t* block_tareas_pop(void);

#endif