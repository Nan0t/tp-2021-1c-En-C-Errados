#ifndef UTILS_DIAGNOSTICS_LOG_H
#define UTILS_DIAGNOSTICS_LOG_H

#include "utils/typedefs.h"

/**
 * @FILE: log.h
 * 
 * @DESC: Header que contiene la interfaz del logger de la libreria. Las macros en que se
 * encuentran al principio del archivo son las que deberian ser usadas al momento de logear.
 * El formato de los mensajes es el mismo que el usado por la funcion printf de la libreria
 * estandar de C.
 * 
 * Ejemplo de uso:
 * 
 *  char* msg = get_new_message();
 * 
 *  U_LOG_INFO("Nuevo mensaje: %s", msg);
 *  .
 *  .
 *  .
 */



/**
 * @NAME: u_log_level_e
 * @DESC: los distintos niveles de logger.
 */
typedef enum
{
    U_LOG_LEVEL_TRACE,
    U_LOG_LEVEL_DEBUG,
    U_LOG_LEVEL_INFO,
    U_LOG_LEVEL_WARN,
    U_LOG_LEVEL_ERROR
} u_log_level_e;

/**
 * @NAME: U_LOG_TRACE
 * @DESC: logea un mensaje con nivel trace.
 * @PARAMS:
 *  [in] const char* message - mensaje con formato printf que se quiere logear.
 *  ...                      - argumentos que correspondan al formato de message.
 */
#define U_LOG_TRACE(...)    u_logger_log(U_LOG_LEVEL_TRACE, __VA_ARGS__)

/**
 * @NAME: U_LOG_DEBUG
 * @DESC: logea un mensaje con nivel debug.
 * @PARAMS:
 *  [in] const char* message - mensaje con formato printf que se quiere logear.
 *  ...                      - argumentos que correspondan al formato de message.
 */
#define U_LOG_DEBUG(...)    u_logger_log(U_LOG_LEVEL_DEBUG, __VA_ARGS__)

/**
 * @NAME: U_LOG_INFO
 * @DESC: logea un mensaje con nivel info.
 * @PARAMS:
 *  [in] const char* message - mensaje con formato printf que se quiere logear.
 *  ...                      - argumentos que correspondan al formato de message.
 */
#define U_LOG_INFO(...)     u_logger_log(U_LOG_LEVEL_INFO, __VA_ARGS__)

/**
 * @NAME: U_LOG_WARN
 * @DESC: logea un mensaje con nivel warning.
 * @PARAMS:
 *  [in] const char* message - mensaje con formato printf que se quiere logear.
 *  ...                      - argumentos que correspondan al formato de message.
 */
#define U_LOG_WARN(...)     u_logger_log(U_LOG_LEVEL_WARN, __VA_ARGS__)

/**
 * @NAME: U_LOG_ERROR
 * @DESC: logea un mensaje con nivel error.
 * @PARAMS:
 *  [in] const char* message - mensaje con formato printf que se quiere logear.
 *  ...                      - argumentos que correspondan al formato de message.
 */
#define U_LOG_ERROR(...)    u_logger_log(U_LOG_LEVEL_ERROR, __VA_ARGS__)


// =====================================================================
//                  *** Funciones de implementacion ***
// =====================================================================

/**
 * @NAME: u_logger_init
 * @DESC: inicializa el logger de la libreria
 * @PARAMS:
 *  [in] const char*       file_name      - nombre del archivo de log.
 *  [in] const char*       program_name   - nombre de la aplicacion.
 *  [in] bool              log_to_console - si se debe logear a la consola o no.
 *  [in] enum u_log_level  log_level      - nivel de logs.
 */
void u_logger_init(const char* file_name, const char* program, bool log_to_console, u_log_level_e log_level);

/**
 * @NAME: u_logger_terminate
 * @DESC: finaliza la instancia de logger de la libreria
 */
void u_logger_terminate(void);


/**
 * @NAME: u_logger_log
 * @DESC: logea un mensaje nuevo en el nivel especificado. Si al inicializar el logger, el
 * nivel de logeo fue seteado a un nivel superior al especificado, el mensaje no se logeara.
 * @PARAMS:
 *  [in] enum u_log_level  log_level  - nivel del log.
 *  [in] const char*       message    - mensaje con formato printf que se logeara.
 *  ...                               - argumentos que correspondan al formato del message.
 * 
 * @NOTA: en caso de que message sea NULL el programa aborta.
 */
void u_logger_log(u_log_level_e log_level, const char* message, ...) UTILS_PRINT_FORMAT(2, 3);

/**
 * @NAME: u_logger_is_init
 * @DESC: responde si el logger esta inicializado o no.
 * 
 * @RETURNS: true en caso de que el logger este inicializado. En caso contrario, false.
 */
bool u_logger_is_init(void);

#endif