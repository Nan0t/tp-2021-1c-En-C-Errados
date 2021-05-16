#ifndef PLANIFICADOR_BLOCK_SABOTAJE_H
#define PLANIFICADOR_BLOCK_SABOTAJE_H

#include "tripulante.h"

void block_sabotaje_init(void);

void          block_sabotaje_push(tripulante_t* trip);
tripulante_t* block_sabotaje_pop(void);

#endif