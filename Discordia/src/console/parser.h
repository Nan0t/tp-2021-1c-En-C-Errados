#ifndef DISCORDIA_PARSER_H
#define DISCORDIA_PARSER_H

#include <utils/utils.h>

/**
 * @NAME: ds_parser_command_e
 * @DESC: posibles commandos devueltos por el parser de la consola.
 */
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
} ds_parser_command_e;

/**
 * @NAME: ds_parser_result_t
 * @DESC: estructura que contendra información sobre un comando ingresado por la consola.
 * @ATTR:
 *  ds_parser_command_e command - tipo de comando ingresado.
 *  void*               data    - informacion extra que dependera del comando obtenido.
 */
typedef struct
{
    ds_parser_command_e command;
    void*               data;
} ds_parser_result_t;

/**
 * @NAME: ds_command_iniciar_patota_t
 * @DESC: estructura que contiene informacion sobre un comando INICIAR_PATOTA
 * @ATTR:
 *  uint32_t    cant_trip      - cantidad de tripulantes especificado.
 *  const char* ruta_tareas    - ruta al archivo de tareas de la patota.
 *  t_list*     trip_positions - lista de estructuras u_pos_t que contiene las posiciones iniciales de los tripulantes
 */
typedef struct
{
    uint32_t     cant_trip;
    const char*  ruta_tareas;
    t_list*      trip_positions;
} ds_command_iniciar_patota_t;

/**
 * @NAME: ds_parse
 * @DESC: parsea un comando ingresado por consola.
 * @PARAMS:
 *  [in] const char*          command - string del comando ingresado.
 *  [out] ds_parser_result_t* result  - estructura donde se almacenara el resultado del parser.
 */
void ds_parse(const char* command, ds_parser_result_t* result);

/**
 * @NAME: ds_parser_result_free
 * @DESC: libera la memoria utilizada por el resultado del parser.
 * @PARAMS:
 *  [in] ds_parser_result_t* result - estructura a liberar.
 */
void ds_parser_result_free(ds_parser_result_t* result);

#endif