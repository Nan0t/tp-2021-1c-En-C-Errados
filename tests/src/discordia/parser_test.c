#include "parser_test.h"

#include <CUnit/Basic.h>
#include <console/parser.h>

#define ARRAY_LENGTH(array) (sizeof((array)) / sizeof(*(array)))

void test_invalid_cant_args_iniciar_patota(void)
{
// Arrange
//--------
    char* args[]  = { "INICIAR_PATOTA", "5" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL(result.command, DS_INVALID_COMMAND);
    CU_ASSERT_STRING_EQUAL(
        result.data,
        "Cantidad insuficiente de argumentos: INICIAR_PATOTA [CANT_TRIP] [RUTA_TAREAS] [...POS]");

    ds_parser_result_free(&result);
}

void test_invalid_cant_trips_iniciar_patota(void)
{
// Arrange
//--------
    char* args[]  = { "INICIAR_PATOTA", "Cinco", "Ruta/A/Tareas", "3|3", "4|3" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_INVALID_COMMAND);
    CU_ASSERT_STRING_EQUAL(
        result.data,
        "Cantidad de tripulantes invalida!: Cinco. Ingrese un valor numerico valido");

    ds_parser_result_free(&result);
}

void test_invalid_positions_iniciar_patota(void)
{
// Arrange
//--------
    char* invalid_args1[]  = { "INICIAR_PATOTA", "5", "Ruta/A/Tareas", "3;3", "4|3" };
    uint32_t invalid_argc1 = ARRAY_LENGTH(invalid_args1);

    char* invalid_args2[]  = { "INICIAR_PATOTA", "5", "Ruta/A/Tareas", "|3|3", "4|3" };
    uint32_t invalid_argc2 = ARRAY_LENGTH(invalid_args1);

    ds_parser_result_t result1 = { 0 };
    ds_parser_result_t result2 = { 0 };

// Act
//----
    ds_parse(invalid_argc1, invalid_args1, &result1);
    ds_parse(invalid_argc2, invalid_args2, &result2);

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(result1.command, DS_INVALID_COMMAND);
    CU_ASSERT_STRING_EQUAL(
        result1.data,
        "Formato de posicion invalida: 3;3");

    CU_ASSERT_EQUAL(result2.command, DS_INVALID_COMMAND);
    CU_ASSERT_STRING_EQUAL(
        result2.data,
        "Formato de posicion invalida: |3|3");

    ds_parser_result_free(&result1);
    ds_parser_result_free(&result2);
}

void test_valid_iniciar_patota_sin_posiciones(void)
{
// Arrange
//--------
    char* args[]  = { "INICIAR_PATOTA", "10", "Ruta/A/Tareas" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_INICIAR_PATOTA);

    ds_command_iniciar_patota_t* command_result =
        (ds_command_iniciar_patota_t*)result.data;

    CU_ASSERT_EQUAL(command_result->cant_trip, 10);
    CU_ASSERT_STRING_EQUAL(command_result->ruta_tareas, "Ruta/A/Tareas");
    CU_ASSERT_PTR_NULL(command_result->trip_positions);

    ds_parser_result_free(&result);
}

void test_valid_iniciar_patota_con_posiciones(void)
{
// Arrange
//--------
    char* args[]  = { "INICIAR_PATOTA", "10", "Ruta/A/Tareas", "3|3", "3|4", "5|10" };
    uint32_t argc = ARRAY_LENGTH(args);

    const u_pos_t expected_positions[] =
    {
        { 3, 3 },
        { 3, 4 },
        { 5, 10 }
    };

    uint32_t i = 0;

    bool _pos_are_equals(const u_pos_t* pos) {
        return pos->x == expected_positions[i].x &&
                pos->y == expected_positions[i ++].y;
    };

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert.
//--------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_INICIAR_PATOTA);

    ds_command_iniciar_patota_t* command_result =
        (ds_command_iniciar_patota_t*)result.data;

    CU_ASSERT_EQUAL(command_result->cant_trip, 10);
    CU_ASSERT_STRING_EQUAL(command_result->ruta_tareas, "Ruta/A/Tareas");
    CU_ASSERT_TRUE(list_all_satisfy(command_result->trip_positions, (void*)_pos_are_equals));

    ds_parser_result_free(&result);
}

void test_invalid_cant_args_expulsar_tripulante(void)
{
// Arrange
//--------
    char* args[]  = { "EXPULSAR_TRIPULANTE" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_INVALID_COMMAND);
    CU_ASSERT_STRING_EQUAL(
        result.data,
        "Cantidad insuficiente de argumentos: EXPULSAR_TRIPULANTE [TID]");

    ds_parser_result_free(&result);
}

void test_invalid_expulsar_tripulante(void)
{
// Arrange
//--------
    char* args[]  = { "EXPULSAR_TRIPULANTE", "UNO" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_INVALID_COMMAND);
    CU_ASSERT_STRING_EQUAL(
        result.data,
        "TID ingresado invalido!: UNO. Ingrese un numero valido");

    ds_parser_result_free(&result);
}

void test_valid_expulsar_tripulante(void)
{
// Arrange
//--------
    char* args[]  = { "EXPULSAR_TRIPULANTE", "1" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_EXPULSAR_TRIPULANTE);
    CU_ASSERT_EQUAL(*((uint32_t*)result.data), 1);

    ds_parser_result_free(&result);
}

void test_invalid_cant_args_obtener_bitacora(void)
{
// Arrange
//--------
    char* args[]  = { "OBTENER_BITACORA" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_INVALID_COMMAND);
    CU_ASSERT_STRING_EQUAL(
        result.data,
        "Cantidad insuficiente de argumentos: OBTENER_BITACORA [TID]");

    ds_parser_result_free(&result);
}

void test_invalid_obtener_bitacora(void)
{
// Arrange
//--------
    char* args[]  = { "OBTENER_BITACORA", "UNO" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_INVALID_COMMAND);
    CU_ASSERT_STRING_EQUAL(
        result.data,
        "TID ingresado invalido!: UNO. Ingrese un numero valido");

    ds_parser_result_free(&result);
}

void test_valid_obtener_bitacora(void)
{
// Arrange
//--------
    char* args[]  = { "OBTENER_BITACORA", "1" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_OBTENER_BITACORA);
    CU_ASSERT_EQUAL(*((uint32_t*)result.data), 1);

    ds_parser_result_free(&result);
}

void test_valid_listar_tripulantes(void)
{
// Arrange
//--------
    char* args[]  = { "LISTAR_TRIPULANTES" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_LISTAR_TRIPULANTES);

    ds_parser_result_free(&result);
}
void test_valid_iniciar_planificacion(void)
{
// Arrange
//--------
    char* args[]  = { "INICIAR_PLANIFICACION" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_INICIAR_PLANIFICACION);

    ds_parser_result_free(&result);
}
void test_valid_pausar_planificacion(void)
{
// Arrange
//--------
    char* args[]  = { "PAUSAR_PLANIFICACION" };
    uint32_t argc = ARRAY_LENGTH(args);

    ds_parser_result_t result = { 0 };

// Act
//----
    ds_parse(argc, args, &result);

// Assert
//-------
    CU_ASSERT_EQUAL_FATAL(result.command, DS_PAUSAR_PLANIFICACION);

    ds_parser_result_free(&result);
}