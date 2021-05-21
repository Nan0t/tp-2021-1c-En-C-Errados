#include "new_queue.h"
#include <semaphore.h>
#include <pthread.h>

private pthread_mutex_t p_new_queue_mx = PTHREAD_MUTEX_INITIALIZER;
private sem_t           p_new_queue_sem;
private t_queue*        p_new_queue    = NULL;

void ds_new_queue_init(void)
{
    if(p_new_queue)
        return;

    p_new_queue = queue_create();
    sem_init(&p_new_queue_sem, 0, 0);
}

void ds_new_queue_push(tripulante_t* trip)
{
    pthread_mutex_lock(&p_new_queue_mx);
    queue_push(p_new_queue, trip);
    pthread_mutex_unlock(&p_new_queue_mx);

    sem_post(&p_new_queue_sem);
}

tripulante_t* ds_new_queue_pop(void)
{
    if(sem_trywait(&p_new_queue_sem) == -1)
        return NULL;
        
    pthread_mutex_lock(&p_new_queue_mx);
    tripulante_t* trip = queue_pop(p_new_queue);
    pthread_mutex_unlock(&p_new_queue_mx);

    return trip;
}
