#include "discordia/discordia.h"
#include "tripulante.h"
#include <pthread.h>

#define CMP_POS(pos1, pos2) (pos1.x == pos2.x && pos1.y == pos2.y)

private void tripulante_loop(tripulante_t* trip_info);
private void tripulante_mover(tripulante_t* tripulante);
private void tripulante_obtener_proxima_tarea(tripulante_t* tripulante);



void tripulante_init(tripulante_t* trip_info)
{
    pthread_t tripulante_thread;
    pthread_create(&tripulante_thread, NULL, (void*)tripulante_loop, trip_info);
    pthread_detach(tripulante_thread);
}

private void tripulante_loop(tripulante_t* trip_info)
{
    bool termino_hilo = false;
    tripulante_obtener_proxima_tarea(trip_info);

    while(!termino_hilo)
    {
        sem_wait(&trip_info->sem_sync);

        if(trip_info->tarea_actual->is_finished)
        {
            discordia_finalizar_tarea(trip_info->tid, trip_info->tarea_actual);
            tripulante_obtener_proxima_tarea(trip_info);

            if(trip_info->tarea_actual == NULL)
            {
                termino_hilo = true;
                discordia_expulsar_tripulante(trip_info->tid);
            }
            else
                discordia_desplazamiento_tripulante(trip_info->tid, &trip_info->pos, &trip_info->tarea_actual->pos);
        }
        
        if(!termino_hilo)
        {
            if(CMP_POS(trip_info->pos, trip_info->tarea_actual->pos))
            {
                if(trip_info->tarea_actual->is_blocking)
                    trip_info->bloquear = true;
                else
                {
                    trip_info->tarea_actual->tiempo_bloqueado --;
                    trip_info->tarea_actual->is_finished = trip_info->tarea_actual->tiempo_bloqueado == 0;
                }
            }
            else
            {
                tripulante_mover(trip_info);
            }
        }  

        sem_post(&trip_info->sem_end_exec);
    }
}

private void tripulante_mover(tripulante_t* tripulante)
{

	if (tripulante->pos.x != tripulante->tarea_actual->pos.x) 
    {
		if(tripulante->pos.x < tripulante->tarea_actual->pos.x) 
        {
			tripulante->pos.x ++;
		} 
        else 
        {
			tripulante->pos.x --;
		}
	}
    else if (tripulante->pos.y != tripulante->tarea_actual->pos.y)
    {
		if(tripulante->pos.y < tripulante->tarea_actual->pos.y) 
        {
			tripulante->pos.y ++;
		} 
        else 
        {
			tripulante->pos.y --;
		}
	}
    discordia_mover_tripulante(tripulante->tid, &tripulante->pos);
}

private void tripulante_obtener_proxima_tarea(tripulante_t* tripulante){
    char* proxima_tarea = discordia_obtener_proxima_tarea(tripulante->tid);
    tripulante->tarea_actual = parsear_tarea(proxima_tarea);
}