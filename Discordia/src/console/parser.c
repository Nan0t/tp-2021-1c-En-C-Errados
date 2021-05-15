#include "parser.h"
#include <stdlib.h>
#include <ctype.h>

#define DS_COMMAND_INDEX 0

#define DS_INICIAR_PATOTA__MIN_CANT_ARGS          3
#define DS_INICIAR_PATOTA__CANT_TRIPULANTES_INDEX 1
#define DS_INICIAR_PATOTA__RUTA_TAREA_INDEX       2
#define DS_INICIAR_PATOTA__POSICIONES_INDEX       3

#define DS_EXPULSAR_TRIPULANTE__CANT_ARGS 2
#define DS_EXPULSAR_TRIPULANTE__TID_INDEX 1

#define DS_OBTENER_BITACORA__CANT_ARGS 2
#define DS_OBTENER_BITACORA__TID_INDEX 1

private ds_parser_command_e ds_command_str_to_enum(const char* command_str);
private bool                ds_is_valid_number(const char* number_str);
private bool                ds_is_valid_position(const char* pos_str);

private void ds_command_iniciar_patota_delete(ds_command_iniciar_patota_t* command);

#ifndef NDEBUG
private void ds_command_desplazar_tripulante_delete(ds_command_desplazamiento_tripulante_t* command);
private void ds_command_iniciar_tarea_delete(ds_command_iniciar_tarea_t* command);
private void ds_command_finalizar_tarea_delete(ds_command_finalizar_tarea_t* command);
private void ds_command_mover_tripulante_delete(ds_command_mover_tripulante_t* command);
private void ds_command_tripulante_nuevo_estado(ds_command_tripulante_nuevo_estado_t* command);
#endif

private void ds_parser_write_error(ds_parser_result_t* result, const char* msg_format, ...);
private void ds_parser_not_enough_args_error(ds_parser_result_t* result, const char* hint);
private void ds_parser_tid_error(ds_parser_result_t* result, const char* actual_tid);
private void ds_parser_pos_error(ds_parser_result_t* result, const char* actual_pos);

