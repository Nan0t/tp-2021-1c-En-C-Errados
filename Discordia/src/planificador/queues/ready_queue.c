#include "ready_queue.h"
#include <semaphore.h>
#include <pthread.h>

private pthread_mutex_t p_ready_queue_mx = PTHREAD_MUTEX_INITIALIZER;
private sem_t           p_ready_queue_sem;
private t_queue*        p_ready_queue    = NULL;

void ds_ready_init(void)
{
    if(p_ready_queue)
        return;

    p_ready_queue = queue_create();
    sem_init(&p_ready_queue_sem, 0, 0);
}

void ds_ready_queue_push(tripulante_t* trip)
{
    pthread_mutex_lock(&p_ready_queue_mx);
    queue_push(p_ready_queue, trip);
    pthread_mutex_unlock(&p_ready_queue_mx);

    sem_post(&p_ready_queue_sem);
}

tripulante_t* ds_ready_queue_pop(void)
{
    if(sem_trywait(&p_ready_queue_sem) == -1)
        return NULL;
        
    pthread_mutex_lock(&p_ready_queue_mx);
    tripulante_t* trip = queue_pop(p_ready_queue);
    pthread_mutex_unlock(&p_ready_queue_mx);

    return trip;
}
