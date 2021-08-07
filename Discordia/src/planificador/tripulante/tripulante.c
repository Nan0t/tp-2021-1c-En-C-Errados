#include "discordia/discordia.h"
#include "planificador/planificador.h"
#include "tripulante.h"
#include <pthread.h>
#include <errno.h>

#define CMP_POS(pos1, pos2) ((pos1).x == (pos2).x && (pos1).y == (pos2).y)

private const char STATES_CHAR[] =
{
    'N',
    'R',
    'E',
    'B',
    'B'
};

private const char* STATES_STR[] =
{
    "NEW",
    "READY",
    "EXEC",
    "BLOCK I/O",
    "BLOCK SABOTAGE",
    "EXIT"
};

private void tripulante_loop(tripulante_t* this);
private void tripulante_mover(tripulante_t* this, const u_pos_t* pos);
private bool tripulante_check_tarea_actual(tripulante_t* this);
private void tripulante_free(tripulante_t* this);
private void tripulante_ejecutar_rutina_sabotaje(tripulante_t* this);

tripulante_t* tripulante_create(uint32_t pid, uint32_t tid, const u_pos_t* pos, uint32_t quantum)
{
    tripulante_t* this = u_malloc(sizeof(tripulante_t));

    this->pid          = pid;
    this->tid          = tid;
    this->pos.x        = pos->x;
    this->pos.y        = pos->y;
    this->quatum       = quantum;
    this->curr_state   = TRIP_STATE_NEW;
    this->es_primera_ejecucion_en_cpu = true;
    this->terminate    = false;
    this->bloquear     = false;
    this->tarea_actual = NULL;
    
    sem_init(&this->sem_sync, 0, 0);
    sem_init(&this->sem_end_exec, 0, 0);

    return this;
}

void tripulante_delete(tripulante_t* this)
{
    discordia_eliminar_tripulante(this->pid, this->tid);

    sem_destroy(&this->sem_sync);
    sem_destroy(&this->sem_end_exec);

    u_free(this);
}

void tripulante_change_state(tripulante_t* this, trip_state_e state)
{
    if(state >= TRIP_STATE_NEW && state <= TRIP_STATE_BLOCK_SABOTAGE)
        discordia_tripulante_nuevo_estado(this->pid, this->tid, STATES_CHAR[state]);

    U_LOG_INFO("Tripulante %d pasa de estado %s a %s",
        this->tid, STATES_STR[this->curr_state], STATES_STR[state]);

    this->curr_state = state;
}

bool tripulante_obtener_proxima_tarea(tripulante_t* this){
    char* proxima_tarea = discordia_obtener_proxima_tarea(this->pid, this->tid);
    
    eliminar_tarea(this->tarea_actual);
    this->tarea_actual = parsear_tarea(proxima_tarea);

    if(this->tarea_actual == NULL)
        return false;

    u_free(proxima_tarea);

    return true;
}

void tripulante_init(tripulante_t* this)
{
    pthread_t tripulante_thread;
    U_ASSERT(pthread_create(&tripulante_thread, NULL, (void*)tripulante_loop, this) != -1,
        "No se pudo crear un hilo nuevo para el tripulante %d: %s", this->tid, strerror(errno));
    pthread_detach(tripulante_thread);

    U_LOG_DEBUG("Hilo del tripulante %d inicializado", this->tid);
}

void tripulante_terminate(tripulante_t* this)
{
    U_LOG_INFO("Tripulante %d pasa de %s a EXIT", this->tid, STATES_STR[this->curr_state]);
    discordia_eliminar_tripulante(this->pid, this->tid);
    this->terminate = true;
    sem_post(&this->sem_sync);
}

private void tripulante_loop(tripulante_t* this)
{
    while(!this->terminate)
    {
        sem_wait(&this->sem_sync);

        if(this->terminate)
            continue;

        if(ds_planificador_esta_en_sabotaje())
            tripulante_ejecutar_rutina_sabotaje(this);
        else
        {
            if(tripulante_check_tarea_actual(this))
                tripulante_mover(this, &this->tarea_actual->pos);
        }

        sem_post(&this->sem_end_exec);
    }

    tripulante_free(this);
}

private void tripulante_mover(tripulante_t* this, const u_pos_t* pos)
{
    u_pos_t prev_pos = { .x = this->pos.x, .y = this->pos.y };

	if (this->pos.x != pos->x) 
    {
		if(this->pos.x < pos->x)
			this->pos.x ++;
        else
			this->pos.x --;
	}
    else if (this->pos.y != pos->y)
    {
		if(this->pos.y < pos->y)
			this->pos.y ++;
        else
			this->pos.y --;
	}

    discordia_mover_tripulante(this->pid, this->tid, &this->pos);
    discordia_desplazamiento_tripulante(this->tid, &prev_pos, &this->pos);
    
    U_LOG_INFO("Tripulante %d se mueve de { %d, %d } a { %d, %d }",
        this->tid, prev_pos.x, prev_pos.y, this->pos.x, this->pos.y);
}

private bool tripulante_check_tarea_actual(tripulante_t* this)
{
    if(CMP_POS(this->pos, this->tarea_actual->pos))
    {
        if(this->tarea_actual->is_blocking)
        {
            this->bloquear = true;
            discordia_iniciar_tarea(this->tid, this->tarea_actual->tarea);
        }
        else
        {
            if(this->es_primera_ejecucion_en_cpu)
            {
              discordia_iniciar_tarea(this->tid, this->tarea_actual->tarea);
              this->es_primera_ejecucion_en_cpu = false;
            }

            this->tarea_actual->tiempo_bloqueado --;
            U_LOG_INFO("Tripulante %d ejecuta tarea %s. Tiempo restante: %d",
                this->tid, this->tarea_actual->tarea, this->tarea_actual->tiempo_bloqueado);

            this->tarea_actual->is_finished = this->tarea_actual->tiempo_bloqueado == 0;

            if(this->tarea_actual->is_finished)
            {
                discordia_finalizar_tarea(this->tid, this->tarea_actual->tarea);
                if(!tripulante_obtener_proxima_tarea(this))
                    discordia_expulsar_tripulante(this->tid);

                this->es_primera_ejecucion_en_cpu = true;
                    
                return false;
            }
        }

        return false;
    }

    return true;
}

private void tripulante_free(tripulante_t* this)
{
    eliminar_tarea(this->tarea_actual);
    sem_destroy(&this->sem_sync);
    sem_destroy(&this->sem_end_exec);
    u_free(this);
}

private void tripulante_ejecutar_rutina_sabotaje(tripulante_t* this)
{
    u_pos_t pos = { 0 };
    ds_planificador_get_pos_sabotaje(&pos);

    if(CMP_POS(pos, this->pos))
    {
        discordia_tripulante_atiende_sabotaje(this->tid);
        this->bloquear = true;
    }
    else
        tripulante_mover(this, &pos);
}
