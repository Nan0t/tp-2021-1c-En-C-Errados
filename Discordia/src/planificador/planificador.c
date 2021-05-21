#include "discordia/discordia.h"

#include "planificador.h"
#include "synchronizer.h"
#include "io.h"

#include "queues/block_sabotaje_queue.h"
#include "queues/block_tripulantes_list.h"
#include "queues/exec_queue.h"
#include "queues/new_queue.h"
#include "queues/ready_queue.h"

#include "cpu.h"

#include <pthread.h>

#include <errno.h>
#include <string.h>

typedef void(*ds_algo_sched_func)(tripulante_t*);

typedef struct
{
    uint32_t cant_cpu;
    uint32_t quantum;
    uint32_t duracion_sabotaje;
    ds_algo_sched_func algorithm;
} planificador_t;

private planificador_t* p_planificador = NULL;

private void ds_algorithm_fifo(tripulante_t* trip);
private void ds_algorithm_rr(tripulante_t* trip);

private ds_algo_sched_func ds_planificador_select_algorithm(void);

private void ds_planificador_init_queues();
private void ds_planificador_init_devices();

private void ds_planificador_loop(void);

private void ds_planificador_admit_from_new_to_ready(void);
private void ds_planificador_check_exec_queue(void);

void planificador_init(void)
{
    if(p_planificador)
        return;

    p_planificador = u_malloc(sizeof(planificador_t));

    p_planificador->cant_cpu          = u_config_get_int_value("GRADO_MULTITAREA");
    p_planificador->quantum           = u_config_get_int_value("QUANTUM");
    p_planificador->duracion_sabotaje = u_config_get_int_value("DURACION_SABOTAJE");
    p_planificador->algorithm         = ds_planificador_select_algorithm();

    ds_planificador_init_queues();
    ds_planificador_init_devices();

    pthread_t thread_planificador;
    U_ASSERT(pthread_create(&thread_planificador, NULL, (void*)ds_planificador_loop, NULL) != -1,
        "No se pudo crear el hilo del Planificador: %s", strerror(errno));

    pthread_detach(thread_planificador);
}

void planificador_iniciar_tripulante(uint32_t tid, const u_pos_t* pos)
{
    tripulante_t* trip_info = u_malloc(sizeof(tripulante_t));

    trip_info->tid = tid;
    trip_info->pos.x = pos->x;
    trip_info->pos.y = pos->y;

    trip_info->tarea_actual = NULL;
    trip_info->bloquear     = false;

    sem_init(&trip_info->sem_sync, 0, 0);
    sem_init(&trip_info->sem_end_exec, 0, 0);
    ds_new_queue_push(trip_info);
}

bool planificador_esta_pausado(void)
{
    return true;
}

private void ds_algorithm_fifo(tripulante_t* trip)
{
    if(trip->bloquear)
    {
        trip->bloquear = false;
        ds_block_tripulantes_add(trip);
    }
    else
        ds_exec_queue_push(trip);
}

private void ds_algorithm_rr(tripulante_t* trip)
{
    trip->quatum --;

    if(trip->bloquear)
    {
        trip->bloquear = false;
        trip->quatum = p_planificador->quantum;
        ds_block_tripulantes_add(trip);
    }
    else if(trip->quatum == 0)
    {
        trip->quatum = p_planificador->quantum;
        ds_block_tripulantes_add(trip);
    }
    else
        ds_exec_queue_push(trip);
}

private ds_algo_sched_func ds_planificador_select_algorithm(void)
{
    const char* algorithm_str = u_config_get_string_value("ALGORITMO");

    if(!strcmp(algorithm_str, "RR"))
        return ds_algorithm_rr;

    return ds_algorithm_fifo;
}

private void ds_planificador_init_queues()
{
    ds_new_queue_init();
    ds_ready_queue_init();
    ds_block_sabotaje_init();
    ds_block_tripulantes_init();
    ds_exec_queue_init(p_planificador->cant_cpu);
}

private void ds_planificador_init_devices()
{
    ds_synchronizer_init(p_planificador->cant_cpu + 1);

    pthread_t thread_id;
    pthread_attr_t thread_attr;

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

    for(uint32_t i = 0; i < p_planificador->cant_cpu; i ++)
        U_ASSERT(pthread_create(&thread_id, &thread_attr, (void*)cpu_init, NULL) != -1,
            "No se pudo crear el hilo del CPU %d: %s", i, strerror(errno));

    U_ASSERT(pthread_create(&thread_id, &thread_attr, (void*)io_init, NULL) != -1,
        "No se pudo crear el hilo de I/O: %s", strerror(errno));
}

extern void ds_synchronizer_sched_notify_new_cycle(void);
extern void ds_synchronizer_sched_wait_end_of_cicle(void);

private void ds_planificador_loop(void)
{
    while(1)
    {
        ds_planificador_admit_from_new_to_ready();
        ds_planificador_check_exec_queue();

        ds_synchronizer_sched_notify_new_cycle();
        ds_synchronizer_sched_wait_end_of_cicle();
    }
}

private void ds_planificador_admit_from_new_to_ready(void)
{
    tripulante_t* trip = ds_new_queue_pop();

    while(trip != NULL)
    {
        ds_ready_queue_push(trip);
        trip = ds_new_queue_pop();
    }
}

private void ds_planificador_check_exec_queue(void)
{
    uint64_t exec_queue_size = ds_exec_queue_size();

    for(uint64_t i = 0; i < exec_queue_size; i ++)
    {
        tripulante_t* trip = ds_exec_queue_pop();
        p_planificador->algorithm(trip);
    }

    while(!ds_exec_is_full())
    {
        tripulante_t* trip = ds_ready_queue_pop();
        ds_exec_queue_push(trip);
    }
}