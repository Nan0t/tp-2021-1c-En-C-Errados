#include "io.h"
#include "synchronizer.h"
#include "queues/block_tareas_list.h"
#include "queues/ready_queue.h"

private void manejo_de_bloqueados_por_io(void);
private void mover_tripulantes_terminados(void);

void io_init(void) {
    uint32_t io_id = ds_synchronizer_get_device_id();

    while(1) {
        ds_synchronizer_wait_for_next_cicle(io_id);

        manejo_de_bloqueados();
        mover_tripulantes_terminados();


        ds_synchronizer_notify_end_of_cicle();
    }
}

private void manejo_de_bloqueados_por_io(void) {
    void _restar_un_tiempo(tripulante_t* tripulante) {
        tripulante->tarea_actual->tiempo_bloqueado --;
    }

    list_iterate(p_block_tareas_list, (void*)_restar_un_tiempo);

}

private void mover_tripulantes_terminados() {
    bool _puede_desbloquearse(tripulante_t* tripulante) {
        return tripulante->tarea_actual->tiempo_bloqueado == 0;
    }

    while(list_any_satisfy(p_block_tareas_list, (void*)_puede_desbloquearse)) {
        tripulante_t* tripulante = list_remove_by_condition(p_block_tareas_list, (void*)_puede_desbloquearse);
        ds_ready_queue_push(tripulante);
    }
}