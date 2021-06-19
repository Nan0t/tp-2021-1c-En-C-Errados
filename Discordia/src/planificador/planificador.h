#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include "tripulante/tripulante.h"
#include <utils/utils.h>

/**
 * @NAME: ds_planificador_init
 * @DESC: inicializa el planificador del Discordiador.
 */
void ds_planificador_init(void);

/**
 * @NAME: ds_planificador_iniciar_tripulante
 * @DESC: inicializa un nuevo tripulante en el planificador.
 * @PARAMS:
 *  [in] uint32_t       pid - id de patota del tripulante.
 *  [in] uint32_t       tid - id del tripulante.
 *  [in] const u_pos_t* pos - posicion del tripulante.
 */
void ds_planificador_iniciar_tripulante(uint32_t pid, uint32_t tid, const u_pos_t* pos);

/**
 * @NAME: ds_planificador_eliminar_tripulante
 * @DESC: elimina un tripulante de la planificacion.
 * @PARAMS:
 *  [in] uint32_t tid - id del tripulante.
 */
void ds_planificador_eliminar_tripulante(uint32_t tid);

/**
 * @NAME: ds_planificador_iniciar
 * @DESC: inicializa el planificador
 */
void ds_planificador_iniciar(void);

/**
 * @NAME: ds_planificador_pausar
 * @DESC: detiene el planificador
 */
void ds_planificador_pausar(void);

#endif