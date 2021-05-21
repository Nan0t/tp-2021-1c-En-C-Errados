#include "synchronizer.h"
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct
{
    uint32_t devices_count;

    pthread_mutex_t next_device_id_mx;
    uint32_t        next_device_id;

    pthread_mutex_t devices_notifier_mx;
    uint32_t        devices_notifier;

    uint32_t cycle_delay;

    sem_t*   devices_sems;
    sem_t    start_cycle_sem;
    sem_t    sched_pause_sem;
    sem_t    sched_notify_end_of_cycle_sem;
} ds_synchronizer_t;

#ifndef NDEBUG
private uint32_t p_tick_counter    = 0;
#endif

private ds_synchronizer_t* p_synchronizer_instance = NULL;

private void ds_synchronizer_loop(void);

void ds_synchronizer_init(uint32_t devices_count)
{
    if(p_synchronizer_instance)
        return;

    p_synchronizer_instance = u_malloc(sizeof(ds_synchronizer_t));

    p_synchronizer_instance->devices_count = devices_count;

    p_synchronizer_instance->next_device_id = 0;
    pthread_mutex_init(&p_synchronizer_instance->next_device_id_mx, NULL);

    p_synchronizer_instance->devices_notifier = 0;
    pthread_mutex_init(&p_synchronizer_instance->devices_notifier_mx, NULL);

    p_synchronizer_instance->cycle_delay = u_config_get_int_value("RETARDO_CICLO_CPU");

    p_synchronizer_instance->devices_sems  = u_malloc(devices_count * sizeof(sem_t));

    for(uint32_t i = 0; i < devices_count; i ++)
        sem_init(&p_synchronizer_instance->devices_sems[i], 0, 0);

    sem_init(&p_synchronizer_instance->sched_pause_sem, 0, 1);
    sem_init(&p_synchronizer_instance->start_cycle_sem, 0, 0);
    sem_init(&p_synchronizer_instance->sched_notify_end_of_cycle_sem, 0, 0);

    pthread_t synchronizer_thread;
    U_ASSERT(pthread_create(&synchronizer_thread, NULL, (void*)ds_synchronizer_loop, NULL) != - 1,
        "No se pudo crear el hilo del Sincronizador");
    pthread_detach(synchronizer_thread);
}

uint32_t ds_synchronizer_get_device_id(void)
{
    pthread_mutex_lock(&p_synchronizer_instance->next_device_id_mx);
    uint32_t next_device_id = p_synchronizer_instance->next_device_id ++;
    pthread_mutex_unlock(&p_synchronizer_instance->next_device_id_mx);

    return next_device_id;
}

void ds_synchronizer_wait_for_next_cicle(uint32_t device_id)
{
    U_ASSERT(device_id < p_synchronizer_instance->devices_count,
        "ID de dispositivo invalido: %d", device_id);

#ifndef NDEBUG
    U_LOG_TRACE("Devices %d start his cicle: %d", device_id);
#endif

    sem_wait(&p_synchronizer_instance->devices_sems[device_id]);
}

void ds_synchronizer_notify_end_of_cicle(void)
{
    pthread_mutex_lock(&p_synchronizer_instance->devices_notifier_mx);
    p_synchronizer_instance->devices_notifier ++;

    if(p_synchronizer_instance->devices_notifier == p_synchronizer_instance->devices_count)
    {
        p_synchronizer_instance->devices_count = 0;
        sem_post(&p_synchronizer_instance->start_cycle_sem);
    }

    pthread_mutex_unlock(&p_synchronizer_instance->devices_notifier_mx);
}

void ds_synchronizer_sched_notify_new_cycle(void)
{
    sem_post(&p_synchronizer_instance->sched_pause_sem);
}

void ds_synchronizer_sched_wait_end_of_cicle(void)
{
    sem_wait(&p_synchronizer_instance->sched_notify_end_of_cycle_sem);
}

private void ds_synchronizer_loop(void)
{
    while(1)
    {
        sem_wait(&p_synchronizer_instance->start_cycle_sem);
        sem_wait(&p_synchronizer_instance->sched_pause_sem);
        
        for(uint32_t i = 0; i < p_synchronizer_instance->devices_count; i ++)
            sem_post(&p_synchronizer_instance->devices_sems[i]);

        sem_post(&p_synchronizer_instance->sched_notify_end_of_cycle_sem);
#ifndef NDEBUG
        U_LOG_TRACE("Tick: %d", ++ p_tick_counter);
#endif
        sleep(p_synchronizer_instance->cycle_delay);
    }
}

// Usado por el planificador.
void ds_syncronizer_sched_notify_next_cicle(void)
{
    sem_post(&p_synchronizer_instance->sched_pause_sem);
}