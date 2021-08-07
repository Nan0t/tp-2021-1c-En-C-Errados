#include "utils/config_test.h"
#include "utils/msg_test.h"

#include "discordia/parser_test.h"
#include "discordia/tripulante_test.h"

#include "fs/block_manager_test.h"
#include "fs/block_tests.h"
#include "fs/file_test.h"
#include "fs/bitacora_test.h"
#include "fs/task_parser_test.h"

#include <CUnit/Basic.h>

#define FUNCTION_TEST(f) { "\033[93m"#f"\033[0m", f }
#define ARRAY_LENGTH(array)	(sizeof((array)) / sizeof(*(array)))
#define ADD_TEST_TO_TEST_SUITE(test_suite, test_functions)\
	for(unsigned long i = 0; i < ARRAY_LENGTH(test_functions); i++)\
		CU_add_test(test_suite, test_functions[i].testName, test_functions[i].test);

typedef struct {
	const char* testName;
	void(*test)(void);
} test_case_t;

static void utils_tests(void);
static void discordia_tests(void);
static void tripulante_test(void);
static void fs_tests(void);

int main(int argc, char** argv)
{
    CU_initialize_registry();
    CU_basic_set_mode(CU_BRM_VERBOSE);

    if(argc < 2)
    {
        utils_tests();
        discordia_tests();
        tripulante_test();
        fs_tests();
    }
    else
    {
        if(!strcmp(argv[1], "utils"))
            utils_tests();
        else if(!strcmp(argv[1], "discordia"))
            discordia_tests();
        else if(!strcmp(argv[1], "fs"))
            fs_tests();
    }

    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}

void utils_tests(void)
{
    CU_pSuite config_test_suite = CU_add_suite_with_setup_and_teardown(
        "Utils Config Test Suite",
        NULL,
        NULL,
        test_utils_config_setup,
        test_utils_config_tear_down);

    test_case_t config_test_cases[] =
    {
        FUNCTION_TEST(test_utils_config_get_string_value),
        FUNCTION_TEST(test_utils_config_get_array_value),
        FUNCTION_TEST(test_utils_config_get_int_value),
        FUNCTION_TEST(test_utils_config_get_float_value),
        FUNCTION_TEST(test_utils_config_get_double_value)
    };

    ADD_TEST_TO_TEST_SUITE(config_test_suite, config_test_cases)

    CU_pSuite msg_test_suite = CU_add_suite_with_setup_and_teardown(
        "Utils Message Serialization And Deserialization Test Suite",
        NULL,
        NULL,
        NULL,
        NULL
    );

    test_case_t msg_test_cases[] =
    {
        FUNCTION_TEST(test_atiende_sabotaje_serializar_y_deserializar),
		FUNCTION_TEST(test_bitacora_serializar_y_deserializar),
        FUNCTION_TEST(test_desplazamiento_tripulante_serializar_y_deserializar),
        FUNCTION_TEST(test_eliminar_tripulante_serializar_y_deserializar),
        FUNCTION_TEST(test_finalizacion_tripulante_serializar_y_deserializar),
        FUNCTION_TEST(test_informar_sabotaje_serializar_y_deserializar),
        FUNCTION_TEST(test_iniciar_patota_serializar_y_deserializar),
        FUNCTION_TEST(test_iniciar_tripulante_serializar_y_deserializar),
        FUNCTION_TEST(test_iniciar_tarea_serializar_y_deserializar),
        FUNCTION_TEST(test_movimiento_tripulante_serializar_y_deserializar),
        FUNCTION_TEST(test_obtener_bitacora_serializar_y_deserializar),
        FUNCTION_TEST(test_proxima_tarea_serializar_y_deserializar),
        FUNCTION_TEST(test_resuelve_sabotaje_serializar_y_deserializar),
        FUNCTION_TEST(test_tripulante_nuevo_estado_serializar_y_deserializar)
    };

    ADD_TEST_TO_TEST_SUITE(msg_test_suite, msg_test_cases)
}

static void discordia_tests(void)
{
    CU_pSuite parser_test_suite = CU_add_suite_with_setup_and_teardown(
        "Discordia Praser Test Suite",
        NULL,
        NULL,
        NULL,
        NULL
    );

    test_case_t parser_test_cases[] =
    {
        FUNCTION_TEST(test_invalid_cant_args_iniciar_patota),
        FUNCTION_TEST(test_invalid_cant_trips_iniciar_patota),
        FUNCTION_TEST(test_invalid_positions_iniciar_patota),
        FUNCTION_TEST(test_valid_iniciar_patota_sin_posiciones),
        FUNCTION_TEST(test_valid_iniciar_patota_con_posiciones),

        FUNCTION_TEST(test_invalid_cant_args_expulsar_tripulante),
        FUNCTION_TEST(test_invalid_expulsar_tripulante),
        FUNCTION_TEST(test_valid_expulsar_tripulante),

        FUNCTION_TEST(test_invalid_cant_args_obtener_bitacora),
        FUNCTION_TEST(test_invalid_obtener_bitacora),
        FUNCTION_TEST(test_valid_obtener_bitacora),

        FUNCTION_TEST(test_valid_listar_tripulantes),
        FUNCTION_TEST(test_valid_iniciar_planificacion),
        FUNCTION_TEST(test_valid_pausar_planificacion)
    };

    ADD_TEST_TO_TEST_SUITE(parser_test_suite, parser_test_cases)
}