private void ds_parser_error(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_iniciar_patota(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_expulsar_tripulante(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_obtener_bitacora(uint32_t argc, char** argv, ds_parser_result_t* result);

private void ds_parser_dummy_func(uint32_t argc, char** argv, ds_parser_result_t* result);

#ifndef NDEBUG

#define DS_DESPLAZAMIENTO_TRIPULANTE__CANT_ARGS     4
#define DS_DESPLAZAMIENTO_TRIPULANTE__TID_INDEX     1
#define DS_DESPLAZAMIENTO_TRIPULANTE__ORIGEN_INDEX  2
#define DS_DESPLAZAMIENTO_TRIPULANTE__DESTINO_INDEX 3

#define DS_INICIAR_TAREA__CANT_ARGS   3
#define DS_INICIAR_TAREA__TID_INDEX   1
#define DS_INICIAR_TAREA__TAREA_INDEX 2

#define DS_FINALIZAR_TAREA__CANT_ARGS   3
#define DS_FINALIZAR_TAREA__TID_INDEX   1
#define DS_FINALIZAR_TAREA__TAREA_INDEX 2

#define DS_TRIP_ATIENDE_SABOTAJE__CANT_ARGS 2
#define DS_TRIP_ATIENDE_SABOTAJE__TID_INDEX 1

#define DS_TRIP_RESUELVE_SABOTAJE__CANT_ARGS 2
#define DS_TRIP_RESUELVE_SABOTAJE__TID_INDEX 1

#define DS_MOVER_TRIPULANTE__CANT_ARGS 3
#define DS_MOVER_TRIPULANTE__TID_INDEX 1
#define DS_MOVER_TRIPULANTE__POS       2

#define DS_PROXIMA_TAREA__CANT_ARGS 2
#define DS_PROXIMA_TAREA__TID_INDEX 1

#define DS_TRIP_NUEVO_ESTADO__CANT_ARGS    3
#define DS_TRIP_NUEVO_ESTADO__TID_INDEX    1
#define DS_TRIP_NUEVO_ESTADO__ESTADO_INDEX 2

private void ds_parser_desplazamiento_tripulante(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_iniciar_tarea(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_finalizar_tarea(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_trip_atiende_sabotaje(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_trip_resuelve_sabotaje(uint32_t argc, char** argv, ds_parser_result_t* result);

private void ds_parser_mover_tripulante(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_proxima_tarea(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_trip_nuevo_estado(uint32_t argc, char** argv, ds_parser_result_t* result);

#endif

typedef void(*ds_parser_func_t)(uint32_t, char**, ds_parser_result_t*);

private ds_parser_func_t PARSER_FUNC_ARRAY[] =
{
    ds_parser_error,
    ds_parser_dummy_func,
    ds_parser_iniciar_patota,
    ds_parser_dummy_func,
    ds_parser_expulsar_tripulante,
    ds_parser_dummy_func,
    ds_parser_dummy_func,
    ds_parser_obtener_bitacora,
#ifndef NDEBUG
    ds_parser_desplazamiento_tripulante,
    ds_parser_iniciar_tarea,
    ds_parser_finalizar_tarea,
    ds_parser_trip_atiende_sabotaje,
    ds_parser_trip_resuelve_sabotaje,

    ds_parser_mover_tripulante,
    ds_parser_proxima_tarea,
    ds_parser_trip_nuevo_estado,
#endif
};

void ds_parse(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    result->command = ds_command_str_to_enum(argv[DS_COMMAND_INDEX]);
    PARSER_FUNC_ARRAY[((int)(result->command)) + 1](argc, argv, result);
}

void ds_parser_result_free(ds_parser_result_t* result)
{
    switch(result->command)
    {
    case DS_INICIAR_PATOTA:
        ds_command_iniciar_patota_delete((ds_command_iniciar_patota_t*)result->data);
        break;

    case DS_EXPULSAR_TRIPULANTE:
    case DS_OBTENER_BITACORA:
    case DS_INVALID_COMMAND:
        u_free(result->data);

    default:
        break;
    }
}

private ds_parser_command_e ds_command_str_to_enum(const char* command_str)
{
    if(!strcmp(command_str, "INICIAR_PATOTA"))
        return DS_INICIAR_PATOTA;
    else if(!strcmp(command_str, "LISTAR_TRIPULANTES"))
        return DS_LISTAR_TRIPULANTES;
    else if(!strcmp(command_str, "EXPULSAR_TRIPULANTE"))
        return DS_EXPULSAR_TRIPULANTE;
    else if(!strcmp(command_str, "INICIAR_PLANIFICACION"))
        return DS_INICIAR_PLANIFICACION;
    else if(!strcmp(command_str, "PAUSAR_PLANIFICACION"))
        return DS_PAUSAR_PLANIFICACION;
    else if(!strcmp(command_str, "OBTENER_BITACORA"))
        return DS_OBTENER_BITACORA;
    else if(!strcmp(command_str, "SALIR"))
        return DS_SALIR;
#ifndef NDEBUG
    else if(!strcmp(command_str, "DESPLAZAMIENTO_TRIPULANTE"))
        return _DEBUG_DS_DESPLAZAMIENTO_TRIPULANTE;
    else if(!strcmp(command_str, "INICIAR_TAREA"))
        return _DEBUG_DS_INICIAR_TAREA;
    else if(!strcmp(command_str, "FINALIZAR_TAREA"))
        return _DEBUG_DS_FINALIZAR_TAREA;
    else if(!strcmp(command_str, "TRIPULANTE_ATIENDE_SABOTAJE"))
        return _DEBUG_DS_TRIPULANTE_ATIENDE_SABOTAJE;
    else if(!strcmp(command_str, "TRIPULANTE_RESUELVE_SABOTAJE"))
        return _DEBUG_DS_TRIPULANTE_RESUELVE_SABOTAJE;
    else if(!strcmp(command_str, "MOVER_TRIPULANTE"))
        return _DEBUG_DS_MOVER_TRIPULANTE;
    else if(!strcmp(command_str, "PROXIMA_TAREA"))
        return _DEBUG_DS_PROXIMA_TAREA;
    else if(!strcmp(command_str, "TRIPULANTE_NUEVO_ESTADO"))
        return _DEBUG_DS_TRIPULANTE_NUEVO_ESTADO;
#endif
    
    return DS_INVALID_COMMAND;
}

private bool ds_is_valid_number(const char* number_str)
{
    for(const char* c = number_str; *c != '\0'; c ++)
        if(!isdigit(*c))
            return false;

    return true;
}

private bool ds_is_valid_position(const char* pos_str)
{
    if(!isdigit(*pos_str))
        return false;

    bool pipe_found = false;

    for(const char* c = pos_str; *c != '\0'; c ++)
    {
        if(*c == '|' && !pipe_found)
            pipe_found = true;
        else if((*c == '|' && pipe_found) || !isdigit(*c))
            return false;
    }

    return true;
}

private void ds_command_iniciar_patota_delete(ds_command_iniciar_patota_t* command)
{
    u_free((char*)command->ruta_tareas);

    if(command->trip_positions != NULL)
        list_destroy_and_destroy_elements(command->trip_positions, u_free);

    u_free(command);
}

#ifndef NDEBUG

private void ds_command_desplazar_tripulante_delete(ds_command_desplazamiento_tripulante_t* command)
{
    u_free(command);
}

private void ds_command_iniciar_tarea_delete(ds_command_iniciar_tarea_t* command)
{
    u_free(command->tarea);
    u_free(command);
}

private void ds_command_finalizar_tarea_delete(ds_command_finalizar_tarea_t* command)
{
    u_free(command->tarea);
    u_free(command);
}

private void ds_command_mover_tripulante_delete(ds_command_mover_tripulante_t* command)
{
    u_free(command);
}

private void ds_command_tripulante_nuevo_estado(ds_command_tripulante_nuevo_estado_t* command)
{
    u_free(command);
}

#endif

private void ds_parser_write_error(ds_parser_result_t* result, const char* msg_format, ...)
{
    va_list args;
    va_start(args, msg_format);

    result->command = DS_INVALID_COMMAND;
    result->data    = string_from_vformat(msg_format, args);
    va_end(args);
}

private void ds_parser_not_enough_args_error(ds_parser_result_t* result, const char* hint)
{
    ds_parser_write_error(result,
        "Cantidad insuficiente de argumentos: %s", hint);
}

private void ds_parser_tid_error(ds_parser_result_t* result, const char* actual_tid)
{
    ds_parser_write_error(result,
        "TID ingresado invalido!: %s. Ingrese un numero valido",
        actual_tid);
}

private void ds_parser_pos_error(ds_parser_result_t* result, const char* actual_pos)
{
    ds_parser_write_error(result,
        "Formato de posicion invalida: %s",
        actual_pos);
}

private void ds_parser_error(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    (void)argc;
    (void)argv;

    result->data = string_duplicate("Comando Invalido!");
}

private void ds_parser_iniciar_patota(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_INICIAR_PATOTA__MIN_CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "INICIAR_PATOTA [CANT_TRIP] [RUTA_TAREAS] [...POS]");
        return;
    }

    if(!ds_is_valid_number(argv[DS_INICIAR_PATOTA__CANT_TRIPULANTES_INDEX]))
    {
        ds_parser_write_error(result,
            "Cantidad de tripulantes invalida!: %s. Ingrese un valor numerico valido",
            argv[DS_INICIAR_PATOTA__CANT_TRIPULANTES_INDEX]);

        return;
    }

    ds_command_iniciar_patota_t* command_result = u_malloc(sizeof(ds_command_iniciar_patota_t));

    command_result->cant_trip      = atoi(argv[DS_INICIAR_PATOTA__CANT_TRIPULANTES_INDEX]);
    command_result->ruta_tareas    = string_duplicate(argv[DS_INICIAR_PATOTA__RUTA_TAREA_INDEX]);
    command_result->trip_positions = NULL;

    if(argc > DS_INICIAR_PATOTA__MIN_CANT_ARGS)
    {
        command_result->trip_positions = list_create();

        for(uint32_t i = DS_INICIAR_PATOTA__MIN_CANT_ARGS; i < argc; i ++)
        {
            if(!ds_is_valid_position(argv[i]))
            {
                ds_parser_pos_error(result, argv[i]);
                ds_command_iniciar_patota_delete(command_result);

                return;
            }
            else
            {
                u_pos_t* pos       = u_malloc(sizeof(u_pos_t));
                char** coordinates = string_split(argv[i], "|");

                pos->x = atoi(coordinates[0]);
                pos->y = atoi(coordinates[1]);

                list_add(command_result->trip_positions, pos);

                u_free(coordinates[0]);
                u_free(coordinates[1]);
                u_free(coordinates);
            }
        }
    }

    result->data = command_result;
}

private void ds_parser_expulsar_tripulante(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_EXPULSAR_TRIPULANTE__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "EXPULSAR_TRIPULANTE [TID]");
        return;
    }
    
    if(!ds_is_valid_number(argv[DS_EXPULSAR_TRIPULANTE__TID_INDEX]))
    {
        ds_parser_tid_error(result, argv[DS_EXPULSAR_TRIPULANTE__TID_INDEX]);
        return;
    }

    uint32_t* data = u_malloc(sizeof(uint32_t));
    *data = atoi(argv[DS_EXPULSAR_TRIPULANTE__TID_INDEX]);

    result->data = data;
}

private void ds_parser_obtener_bitacora(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_OBTENER_BITACORA__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "OBTENER_BITACORA [TID]");
        return;
    }
    
    if(!ds_is_valid_number(argv[DS_OBTENER_BITACORA__TID_INDEX]))
    {
        ds_parser_tid_error(result, argv[DS_EXPULSAR_TRIPULANTE__TID_INDEX]);
        return;
    }

    uint32_t* data = u_malloc(sizeof(uint32_t));
    *data = atoi(argv[DS_OBTENER_BITACORA__TID_INDEX]);

    result->data = data;
}

private void ds_parser_dummy_func(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    (void)argc;
    (void)argv;
    (void)result;
}

#ifndef NDEBUG

private void ds_parser_desplazamiento_tripulante(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_DESPLAZAMIENTO_TRIPULANTE__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "DESPLAZAMIENTO_TRIPULANTE [TID] [ORIGEN] [DESTION]");
        return;
    }

    if(!ds_is_valid_number(argv[DS_DESPLAZAMIENTO_TRIPULANTE__TID_INDEX]))
    {
        ds_parser_tid_error(result, argv[DS_DESPLAZAMIENTO_TRIPULANTE__TID_INDEX]);
        return;
    }

    if(!ds_is_valid_position(argv[DS_DESPLAZAMIENTO_TRIPULANTE__ORIGEN_INDEX]))
    {
        ds_parser_pos_error(result, argv[DS_DESPLAZAMIENTO_TRIPULANTE__ORIGEN_INDEX]);
        return;
    }

    if(!ds_is_valid_position(argv[DS_DESPLAZAMIENTO_TRIPULANTE__DESTINO_INDEX]))
    {
        ds_parser_pos_error(result, argv[DS_DESPLAZAMIENTO_TRIPULANTE__DESTINO_INDEX]);
        return;
    }

    uint32_t tid = atoi(argv[DS_DESPLAZAMIENTO_TRIPULANTE__TID_INDEX]);
    char** origen_coords = string_split(argv[DS_DESPLAZAMIENTO_TRIPULANTE__ORIGEN_INDEX], "|");
    char** destion_coords = string_split(argv[DS_DESPLAZAMIENTO_TRIPULANTE__DESTINO_INDEX], "|");

    ds_command_desplazamiento_tripulante_t* command =
        u_malloc(sizeof(ds_command_desplazamiento_tripulante_t));

    command->tid = tid;

    command->origen.x = atoi(origen_coords[0]);
    command->origen.y = atoi(origen_coords[1]);

    command->destion.x = atoi(destion_coords[0]);
    command->destion.y = atoi(destion_coords[1]);

    result->data = command;

    u_free(origen_coords[0]);
    u_free(origen_coords[1]);
    u_free(origen_coords);

    u_free(destion_coords[0]);
    u_free(destion_coords[1]);
    u_free(destion_coords);
}

private void ds_parser_iniciar_tarea(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_INICIAR_TAREA__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "INICIAR_TAREA [TID] [TAREA]");
        return;
    }

    if(!ds_is_valid_number(argv[DS_INICIAR_TAREA__TID_INDEX]))
    {
        ds_parser_tid_error(result, argv[DS_INICIAR_TAREA__TID_INDEX]);
        return;
    }

    ds_command_iniciar_tarea_t* command = u_malloc(sizeof(ds_command_iniciar_tarea_t));

    command->tid   = atoi(argv[DS_INICIAR_TAREA__TID_INDEX]);
    command->tarea = string_duplicate(argv[DS_INICIAR_TAREA__TAREA_INDEX]);
}

