#include "discordia/discordia.h"

#include "console.h"
#include "parser.h"
#include "print.h"

#include <utils/console/console.h>

private uint32_t ds_calculate_args_count(char** args);
private void     ds_check_result(const ds_parser_result_t* result);
private void     ds_delete_args(char** args);

void ds_console_init(void)
{
    char* input = NULL;
    ds_parser_result_t result = { 0 };

    ds_print("Bienvenido a la consola de Discordia.\n");
    ds_print("Para salir: Ctrl + C\n\n");

    while(1)
    {
        input = u_console_read("[Discordia]:> ");

        char**   args = string_split(input, " ");
        uint32_t argc = ds_calculate_args_count(args);

        ds_parse(argc, args, &result);

        ds_check_result(&result);

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

private void ds_check_result(const ds_parser_result_t* result)
{
    switch(result->command)
    {
        case DS_INICIAR_PATOTA:
        {
            ds_command_iniciar_patota_t* command_result =
                (ds_command_iniciar_patota_t*)result->data;

            discordia_inicializar_patota(
                command_result->ruta_tareas,
                command_result->cant_trip,
                command_result->trip_positions);

            break;
        }
        case DS_LISTAR_TRIPULANTES:
        {
            char* lista_tripulantes = discordia_obtener_tripulantes();

            if(lista_tripulantes == NULL)
                ds_print("No hay tripulantes disponibles en el sistema\n");
            else
            {
                ds_print("Tripulantes: { %s }\n", lista_tripulantes);
                u_free(lista_tripulantes);
            }

            break;
        }
        case DS_EXPULSAR_TRIPULANTE:
            discordia_expulsar_tripulante(*((uint32_t*)result->data));
            break;

        case DS_INICIAR_PLANIFICACION:
            discordia_iniciar_planificacion();
            break;

        case DS_PAUSAR_PLANIFICACION:
            discordia_pausar_planificacion();
            break;

        case DS_OBTENER_BITACORA:
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

            break;
        }
        default:
            ds_print("Parser error: %s\n", (char*)result->data);
    }
}

private void ds_delete_args(char** args)
{
    for(char** arg = args; *arg != NULL; arg ++)
        u_free(*arg);

    u_free(args);
}