static void tripulante_test(void)
{
    CU_pSuite parser_test_suite = CU_add_suite_with_setup_and_teardown(
        "Discordia Tripulante Test Suite",
        NULL,
        NULL,
        NULL,
        NULL
    );
    test_case_t parser_test_cases[] =
    {
        FUNCTION_TEST(test_valid_parsear_tarea_con_parametro_bloqueante),
        FUNCTION_TEST(test_valid_parsear_tarea_sin_parametro_bloqueante),
        FUNCTION_TEST(test_valid_parsear_tarea_sin_parametro_no_bloqueante)

    };

    ADD_TEST_TO_TEST_SUITE(parser_test_suite, parser_test_cases)
}

static void fs_tests(void)
{
    CU_pSuite block_manager_test_suite = CU_add_suite_with_setup_and_teardown(
        "I-Mongo-Store BlockManager Test Suite",
        NULL,
        NULL,
        test_fs_block_manager_setup,
        test_fs_block_manager_tear_down
    );

    test_case_t block_manager_test_cases[] =
    {
        FUNCTION_TEST(test_fs_block_manager_request_and_release_blocks),
        FUNCTION_TEST(test_fs_block_manager_request_and_get_a_intermidiate_block),
        FUNCTION_TEST(test_fs_block_manager_request_and_reject)
    };

    ADD_TEST_TO_TEST_SUITE(block_manager_test_suite, block_manager_test_cases)

    CU_pSuite block_test_suite = CU_add_suite_with_setup_and_teardown(
        "I-Mongo-Store Block Test Suite",
        NULL,
        NULL,
        test_block_setup,
        test_block_tear_down
    );

    test_case_t block_test_cases[] =
    {
        FUNCTION_TEST(test_block_write_with_no_overflow),
        FUNCTION_TEST(test_block_write_with_overflow),
        FUNCTION_TEST(test_block_write_in_non_contiguous_blocks),
        FUNCTION_TEST(test_block_read_with_no_overflow),
        FUNCTION_TEST(test_block_read_with_overflow),
        FUNCTION_TEST(test_block_read_in_non_contiguous_blocks)
    };

    ADD_TEST_TO_TEST_SUITE(block_test_suite, block_test_cases)

    CU_pSuite file_test_suite = CU_add_suite_with_setup_and_teardown(
        "I-Mongo-Store File Test Suite",
        NULL,
        NULL,
        test_file_setup,
        test_file_tear_down
    );

    test_case_t file_test_cases[] =
    {
        FUNCTION_TEST(test_file_create),
        FUNCTION_TEST(test_file_add_fill_char_with_no_overflow),
        FUNCTION_TEST(test_file_add_fill_char_with_overflow),
        FUNCTION_TEST(test_file_remove_fill_char_with_no_shrink),
        FUNCTION_TEST(test_file_remove_fill_char_with_shrink)
    };

    ADD_TEST_TO_TEST_SUITE(file_test_suite, file_test_cases)

    CU_pSuite bitacora_test_suite = CU_add_suite_with_setup_and_teardown(
        "I-Mongo-Store Bitacora Test Suite",
        NULL,
        NULL,
        test_bitacora_setup,
        test_bitacora_tear_down
    );

    test_case_t bitacora_test_cases[] =
    {
        FUNCTION_TEST(test_bitacora_create),
        FUNCTION_TEST(test_bitacora_add_content_with_no_overflow),
        FUNCTION_TEST(test_bitacora_add_content_with_overflow)
    };

    ADD_TEST_TO_TEST_SUITE(bitacora_test_suite, bitacora_test_cases)

    CU_pSuite task_parser_test_suite = CU_add_suite_with_setup_and_teardown(
        "I-Mongo-Store Task Parser Test Suite",
        NULL,
        NULL,
        NULL,
        NULL
    );

    test_case_t task_parser_test_cases[] =
    {
        FUNCTION_TEST(test_task_parser_unknown_task),
        FUNCTION_TEST(test_task_parser_generar_oxigeno),
        FUNCTION_TEST(test_task_parser_consumir_oxigeno),
        FUNCTION_TEST(test_task_parser_generar_comida),
        FUNCTION_TEST(test_task_parser_consumir_comida),
        FUNCTION_TEST(test_task_parser_generar_basura),
        FUNCTION_TEST(test_task_parser_descartar_basura)
    };

    ADD_TEST_TO_TEST_SUITE(task_parser_test_suite, task_parser_test_cases)
}