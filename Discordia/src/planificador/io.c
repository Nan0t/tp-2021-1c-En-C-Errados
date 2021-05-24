#include "io.h"
#include "synchronizer.h"
#include "queues/block_tripulantes_list.h"
#include "queues/ready_queue.h"

private void manejo_de_bloqueados_por_io(void);
private void mover_tripulantes_terminados(void);

void io_init(void) 
{
    uint32_t io_id = ds_synchronizer_get_device_id();

    while(1) 
    {
        ds_synchronizer_wait_for_next_cicle(io_id);

        manejo_de_bloqueados_por_io();
        mover_tripulantes_terminados();

        ds_synchronizer_notify_end_of_cicle();
    }
}

private void manejo_de_bloqueados_por_io(void) 
{
    void _restar_un_tiempo(tripulante_t* tripulante) 
    {
        tripulante->tarea_actual->tiempo_bloqueado --;
    }

    ds_block_tripulantes_iterate((void*)_restar_un_tiempo);
}

private void mover_tripulantes_terminados() 
{
    bool _puede_desbloquearse(tripulante_t* tripulante) 
    {
        return tripulante->tarea_actual->tiempo_bloqueado == 0;
    }

    while(ds_block_tripulantes_any_satisfy((void*)_puede_desbloquearse)) 
    {
        tripulante_t* tripulante = ds_block_tripulantes_remove_by_condition((void*)_puede_desbloquearse);
        tripulante->tarea_actual->is_finished = true;
        ds_ready_queue_push(tripulante);
    }
}