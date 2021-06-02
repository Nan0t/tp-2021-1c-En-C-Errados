#include "discordia/discordia.h"
#include "tripulante.h"
#include <pthread.h>

#define CMP_POS(pos1, pos2) (pos1.x == pos2.x && pos1.y == pos2.y)

private void tripulante_loop(tripulante_t* this);
private void tripulante_mover(tripulante_t* tripulante);

tripulante_t* tripulante_create(uint32_t pid, uint32_t tid, const u_pos_t* pos, uint32_t quantum)
{
    tripulante_t* this = u_malloc(sizeof(tripulante_t));

    this->pid          = pid;
    this->tid          = tid;
    this->pos.x        = pos->x;
    this->pos.y        = pos->y;
    this->quatum       = quantum;
    this->bloquear     = false;
    this->tarea_actual = NULL;
    
    sem_init(&this->sem_sync, 0, 0);
    sem_init(&this->sem_end_exec, 0, 0);

    return this;
}

void tripulante_delete(tripulante_t* this)
{
    sem_destroy(&this->sem_sync);
    sem_destroy(&this->sem_end_exec);
    u_free(this);
}

bool tripulante_obtener_proxima_tarea(tripulante_t* tripulante){
    char* proxima_tarea = discordia_obtener_proxima_tarea(tripulante->pid, tripulante->tid);

    if(proxima_tarea == NULL)
        return false;

    eliminar_tarea(tripulante->tarea_actual);
    tripulante->tarea_actual = parsear_tarea(proxima_tarea);

    return true;
}

void tripulante_init(tripulante_t* this)
{
    pthread_t tripulante_thread;
    pthread_create(&tripulante_thread, NULL, (void*)tripulante_loop, this);
    pthread_detach(tripulante_thread);
}

private void tripulante_loop(tripulante_t* this)
{
    bool termino_hilo = false;
    tripulante_obtener_proxima_tarea(this);

    while(!termino_hilo)
    {
        sem_wait(&this->sem_sync);

        if(this->tarea_actual->is_finished)
        {
            discordia_finalizar_tarea(this->tid, this->tarea_actual->tarea);

            if(!tripulante_obtener_proxima_tarea(this))
            {
                termino_hilo = true;
                discordia_expulsar_tripulante(this->tid);
            }
            else
                discordia_desplazamiento_tripulante(this->tid, &this->pos, &this->tarea_actual->pos);
        }
        
        if(!termino_hilo)
        {
            if(CMP_POS(this->pos, this->tarea_actual->pos))
            {
                if(this->tarea_actual->is_blocking)
                    this->bloquear = true;
                else
                {
                    this->tarea_actual->tiempo_bloqueado --;
                    U_LOG_INFO("Tripulante %d ejecuta tarea %s. Tiempo restante: %d",
                        this->tid, this->tarea_actual->tarea, this->tarea_actual->tiempo_bloqueado);

                    this->tarea_actual->is_finished = this->tarea_actual->tiempo_bloqueado == 0; //TODO: PEDIR NUEVA TAREA Y MANEJAR SI DEVUELVE NULL
                }
            }
            else
            {
                tripulante_mover(this);
            }
        }  

        sem_post(&this->sem_end_exec);
    }

    tripulante_delete(this);
}

private void tripulante_mover(tripulante_t* tripulante)
{
    u_pos_t prev_pos = { .x = tripulante->pos.x, .y = tripulante->pos.y };

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
    discordia_mover_tripulante(tripulante->pid, tripulante->tid, &tripulante->pos);
    U_LOG_INFO("Tripulante %d se mueve de { %d, %d } a { %d, %d }",
        tripulante->tid, prev_pos.x, prev_pos.y, tripulante->pos.x, tripulante->pos.y);
}