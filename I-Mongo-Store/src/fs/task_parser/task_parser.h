#ifndef FS_TASK_H
#define FS_TASK_H

#include <utils/utils.h>

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

typedef struct
{
    char*          file_name;
    fs_task_type_e type;
    uint32_t       arg;
} fs_task_parser_result_t;

fs_task_parser_result_t* fs_task_parser_parse(const char* task);
void                     fs_task_parser_result_delete(fs_task_parser_result_t* parser_result);

#endif