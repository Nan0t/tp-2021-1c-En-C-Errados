#include "task_parser.h"
#include <stdlib.h>
#include <ctype.h>

private const char* TASK_FILE_NAMES[] =
{
    "Unknown",
    "Oxigeno",
    "Oxigeno",
    "Comida",
    "Comida",
    "Basura",
    "Basura"
};

private fs_task_type_e fs_task_parser_result_type_to_enum(const char* task_name);

fs_task_parser_result_t* fs_task_parser_parse(const char* task)
{
    fs_task_parser_result_t* this = u_malloc(sizeof(fs_task_parser_result_t));

    char** args = string_split((char*)task, " ");

    this->type      = fs_task_parser_result_type_to_enum(args[0]);
    this->file_name = strdup(TASK_FILE_NAMES[(int)(this->type) + 1]);

    if(this->type != FS_TASK_UNKNOWN)
        this->arg = atoi(args[1]); // TODO: Chequear que args[1] sea realmente un numero
    else
        this->arg = 0;

    for(char** it = args; *it != NULL; it ++)
        u_free(*it);
    u_free(args);

    return this;
}

void fs_task_parser_result_delete(fs_task_parser_result_t* this)
{
    u_free(this->file_name);
    u_free(this);
}

private fs_task_type_e fs_task_parser_result_type_to_enum(const char* task_name)
{
    if(!strcmp(task_name, "GENERAR_OXIGENO"))
        return FS_TASK_GENERAR_OXIGENO;
    else if(!strcmp(task_name, "CONSUMIR_OXIGENO"))
        return FS_TASK_CONSUMIR_OXIGENO;
    else if(!strcmp(task_name, "GENERAR_COMIDA"))
        return FS_TASK_GENERAR_COMIDA;
    else if(!strcmp(task_name, "CONSUMIR_COMIDA"))
        return FS_TASK_CONSUMIR_COMIDA;
    else if(!strcmp(task_name, "GENERAR_BASURA"))
        return FS_TASK_GENERAR_BASURA;
    else if(!strcmp(task_name, "DESCARTAR_BASURA"))
        return FS_TASK_DESCARTAR_BASURA;
    
    return FS_TASK_UNKNOWN;
}