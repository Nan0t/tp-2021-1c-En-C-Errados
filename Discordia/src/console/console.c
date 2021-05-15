#include "discordia/discordia.h"

#include "console.h"
#include "parser.h"
#include "print.h"

#include <utils/console/console.h>

private uint32_t ds_calculate_args_count(char** args);
private bool     ds_check_result(const ds_parser_result_t* result);
private void     ds_delete_args(char** args);

private bool ds_error(const ds_parser_result_t* result);
private bool ds_salir(const ds_parser_result_t* result);
private bool ds_iniciar_patota(const ds_parser_result_t* result);
private bool ds_lista_tripulantes(const ds_parser_result_t* result);
private bool ds_expulsar_tripulante(const ds_parser_result_t* result);
private bool ds_iniciar_planificacion(const ds_parser_result_t* result);
private bool ds_pausar_planificacion(const ds_parser_result_t* result);
private bool ds_obtener_bitacora(const ds_parser_result_t* result);

typedef bool(*ds_command_func_t)(const ds_parser_result_t*);

private ds_command_func_t COMMAND_FUNCS_ARRAY[] =
{
    ds_error,
    ds_salir,
    ds_iniciar_patota,
    ds_lista_tripulantes,
    ds_expulsar_tripulante,
    ds_iniciar_planificacion,
    ds_pausar_planificacion,
    ds_obtener_bitacora
};

private const char* COMMANDS[] =
{
    "SALIR",
    "INICIAR_PATOTA",
    "LISTAR_TRIPULANTES",
    "EXPULSAR_TRIPULANTE",
    "INICIAR_PLANIFICACION",
    "PAUSAR_PLANIFICACION",
    "OBTENER_BITACORA"
};

void ds_console_init(void)
{
    char* input = NULL;
    ds_parser_result_t result = { 0 };

    ds_print("Bienvenido a la consola de Discordia.\n");
    ds_print("Para salir: Ctrl + C\n\n");

    bool should_exit = false;

    u_console_set_commands(COMMANDS);

    while(!should_exit)
    {
        input = u_console_read("[Discordia]:> ");

        char**   args = string_split(input, " ");
        uint32_t argc = ds_calculate_args_count(args);

        ds_parse(argc, args, &result);
        should_exit = ds_check_result(&result);

        ds_delete_args(args);
        ds_parser_result_free(&result);
    }
}

private uint32_t ds_calculate_args_count(char** args)
{
    uint32_t count = 0;
    for(char** arg = args; *arg != NULL; arg ++, count ++);

    return count;
}

private bool ds_check_result(const ds_parser_result_t* result)
{
    return COMMAND_FUNCS_ARRAY[(int)(result->command) + 1](result);
}

private void ds_delete_args(char** args)
{
    for(char** arg = args; *arg != NULL; arg ++)
        u_free(*arg);

    u_free(args);
}

private bool ds_error(const ds_parser_result_t* result)
{
    ds_print("Parser error: %s\n", (char*)result->data);
    return false;
}

private bool ds_salir(const ds_parser_result_t* result)
{
    (void)result;
    return true;
}

private bool ds_iniciar_patota(const ds_parser_result_t* result)
{
    ds_command_iniciar_patota_t* command_result =
        (ds_command_iniciar_patota_t*)result->data;

    discordia_inicializar_patota(
        command_result->ruta_tareas,
        command_result->cant_trip,
        command_result->trip_positions);

    return false;
}

private bool ds_lista_tripulantes(const ds_parser_result_t* result)
{
    (void)result;
    char* lista_tripulantes = discordia_obtener_tripulantes();

    if(lista_tripulantes == NULL)
        ds_print("No hay tripulantes disponibles en el sistema\n");
    else
    {
        ds_print("Tripulantes: { %s }\n", lista_tripulantes);
        u_free(lista_tripulantes);
    }

    return false;
}

private bool ds_expulsar_tripulante(const ds_parser_result_t* result)
{
    discordia_expulsar_tripulante(*((uint32_t*)result->data));
    return false;
}

private bool ds_iniciar_planificacion(const ds_parser_result_t* result)
{
    (void)result;
    discordia_iniciar_planificacion();
    return false;
}

private bool ds_pausar_planificacion(const ds_parser_result_t* result)
{
    (void)result;
    discordia_pausar_planificacion();
    return false;
}

private bool ds_obtener_bitacora(const ds_parser_result_t* result)
{
    uint32_t tid = *((uint32_t*)result->data);
    char* bitacora = discordia_obtener_bitacora(tid);

    if(bitacora == NULL)
        ds_print("No existe bitacora para el tripulante %d\n", tid);
    else
    {
        ds_print("%s\n", bitacora);
        u_free(bitacora);
    }

    return false;
}
