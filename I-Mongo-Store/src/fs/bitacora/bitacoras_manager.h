#ifndef FS_BITACORA_BITACORAS_MANAGER_H
#define FS_BITACORA_BITACORAS_MANAGER_H

#include "bitacora.h"

typedef struct
{
    pthread_mutex_t ref_counter_mx;
    uint32_t        ref_counter;
    pthread_cond_t  can_be_deleted;

    fs_bitacora_t* bitacora;
} fs_bitacora_ref_t;

typedef struct
{
    char*           mount_point;
    pthread_mutex_t bitacoras_mx;
    t_dictionary*   bitacoras;
} fs_bitacoras_manager_t;

/**
 * @NAME: fs_bitacoras_manager_init
 * @DESC: inicializa el administrador de bitacoras.
 * @PARAMS:
 *  [in] const char* mount_point - punto de montaje del sistema.
 */
void fs_bitacoras_manager_init(const char* mount_point);

/**
 * @NAME: fs_bitacoras_manager_create_bitacora
 * @DESC: crea una bitacora para el tripulante "tid".
 * @PARAMS:
 *  [in] uint32_t tid - numero de tripulante al cual se le quiere crear una bitacora.
 */
void fs_bitacoras_manager_create_bitacora(uint32_t tid);

/**
 * @NAME: fs_bitacoras_manager_hold_bitacora
 * @DESC: devuelve una referencia a una bitacora en el sistema. Esta debe ser liberada luego de ser utilizada
 *          llamando a la funcion fs_bitacoras_manager_release_bitacora.
 * @PARAMS:
 *  [in] uint32_t tid - numero de tripulante del cual se quiere obtener su bitacora.
 * 
 * @RETURNS: una instancia fs_bitacora_t o NULL en caso de no existir una bitacora para el tripulante pedido.
 */
fs_bitacora_t* fs_bitacoras_manager_hold_bitacora(uint32_t tid);

/**
 * @NAME: fs_bitacoras_manager_release_bitacora
 * @DESC: libera una referencia de bitacora pedida anteriormente con fs_bitacoras_manager_hold_bitacora.
 * @PARAMS:
 *  [in] uint32_t tid - numero de tripulante del cual se quiere liberar la bitacora.
 */
void fs_bitacoras_manager_release_bitacora(uint32_t tid);

/**
 * @NAME: fs_bitacoras_manager_delete_bitacora
 * @DESC: elimina una bitacora del sistema
 * @PARAMS:
 *  [in] uint32_t tid - numero de tripulante del cual se quiere eliminar su bitacora.
 */
void fs_bitacoras_manager_delete_bitacora(uint32_t tid);

#endif
