#include "cpu.h"
#include "queues/queue_manager.h"
#include "synchronizer.h"

#include <pthread.h>

private void exec_trip(tripulante_t* tripulante);

void cpu_init(void)
{
    uint32_t cpu_id = ds_synchronizer_get_device_id();

    while(1)
    {
        ds_synchronizer_wait_for_next_cicle(cpu_id);

        ds_queue_mt_t* exec_queue = ds_queue_manager_hold(DS_QUEUE_EXEC);
        tripulante_t* tripulante = ds_queue_mt_pop(exec_queue);
        ds_queue_manager_release(DS_QUEUE_EXEC);

        if(tripulante)
        {
            exec_trip(tripulante);

            ds_queue_manager_hold(DS_QUEUE_EXEC);
            ds_queue_mt_push(exec_queue, tripulante);
            ds_queue_manager_release(DS_QUEUE_EXEC);
        }

        ds_synchronizer_notify_end_of_cicle();
    }
}

private void exec_trip(tripulante_t* tripulante)
{
    sem_post(&tripulante->sem_sync);
    sem_wait(&tripulante->sem_end_exec);
}