#include "cpu.h"

private void exec_trip(tripulante_t* tripulante);

void cpu_init(void)
{
    uint32_t cpu_id = sincronizador_generar_nuevo_id();

    while(1)
    {
        start_new_cicle(cpu_id);

        tripulante_t* tripulante = exec_queue_pop();

        if(tripulante)
            exec_trip(tripulante);

        sincronizar_cpu();
    }
}

private void exec_trip(tripulante_t* tripulante)
{
    sem_post(&tripulante->sem_sync);
    sem_wait(&tripulante->sem_end_exec);
}