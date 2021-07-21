#include "bitacoras_manager.h"

#include <pthread.h>
#include <sys/stat.h>

#include <stdio.h>

typedef struct
{
    pthread_mutex_t ref_counter_mx;
    uint32_t        ref_counter;
    pthread_cond_t  can_be_deleted;

    fs_bitacora_t* bitacora;
} fs_bitacora_ref_t;

typedef struct
{
    char*           mount_point;
    pthread_mutex_t bitacoras_mx;
    t_dictionary*   bitacoras;
} fs_bitacoras_manager_t;

private fs_bitacoras_manager_t* p_bitacoras_manager_instance = NULL;

private fs_bitacora_ref_t* fs_bitacora_ref_create(uint32_t tid);
private void               fs_bitacora_ref_delete(fs_bitacora_ref_t* bitacora_ref);

private void           fs_bitacoras_manager_add_ref(fs_bitacora_ref_t* bitacora_ref);
private void           fs_bitacoras_manager_rm_ref(uint32_t tid);
private fs_bitacora_t* fs_bitacoras_manager_hold_ref(uint32_t tid);
private void           fs_bitacoras_manager_release_ref(uint32_t tid);

void fs_bitacoras_manager_init(const char* mount_point, bool is_clean_initialization)
{
    if(p_bitacoras_manager_instance)
        return;

    p_bitacoras_manager_instance = u_malloc(sizeof(fs_bitacoras_manager_t));

    p_bitacoras_manager_instance->mount_point = strdup(mount_point);
    p_bitacoras_manager_instance->bitacoras   = dictionary_create();
    
    pthread_mutex_init(&p_bitacoras_manager_instance->bitacoras_mx, NULL);

    if(is_clean_initialization)
    {
        char* bitacoras_path = string_from_format("%s/Bitacoras", u_config_get_string_value("PUNTO_MONTAJE"));
        mkdir(bitacoras_path, 0700);
        u_free(bitacoras_path);
    }
}

void fs_bitacoras_manager_create_bitacora(uint32_t tid)
{
    fs_bitacora_ref_t* bitacora_ref = fs_bitacora_ref_create(tid);
    fs_bitacoras_manager_add_ref(bitacora_ref);
}

fs_bitacora_t* fs_bitacoras_manager_hold_bitacora(uint32_t tid)
{
    return fs_bitacoras_manager_hold_ref(tid);
}

void fs_bitacoras_manager_release_bitacora(uint32_t tid)
{
    fs_bitacoras_manager_release_ref(tid);
}

void fs_bitacoras_manager_delete_bitacora(uint32_t tid)
{
    fs_bitacoras_manager_rm_ref(tid);
}

// ========================================================
//             *** Private Functions ***
// ========================================================

private fs_bitacora_ref_t* fs_bitacora_ref_create(uint32_t tid)
{
    fs_bitacora_t* bitacora = fs_bitacora_create(p_bitacoras_manager_instance->mount_point, tid);
    fs_bitacora_ref_t* bitacora_ref = u_malloc(sizeof(fs_bitacora_ref_t));

    bitacora_ref->bitacora    = bitacora;
    bitacora_ref->ref_counter = 0;
    
    pthread_mutex_init(&bitacora_ref->ref_counter_mx, NULL);
    pthread_cond_init(&bitacora_ref->can_be_deleted, NULL);

    return bitacora_ref;
}

private void fs_bitacora_ref_delete(fs_bitacora_ref_t* bitacora_ref)
{
    pthread_mutex_lock(&bitacora_ref->ref_counter_mx);
    if(bitacora_ref->ref_counter != 0)
        pthread_cond_wait(&bitacora_ref->can_be_deleted, &bitacora_ref->ref_counter_mx);
    pthread_mutex_unlock(&bitacora_ref->ref_counter_mx);

    fs_bitacora_delete(bitacora_ref->bitacora);

    pthread_mutex_destroy(&bitacora_ref->ref_counter_mx);
    pthread_cond_destroy(&bitacora_ref->can_be_deleted);

    u_free(bitacora_ref);
}

private void fs_bitacoras_manager_add_ref(fs_bitacora_ref_t* bitacora_ref)
{
    pthread_mutex_lock(&p_bitacoras_manager_instance->bitacoras_mx);

    char bitacora_key[64] = { 0 };
    sprintf(bitacora_key, "Tripulante%d", fs_bitacora_get_tid(bitacora_ref->bitacora));

    dictionary_put(p_bitacoras_manager_instance->bitacoras, (char*)bitacora_key, bitacora_ref);

    pthread_mutex_unlock(&p_bitacoras_manager_instance->bitacoras_mx);
}

private void fs_bitacoras_manager_rm_ref(uint32_t tid)
{
    pthread_mutex_lock(&p_bitacoras_manager_instance->bitacoras_mx);

    char bitacora_key[64] = { 0 };
    sprintf(bitacora_key, "Tripulante%d", tid);

    fs_bitacora_ref_t* bitacora_ref = dictionary_remove(p_bitacoras_manager_instance->bitacoras, (char*)bitacora_key);

    pthread_mutex_unlock(&p_bitacoras_manager_instance->bitacoras_mx);

    fs_bitacora_ref_delete(bitacora_ref);
}

private fs_bitacora_t* fs_bitacoras_manager_hold_ref(uint32_t tid)
{
    fs_bitacora_t* bitacora = NULL;

    char bitacora_key[64] = { 0 };
    sprintf(bitacora_key, "Tripulante%d", tid);

    pthread_mutex_lock(&p_bitacoras_manager_instance->bitacoras_mx);
    fs_bitacora_ref_t* bitacora_ref = dictionary_get(p_bitacoras_manager_instance->bitacoras, (char*)bitacora_key);

    if(bitacora_ref)
    {
        pthread_mutex_lock(&bitacora_ref->ref_counter_mx);
        bitacora_ref->ref_counter ++;
        pthread_mutex_unlock(&bitacora_ref->ref_counter_mx);

        bitacora = bitacora_ref->bitacora;
    }

    pthread_mutex_unlock(&p_bitacoras_manager_instance->bitacoras_mx);

    return bitacora;
}

private void fs_bitacoras_manager_release_ref(uint32_t tid)
{
    char bitacora_key[64] = { 0 };
    sprintf(bitacora_key, "Tripulante%d", tid);

    pthread_mutex_lock(&p_bitacoras_manager_instance->bitacoras_mx);
    fs_bitacora_ref_t* bitacora_ref = dictionary_get(p_bitacoras_manager_instance->bitacoras, (char*)bitacora_key);

    if(bitacora_ref)
    {
        pthread_mutex_lock(&bitacora_ref->ref_counter_mx);
        bitacora_ref->ref_counter --;
        if(bitacora_ref->ref_counter == 0)
            pthread_cond_signal(&bitacora_ref->can_be_deleted);
        pthread_mutex_unlock(&bitacora_ref->ref_counter_mx);
    }

    pthread_mutex_unlock(&p_bitacoras_manager_instance->bitacoras_mx);
}