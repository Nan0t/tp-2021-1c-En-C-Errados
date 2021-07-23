#include "files_manager.h"

#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <errno.h>

typedef struct
{
    pthread_mutex_t ref_counter_mx;
    uint32_t        ref_counter;
    pthread_cond_t  can_be_deleted;

    fs_file_t* file;
} fs_file_ref_t;

typedef struct
{
    char*           mount_point;
    pthread_mutex_t files_mx;
    t_dictionary*   files;
} fs_files_manager_t;

private fs_files_manager_t* p_files_manager_instance = NULL;

private fs_file_ref_t* fs_file_ref_create(const char* file_name, char fill_char);
private void           fs_file_ref_delete(fs_file_ref_t* bitacora_ref);

private void       fs_files_manager_load_all_files(const char* files_directory_path);
private void       fs_files_manager_load_file(const char* file_path);
private void       fs_files_manager_add_ref(fs_file_ref_t* bitacora_ref);
private void       fs_files_manager_rm_ref(const char* file_name);
private fs_file_t* fs_files_manager_hold_ref(const char* file_name);
private void       fs_files_manager_release_ref(const char* file_name);

void fs_files_manager_init(const char* mount_point, bool is_clean_initialization)
{
    if(p_files_manager_instance)
        return;

    p_files_manager_instance = u_malloc(sizeof(fs_files_manager_t));

    p_files_manager_instance->files       = dictionary_create();
    p_files_manager_instance->mount_point = strdup(mount_point);

    pthread_mutex_init(&p_files_manager_instance->files_mx, NULL);

    char* files_path = string_from_format("%s/Files", u_config_get_string_value("PUNTO_MONTAJE"));

    if(is_clean_initialization)
    {
        mkdir(files_path, 0700);
        u_free(files_path);
    }
    else
        fs_files_manager_load_all_files(files_path);
}

void fs_files_manager_create_file(const char* name, char fill_char)
{
    char file_path[1024] = { 0 };
    sprintf(file_path, "%s/%s", p_files_manager_instance->mount_point, name);

    if(access(file_path, F_OK) == 0)
    {
        U_LOG_WARN("El archivo %s ya existe en el FileSystem.", file_path);
        return;
    }

    fclose(fopen(file_path, "w")); // Trambolico xd
    fs_files_manager_add_ref(fs_file_ref_create(file_path, fill_char));
}

fs_file_t* fs_files_manager_hold_file(const char* name)
{
    return fs_files_manager_hold_ref(name);
}

void fs_files_manager_release_file(const char* name)
{
    fs_files_manager_release_ref(name);
}

void fs_files_manager_delete_file(const char* name)
{
    fs_files_manager_rm_ref(name);
}

bool fs_files_manager_check_files_integrity(void)
{
    bool corrupt_file_was_found = false;

    pthread_mutex_lock(&p_files_manager_instance->files_mx);

    void _check_file_integrity(fs_file_ref_t* file_ref) {
        if(fs_file_check_integrity(file_ref->file))
            corrupt_file_was_founded = true;
    };
    dictionary_iterator(p_files_manager_instance->files, (void*)_check_file_integrity);

    pthread_mutex_unlock(&p_files_manager_instance->files_mx);

    return corrupt_file_was_found;
}

t_list* fs_files_manager_get_blocks_id(void)
{
    t_list* bloques = list_create();
    pthread_mutex_lock(&p_files_manager_instance->files_mx);

    void _get_file_blocks_id(fs_file_ref_t* file_ref) {
        t_list* lista_temporal = fs_file_get_blocks(file_ref->file);
        list_add_all(bloques, lista_temporal);
        free(lista_temporal);
    };
    dictionary_iterator(p_files_manager_instance->files, (void*)_get_file_blocks_id);

    pthread_mutex_unlock(&p_files_manager_instance->files_mx);

    return bloques;
}

// ========================================================
//             *** Private Functions ***
// ========================================================

