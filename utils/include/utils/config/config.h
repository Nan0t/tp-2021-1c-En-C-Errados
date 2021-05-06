#ifndef UTILS_CONFIG_H
#define UTILS_CONFIG_H

/**
 * @FILE: config.h
 * 
 * @DESC: Header contiene la interfaz del config de la libreria.
 */

#include "utils/typedefs.h"

/**
 * @NAME: u_config_init
 * @DESC: Inicializa la instancia de config de la libreria.
 * @PARAMS:
 *  [in] const char* file_path   --> ruta del archivo de configuracion.
 * 
 * @RETURNS: true en caso de que se haya podido abrir el archivo de
 * configuracion correctamente. En caso contrario, false.
 * 
 * @NOTA: en caso de que file_path sea NULL, el programa aborta.
 */
bool u_config_init(const char* file_path);

/**
 * @NAME: u_config_terminate
 * @DESC: Finaliza la instancia de config de la libreria.
 */
void u_config_terminate(void);

/**
 * @NAME: u_config_is_init
 * @DESC: responde si la instancia de config esta inicializada o no.
 * 
 * @RETURNS: true en caso de que la instancia de config este incializada.
 * En caso contrario, false.
 */
bool u_config_is_init(void);

/**
 * @NAME: u_config_get_string_value
 * @DESC: devuelve un string correspondiente al valor de la key.
 * @PARAMS:
 *  const char*  key --> clave con el valor que se quiere obtener.
 * 
 * @RETURNS: el string correspondiente a la key. En caso contrario.
 */
const char* u_config_get_string_value(const char* key);

/**
 * @NAME: u_config_get_array_value
 * @DESC: devuelve un array de strings correspondiente al valor de la key.
 * @PARAMS:
 *  const char*  key  -->  clave con el valor que se quiere obtener.
 * 
 * @RETURNS: un array de strings con los valores de la key. El ultimo
 * valor del array es NULL, indicando el final del array.
 */
char**      u_config_get_array_value(const char* key);

/**
 * @NAME: u_config_get_int_value
 * @DESC: devuelve un int correspondiente al valor de la key.
 * @PARAMS:
 *  const char*  key  -->  clave con el valor que se quiere obtener.
 * 
 * @RETURNS: un int con el valor de la key.
 */
int         u_config_get_int_value(const char* key);

/**
 * @NAME: u_config_get_float_value
 * @DESC: devuelve un float correspondiente al valor de la key.
 * @PARAMS:
 *  const char*  key  -->  clave con el valor que se quiere obtener.
 * 
 * @RETURNS: un float con el valor de la key.
 */
float       u_config_get_float_value(const char* key);

/**
 * @NAME: u_config_get_double_value
 * @DESC: devuelve un double correspondiente al valor de la key.
 * @PARAMS:
 *  const char*  key  -->  clave con el valor que se quiere obtener.
 * 
 * @RETURNS: un double con el valor de la key.
 */
double      u_config_get_double_value(const char* key);

#endif