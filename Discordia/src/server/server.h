
#ifndef DISCORDIA_SERVER_H
#define DISCORDIA_SERVER_H

#include <utils/utils.h>

/**
 * @NAME: ds_server_init
 * @DESC: inicializa el servidor del Discorida.
 * @PARAMS:
 *  [in] const char* port - puerto de escucha del servidor.
 * 
 * @RETURNS: 0 en caso de que se haya podido inicializar el servidor. -1 en caso contrario.
 */
int ds_server_init(const char* port);

#endif