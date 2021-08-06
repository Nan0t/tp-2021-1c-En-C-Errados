#ifndef PLANIFICADOR_SYNCHRONIZER_H
#define PLANIFICADOR_SYNCHRONIZER_H

#include <utils/utils.h>

/**
 * @NAME: ds_synchronizer_init
 * @DESC: inicializa el sincronizador del planificador.
 * @PARAMS:
 *  [in] uint32_t devices_count - cantidad de dispositivos que tendra que sincronizar el sincronizador.
 */
void ds_synchronizer_init(uint32_t devices_count);

/**
 * @NAME: ds_synchronizer_get_device_id
 * @DESC: le solicita al sincronizador un nuevo id para que este lo pueda identificar.
 * 
 * @RETURNS: un nuevo id de dispositivo o 0 en caso de que se haya pedido el maximo numero de dispositivos.
 */
uint32_t ds_synchronizer_get_device_id(void);

/**
 * @NAME: ds_synchronizer_wait_for_next_cycle
 * @DESC: detiene la ejecucion del dispositivo hasta que el sincronizador arranque el proximo ciclo.
 * @PARAMS:
 *  [in] uint32_t device_id - id del dispositivo.
 */
void ds_synchronizer_wait_for_next_cicle(uint32_t device_id);

/**
 * @NAME: ds_synchronizer_notify_end_of_cicle
 * @DESC: notifica al sincronizador que un dispositivo finalizo su ciclo.
 */
void ds_synchronizer_notify_end_of_cicle(void);

/**
 * @NAME: ds_synchronizer_get_tick
 * @DESC: devuelve la cantidad de ciclos ejecutados desde que se inicio el programa.
 * 
 * @RETURNS: la cantidad de ciclos ejecutados desde que inicio el programa.
 */
uint32_t ds_synchronizer_get_tick(void);

#endif