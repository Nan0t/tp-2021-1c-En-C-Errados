#include "exec_queue.h"
#include <semaphore.h>
#include <pthread.h>

typedef struct
{
    uint32_t curr_elements;
    uint32_t max_elements;

    pthread_mutex_t mx;

    t_queue*  queue;
} ds_exec_queue_t;

private ds_exec_queue_t* p_exec_queue = NULL;

void ds_exec_queue_init(uint32_t max_size)
{
    if(p_exec_queue)
        return;

    p_exec_queue = u_malloc(sizeof(ds_exec_queue_t));

    p_exec_queue->curr_elements = 0;
    p_exec_queue->max_elements  = max_size;
    p_exec_queue->queue         = queue_create();

    pthread_mutex_init(&p_exec_queue->mx, NULL);
}

bool ds_exec_is_full(void)
{
    pthread_mutex_lock(&p_exec_queue->mx);
    bool is_full = p_exec_queue->curr_elements == p_exec_queue->max_elements;
    pthread_mutex_unlock(&p_exec_queue->mx);

    return is_full;
}

void ds_exec_queue_push(tripulante_t* trip)
{
    pthread_mutex_lock(&p_exec_queue->mx);
    queue_push(p_exec_queue->queue, trip);
    p_exec_queue->curr_elements ++;
    pthread_mutex_unlock(&p_exec_queue->mx);
}

tripulante_t* ds_exec_queue_pop(void)
{
    pthread_mutex_lock(&p_exec_queue->mx);
    if(p_exec_queue->curr_elements == 0)
    {
        pthread_mutex_unlock(&p_exec_queue->mx);
        return NULL;
    }
    
    tripulante_t* trip = queue_pop(p_exec_queue->queue);
    p_exec_queue->curr_elements --;
    pthread_mutex_unlock(&p_exec_queue->mx);

    return trip;
}

uint64_t ds_exec_queue_size(void)
{
    return p_exec_queue->curr_elements;
}
