#include "cpu.h"
#include "synchronizer.h"

private void exec_trip(tripulante_t* tripulante);

void cpu_init(void)
{
    uint32_t cpu_id = ds_synchronizer_get_device_id();

    while(1)
    {
        ds_synchronizer_wait_for_next_cicle(cpu_id);

        tripulante_t* tripulante = ds_exec_queue_pop();

        if(tripulante)
        {
            exec_trip(tripulante);
            ds_exec_queue_push(tripulante);
        }

        ds_synchronizer_notify_end_of_cicle();
    }
}

private void exec_trip(tripulante_t* tripulante)
{
    sem_post(&tripulante->sem_sync);
    sem_wait(&tripulante->sem_end_exec);
}