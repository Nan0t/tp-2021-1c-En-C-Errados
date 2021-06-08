#include "queue_mt.h"
#include <pthread.h>

struct ds_queue_mt_t
{
    pthread_mutex_t mx;
    t_queue*        data;
    uint32_t        max_size;
    uint32_t        curr_elements;
};

ds_queue_mt_t* ds_queue_mt_create(uint32_t max_elements)
{
    ds_queue_mt_t* this = u_malloc(sizeof(ds_queue_mt_t));

    this->data          = queue_create();
    this->max_size      = max_elements;
    this->curr_elements = 0;
    
    pthread_mutex_init(&this->mx, NULL);

    return this;
}

void ds_queue_mt_delete(ds_queue_mt_t* queue)
{
    pthread_mutex_destroy(&queue->mx);
    queue_destroy(queue->data);
    u_free(queue);
}

void ds_queue_mt_delete_and_delete_elementes(ds_queue_mt_t* queue)
{
    pthread_mutex_destroy(&queue->mx);
    queue_destroy_and_destroy_elements(queue->data, (void*)tripulante_terminate);
    u_free(queue);
}

void ds_queue_mt_hold(ds_queue_mt_t* queue)
{
    pthread_mutex_lock(&queue->mx);
}

void ds_queue_mt_release(ds_queue_mt_t* queue)
{
    pthread_mutex_unlock(&queue->mx);
}

bool ds_queue_mt_is_full(const ds_queue_mt_t* queue)
{
    if(queue->max_size == 0)
        return false;

    return queue->max_size == queue->curr_elements;
}

uint32_t ds_queue_mt_get_size(const ds_queue_mt_t* queue)
{
    return queue->curr_elements;
}

void ds_queue_mt_push(ds_queue_mt_t* queue, tripulante_t* trip)
{
    if(queue->max_size == 0 || queue->max_size > queue->curr_elements)
    {
        queue->curr_elements ++;
        queue_push(queue->data, trip);
    }
}

tripulante_t* ds_queue_mt_pop(ds_queue_mt_t* queue)
{
    if(queue->curr_elements == 0)
        return NULL;

    U_LOG_DEBUG("MT_Queue Size: %d | Commons_Queue Size: %d", queue->curr_elements, queue->data->elements->elements_count);
    queue->curr_elements --;
    return queue_pop(queue->data);
}

tripulante_t* ds_queue_mt_pop_by_tid(ds_queue_mt_t* queue, uint32_t tid)
{
    if(queue->curr_elements == 0)
        return NULL;
        
    DS_QUEUE_MT_FOREACH_INDEX(i, queue)

        tripulante_t* trip = ds_queue_mt_iterator_next(it);
        
        if(trip->tid == tid)
        {
            queue->curr_elements --;
            DS_QUEUE_MT_RETURN_FOREACH(list_remove(queue->data->elements, i))
        }

    DS_QUEUE_MT_END_FOREACH

    return NULL;
}

ds_queue_mt_iterator_t* ds_queue_mt_get_iterator(const ds_queue_mt_t* queue)
{
    ds_queue_mt_iterator_t* it = u_malloc(sizeof(ds_queue_mt_iterator_t));

    it->queue = (ds_queue_mt_t*)queue;
    it->it    = list_iterator_create(queue->data->elements);
    
    return it;
}

tripulante_t* ds_queue_mt_iterator_next(ds_queue_mt_iterator_t* it)
{
    return list_iterator_next(it->it);
}

bool ds_queue_mt_iterator_has_next(ds_queue_mt_iterator_t* it)
{
    return list_iterator_has_next(it->it);
}

void ds_queue_mt_iterator_remove(ds_queue_mt_iterator_t* it)
{
    it->queue->curr_elements --;
    list_iterator_remove(it->it);
}

void ds_queue_mt_iterator_delete(ds_queue_mt_iterator_t* it)
{
    list_iterator_destroy(it->it);
    u_free(it);
}