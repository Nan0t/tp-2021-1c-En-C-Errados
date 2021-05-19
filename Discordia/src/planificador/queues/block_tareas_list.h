#ifndef PLANIFICADOR_BLOCK_TAREAS_H
#define PLANIFICADOR_BLOCK_TAREAS_H

#include "tripulante.h"

void ds_block_tareas_init(void);

void          ds_block_tareas_add(tripulante_t* trip);
tripulante_t* ds_block_tareas_get_and_remove(void);

#endif