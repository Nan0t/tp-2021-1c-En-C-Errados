#include "tripulante_test.h"
#include "planificador/tripulante/tareas.h"
#include "stdlib.h"
#include <CUnit/Basic.h>



void test_valid_parsear_tarea_con_parametro_bloqueante(void)
{
// Act
//--------
    char* tarea_sin_parsear = "GENERAR_OXIGENO 12;2;3;5";
    tarea_t* tarea_parseada = parsear_tarea(tarea_sin_parsear);

// Assert
//-------
    CU_ASSERT_STRING_EQUAL(tarea_parseada->tarea, "GENERAR_OXIGENO 12");
    CU_ASSERT_EQUAL(tarea_parseada->is_blocking, true);
    CU_ASSERT_EQUAL(tarea_parseada->is_finished, false);
    CU_ASSERT_EQUAL(tarea_parseada->tiempo_bloqueado, 5);
    CU_ASSERT_EQUAL(tarea_parseada->pos.x, 2);
    CU_ASSERT_EQUAL(tarea_parseada->pos.y, 3);
    

    eliminar_tarea(tarea_parseada);

}

void test_valid_parsear_tarea_sin_parametro_bloqueante(void)
{
    char* tarea_sin_parsear = "DESCARTAR_BASURA 0;2;3;5";
    tarea_t* tarea_parseada = parsear_tarea(tarea_sin_parsear);

    // Assert
    //-------
    CU_ASSERT_STRING_EQUAL(tarea_parseada->tarea, "DESCARTAR_BASURA 0");
    CU_ASSERT_EQUAL(tarea_parseada->is_blocking, true);
    CU_ASSERT_EQUAL(tarea_parseada->is_finished, false);
    CU_ASSERT_EQUAL(tarea_parseada->tiempo_bloqueado, 5);
    CU_ASSERT_EQUAL(tarea_parseada->pos.x, 2);
    CU_ASSERT_EQUAL(tarea_parseada->pos.y, 3);

    eliminar_tarea(tarea_parseada);
}

void test_valid_parsear_tarea_sin_parametro_no_bloqueante(void)
{
    char* tarea_sin_parsear = "PAVEAR;2;3;5";
    tarea_t* tarea_parseada = parsear_tarea(tarea_sin_parsear);

    // Assert
    //-------
    CU_ASSERT_STRING_EQUAL(tarea_parseada->tarea, "PAVEAR");
    CU_ASSERT_EQUAL(tarea_parseada->is_blocking, false);
    CU_ASSERT_EQUAL(tarea_parseada->is_finished, false);
    CU_ASSERT_EQUAL(tarea_parseada->tiempo_bloqueado, 5);
    CU_ASSERT_EQUAL(tarea_parseada->pos.x, 2);
    CU_ASSERT_EQUAL(tarea_parseada->pos.y, 3);

    eliminar_tarea(tarea_parseada);
}
