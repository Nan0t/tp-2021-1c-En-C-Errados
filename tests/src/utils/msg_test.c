#include "msg_test.h"
#include <CUnit/Basic.h>
#include <utils/utils.h>

#define ARRAY_LENGTH(array) (sizeof((array)) / sizeof((*array)))

void test_atiende_sabotaje_serializar_y_deserializar(void)
{
// Arrange
//--------
    u_msg_atender_sabotaje_t* ser_msg   = NULL;
    u_msg_atender_sabotaje_t* deser_msg = NULL;
    u_paquete_t* paquete;
// Act
//----
    ser_msg = u_msg_atiende_sabotaje_crear(10);
    paquete = u_msg_atiende_sabotaje_serializar(ser_msg);
    deser_msg = u_msg_atiende_sabotaje_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);

    u_msg_atiende_sabotaje_eliminar(ser_msg);
    u_msg_atiende_sabotaje_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_bitacora_serializar_y_deserializar(void)
{
// Arrange
//--------
	u_trip_movimiento_t expected_movs[] =
	{
		{
			{ 3, 4 },
			{ 4, 5 },
		},
		{
			{ 5, 6 },
			{ 6, 7 },
		},
		{
			{ 7, 8 },
			{ 8, 9 },
		},
		{
			{ 9, 10 },
			{ 10, 11 },
		}
	};

	const char* expected_tareas_asignadas[] =
	{
		"Tarea1",
		"Tarea2",
		"Tarea3",
		"Tarea4",
	};

	const char* expected_tareas_finalizadas[] =
	{
		"Tarea1",
		"Tarea2",
		"Tarea3",
	};

	const char* expected_sabotajes_asignadas[] =
	{
		"Sabotaje1",
		"Sabotaje2",
		"Sabotaje3",
		"Sabotaje4",
	};

	const char* expected_sabotajes_resueltos[] =
	{
		"Sabotaje1",
		"Sabotaje2",
		"Sabotaje3",
	};

	u_msg_bitacora_t* ser_msg = NULL;
	u_msg_bitacora_t* deser_msg = NULL;
	u_paquete_t* paquete = NULL;

// Act
//----
	ser_msg = u_msg_bitacora_crear();

	for(uint32_t i = 0; i < ARRAY_LENGTH(expected_movs); i ++)
		u_msg_bitacora_agregar_movimiento(ser_msg, expected_movs[i]);

	for(uint32_t i = 0; i < ARRAY_LENGTH(expected_tareas_asignadas); i ++)
		u_msg_bitacora_agregar_tarea_asignada(ser_msg, expected_tareas_asignadas[i]);

	for(uint32_t i = 0; i < ARRAY_LENGTH(expected_tareas_finalizadas); i ++)
		u_msg_bitacora_agregar_tarea_finalizadas(ser_msg, expected_tareas_finalizadas[i]);

	for(uint32_t i = 0; i < ARRAY_LENGTH(expected_sabotajes_asignadas); i ++)
		u_msg_bitacora_agregar_sabotaje_asignado(ser_msg, expected_sabotajes_asignadas[i]);

	for(uint32_t i = 0; i < ARRAY_LENGTH(expected_sabotajes_resueltos); i ++)
		u_msg_bitacora_agregar_sabotaje_resuelto(ser_msg, expected_sabotajes_resueltos[i]);

	paquete = u_msg_bitacora_serializar(ser_msg);
	deser_msg = u_msg_bitacora_deserializar(paquete->buffer);

// Assert
//-------
	uint32_t i;

	bool _comparar_movimiento(const u_trip_movimiento_t* mov) {
		return mov->inicio.x == expected_movs[i].inicio.x &&
				mov->inicio.y == expected_movs[i].inicio.y &&
				mov->dest.x == expected_movs[i].dest.x &&
				mov->dest.y == expected_movs[i ++].dest.y;
	}

	bool _comparar_tarea_asignada(const char* tarea_asignada) {
		return !strcmp(tarea_asignada, expected_tareas_asignadas[i ++]);
	}

	bool _comparar_tarea_finalizada(const char* tarea_finalizada) {
		return !strcmp(tarea_finalizada, expected_tareas_finalizadas[i ++]);
	}

	bool _comparar_sabotaje_asignada(const char* sabotaje_asignado) {
		return !strcmp(sabotaje_asignado, expected_sabotajes_asignadas[i ++]);
	}

	bool _comparar_sabotaje_resuelto(const char* sabotaje_realizado) {
		return !strcmp(sabotaje_realizado, expected_sabotajes_resueltos[i ++]);
	}

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(ser_msg->movimientos, (void*)_comparar_movimiento));

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(ser_msg->tareas_asignadas, (void*)_comparar_tarea_asignada));

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(ser_msg->tareas_finalizadas, (void*)_comparar_tarea_finalizada));

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(ser_msg->sabotajes_asignados, (void*)_comparar_sabotaje_asignada));

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(ser_msg->sabotajes_resueltos, (void*)_comparar_sabotaje_resuelto));

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(deser_msg->movimientos, (void*)_comparar_movimiento));

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(deser_msg->tareas_asignadas, (void*)_comparar_tarea_asignada));

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(deser_msg->tareas_finalizadas, (void*)_comparar_tarea_finalizada));

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(deser_msg->sabotajes_asignados, (void*)_comparar_sabotaje_asignada));

	i = 0;
	CU_ASSERT_TRUE(list_all_satisfy(deser_msg->sabotajes_resueltos, (void*)_comparar_sabotaje_resuelto));

	u_msg_bitacora_eliminar(ser_msg);
	u_msg_bitacora_eliminar(deser_msg);
	u_paquete_delete(paquete);
}

