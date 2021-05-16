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

#ifndef NDEBUG
// Mensajes de prueba para el FS
private bool _debug_ds_desplazamiento_tripulante(const ds_parser_result_t* result);
private bool _debug_ds_iniciar_tarea(const ds_parser_result_t* result);
private bool _debug_ds_finalizar_tarea(const ds_parser_result_t* result);
private bool _debug_ds_tripulante_atiende_sabotaje(const ds_parser_result_t* result);
private bool _debug_ds_tripulante_resuelve_sabotaje(const ds_parser_result_t* result);

// Mensajes de prueba para la Memoria.
private bool _debug_ds_mover_tripulante(const ds_parser_result_t* result);
private bool _debug_ds_proxima_tarea(const ds_parser_result_t* result);
private bool _debug_ds_tripulante_nuevo_estado(const ds_parser_result_t* result);
#endif

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
    ds_obtener_bitacora,
#ifndef NDEBUG
    _debug_ds_desplazamiento_tripulante,
    _debug_ds_iniciar_tarea,
    _debug_ds_finalizar_tarea,
    _debug_ds_tripulante_atiende_sabotaje,
    _debug_ds_tripulante_resuelve_sabotaje,
    _debug_ds_mover_tripulante,
    _debug_ds_proxima_tarea,
    _debug_ds_tripulante_nuevo_estado
#endif
};

private const char* COMMANDS[] =
{
    "SALIR",
    "INICIAR_PATOTA",
    "LISTAR_TRIPULANTES",
    "EXPULSAR_TRIPULANTE",
    "INICIAR_PLANIFICACION",
    "PAUSAR_PLANIFICACION",
    "OBTENER_BITACORA",
#ifndef NDEBUG
    "DESPLAZAMIENTO_TRIPULANTE",
    "INICIAR_TAREA",
    "FINALIZAR_TAREA",
    "TRIPULANTE_ATIENDE_SABOTAJE",
    "TRIPULANTE_RESUELVE_SABOTAJE",
    "MOVER_TRIPULANTE",
    "PROXIMA_TAREA",
    "TRIPULANTE_NUEVO_ESTADO"
#endif
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

#ifndef NDEBUG

// Mensajes de prueba para el FS
private bool _debug_ds_desplazamiento_tripulante(const ds_parser_result_t* result)
{
    ds_command_desplazamiento_tripulante_t* command =
        (ds_command_desplazamiento_tripulante_t*)result->data;

    discordia_desplazamiento_tripulante(command->tid, &command->origen, &command->destion);

    return false;
}

private bool _debug_ds_iniciar_tarea(const ds_parser_result_t* result)
{
    ds_command_iniciar_tarea_t* command =
        (ds_command_iniciar_tarea_t*)result->data;

    discordia_iniciar_tarea(command->tid, command->tarea);

    return false;
}

private bool _debug_ds_finalizar_tarea(const ds_parser_result_t* result)
{
    ds_command_finalizar_tarea_t* command =
        (ds_command_finalizar_tarea_t*)result->data;

    discordia_finalizar_tarea(command->tid, command->tarea);

    return false;
}

private bool _debug_ds_tripulante_atiende_sabotaje(const ds_parser_result_t* result)
{
    uint32_t* tid = (uint32_t*)result->data;
    discordia_tripulante_atiende_sabotaje(*tid);

    return false;
}

private bool _debug_ds_tripulante_resuelve_sabotaje(const ds_parser_result_t* result)
{
    uint32_t* tid = (uint32_t*)result->data;
    discordia_tripulante_resuelve_sabotaje(*tid);

    return false;
}

// Mensajes de prueba para la Memoria.
private bool _debug_ds_mover_tripulante(const ds_parser_result_t* result)
{
    ds_command_mover_tripulante_t* command =
        (ds_command_mover_tripulante_t*)result->data;

    discordia_mover_tripulante(command->tid, &command->pos);

    return false;
}

private bool _debug_ds_proxima_tarea(const ds_parser_result_t* result)
{
    uint32_t* tid = (uint32_t*)result->data;
    char* tarea = discordia_obtener_proxima_tarea(*tid);

    if(tarea)
    {
        ds_print("Tarea obtenida del tripulante %d: %s\n", *tid, tarea);
        u_free(tarea);
    }
    else
        ds_print("No se pudo obtener ninguna tarea para el tripulante %d\n", *tid);

    return false;
}

private bool _debug_ds_tripulante_nuevo_estado(const ds_parser_result_t* result)
{
    ds_command_tripulante_nuevo_estado_t* command =
        (ds_command_tripulante_nuevo_estado_t*)result->data;

    discordia_tripulante_nuevo_estado(command->tid, command->estado);
    return false;
}

#endif