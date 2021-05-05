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
    const char* expected_contenido_bitacora = "Bitacora";

    u_msg_bitacora_t* ser_msg = NULL;
    u_msg_bitacora_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_bitacora_crear(expected_contenido_bitacora);
    paquete = u_msg_bitacora_serializar(ser_msg);
    deser_msg = u_msg_bitacora_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_STRING_EQUAL(ser_msg->contenido, expected_contenido_bitacora);
    CU_ASSERT_STRING_EQUAL(ser_msg->contenido, deser_msg->contenido);

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
    ser_msg = u_msg_desplazamiento_tripulante_crear(10, (u_pos_t){ 4, 3 }, (u_pos_t){ 2, 1 });
    paquete = u_msg_desplazamiento_tripulante_serializar(ser_msg);
    deser_msg = u_msg_desplazamiento_tripulante_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tripulante_id, 10);
    CU_ASSERT_EQUAL(ser_msg->origen.x, 4);
    CU_ASSERT_EQUAL(ser_msg->origen.y, 3);
    CU_ASSERT_EQUAL(ser_msg->destino.x, 2);
    CU_ASSERT_EQUAL(ser_msg->destino.y, 1);

    CU_ASSERT_EQUAL(ser_msg->tripulante_id, deser_msg->tripulante_id);
    CU_ASSERT_EQUAL(ser_msg->origen.x, deser_msg->origen.x);
    CU_ASSERT_EQUAL(ser_msg->origen.y, deser_msg->origen.y);
    CU_ASSERT_EQUAL(ser_msg->destino.x, deser_msg->destino.x);
    CU_ASSERT_EQUAL(ser_msg->destino.y, deser_msg->destino.y);

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

void test_iniciar_patota_serializar_y_deserializar(void)
{
// Arrange
//--------
    const char* expected_lista_tareas = "ListaTareas";

    u_msg_iniciar_patota_t* ser_msg = NULL;
    u_msg_iniciar_patota_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_iniciar_patota_crear(10, expected_lista_tareas);
    paquete = u_msg_iniciar_patota_serializar(ser_msg);
    deser_msg = u_msg_iniciar_patota_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->pid, 10);
    CU_ASSERT_STRING_EQUAL(ser_msg->lista_tareas, expected_lista_tareas);

    CU_ASSERT_EQUAL(ser_msg->pid, deser_msg->pid);
    CU_ASSERT_STRING_EQUAL(ser_msg->lista_tareas, deser_msg->lista_tareas);

    u_msg_iniciar_patota_eliminar(ser_msg);
    u_msg_iniciar_patota_eliminar(deser_msg);
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

void test_movimiento_tripulante_serializar_y_deserializar(void)
{
// Arrange
//--------
    u_msg_movimiento_tripulante_t* ser_msg = NULL;
    u_msg_movimiento_tripulante_t* deser_msg = NULL;
    u_paquete_t* paquete = NULL;

// Act
//----
    ser_msg = u_msg_movimiento_tripulante_crear(10, (u_pos_t){ 3, 4 });
    paquete = u_msg_movimiento_tripulante_serializar(ser_msg);
    deser_msg = u_msg_movimiento_tripulante_deserializar(paquete->buffer);

// Assert
//-------
    CU_ASSERT_EQUAL(ser_msg->tid, 10);
    CU_ASSERT_EQUAL(ser_msg->pos.x, 3);
    CU_ASSERT_EQUAL(ser_msg->pos.y, 4);

    CU_ASSERT_EQUAL(ser_msg->tid, ser_msg->tid);
    CU_ASSERT_EQUAL(ser_msg->pos.x, ser_msg->pos.x);
    CU_ASSERT_EQUAL(ser_msg->pos.y, ser_msg->pos.y);

    u_msg_movimiento_tripulante_eliminar(ser_msg);
    u_msg_movimiento_tripulante_eliminar(deser_msg);
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
