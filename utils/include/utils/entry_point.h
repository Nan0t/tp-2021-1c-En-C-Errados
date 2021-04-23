#ifndef UTILS_ENTRY_POINT_H
#define UTILS_ENTRY_POINT_H

/**
 * @File: entry_point.h
 * 
 * @Desc: Header que debe ser incluido en el archivo que sirva como punto de entrada
 * de la aplicacion. Contiene la funcion main donde se inicializaran todos los submodulos
 * de la libreria (archivos de configuracion, loggers, etc). La aplicacion debe crear definir
 * dos funciones: get_app_setup y entry_point.
 */

#include <utils/utils.h>

/**
 * @Struct: app_attr_t
 * 
 * @Desc: estructura que contendra informacion sobre la aplicacion.
 * 
 * @Attr:
 *  const char*       app_name            --> nombre de la aplicacion.
 *  const char*       log_file_path       --> ruta donde se creara el archivo de log
 *  bool              log_active_console  --> si los logs van a imprimirse por consola o no.
 *  enum u_log_level  log_level           --> el nivel de los logs.
 *  const char*       config_file_path    --> ruta donde se encuentra el archivo de configuracion.
 */
typedef struct
{
    const char* app_name;

    const char*   log_file_path;
    bool          log_active_console;
    u_log_level_e log_level;

    const char* config_file_path;
} app_attr_t;

/**
 * @Func: get_app_setup
 * 
 * @Desc: funcion a definir por la aplicacion. Devolvera informacion sobre la aplicacion.
 * 
 * @Returns: una estructura app_attr_t con la informacion de la aplicacion.
 */
static app_attr_t get_app_setup(void);

/**
 * @Fun: entry_point
 * 
 * @Desc: punto de entrada de la aplicacion. Esta es la primera funcion que se llamara
 * luego de inicializar la libreria (siempre y cuando no haya ningun error en la inicializacion)
 * 
 * @Params:
 *  int          argc  --> cantidad de argumentos que contiene argv
 *  const char*  argv  --> argumentos pasados por la linea de comandos
 * 
 * @Returns: el resultado del programa, que luego sera el valor de de retorno en el main.
 */
static int entry_point(int argc, char** argv);

int main(int argc, char** argv)
{
    app_attr_t app_attr = get_app_setup();

    u_logger_init(
        app_attr.log_file_path,
        app_attr.app_name,
        app_attr.log_active_console,
        app_attr.log_level);

    U_ASSERT(
        u_config_init(app_attr.config_file_path),
        "Couldn't open the config file at: %s",
        app_attr.config_file_path);

    int result = entry_point(argc, argv);

    u_config_terminate();
    u_logger_terminate();

    return result;
}

#endif