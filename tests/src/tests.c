#include "utils/config_test.h"
#include "utils/msg_test.h"
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

int main(void)
{
    CU_initialize_registry();
    CU_basic_set_mode(CU_BRM_VERBOSE);

    utils_tests();

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
        FUNCTION_TEST(test_iniciar_tripulante_serializar_y_deserializar),
        FUNCTION_TEST(test_iniciar_tarea_serializar_y_deserializar),
        FUNCTION_TEST(test_obtener_bitacora_serializar_y_deserializar),
        FUNCTION_TEST(test_proxima_tarea_serializar_y_deserializar),
        FUNCTION_TEST(test_resuelve_sabotaje_serializar_y_deserializar),
        FUNCTION_TEST(test_tripulante_nuevo_estado_serializar_y_deserializar)
    };

    ADD_TEST_TO_TEST_SUITE(msg_test_suite, msg_test_cases)
}