private void ds_parser_finalizar_tarea(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_FINALIZAR_TAREA__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "FINALIZAR_TAREA [TID] [TAREA]");
        return;
    }

    if(!ds_is_valid_number(argv[DS_FINALIZAR_TAREA__TID_INDEX]))
    {
        ds_parser_tid_error(result, argv[DS_FINALIZAR_TAREA__TID_INDEX]);
        return;
    }

    ds_command_iniciar_tarea_t* command = u_malloc(sizeof(ds_command_iniciar_tarea_t));

    command->tid   = atoi(argv[DS_FINALIZAR_TAREA__TID_INDEX]);
    command->tarea = string_duplicate(argv[DS_FINALIZAR_TAREA__TAREA_INDEX]);
}

private void ds_parser_trip_atiende_sabotaje(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_TRIP_ATIENDE_SABOTAJE__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "TRIPULANTE_ATIENDE_SABOTAJE [TID]");
        return;
    }

    if(!ds_is_valid_number(argv[DS_TRIP_ATIENDE_SABOTAJE__TID_INDEX]))
    {
        ds_parser_tid_error(result, argv[DS_TRIP_ATIENDE_SABOTAJE__TID_INDEX]);
        return;
    }

    result->data = u_malloc(sizeof(uint32_t));
    *(uint32_t*)(result->data) = atoi(argv[DS_TRIP_ATIENDE_SABOTAJE__TID_INDEX]);
}

