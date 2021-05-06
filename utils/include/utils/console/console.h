#ifndef UTILS_CONSOLE_H
#define UTILS_CONSOLE_H

#include "utils/typedefs.h"

/**
 * @NAME: u_console_read
 * @DESC: Lee un input desde el stdin
 * @PARAMS:
 *  [in] const char* prompt - Simbolo que se va a mostrar en la consola.
 * 
 * @RETURN: un string que contiene el input del usuario
 * 
 * @NOTA: el string debe ser liberado posteriormente.
 */
char* u_console_read(const char* prompt);

#endif