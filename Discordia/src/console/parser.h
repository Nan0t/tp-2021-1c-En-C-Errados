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
    DS_OBTENER_BITACORA,
#ifndef NDEBUG
    _DEBUG_DS_DESPLAZAMIENTO_TRIPULANTE,
    _DEBUG_DS_INICIAR_TAREA,
    _DEBUG_DS_FINALIZAR_TAREA,
    _DEBUG_DS_TRIPULANTE_ATIENDE_SABOTAJE,
    _DEBUG_DS_TRIPULANTE_RESUELVE_SABOTAJE,

    _DEBUG_DS_MOVER_TRIPULANTE,
    _DEBUG_DS_PROXIMA_TAREA,
    _DEBUG_DS_TRIPULANTE_NUEVO_ESTADO
#endif
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

typedef struct
{
    uint32_t pid;
    uint32_t tid;
} ds_command_expulsar_tripulante_t;

#ifndef NDEBUG
typedef struct
{
    uint32_t tid;
    u_pos_t  origen;
    u_pos_t  destion;
} ds_command_desplazamiento_tripulante_t;

typedef struct
{
    uint32_t tid;
    char*    tarea;
} ds_command_iniciar_tarea_t;

typedef struct
{
    uint32_t tid;
    char*    tarea;
} ds_command_finalizar_tarea_t;

typedef struct
{
    uint32_t pid;
    uint32_t tid;
    u_pos_t  pos;
} ds_command_mover_tripulante_t;

typedef struct
{
    uint32_t pid;
    uint32_t tid;
} ds_command_proxima_tarea_t;
 
typedef struct
{
    uint32_t pid;
    uint32_t tid;
    char     estado;
} ds_command_tripulante_nuevo_estado_t;
#endif

void ds_parse(uint32_t argc, char** argv, ds_parser_result_t* result);
void ds_parser_result_free(ds_parser_result_t* result);

#endif