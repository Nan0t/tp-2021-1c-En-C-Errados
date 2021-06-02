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
    uint32_t ciclos_counter;

    pthread_mutex_t pause_mx;
    pthread_cond_t  pause_cond;
    bool            pause;

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
private void ds_planificador_admit_from_ready_to_exec(void);
private void ds_planificador_check_exec_queue(void);

void ds_planificador_init(void)
{
    if(p_planificador)
        return;

    p_planificador = u_malloc(sizeof(planificador_t));

    p_planificador->cant_cpu          = u_config_get_int_value("GRADO_MULTITAREA");
    p_planificador->quantum           = u_config_get_int_value("QUANTUM");
    p_planificador->duracion_sabotaje = u_config_get_int_value("DURACION_SABOTAJE");
    p_planificador->ciclos_counter    = 0;
    p_planificador->algorithm         = ds_planificador_select_algorithm();
    p_planificador->pause             = true;

    pthread_mutex_init(&p_planificador->pause_mx, NULL);
    pthread_cond_init(&p_planificador->pause_cond, NULL);

    ds_planificador_init_queues();
    ds_planificador_init_devices();

    U_LOG_INFO("Planificador inicializado:");
    U_LOG_INFO("  Grado Multiprocesamiento: %d", p_planificador->cant_cpu);
    U_LOG_INFO("  Duracion Sabotajes: %d", p_planificador->duracion_sabotaje);
    U_LOG_INFO("  Algoritmo Seleccionado: %s", u_config_get_string_value("ALGORITMO"));
    
    if(p_planificador->algorithm == ds_algorithm_rr)
        U_LOG_INFO("  Quantum: %d", p_planificador->quantum);

    pthread_t thread_planificador;
    U_ASSERT(pthread_create(&thread_planificador, NULL, (void*)ds_planificador_loop, NULL) != -1,
        "No se pudo crear el hilo del Planificador: %s", strerror(errno));

    pthread_detach(thread_planificador);
}

void ds_planificador_iniciar_tripulante(uint32_t pid, uint32_t tid, const u_pos_t* pos)
{
    tripulante_t* trip = tripulante_create(pid, tid, pos, p_planificador->quantum);
    tripulante_init(trip);

    ds_new_queue_push(trip);
}

void ds_planificador_eliminar_tripulante(uint32_t tid)
{
    (void)tid;
    // TODO: Eliminar un tripulante del planificador
}

void ds_planificador_iniciar(void)
{
    pthread_mutex_lock(&p_planificador->pause_mx);
    p_planificador->pause = false;
    pthread_cond_signal(&p_planificador->pause_cond);
    pthread_mutex_unlock(&p_planificador->pause_mx);
}

void ds_planificador_pausar(void)
{
    pthread_mutex_lock(&p_planificador->pause_mx);
    p_planificador->pause = true;
    pthread_mutex_unlock(&p_planificador->pause_mx);
}

private void ds_algorithm_fifo(tripulante_t* trip)
{
    if(trip->bloquear)
    {
        trip->bloquear = false;
        discordia_tripulante_nuevo_estado(trip->pid, trip->tid, 'B');
        U_LOG_INFO("Tripulante %d pasa de EXEC a BLOCK por Tarea", trip->tid);
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
        discordia_tripulante_nuevo_estado(trip->pid, trip->tid, 'B');
        U_LOG_INFO("Tripulante %d pasa de EXEC a BLOCK por Tarea", trip->tid);
        ds_block_tripulantes_add(trip);
    }
    else if(trip->quatum == 0)
    {
        trip->quatum = p_planificador->quantum;
        discordia_tripulante_nuevo_estado(trip->pid, trip->tid, 'R');
        U_LOG_INFO("Tripulante %d pasa de EXEC a READY por fin de Quatum", trip->tid);
        ds_ready_queue_push(trip);
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

extern void ds_synchronizer_execute_next_cicle(void);

private void ds_planificador_loop(void)
{
    while(1)
    {
        pthread_mutex_lock(&p_planificador->pause_mx);
        if(p_planificador->pause)
            pthread_cond_wait(&p_planificador->pause_cond, &p_planificador->pause_mx);
        pthread_mutex_unlock(&p_planificador->pause_mx);

        U_LOG_INFO("***Comienzo del ciclo: %d***", p_planificador->ciclos_counter);

        ds_planificador_admit_from_new_to_ready();
        ds_planificador_admit_from_ready_to_exec();

        ds_synchronizer_execute_next_cicle();
        ds_planificador_check_exec_queue();

        U_LOG_INFO("***Fin del ciclo: %d***\n", p_planificador->ciclos_counter ++);
    }
}

private void ds_planificador_admit_from_new_to_ready(void)
{
    tripulante_t* trip = ds_new_queue_pop();

    while(trip != NULL)
    {
        discordia_tripulante_nuevo_estado(trip->pid, trip->tid, 'R');
        U_LOG_INFO("Tripulante %d pasa de NEW a READY", trip->tid);
        ds_ready_queue_push(trip);
        trip = ds_new_queue_pop();
    }
}

private void ds_planificador_admit_from_ready_to_exec(void)
{
    while(!ds_exec_is_full())
    {
        tripulante_t* trip = ds_ready_queue_pop();
        if(trip != NULL)
        {
            discordia_tripulante_nuevo_estado(trip->pid, trip->tid, 'E');
            U_LOG_INFO("Tripulante %d pasa de READY a EXEC", trip->tid);
            ds_exec_queue_push(trip);
        }
        else
            break;
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
}