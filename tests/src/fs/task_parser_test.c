#include "task_parser_test.h"
#include <CUnit/Basic.h>
#include <fs/task_parser/task_parser.h>

void test_task_parser_unknown_task(void)
{
// Arrange.
//---------
    const char* task_str = "ABURRIRSE 20";
    fs_task_parser_result_t* task = NULL;

// Act.
//-----
    task = fs_task_parser_parse(task_str);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(task);

    CU_ASSERT_EQUAL(task->type, FS_TASK_UNKNOWN);
    CU_ASSERT_STRING_EQUAL(task->file_name, "Unknown");
    CU_ASSERT_EQUAL(task->arg, 0);

    fs_task_parser_result_delete(task);
}

void test_task_parser_generar_oxigeno(void)
{
// Arrange.
//---------
    const char* task_str = "GENERAR_OXIGENO 5";
    fs_task_parser_result_t* task = NULL;

// Act.
//-----
    task = fs_task_parser_parse(task_str);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(task);

    CU_ASSERT_EQUAL(task->type, FS_TASK_GENERAR_OXIGENO);
    CU_ASSERT_STRING_EQUAL(task->file_name, "Oxigeno");
    CU_ASSERT_EQUAL(task->arg, 5);

    fs_task_parser_result_delete(task);
}

void test_task_parser_consumir_oxigeno(void)
{
// Arrange.
//---------
    const char* task_str = "CONSUMIR_OXIGENO 10";
    fs_task_parser_result_t* task = NULL;

// Act.
//-----
    task = fs_task_parser_parse(task_str);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(task);

    CU_ASSERT_EQUAL(task->type, FS_TASK_CONSUMIR_OXIGENO);
    CU_ASSERT_STRING_EQUAL(task->file_name, "Oxigeno");
    CU_ASSERT_EQUAL(task->arg, 10);

    fs_task_parser_result_delete(task);
}

void test_task_parser_generar_comida(void)
{
// Arrange.
//---------
    const char* task_str = "GENERAR_COMIDA 3";
    fs_task_parser_result_t* task = NULL;

// Act.
//-----
    task = fs_task_parser_parse(task_str);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(task);

    CU_ASSERT_EQUAL(task->type, FS_TASK_GENERAR_COMIDA);
    CU_ASSERT_STRING_EQUAL(task->file_name, "Comida");
    CU_ASSERT_EQUAL(task->arg, 3);

    fs_task_parser_result_delete(task);
}

void test_task_parser_consumir_comida(void)
{
// Arrange.
//---------
    const char* task_str = "CONSUMIR_COMIDA 1";
    fs_task_parser_result_t* task = NULL;

// Act.
//-----
    task = fs_task_parser_parse(task_str);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(task);

    CU_ASSERT_EQUAL(task->type, FS_TASK_CONSUMIR_COMIDA);
    CU_ASSERT_STRING_EQUAL(task->file_name, "Comida");
    CU_ASSERT_EQUAL(task->arg, 1);

    fs_task_parser_result_delete(task);
}

void test_task_parser_generar_basura(void)
{
// Arrange.
//---------
    const char* task_str = "GENERAR_BASURA 5";
    fs_task_parser_result_t* task = NULL;

// Act.
//-----
    task = fs_task_parser_parse(task_str);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(task);

    CU_ASSERT_EQUAL(task->type, FS_TASK_GENERAR_BASURA);
    CU_ASSERT_STRING_EQUAL(task->file_name, "Basura");
    CU_ASSERT_EQUAL(task->arg, 5);

    fs_task_parser_result_delete(task);
}

void test_task_parser_descartar_basura(void)
{
// Arrange.
//---------
    const char* task_str = "DESCARTAR_BASURA 0";
    fs_task_parser_result_t* task = NULL;

// Act.
//-----
    task = fs_task_parser_parse(task_str);

// Assert.
//--------
    CU_ASSERT_PTR_NOT_NULL_FATAL(task);

    CU_ASSERT_EQUAL(task->type, FS_TASK_DESCARTAR_BASURA);
    CU_ASSERT_STRING_EQUAL(task->file_name, "Basura");
    CU_ASSERT_EQUAL(task->arg, 0);

    fs_task_parser_result_delete(task);
}
