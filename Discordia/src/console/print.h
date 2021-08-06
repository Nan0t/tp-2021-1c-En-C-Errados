#ifndef DISCORDIA_PRINT_H
#define DISCORDIA_PRINT_H

#include <utils/utils.h>

/**
 * @NAME: ds_print
 * @DESC: Imprime un mensaje en la consola del Discordiador.
 * @PARAMS:
 *  [in] const char* format - formato del string que se va a mostrar.
 */
void ds_print(const char* format, ...) UTILS_PRINT_FORMAT(1, 2);

#endif