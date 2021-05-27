#ifndef PLANIFICADOR_BLOCK_TRIPULANTES_H
#define PLANIFICADOR_BLOCK_TRIPULANTES_H

#include "planificador/tripulante/tripulante.h"

void ds_block_tripulantes_init(void);

void          ds_block_tripulantes_add(tripulante_t* trip);
tripulante_t* ds_block_tripulantes_get_and_remove(void);

#endif