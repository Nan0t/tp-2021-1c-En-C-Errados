#include "io.h"
#include "synchronizer.h"
#include "queues/block_tripulantes_list.h"
#include "queues/ready_queue.h"
#include "discordia/discordia.h"

private void manejo_de_bloqueado_por_io(tripulante_t* tripulante);
private void mover_tripulante_terminado(tripulante_t* tripulante);
private bool puede_desbloquearse(tripulante_t* tripulante);
private void tripulante_obtener_proxima_tarea(tripulante_t* tripulante);

void io_init(void) 
{
    uint32_t io_id = ds_synchronizer_get_device_id();
    bool se_debe_buscar_trupulante = true;
    tripulante_t* tripulante;
    while(1) 
    {
        ds_synchronizer_wait_for_next_cicle(io_id);

        if(se_debe_buscar_trupulante)
        {
            tripulante = ds_block_tripulantes_get_and_remove();
        }
        if(tripulante)
        {
            se_debe_buscar_trupulante = false;
            manejo_de_bloqueado_por_io(tripulante);

            if(puede_desbloquearse(tripulante))
            {
                mover_tripulante_terminado(tripulante);
                se_debe_buscar_trupulante = true;
                tripulante = NULL;
            }
        }
        ds_synchronizer_notify_end_of_cicle();
    }
}

private void manejo_de_bloqueado_por_io(tripulante_t* tripulante) 
{
        tripulante->tarea_actual->tiempo_bloqueado --;
}

private void mover_tripulante_terminado(tripulante_t* tripulante) 
{
        tripulante->tarea_actual->is_finished = true;
        tripulante_obtener_proxima_tarea(tripulante);
        if(tripulante->tarea_actual){
            ds_ready_queue_push(tripulante);
            discordia_tripulante_nuevo_estado(tripulante->pid, tripulante->tid, 'R');
            U_LOG_INFO("Tripulante %d pasa de BLOCK por Tareas a READY", tripulante->tid);
        }
        else
        {
            discordia_expulsar_tripulante(tripulante->tid);
            U_LOG_INFO("Tripulante %d pasa de BLOCK por Tareas a EXIT", tripulante->tid);
            // TODO:
        }
}

private bool puede_desbloquearse(tripulante_t* tripulante) 
{
    return tripulante->tarea_actual->tiempo_bloqueado == 0;
}

private void tripulante_obtener_proxima_tarea(tripulante_t* tripulante){
    char* proxima_tarea = discordia_obtener_proxima_tarea(tripulante->pid, tripulante->tid);
    tripulante->tarea_actual = parsear_tarea(proxima_tarea);
}