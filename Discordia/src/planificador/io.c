#include "io.h"
#include "synchronizer.h"
#include "queues/queue_manager.h"
#include "planificador.h"

#include <pthread.h>

private pthread_mutex_t p_curr_trip_mx = PTHREAD_MUTEX_INITIALIZER;
private tripulante_t*   p_curr_trip    = NULL;

private void manejo_de_bloqueado_por_io(tripulante_t* tripulante);
private void mover_tripulante_terminado(tripulante_t* tripulante);
private bool puede_desbloquearse(tripulante_t* tripulante);

private void find_and_terminate_from_block(uint32_t tid);

void io_init(void) 
{
    uint32_t io_id = ds_synchronizer_get_device_id();

    while(1)
    {
        ds_synchronizer_wait_for_next_cicle(io_id);

        if(p_curr_trip == NULL)
        {
            ds_queue_mt_t* block_queue = ds_queue_manager_hold(DS_QUEUE_BLOCK);

            pthread_mutex_lock(&p_curr_trip_mx);
            p_curr_trip = ds_queue_mt_pop(block_queue);
            pthread_mutex_unlock(&p_curr_trip_mx);
            
            ds_queue_manager_release(DS_QUEUE_BLOCK);
        }

        pthread_mutex_lock(&p_curr_trip_mx);
        if(p_curr_trip)
        {
            manejo_de_bloqueado_por_io(p_curr_trip);

            if(puede_desbloquearse(p_curr_trip))
            {
                mover_tripulante_terminado(p_curr_trip);
                p_curr_trip = NULL;
            }
        }
        pthread_mutex_unlock(&p_curr_trip_mx);

        ds_synchronizer_notify_end_of_cicle();
    }
}

void io_find_and_terminate_from_block(uint32_t tid)
{
    bool keep_looking = true;

    pthread_mutex_lock(&p_curr_trip_mx);

    if(p_curr_trip && p_curr_trip->tid == tid)
    {
        tripulante_terminate(p_curr_trip);

        p_curr_trip = NULL;
        keep_looking = false;
    }

    if(keep_looking)
        find_and_terminate_from_block(tid);

    pthread_mutex_unlock(&p_curr_trip_mx);
}

private void manejo_de_bloqueado_por_io(tripulante_t* tripulante) 
{
        tripulante->tarea_actual->tiempo_bloqueado --;
}

private void mover_tripulante_terminado(tripulante_t* tripulante) 
{
        tripulante->tarea_actual->is_finished = true;
        
        if(tripulante_obtener_proxima_tarea(tripulante))
        {
            if(!ds_planificador_esta_en_sabotaje())
            {
                tripulante_change_state(tripulante, TRIP_STATE_READY);

                ds_queue_mt_t* ready_queue = ds_queue_manager_hold(DS_QUEUE_READY);
                ds_queue_mt_push(ready_queue, tripulante);
                ds_queue_manager_release(DS_QUEUE_READY);
            }
            else
            {
                tripulante_change_state(tripulante, TRIP_STATE_BLOCK_SABOTAGE);

                ds_queue_mt_t* queue_sabotaje = ds_queue_manager_hold(DS_QUEUE_SABOTAGE);
                ds_queue_mt_push(queue_sabotaje, tripulante);
                ds_queue_manager_release(DS_QUEUE_SABOTAGE);
            }
        }
        else
            tripulante_terminate(tripulante);
}

private bool puede_desbloquearse(tripulante_t* tripulante) 
{
    return tripulante->tarea_actual->tiempo_bloqueado == 0;
}

private void find_and_terminate_from_block(uint32_t tid)
{
    ds_queue_mt_t* block_queue = ds_queue_manager_hold(DS_QUEUE_BLOCK);

    DS_QUEUE_MT_FOREACH(block_queue)

        tripulante_t* trip = ds_queue_mt_iterator_next(it);
        if(trip->tid == tid)
        {
            ds_queue_mt_iterator_remove(it);
            tripulante_terminate(trip);

            DS_QUEUE_MT_BREAK_FOREACH
        }

    DS_QUEUE_MT_END_FOREACH

    ds_queue_manager_release(DS_QUEUE_BLOCK);
}