#include "exec_queue.h"
#include <semaphore.h>
#include <pthread.h>

private pthread_mutex_t p_exec_queue_mx = PTHREAD_MUTEX_INITIALIZER;
private sem_t           p_exec_queue_sem;
private t_queue*        p_exec_queue    = NULL;

void ds_exec_init(void)
{
    if(p_exec_queue)
        return;

    p_exec_queue = queue_create();
    sem_init(&p_exec_queue_sem, 0, 0);
}

void ds_exec_push(tripulante_t* trip)
{
    pthread_mutex_lock(&p_exec_queue_mx);
    queue_push(p_exec_queue, trip);
    pthread_mutex_unlock(&p_exec_queue_mx);

    sem_post(&p_exec_queue_sem);
}

tripulante_t* ds_exec_pop(void)
{
    if(sem_trywait(&p_exec_queue_sem) == -1)
        return NULL;
        
    pthread_mutex_lock(&p_exec_queue_mx);
    tripulante_t* trip = queue_pop(p_exec_queue);
    pthread_mutex_unlock(&p_exec_queue_mx);

    return trip;
}
