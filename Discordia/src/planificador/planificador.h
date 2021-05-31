#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include "tripulante/tripulante.h"
#include <utils/utils.h>

void ds_planificador_init(void);
void ds_planificador_iniciar_tripulante(uint32_t pid, uint32_t tid, const u_pos_t* pos);

void ds_planificador_eliminar_tripulante(uint32_t tid);

void ds_planificador_iniciar(void);
void ds_planificador_pausar(void);

#endif