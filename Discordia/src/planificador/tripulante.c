#include "discordia/discordia.h"
#include "tripulante.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define CMP_POS(pos1, pos2) (pos1.x == pos2.x && pos1.y == pos2.y)

private void tripulante_loop(tripulante_t* trip_info);
private bool tarea_es_bloqueante (char* tarea);


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

    trip_info->tarea_actual = parsear_tarea(proxima_tarea);

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
                trip_info->tarea_actual = parsear_tarea(proxima_tarea);
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
        
        sem_post(&trip_info->sem_end_exec);
    }
}

tarea_t* parsear_tarea (char* tarea_sin_parsear){
        tarea_t* tarea = malloc(sizeof(tarea_t));
        char** tarea_separada = string_split(tarea_sin_parsear, ";");
        tarea->tarea = string_duplicate(tarea_separada[0]);
        tarea->is_blocking = tarea_es_bloqueante(tarea_separada[0]);
        tarea->is_finished = false;
        tarea->tiempo_bloqueado = (uint32_t) atoi(tarea_separada[3]);
        tarea->pos.x = (uint32_t) atoi(tarea_separada[1]);
        tarea->pos.y = (uint32_t) atoi(tarea_separada[2]);
        string_iterate_lines(tarea_separada, (void*) free);
        free(tarea_separada);

        return tarea;
    }
void eliminar_tarea (tarea_t* tarea){
    free(tarea->tarea);
    free(tarea);
}

private bool tarea_es_bloqueante (char* tarea){
    return string_contains(tarea, " ") || string_contains(tarea, "DESCARTAR_BASURA");
    }