private void ds_parser_trip_resuelve_sabotaje(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_TRIP_RESUELVE_SABOTAJE__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "TRIPULANTE_RESUELVE_SABOTAJE [TID]");
        return;
    }

    if(!ds_is_valid_number(argv[DS_TRIP_RESUELVE_SABOTAJE__TID_INDEX]))
    {
        ds_parser_tid_error(result, argv[DS_TRIP_ATIENDE_SABOTAJE__TID_INDEX]);
        return;
    }

    result->data = u_malloc(sizeof(uint32_t));
    *(uint32_t*)(result->data) = atoi(argv[DS_TRIP_RESUELVE_SABOTAJE__TID_INDEX]);
}

private void ds_parser_mover_tripulante(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_MOVER_TRIPULANTE__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "MOVER_TRIPULANTE [TID] [POS]");
        return;
    }

    if(!ds_is_valid_number(argv[DS_MOVER_TRIPULANTE__TID_INDEX]))
    {
        ds_parser_tid_error(result, argv[DS_MOVER_TRIPULANTE__TID_INDEX]);
        return;
    }

    if(!ds_is_valid_position(argv[DS_MOVER_TRIPULANTE__POS]))
    {
        ds_parser_pos_error(result, argv[DS_MOVER_TRIPULANTE__POS]);
        return;
    }

    ds_command_mover_tripulante_t* command = u_malloc(sizeof(ds_command_mover_tripulante_t));

    command->tid = atoi(argv[DS_MOVER_TRIPULANTE__TID_INDEX]);

    char** pos_coords = string_split(argv[DS_MOVER_TRIPULANTE__POS], "|");
    
    command->pos.x = atoi(pos_coords[0]);
    command->pos.y = atoi(pos_coords[1]);

    u_free(pos_coords[0]);
    u_free(pos_coords[1]);
    u_free(pos_coords);
}

