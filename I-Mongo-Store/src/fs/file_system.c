#include "file_system.h"
#include "blocks/blocks_manager.h"
#include "bitacora/bitacoras_manager.h"
#include "file/files_manager.h"
#include "sabotage/sabotage_notifier.h"
#include "task_parser/task_parser.h"

#include <pthread.h>

#include <dirent.h>
#include <sys/stat.h>

typedef void(*fs_task_exec_func)(const fs_task_parser_result_t*);

private bool file_system_check_for_clean_initialization(void);

private void  file_system_update_bitacora(uint32_t tid, const char* content);
private char* file_system_get_bitacora(uint32_t tid);

private void file_system_execute_task(const char* task);

private void file_system_add_char_fill(const fs_task_parser_result_t* task);
private void file_system_rm_char_fill(const fs_task_parser_result_t* task);
private void file_system_rm_file(const fs_task_parser_result_t* task);

private const fs_task_exec_func TASK_EXEC_FUNCS[] =
{
    file_system_add_char_fill,
    file_system_rm_char_fill,
    file_system_add_char_fill,
    file_system_rm_char_fill,
    file_system_add_char_fill,
    file_system_rm_file,
};

void file_system_init(const file_system_attr_t* attr){
	//TODO agregar condiciones de inicialización
    U_LOG_INFO("I-Mongo-Store File System");
    U_LOG_INFO("Punto de Montaje: %s", attr->mount_point);
    U_LOG_INFO("Tiempo de Sincronizacion: %d", attr->sync_time);

    bool is_clean_initialization = file_system_check_for_clean_initialization();

    fs_blocks_manager_init(attr->mount_point, is_clean_initialization);
    fs_bitacoras_manager_init(attr->mount_point, is_clean_initialization);
    fs_files_manager_init(attr->mount_point, is_clean_initialization);
    fs_sabotage_notifier_init(attr->sabotage_positions);
}

void  file_system_desplazamiento_tripulante(uint32_t tid, const u_pos_t* from, const u_pos_t* to)
{
    char* desplazamiento =
        string_from_format("Tripulante %d se desplaza desde { %d, %d } hasta { %d, %d }\n",
        tid, from->x, from->y, to->x, to->y);

    file_system_update_bitacora(tid, desplazamiento);
    u_free(desplazamiento);
}

void  file_system_inicio_tarea(uint32_t tid, const char* tarea)
{
    char* inicio_tarea =
        string_from_format("Tripulante %d inicia tarea %s\n", tid, tarea);

    file_system_update_bitacora(tid, inicio_tarea);
    file_system_execute_task(tarea);

    u_free(inicio_tarea);
}

void  file_system_finalizacion_tarea(uint32_t tid, const char* tarea)
{
    char* finaliza_tarea =
        string_from_format("Tripulante %d finalizo tarea %s\n",
        tid, tarea);

    file_system_update_bitacora(tid, finaliza_tarea);

    u_free(finaliza_tarea);
}

void  file_system_atiende_sabotaje(uint32_t tid)
{
    char* atiende_sabotaje =
        string_from_format("Tripulante %d atiende sabotaje\n", tid);

    file_system_update_bitacora(tid, atiende_sabotaje);

    u_free(atiende_sabotaje);
}

void  file_system_resuelve_sabotaje(uint32_t tid)
{
    char* resuelve_sabotaje =
        string_from_format("Tripulante %d resuelve sabotaje\n", tid);

    file_system_update_bitacora(tid, resuelve_sabotaje);

    u_free(resuelve_sabotaje);
}

char* file_system_obtener_bitacora(uint32_t tid)
{
    return file_system_get_bitacora(tid);
}

// ========================================================
//             *** Private Functions ***
// ========================================================

private bool file_system_check_for_clean_initialization(void)
{
    const char* mount_point_path        = u_config_get_string_value("PUNTO_MONTAJE");
    DIR*        mount_point             = opendir(mount_point_path);
    bool        is_clean_initialization = true;

    if(!mount_point)
        mkdir(mount_point_path, 0700);
    else
        is_clean_initialization = false;

    return is_clean_initialization;
}

private void  file_system_update_bitacora(uint32_t tid, const char* content)
{
    fs_bitacora_t* bitacora = fs_bitacoras_manager_hold_bitacora(tid);

    if(bitacora == NULL)
    {
        fs_bitacoras_manager_create_bitacora(tid);
        bitacora = fs_bitacoras_manager_hold_bitacora(tid);
    }

    fs_bitacora_add_content(bitacora, content);
    fs_bitacoras_manager_release_bitacora(tid);
}

private char* file_system_get_bitacora(uint32_t tid)
{
    U_LOG_TRACE("Se pide bitacora de tripulante %d", tid);

    fs_bitacora_t* bitacora = fs_bitacoras_manager_hold_bitacora(tid);

    if(bitacora == NULL)
        return NULL;

    char* content = fs_bitacora_get_content(bitacora);
    fs_bitacoras_manager_release_bitacora(tid);

    return content;
}

private void file_system_execute_task(const char* task)
{
    fs_task_parser_result_t* task_instance = fs_task_parser_parse(task);

    if(task_instance == NULL)
        U_LOG_ERROR("No se pudo parsear la tarea recibida: %s", task);
    else
    {
        if(task_instance->type != FS_TASK_UNKNOWN)
            TASK_EXEC_FUNCS[task_instance->type](task_instance);

        fs_task_parser_result_delete(task_instance);
    }
}

private void file_system_add_char_fill(const fs_task_parser_result_t* task)
{
    fs_file_t* file = fs_files_manager_hold_file(task->file_name);
    
    if(file == NULL)
    {
        fs_files_manager_create_file(task->file_name, task->file_name[0]);
        file = fs_files_manager_hold_file(task->file_name);
    }

    fs_file_add_fill_char(file, task->arg);
    fs_files_manager_release_file(task->file_name);
}

private void file_system_rm_char_fill(const fs_task_parser_result_t* task)
{
    fs_file_t* file = fs_files_manager_hold_file(task->file_name);
    
    if(file == NULL)
    {
        fs_files_manager_create_file(task->file_name, task->file_name[0]);
        file = fs_files_manager_hold_file(task->file_name);
    }

    fs_file_remove_fill_char(file, task->arg);
    fs_files_manager_release_file(task->file_name);
}

private void file_system_rm_file(const fs_task_parser_result_t* task)
{
    fs_files_manager_delete_file(task->file_name);
}
