#include "discordia/discordia.h"
#include "tripulante.h"
#include <pthread.h>

#define CMP_POS(pos1, pos2) (pos1.x == pos2.x && pos1.y == pos2.y)

private void tripulante_loop(tripulante_t* trip_info);

void tripulante_init(tripulante_t* trip_info)
{
    pthread_t tripulante_thread;
    pthread_create(&tripulante_thread, NULL, (void*)tripulante_loop, trip_info);
    pthread_detach(tripulante_thread);
}

private void tripulante_loop(tripulante_t* trip_info)
{
    bool termino_hilo = false;
    char* proxima_tarea = discordia_obtener_proxima_tarea(trip_info->tid);

    // TODO: Parsear tarea.

    while(!termino_hilo)
    {
        sem_wait(&trip_info->sem_sync);

        if(trip_info->tarea_actual->is_finished)
        {
            char* proxima_tarea = discordia_obtener_proxima_tarea(trip_info->tid);

            if(proxima_tarea == NULL)
            {
                termino_hilo = true;
                discordia_expulsar_tripulante(trip_info->tid);
            }
            else
            {
                // TODO: Parsear tarea.
            }
        }
        
        if(!termino_hilo && CMP_POS(trip_info->pos, trip_info->tarea_actual->pos))
        {
            if(trip_info->tarea_actual->is_blocking)
                trip_info->bloquear = true;
            else
            {
                trip_info->tarea_actual->tiempo_bloqueado --;
                trip_info->tarea_actual->is_finished = trip_info->tarea_actual->tiempo_bloqueado == 0;
            }

            discordia_iniciar_tarea(trip_info->tid, trip_info->tarea_actual->tarea);
        }

        planificador_replanificar(trip_info);
        sem_post(&trip_info->sem_end_exec);
    }
}