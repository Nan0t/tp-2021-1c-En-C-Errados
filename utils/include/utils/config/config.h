#ifndef UTILS_CONFIG_H
#define UTILS_CONFIG_H

/**
 * @File: config.h
 * 
 * @Desc: Header contiene la interfaz del config de la libreria.
 */

#include "utils/typedefs.h"

/**
 * @Func: u_config_init
 * 
 * @Desc: Inicializa la instancia de config de la libreria.
 * 
 * @Params:
 *  const char* file_path   --> ruta del archivo de configuracion.
 * 
 * @Returns: true en caso de que se haya podido abrir el archivo de
 * configuracion correctamente. En caso contrario, false.
 * 
 * @Abort: en caso de que file_path sea NULL.
 */
bool u_config_init(const char* file_path);

/**
 * @Func: u_config_terminate
 * 
 * @Desc: Finaliza la instancia de config de la libreria.
 */
void u_config_terminate(void);

/**
 * @Func: u_config_is_init
 * 
 * @Desc: responde si la instancia de config esta inicializada o no.
 * 
 * @Returns: true en caso de que la instancia de config este incializada.
 * En caso contrario, false.
 */
bool u_config_is_init(void);

/**
 * @Func: u_config_get_string_value
 * 
 * @Desc: devuelve un string correspondiente al valor de la key.
 * 
 * @Params:
 *  const char*  key --> clave con el valor que se quiere obtener.
 * 
 * @Returns: el string correspondiente a la key. En caso contrario.
 */
const char* u_config_get_string_value(const char* key);

/**
 * @Func: u_config_get_array_value
 * 
 * @Desc: devuelve un array de strings correspondiente al valor de la key.
 * 
 * @Params:
 *  const char*  key  -->  clave con el valor que se quiere obtener.
 * 
 * @Returns: un array de strings con los valores de la key. El ultimo
 * valor del array es NULL, indicando el final del array.
 */
char**      u_config_get_array_value(const char* key);

/**
 * @Func: u_config_get_int_value
 * 
 * @Desc: devuelve un int correspondiente al valor de la key.
 * 
 * @Params:
 *  const char*  key  -->  clave con el valor que se quiere obtener.
 * 
 * @Returns: un int con el valor de la key.
 */
int         u_config_get_int_value(const char* key);

/**
 * @Func: u_config_get_float_value
 * 
 * @Desc: devuelve un float correspondiente al valor de la key.
 * 
 * @Params:
 *  const char*  key  -->  clave con el valor que se quiere obtener.
 * 
 * @Returns: un float con el valor de la key.
 */
float       u_config_get_float_value(const char* key);

/**
 * @Func: u_config_get_double_value
 * 
 * @Desc: devuelve un double correspondiente al valor de la key.
 * 
 * @Params:
 *  const char*  key  -->  clave con el valor que se quiere obtener.
 * 
 * @Returns: un double con el valor de la key.
 */
double      u_config_get_double_value(const char* key);

#endif