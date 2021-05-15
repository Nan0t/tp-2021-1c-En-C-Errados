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

/**
 * @NAME: u_console_set_commands
 * @DESC: Dado una lista de comandos, permite autocompletar dependiendo de los
 *        comandos seteados por argumento:
 * @PARAMS:
 *  [in] char** commands - array con strings que representan los comandos disponibles.
 *                         El ultimo elemento del array debne ser NULL.
 */
void  u_console_set_commands(char** commands);


#endif