private fs_file_ref_t* fs_file_ref_create(const char* file_path, char fill_char)
{
    char fill_char_str[2] = { fill_char, '\0' };

    fs_file_t*     file     = fs_file_create(file_path, fill_char_str);
    fs_file_ref_t* file_ref = u_malloc(sizeof(fs_file_ref_t));

    file_ref->file        = file;
    file_ref->ref_counter = 0;

    pthread_mutex_init(&file_ref->ref_counter_mx, NULL);
    pthread_cond_init(&file_ref->can_be_deleted, NULL);

    return file_ref;
}

private void fs_file_ref_delete(fs_file_ref_t* file_ref)
{
    pthread_mutex_lock(&file_ref->ref_counter_mx);
    if(file_ref->ref_counter != 0)
        pthread_cond_wait(&file_ref->can_be_deleted, &file_ref->ref_counter_mx);
    pthread_mutex_unlock(&file_ref->ref_counter_mx);

    fs_file_delete(file_ref->file);

    pthread_mutex_destroy(&file_ref->ref_counter_mx);
    pthread_cond_destroy(&file_ref->can_be_deleted);

    u_free(file_ref);
}

private void fs_files_manager_load_all_files(const char* files_directory_path)
{
    char filename[512] = { 0 };

    struct dirent* dir_info;
    DIR* directory = opendir(files_directory_path);
    U_ASSERT(directory != NULL, "Error al obtener el directorio %s: %s", files_directory_path, strerror(errno));


    while((dir_info = readdir(directory)) != NULL)
    {
        sprintf(filename, "%s/%s", files_directory_path, dir_info->d_name);
        fs_files_manager_load_file(filename);
    }
}

private void fs_files_manager_load_file(const char* file_path)
{
    // TODO: Crear un fs_file_t a partir de un archivo existente.
}

private void fs_files_manager_add_ref(fs_file_ref_t* file_ref)
{
    pthread_mutex_lock(&p_files_manager_instance->files_mx);
    dictionary_put(p_files_manager_instance->files, (char*)fs_file_get_name(file_ref->file), file_ref);
    pthread_mutex_unlock(&p_files_manager_instance->files_mx);
}

private void fs_files_manager_rm_ref(const char* file_name)
{
    pthread_mutex_lock(&p_files_manager_instance->files_mx);
    fs_file_ref_t* file_ref = dictionary_remove(p_files_manager_instance->files, (char*)file_name);
    pthread_mutex_unlock(&p_files_manager_instance->files_mx);

    if(file_ref)
        fs_file_ref_delete(file_ref);
}

private fs_file_t* fs_files_manager_hold_ref(const char* file_name)
{
    fs_file_t* file = NULL;

    pthread_mutex_lock(&p_files_manager_instance->files_mx);
    fs_file_ref_t* file_ref = dictionary_get(p_files_manager_instance->files, (char*)file_name);

    if(file_ref)
    {
        pthread_mutex_lock(&file_ref->ref_counter_mx);
        file_ref->ref_counter ++;
        pthread_mutex_unlock(&file_ref->ref_counter_mx);

        file = file_ref->file;
    }

    pthread_mutex_unlock(&p_files_manager_instance->files_mx);

    return file;
}

private void fs_files_manager_release_ref(const char* file_name)
{
    pthread_mutex_lock(&p_files_manager_instance->files_mx);
    fs_file_ref_t* file_ref = dictionary_get(p_files_manager_instance->files, (char*)file_name);

    if(file_ref)
    {
        pthread_mutex_lock(&file_ref->ref_counter_mx);
        file_ref->ref_counter --;

        if(file_ref->ref_counter == 0)
            pthread_cond_signal(&file_ref->can_be_deleted);
        pthread_mutex_unlock(&file_ref->ref_counter_mx);
    }

    pthread_mutex_unlock(&p_files_manager_instance->files_mx);    
}
