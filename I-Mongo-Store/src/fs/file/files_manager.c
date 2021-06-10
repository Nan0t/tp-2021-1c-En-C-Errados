#include "files_manager.h"
#include <pthread.h>

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

private void       fs_files_manager_add_ref(fs_file_ref_t* bitacora_ref);
private void       fs_files_manager_rm_ref(const char* file_name);
private fs_file_t* fs_files_manager_hold_ref(const char* file_name);
private void       fs_files_manager_release_ref(const char* file_name);

void fs_files_manager_init(const char* mount_point)
{
    if(p_files_manager_instance)
        return;

    p_files_manager_instance = u_malloc(sizeof(fs_files_manager_t));

    p_files_manager_instance->files       = dictionary_create();
    p_files_manager_instance->mount_point = strdup(mount_point);

    pthread_mutex_init(&p_files_manager_instance->files_mx, NULL);
}

void fs_files_manager_create_file(const char* name, char fill_char)
{
    fs_file_ref_t* file_ref = fs_file_ref_create(name, fill_char);
    fs_files_manager_add_ref(file_ref);
}

fs_file_t* fs_files_manager_hold_file(const char* name)
{
    return fs_files_manager_hold_file(name);
}

void fs_files_manager_release_file(const char* name)
{
    fs_files_manager_release_ref(name);
}

void fs_files_manager_delete_file(const char* name)
{
    fs_files_manager_rm_ref(name);
}

// ========================================================
//             *** Private Functions ***
// ========================================================

private fs_file_ref_t* fs_file_ref_create(const char* file_name, char fill_char)
{
    fs_file_t* file = fs_file_create(p_files_manager_instance->mount_point, file_name, fill_char);
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
