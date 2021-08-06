#ifndef FS_TASK_H
#define FS_TASK_H

#include <utils/utils.h>

/**
 * @ENUM: fs_task_type_e
 * @DESC: tipos de tareas validas que pueden generar/actualizar recursos del file system.
 */
typedef enum
{
    FS_TASK_UNKNOWN = -1,
    FS_TASK_GENERAR_OXIGENO,
    FS_TASK_CONSUMIR_OXIGENO,
    FS_TASK_GENERAR_COMIDA,
    FS_TASK_CONSUMIR_COMIDA,
    FS_TASK_GENERAR_BASURA,
    FS_TASK_DESCARTAR_BASURA
} fs_task_type_e;

/**
 * @NAME: fs_task_parser_result_t
 * @DESC: estructura que contiene informacion referida a una tarea.
 * @ATTR:
 *  char*          file_name - nombre del archivo correspondiente a la tarea.
 *  fs_task_type_e type      - tipo de tarea. En caso de no ser una tarea que actualize recursos, el tipo sera
 *                              FS_TASK_UNKNOWN.
 *  uint32_t       arg       - argumento de la tarea.
 */
typedef struct
{
    char*          file_name;
    fs_task_type_e type;
    uint32_t       arg;
} fs_task_parser_result_t;

/**
 * @NAME: fs_task_parser_parse
 * @DESC: parsea el string de una tarea.
 * @PARAMS:
 *  [in] const char* task - tarea a parsear.
 * 
 * @RETURNS: una instancia fs_task_parser_result_t con la informacion de la tarea, o NULL
 *              en caso de que no se haya podido parsear.
 */
fs_task_parser_result_t* fs_task_parser_parse(const char* task);

/**
 * @NAME: fs_task_parser_result_delete
 * @DESC: elimina una instancia fs_task_parser_result_t
 * @PARAMS:
 *  [in] fs_task_parser_result_t* parser_result - instancia que se quiere eliminar
 */
void fs_task_parser_result_delete(fs_task_parser_result_t* parser_result);

#endif