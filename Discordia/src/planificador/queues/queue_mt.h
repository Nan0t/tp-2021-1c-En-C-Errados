#ifndef DISCORDIA_PLANIFICADOR_QUEUE_MT_H
#define DISCORDIA_PLANIFICADOR_QUEUE_MT_H

#include "planificador/tripulante/tripulante.h"

typedef struct ds_queue_mt_t ds_queue_mt_t;
typedef struct
{
    ds_queue_mt_t*   queue;
    t_list_iterator* it;
} ds_queue_mt_iterator_t;

ds_queue_mt_t* ds_queue_mt_create(uint32_t max_elements);
void           ds_queue_mt_delete(ds_queue_mt_t* queue);
void           ds_queue_mt_delete_and_delete_elementes(ds_queue_mt_t* queue);

bool     ds_queue_mt_is_full(const ds_queue_mt_t* queue);
uint32_t ds_queue_mt_get_size(const ds_queue_mt_t* queue);

void          ds_queue_mt_push(ds_queue_mt_t* queue, tripulante_t* trip);
tripulante_t* ds_queue_mt_pop(ds_queue_mt_t* queue);
tripulante_t* ds_queue_mt_pop_by_tid(ds_queue_mt_t* queue, uint32_t tid);

ds_queue_mt_iterator_t* ds_queue_mt_get_iterator(const ds_queue_mt_t* queue);

tripulante_t* ds_queue_mt_iterator_next(ds_queue_mt_iterator_t* it);
bool          ds_queue_mt_iterator_has_next(ds_queue_mt_iterator_t* it);
void          ds_queue_mt_iterator_remove(ds_queue_mt_iterator_t* it);
void          ds_queue_mt_iterator_delete(ds_queue_mt_iterator_t* it);

#define DS_QUEUE_MT_FOREACH(queue) {\
    ds_queue_mt_iterator_t* it = ds_queue_mt_get_iterator(queue);\
    while(ds_queue_mt_iterator_has_next(it)) {

#define DS_QUEUE_MT_FOREACH_INDEX(i, queue) {\
    ds_queue_mt_iterator_t* it = ds_queue_mt_get_iterator(queue);\
    for(uint32_t i = 0; ds_queue_mt_iterator_has_next(it); i ++) {

#define DS_QUEUE_MT_END_FOREACH } ds_queue_mt_iterator_delete(it); }
#define DS_QUEUE_MT_RETURN_FOREACH(...) ds_queue_mt_iterator_delete(it); return __VA_ARGS__;
#define DS_QUEUE_MT_BREAK_FOREACH break;

#endif