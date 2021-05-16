#ifndef TESTS_DISCORDIA_PARSER_TEST_H
#define TESTS_DISCORDIA_PARSER_TEST_H

void test_invalid_cant_args_iniciar_patota(void);
void test_invalid_cant_trips_iniciar_patota(void);
void test_invalid_positions_iniciar_patota(void);
void test_valid_iniciar_patota_sin_posiciones(void);
void test_valid_iniciar_patota_con_posiciones(void);

void test_invalid_cant_args_expulsar_tripulante(void);
void test_invalid_expulsar_tripulante(void);
void test_valid_expulsar_tripulante(void);

void test_invalid_cant_args_obtener_bitacora(void);
void test_invalid_obtener_bitacora(void);
void test_valid_obtener_bitacora(void);

void test_valid_listar_tripulantes(void);
void test_valid_iniciar_planificacion(void);
void test_valid_pausar_planificacion(void);

#endif