private void ds_parser_proxima_tarea(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_PROXIMA_TAREA__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "PROXIMA_TAREA [TID]");
        return;
    }

    uint32_t* tid = u_malloc(sizeof(uint32_t));
    *tid = atoi(argv[DS_PROXIMA_TAREA__TID_INDEX]);

    result->data = tid;
}

__attribute__((always_inline)) inline
private bool _is_valid_state(char s)
{
    return (s == 'E' || s == 'R' || s == 'B');
}

private void ds_parser_trip_nuevo_estado(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_TRIP_NUEVO_ESTADO__CANT_ARGS)
    {
        ds_parser_not_enough_args_error(result, "TRIPULANTE_NUEVO_ESTADO [TID] [ESTADO]");
        return;
    }

    if(!_is_valid_state(argv[DS_TRIP_NUEVO_ESTADO__ESTADO_INDEX][0]))
    {
        ds_parser_write_error(result, "Se ingreso un estado invalido: %s", argv[DS_TRIP_NUEVO_ESTADO__ESTADO_INDEX]);
        return;
    }

    ds_command_tripulante_nuevo_estado_t* command =
        u_malloc(sizeof(ds_command_tripulante_nuevo_estado_t));

    command->tid    = atoi(argv[DS_TRIP_NUEVO_ESTADO__TID_INDEX]);
    command->estado = argv[DS_TRIP_NUEVO_ESTADO__ESTADO_INDEX][0];
}

#endif