#include "exec_queue.h"
#include <semaphore.h>
#include <pthread.h>

private pthread_mutex_t p_exec_queue_mx = PTHREAD_MUTEX_INITIALIZER;
private t_queue*        p_exec_queue    = NULL;

private sem_t           p_exec_queue_sem;
private sem_t           p_exec_queue_full_sem;

void ds_exec_init(uint32_t max_size)
{
    if(p_exec_queue)
        return;

    p_exec_queue = queue_create();
    sem_init(&p_exec_queue_sem, 0, 0);
    sem_init(&p_exec_queue_full_sem, 0, max_size);
}

bool ds_exec_is_full(void)
{
    return sem_trywait(&p_exec_queue_full_sem) != -1;
}

void ds_exec_queue_push(tripulante_t* trip)
{
    sem_wait(&p_exec_queue_full_sem);
    pthread_mutex_lock(&p_exec_queue_mx);
    queue_push(p_exec_queue, trip);
    pthread_mutex_unlock(&p_exec_queue_mx);

    sem_post(&p_exec_queue_sem);
}

tripulante_t* ds_exec_queue_pop(void)
{
    if(sem_trywait(&p_exec_queue_sem) == -1)
        return NULL;
        
    pthread_mutex_lock(&p_exec_queue_mx);
    tripulante_t* trip = queue_pop(p_exec_queue);
    pthread_mutex_unlock(&p_exec_queue_mx);

    sem_post(&p_exec_queue_full_sem);

    return trip;
}