void test_desplazamiento_tripulante_serializar_y_deserializar(void)
{
// Arrange
//--------
    u_msg_desplazamiento_tripulante_t* ser_msg   = NULL;
    u_msg_desplazamiento_tripulante_t* deser_msg = NULL;
    u_paquete_t* paquete;

// Act
//----
    ser_msg = u_msg_desplazamiento_tripulante_crear(10, 4, 3);
    paquete = u_msg_desplazamiento_tripulante_serializar(ser_msg);
    deser_msg = u_msg_desplazamiento_tripulante_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->pos_x, 4);
    CU_ASSERT_EQUAL(ser_msg->pos_y, 3);

    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);
    CU_ASSERT_EQUAL(ser_msg->pos_x, deser_msg->pos_x);
    CU_ASSERT_EQUAL(ser_msg->pos_y, deser_msg->pos_y);

    u_msg_desplazamiento_tripulante_eliminar(ser_msg);
    u_msg_desplazamiento_tripulante_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_eliminar_tripulante_serializar_y_deserializar(void)
{
// Arrange
//--------
    u_msg_eliminar_tripulante_t* ser_msg = NULL;
    u_msg_eliminar_tripulante_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_eliminar_tripulante_crear(10);
    paquete = u_msg_eliminar_tripulante_serializar(ser_msg);
    deser_msg = u_msg_eliminar_tripulante_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);

    u_msg_eliminar_tripulante_eliminar(ser_msg);
    u_msg_eliminar_tripulante_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_finalizacion_tripulante_serializar_y_deserializar(void)
{
// Arrange
//--------
    const char* expected_task_name = "Tarea1";
    uint32_t expected_task_name_length = strlen(expected_task_name) + 1;

    u_msg_finalizacion_tarea_t* ser_msg = NULL;
    u_msg_finalizacion_tarea_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_finalizacion_tarea_crear(10, "Tarea1");
    paquete = u_msg_finalizacion_tarea_serializar(ser_msg);
    deser_msg = u_msg_finalizacion_tarea_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->largo_nombre_tarea, expected_task_name_length);
    CU_ASSERT_STRING_EQUAL(ser_msg->tarea, expected_task_name);

    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);
    CU_ASSERT_EQUAL(ser_msg->largo_nombre_tarea, deser_msg->largo_nombre_tarea);
    CU_ASSERT_STRING_EQUAL(ser_msg->tarea, deser_msg->tarea);

    u_msg_finalizacion_tarea_eliminar(ser_msg);
    u_msg_finalizacion_tarea_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_informar_sabotaje_serializar_y_deserializar(void)
{
// Arrange
//--------
    u_msg_atender_sabotaje_t* ser_msg = NULL;
    u_msg_atender_sabotaje_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_atiende_sabotaje_crear(10);
    paquete = u_msg_atiende_sabotaje_serializar(ser_msg);
    deser_msg = u_msg_atiende_sabotaje_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);

    u_msg_atiende_sabotaje_eliminar(ser_msg);
    u_msg_atiende_sabotaje_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_iniciar_tripulante_serializar_y_deserializar(void)
{
// Arrange
//--------
    u_msg_iniciar_tripulante_t* ser_msg = NULL;
    u_msg_iniciar_tripulante_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_iniciar_tripulante_crear(10, 10);
    paquete = u_msg_iniciar_tripulante_serializar(ser_msg);
    deser_msg = u_msg_iniciar_tripulante_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->patota_id, 10);
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);

    CU_ASSERT_EQUAL(ser_msg->patota_id, deser_msg->patota_id);
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);

    u_msg_iniciar_tripulante_eliminar(ser_msg);
    u_msg_iniciar_tripulante_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_iniciar_tarea_serializar_y_deserializar(void)
{
// Arrange
//--------
    const char* expected_task_name = "Tarea1";
    uint32_t expected_task_name_length = strlen(expected_task_name) + 1;

    u_msg_inicio_tarea_t* ser_msg = NULL;
    u_msg_inicio_tarea_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_inicio_tarea_crear(10, "Tarea1");
    paquete = u_msg_inicio_tarea_serializar(ser_msg);
    deser_msg = u_msg_inicio_tarea_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->largo_nombre_tarea, expected_task_name_length);
    CU_ASSERT_STRING_EQUAL(ser_msg->tarea, expected_task_name);

    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);
    CU_ASSERT_EQUAL(ser_msg->largo_nombre_tarea, deser_msg->largo_nombre_tarea);
    CU_ASSERT_STRING_EQUAL(ser_msg->tarea, deser_msg->tarea);

    u_msg_inicio_tarea_eliminar(ser_msg);
    u_msg_inicio_tarea_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_obtener_bitacora_serializar_y_deserializar(void)
{
// Arrange
//--------
    u_msg_obtener_bitacora_t* ser_msg = NULL;
    u_msg_obtener_bitacora_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_obtener_bitacora_crear(10);
    paquete = u_msg_obtener_bitacora_serializar(ser_msg);
    deser_msg = u_msg_obtener_bitacora_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);

    u_msg_obtener_bitacora_eliminar(ser_msg);
    u_msg_obtener_bitacora_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_proxima_tarea_serializar_y_deserializar(void)
{
// Arrange
//--------
    u_msg_proxima_tarea_t* ser_msg = NULL;
    u_msg_proxima_tarea_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_proxima_tarea_crear(10, 10);
    paquete = u_msg_proxima_tarea_serializar(ser_msg);
    deser_msg = u_msg_proxima_tarea_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->patota_id, 10);
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);

    CU_ASSERT_EQUAL(ser_msg->patota_id, deser_msg->patota_id);
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);

    u_msg_proxima_tarea_eliminar(ser_msg);
    u_msg_proxima_tarea_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_resuelve_sabotaje_serializar_y_deserializar(void)
{
// Arrange
//--------
    u_msg_resuelve_sabotaje_t* ser_msg = NULL;
    u_msg_resuelve_sabotaje_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_resuelve_sabotaje_crear(10);
    paquete = u_msg_resuelve_sabotaje_serializar(ser_msg);
    deser_msg = u_msg_resuelve_sabotaje_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);

    u_msg_resuelve_sabotaje_eliminar(ser_msg);
    u_msg_resuelve_sabotaje_eliminar(deser_msg);
    u_paquete_delete(paquete);
}

void test_tripulante_nuevo_estado_serializar_y_deserializar(void)
{
// Arrange
//--------
    uint32_t expected_state = 30; // Reemplazar en el futuro por el enum de los estados del tripulante.

    u_msg_tripulante_nuevo_estado_t* ser_msg = NULL;
    u_msg_tripulante_nuevo_estado_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_tripulante_nuevo_estado_crear(10, expected_state);
    paquete = u_msg_tripulante_nuevo_estado_serializar(ser_msg);
    deser_msg = u_msg_tripulante_nuevo_estado_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->nuevo_estado, expected_state);

    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);
    CU_ASSERT_EQUAL(ser_msg->nuevo_estado, deser_msg->nuevo_estado);

    u_msg_tripulante_nuevo_estado_eliminar(ser_msg);
    u_msg_tripulante_nuevo_estado_eliminar(deser_msg);
    u_paquete_delete(paquete);
}
