#ifndef PLANIFICADOR_BLOCK_SABOTAJE_H
#define PLANIFICADOR_BLOCK_SABOTAJE_H

#include "tripulante.h"

void ds_block_sabotaje_init(void);

void          ds_block_sabotaje_push(tripulante_t* trip);
tripulante_t* ds_block_sabotaje_pop(void);

#endif