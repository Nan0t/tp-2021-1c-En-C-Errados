#include "block_tripulantes_list.h"
#include <semaphore.h>
#include <pthread.h>

private pthread_mutex_t p_block_tripulantes_list_mx = PTHREAD_MUTEX_INITIALIZER;
private sem_t           p_block_tripulantes_list_sem;
private t_list*         p_block_tripulantes_list    = NULL;

void ds_block_tripulantes_init(void)
{
    if(p_block_tripulantes_list)
        return;

    p_block_tripulantes_list = list_create();
    sem_init(&p_block_tripulantes_list_sem, 0, 0);
}

void ds_block_tripulantes_add(tripulante_t* trip)
{
    pthread_mutex_lock(&p_block_tripulantes_list_mx);
    list_add(p_block_tripulantes_list, trip);
    pthread_mutex_unlock(&p_block_tripulantes_list_mx);

    sem_post(&p_block_tripulantes_list_sem);
}

tripulante_t* ds_block_tripulantes_get_and_remove(void)
{
    if(sem_trywait(&p_block_tripulantes_list_sem) == -1)
        return NULL;
        
    pthread_mutex_lock(&p_block_tripulantes_list_mx);
    tripulante_t* trip = list_remove(p_block_tripulantes_list,0);
    pthread_mutex_unlock(&p_block_tripulantes_list_mx);

    return trip;
}
