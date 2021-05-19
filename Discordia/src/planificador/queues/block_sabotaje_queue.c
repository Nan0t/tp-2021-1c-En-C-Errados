#include "block_sabotaje_queue.h"
#include <semaphore.h>
#include <pthread.h>

private pthread_mutex_t p_block_sabotage_queue_mx = PTHREAD_MUTEX_INITIALIZER;
private sem_t           p_block_sabotage_queue_sem;
private t_queue*        p_block_sabotage_queue    = NULL;

void ds_block_sabotaje_init(void)
{
    if(p_block_sabotage_queue)
        return;

    p_block_sabotage_queue = queue_create();
    sem_init(&p_block_sabotage_queue_sem, 0, 0);
}

void ds_block_sabotaje_push(tripulante_t* trip)
{
    pthread_mutex_lock(&p_block_sabotage_queue_mx);
    queue_push(p_block_sabotage_queue, trip);
    pthread_mutex_unlock(&p_block_sabotage_queue_mx);

    sem_post(&p_block_sabotage_queue_sem);
}

tripulante_t* ds_block_sabotaje_pop(void)
{
    if(sem_trywait(&p_block_sabotage_queue_sem) == -1)
        return NULL;

    pthread_mutex_lock(&p_block_sabotage_queue_mx);
    tripulante_t* trip = queue_pop(p_block_sabotage_queue);
    pthread_mutex_unlock(&p_block_sabotage_queue_mx);

    return trip;
}
