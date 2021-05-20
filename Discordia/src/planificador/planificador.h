#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include "tripulante/tripulante.h"
#include <utils/utils.h>

void planificador_init(void);
void planificador_iniciar_tripulante(uint32_t tid, const u_pos_t* pos);

void planificador_iniciar(void);
void planificador_pausar(void);

bool planificador_esta_pausado(void);

#endif