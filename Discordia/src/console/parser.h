#ifndef DISCORDIA_PARSER_H
#define DISCORDIA_PARSER_H

#include <utils/utils.h>

typedef enum
{
    DS_INVALID_COMMAND = -1,
    DS_SALIR,
    DS_INICIAR_PATOTA,
    DS_LISTAR_TRIPULANTES,
    DS_EXPULSAR_TRIPULANTE,
    DS_INICIAR_PLANIFICACION,
    DS_PAUSAR_PLANIFICACION,
    DS_OBTENER_BITACORA
} ds_parser_command_e;

typedef struct
{
    ds_parser_command_e command;
    void*               data;
} ds_parser_result_t;

typedef struct
{
    uint32_t     cant_trip;
    const char*  ruta_tareas;
    t_list*      trip_positions;
} ds_command_iniciar_patota_t;

void ds_parse(uint32_t argc, char** argv, ds_parser_result_t* result);
void ds_parser_result_free(ds_parser_result_t* result);

#endif