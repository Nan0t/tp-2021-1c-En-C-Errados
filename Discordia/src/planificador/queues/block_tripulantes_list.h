#ifndef PLANIFICADOR_BLOCK_TRIPULANTES_H
#define PLANIFICADOR_BLOCK_TRIPULANTES_H

#include "planificador/tripulante/tripulante.h"

void ds_block_tripulantes_init(void);

void          ds_block_tripulantes_add(tripulante_t* trip);
tripulante_t* ds_block_tripulantes_get_and_remove(void);
void ds_block_tripulantes_iterate(void(*closure)(tripulante_t*));
bool ds_block_tareas_any_satisfy(bool(*closure)(tripulante_t*));
tripulante_t* ds_block_tripulantes_remove_by_condition(bool(*closure)(tripulante_t*));

#endif