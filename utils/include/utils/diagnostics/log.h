#ifndef UTILS_DIAGNOSTICS_LOG_H
#define UTILS_DIAGNOSTICS_LOG_H

#include "utils/typedefs.h"

/**
 * @File: log.h
 * 
 * @Desc: Header que contiene la interfaz del logger de la libreria. Las macros en que se
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
 * @Enum: u_log_level_e
 * 
 * @Desc: los distintos niveles de logger.
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
 * @Macro: U_LOG_TRACE
 * 
 * @Desc: logea un mensaje con nivel trace.
 * 
 * @Params:
 *  const char* message  --> mensaje con formato printf que se quiere logear.
 *  ...                  --> argumentos que correspondan al formato de message.
 */
#define U_LOG_TRACE(...)    u_logger_log(U_LOG_LEVEL_TRACE, __VA_ARGS__)

/**
 * @Macro: U_LOG_DEBUG
 * 
 * @Desc: logea un mensaje con nivel debug.
 * 
 * @Params:
 *  const char* message  --> mensaje con formato printf que se quiere logear.
 *  ...                  --> argumentos que correspondan al formato de message.
 */
#define U_LOG_DEBUG(...)    u_logger_log(U_LOG_LEVEL_DEBUG, __VA_ARGS__)

/**
 * @Macro: U_LOG_INFO
 * 
 * @Desc: logea un mensaje con nivel info.
 * 
 * @Params:
 *  const char* message  --> mensaje con formato printf que se quiere logear.
 *  ...                  --> argumentos que correspondan al formato de message.
 */
#define U_LOG_INFO(...)     u_logger_log(U_LOG_LEVEL_INFO, __VA_ARGS__)

/**
 * @Macro: U_LOG_WARN
 * 
 * @Desc: logea un mensaje con nivel warning.
 * 
 * @Params:
 *  const char* message  --> mensaje con formato printf que se quiere logear.
 *  ...                  --> argumentos que correspondan al formato de message.
 */
#define U_LOG_WARN(...)     u_logger_log(U_LOG_LEVEL_WARN, __VA_ARGS__)

/**
 * @Macro: U_LOG_ERROR
 * 
 * @Desc: logea un mensaje con nivel error.
 * 
 * @Params:
 *  const char* message  --> mensaje con formato printf que se quiere logear.
 *  ...                  --> argumentos que correspondan al formato de message.
 */
#define U_LOG_ERROR(...)    u_logger_log(U_LOG_LEVEL_ERROR, __VA_ARGS__)


// =====================================================================
//                  *** Funciones de implementacion ***
// =====================================================================

/**
 * @Func: u_logger_init
 * 
 * @Desc: inicializa el logger de la libreria
 * 
 * @Params:
 *  const char*       file_name       --> nombre del archivo de log.
 *  const char*       program_name    --> nombre de la aplicacion.
 *  bool              log_to_console  --> si se debe logear a la consola o no.
 *  enum u_log_level  log_level       --> nivel de logs.
 */
void u_logger_init(const char* file_name, const char* program, bool log_to_console, u_log_level_e log_level);

/**
 * @Func: u_logger_terminate
 * 
 * @Desc: finaliza la instancia de logger de la libreria
 */
void u_logger_terminate(void);


/**
 * @Func: u_logger_log
 * 
 * @Desc: logea un mensaje nuevo en el nivel especificado. Si al inicializar el logger, el
 * nivel de logeo fue seteado a un nivel superior al especificado, el mensaje no se logeara.
 * 
 * @Params:
 *  enum u_log_level  log_level  --> nivel del log.
 *  const char*       message    --> mensaje con formato printf que se logeara.
 *  ...                          --> argumentos que correspondan al formato del message.
 * 
 * @Assert: en caso de que message sea NULL.
 */
void u_logger_log(u_log_level_e log_level, const char* message, ...);

/**
 * @Func: u_logger_is_init
 * 
 * @Desc: responde si el logger esta inicializado o no.
 * 
 * @Returns: true en caso de que el logger este inicializado. En caso contrario, false.
 */
bool u_logger_is_init(void);

#endif