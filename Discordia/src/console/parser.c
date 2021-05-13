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

private void ds_parser_iniciar_patota(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_expulsar_tripulante(uint32_t argc, char** argv, ds_parser_result_t* result);
private void ds_parser_obtener_bitacora(uint32_t argc, char** argv, ds_parser_result_t* result);

void ds_parse(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    result->command = ds_command_str_to_enum(argv[DS_COMMAND_INDEX]);

    switch(result->command)
    {
    case DS_INICIAR_PATOTA:
        ds_parser_iniciar_patota(argc, argv, result);
        break;

    case DS_EXPULSAR_TRIPULANTE:
        ds_parser_expulsar_tripulante(argc, argv, result);
        break;

    case DS_OBTENER_BITACORA:
        ds_parser_obtener_bitacora(argc, argv, result);
        break;

    case DS_SALIR:
    case DS_LISTAR_TRIPULANTES:
    case DS_INICIAR_PLANIFICACION:
    case DS_PAUSAR_PLANIFICACION:
        result->data = NULL;
        break;

    default:
        result->data = string_duplicate("Comando Invalido!");
    }
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

private void ds_parser_iniciar_patota(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_INICIAR_PATOTA__MIN_CANT_ARGS)
    {
        result->command = DS_INVALID_COMMAND;
        result->data =
            string_duplicate("Insuficiente cantidad de argumentos: INICIAR_PATOTA [CANT_TRIP] [RUTA_TAREAS] [...POS]");

        return;
    }

    if(!ds_is_valid_number(argv[DS_INICIAR_PATOTA__CANT_TRIPULANTES_INDEX]))
    {
        result->command = DS_INVALID_COMMAND;
        result->data =
            string_from_format(
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
                result->command = DS_INVALID_COMMAND;
                result->data    = string_from_format("Formato de posicion invalida: %s", argv[i]);

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
        result->command = DS_INVALID_COMMAND;
        result->data =
            string_duplicate("Insuficiente cantidad de argumentos: EXPULSAR_TRIPULANTE [TID]");
    }
    else if(!ds_is_valid_number(argv[DS_EXPULSAR_TRIPULANTE__TID_INDEX]))
    {
        result->command = DS_INVALID_COMMAND;
        result->data =
            string_from_format(
                "TID ingresado invalido!: %s. Ingrese un numero valido",
                argv[DS_EXPULSAR_TRIPULANTE__TID_INDEX]);
    }
    else
    {
        uint32_t* data = u_malloc(sizeof(uint32_t));
        *data = atoi(argv[DS_EXPULSAR_TRIPULANTE__TID_INDEX]);

        result->data = data;
    }
}

private void ds_parser_obtener_bitacora(uint32_t argc, char** argv, ds_parser_result_t* result)
{
    if(argc < DS_OBTENER_BITACORA__CANT_ARGS)
    {
        result->command = DS_INVALID_COMMAND;
        result->data =
            string_duplicate("Insuficiente cantidad de argumentos: OBTENER_BITACORA [TID]");
    }
    else if(!ds_is_valid_number(argv[DS_OBTENER_BITACORA__TID_INDEX]))
    {
        result->command = DS_INVALID_COMMAND;
        result->data =
            string_from_format(
                "TID ingresado invalido!: %s. Ingrese un numero valido",
                argv[DS_EXPULSAR_TRIPULANTE__TID_INDEX]);
    }
    else
    {
        uint32_t* data = u_malloc(sizeof(uint32_t));
        *data = atoi(argv[DS_OBTENER_BITACORA__TID_INDEX]);

        result->data = data;
    }
}
