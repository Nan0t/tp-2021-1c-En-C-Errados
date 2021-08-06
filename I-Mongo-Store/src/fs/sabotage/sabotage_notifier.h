#ifndef FS_SABOTAGE_NOTIFIER_H
#define FS_SABOTAGE_NOTIFIER_H

#include <utils/utils.h>

/**
 * @NAME: fs_sabotage_notifier_init
 * @DESC: inicializa el notificador de sabotajes.
 * @PARAMS:
 *  [in] t_queue* sabotage_positions - posiciones de los sabotajes.
 */
void fs_sabotage_notifier_init(t_queue* sabotage_positions);

#endif