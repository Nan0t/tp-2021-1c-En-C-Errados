// #include "discordia/discordia.h"

#include "planificador.h"
#include "synchronizer.h"
#include "io.h"
#include "queues/queue_manager.h"
#include "cpu.h"

#include <pthread.h>

#include <errno.h>
#include <string.h>

typedef void(*ds_algo_sched_func)(void);

typedef struct
{
    uint32_t            cant_cpu;
    uint32_t            quantum;
    uint32_t            duracion_sabotaje;
    uint32_t            ciclos_counter;

    pthread_mutex_t     sabotaje_pos_mx;
    u_pos_t             sabotaje_pos;

    pthread_mutex_t     pause_mx;
    pthread_cond_t      pause_cond;
    bool                pause;

    pthread_mutex_t     esta_en_sabotaje_mx;
    bool                esta_en_sabotaje;

    pthread_mutex_t     inicializar_rutina_sabotaje_mx;
    bool                inicializar_rutina_sabotaje;

    pthread_mutex_t     expulsar_trip_request_queue_mx;
    t_queue*            expulsar_trip_request_queue;

    ds_algo_sched_func  algorithm;
} planificador_t;

private planificador_t* p_planificador = NULL;

private void ds_algorithm_fifo(void);
private void ds_algorithm_rr(void);

private ds_algo_sched_func ds_planificador_select_algorithm(void);

private void ds_planificador_init_queues(void);
private void ds_planificador_init_devices(void);
private void ds_planificador_init_rutina_sabotaje(const u_pos_t* pos);

private void ds_planificador_loop(void);
private void ds_planificador_check_pausa(void);
private bool ds_planificador_should_init_sabotaje_routine(void);

private void ds_planificador_admit_from_new_to_ready(void);
private void ds_planificador_admit_from_ready_to_exec(void);
private void ds_planificador_check_exec_queue(void);
private void ds_planificador_check_exp_trip_requests(void);

private void ds_planificador_find_and_terminate_trip(uint32_t tid);
private void ds_planificador_find_and_terminate_from_new(uint32_t tid);
private void ds_planificador_find_and_terminate_from_ready(uint32_t tid);
private void ds_planificador_find_and_terminate_from_exec(uint32_t tid);
private void ds_planificador_find_and_terminate_from_block_sabotage(uint32_t tid);

private void ds_planificador_move_from_ready_to_blocked_by_sabotage(void);
private tripulante_t* ds_planificador_find_tripulante_closer_to_sabotage(const uint32_t pos_unificado_sabotaje);
private void ds_planificador_move_choosen_tripulante_to_ready(tripulante_t* tripulante);

void ds_planificador_init(void)
{
    if(p_planificador)
        return;

    p_planificador = u_malloc(sizeof(planificador_t));

    p_planificador->cant_cpu                    = u_config_get_int_value("GRADO_MULTITAREA");
    p_planificador->quantum                     = u_config_get_int_value("QUANTUM");
    p_planificador->duracion_sabotaje           = u_config_get_int_value("DURACION_SABOTAJE");
    p_planificador->ciclos_counter              = 0;
    p_planificador->sabotaje_pos.x              = 0;
    p_planificador->sabotaje_pos.y              = 0;
    p_planificador->pause                       = true;
    p_planificador->esta_en_sabotaje            = false;
    p_planificador->inicializar_rutina_sabotaje = false;
    p_planificador->expulsar_trip_request_queue = queue_create();
    p_planificador->algorithm                   = ds_planificador_select_algorithm();

    pthread_mutex_init(&p_planificador->sabotaje_pos_mx, NULL);
    
    pthread_mutex_init(&p_planificador->pause_mx, NULL);
    pthread_cond_init(&p_planificador->pause_cond, NULL);

    pthread_mutex_init(&p_planificador->esta_en_sabotaje_mx, NULL);
    pthread_mutex_init(&p_planificador->inicializar_rutina_sabotaje_mx, NULL);
    pthread_mutex_init(&p_planificador->expulsar_trip_request_queue_mx, NULL);

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

    ds_queue_mt_t* new_queue = ds_queue_manager_hold(DS_QUEUE_NEW);
    ds_queue_mt_push(new_queue, trip);
    ds_queue_manager_release(DS_QUEUE_NEW);
}

void ds_planificador_eliminar_tripulante(uint32_t tid)
{
    pthread_mutex_lock(&p_planificador->pause_mx);
    if(p_planificador->pause)
        ds_planificador_find_and_terminate_trip(tid);
    else
    {
        uint32_t* tid_ptr = u_malloc(sizeof(uint32_t));
        *tid_ptr = tid;

        pthread_mutex_lock(&p_planificador->expulsar_trip_request_queue_mx);
        queue_push(p_planificador->expulsar_trip_request_queue, tid_ptr);
        pthread_mutex_unlock(&p_planificador->expulsar_trip_request_queue_mx);
    }
    pthread_mutex_unlock(&p_planificador->pause_mx);
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

void ds_planificador_notificar_sabotaje(const u_pos_t* pos)
{
    p_planificador->sabotaje_pos.x = pos->x;
    p_planificador->sabotaje_pos.y = pos->y;

    pthread_mutex_lock(&p_planificador->inicializar_rutina_sabotaje_mx);
    p_planificador->inicializar_rutina_sabotaje = true;
    pthread_mutex_unlock(&p_planificador->inicializar_rutina_sabotaje_mx);

    pthread_mutex_lock(&p_planificador->esta_en_sabotaje_mx);
    p_planificador->esta_en_sabotaje = true;
    pthread_mutex_lock(&p_planificador->esta_en_sabotaje_mx);
}

bool ds_planificador_esta_en_sabotaje(void)
{
    pthread_mutex_lock(&p_planificador->esta_en_sabotaje_mx);
    bool esta_en_sabotaje = p_planificador->esta_en_sabotaje;
    pthread_mutex_unlock(&p_planificador->esta_en_sabotaje_mx);

    return esta_en_sabotaje;
}

void ds_planificador_get_pos_sabotaje(u_pos_t* pos)
{
    pthread_mutex_lock(&p_planificador->sabotaje_pos_mx);
    pos->x = p_planificador->sabotaje_pos.x;
    pos->y = p_planificador->sabotaje_pos.y;
    pthread_mutex_unlock(&p_planificador->sabotaje_pos_mx);
}

private void ds_algorithm_fifo(void)
{
    ds_queue_mt_t* exec_queue = ds_queue_manager_hold(DS_QUEUE_EXEC);
    
    DS_QUEUE_MT_FOREACH(exec_queue)

        tripulante_t* trip = ds_queue_mt_iterator_next(it);

        if(trip->bloquear)
        {
            ds_queue_mt_iterator_remove(it);
            trip->bloquear = false;
            
            if(!p_planificador->esta_en_sabotaje)
            {
                tripulante_change_state(trip, TRIP_STATE_BLOCK_IO);

                ds_queue_mt_t* block_queue = ds_queue_manager_hold(DS_QUEUE_BLOCK);
                ds_queue_mt_push(block_queue, trip);
                ds_queue_manager_release(DS_QUEUE_BLOCK);
            }
            else
            {
                tripulante_change_state(trip, TRIP_STATE_BLOCK_SABOTAGE);

                ds_queue_mt_t* block_queue_sabotaje = ds_queue_manager_hold(DS_QUEUE_SABOTAGE);
                ds_queue_mt_push(block_queue_sabotaje, trip);
                ds_queue_manager_release(DS_QUEUE_SABOTAGE);

                // TODO: Inicializar rutina de desbloquea y reanudacion de planificacion.
            }
        }
        
    DS_QUEUE_MT_END_FOREACH

    ds_queue_manager_release(DS_QUEUE_EXEC);
}

private void ds_algorithm_rr(void)
{
    ds_queue_mt_t* exec_queue = ds_queue_manager_hold(DS_QUEUE_EXEC);

    DS_QUEUE_MT_FOREACH(exec_queue)
        tripulante_t* trip = ds_queue_mt_iterator_next(it);

        trip->quatum --;

        if(trip->bloquear)
        {
            ds_queue_mt_iterator_remove(it);

            trip->bloquear = false;
            trip->quatum   = p_planificador->quantum;

            if(!p_planificador->esta_en_sabotaje)
            {
                tripulante_change_state(trip, TRIP_STATE_BLOCK_IO);

                ds_queue_mt_t* block_queue = ds_queue_manager_hold(DS_QUEUE_BLOCK);
                ds_queue_mt_push(block_queue, trip);
                ds_queue_manager_release(DS_QUEUE_BLOCK);
            }
            else
            {
                tripulante_change_state(trip, TRIP_STATE_BLOCK_SABOTAGE);

                ds_queue_mt_t* block_queue_sabotaje = ds_queue_manager_hold(DS_QUEUE_SABOTAGE);
                ds_queue_mt_push(block_queue_sabotaje, trip);
                ds_queue_manager_release(DS_QUEUE_SABOTAGE);

                // TODO: Inicializar rutina de desbloquea y reanudacion de planificacion.
            }
        }
        else if(trip->quatum == 0)
        {
            ds_queue_mt_iterator_remove(it);

            trip->quatum = p_planificador->quantum;
            tripulante_change_state(trip, TRIP_STATE_READY);

            ds_queue_mt_t* ready_queue = ds_queue_manager_hold(DS_QUEUE_READY);
            ds_queue_mt_push(ready_queue, trip);
            ds_queue_manager_release(DS_QUEUE_READY);
        }
    DS_QUEUE_MT_END_FOREACH
    
    ds_queue_manager_release(DS_QUEUE_EXEC);
}

private ds_algo_sched_func ds_planificador_select_algorithm(void)
{
    const char* algorithm_str = u_config_get_string_value("ALGORITMO");

    if(!strcmp(algorithm_str, "RR"))
        return ds_algorithm_rr;

    return ds_algorithm_fifo;
}

private void ds_planificador_init_queues(void)
{
    ds_queue_manager_init();
}

private void ds_planificador_init_devices(void)
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

        ds_planificador_check_pausa();

        U_LOG_INFO("***Comienzo del ciclo: %d***", p_planificador->ciclos_counter);

        if(!ds_planificador_esta_en_sabotaje())
            ds_planificador_admit_from_new_to_ready();
        else if(ds_planificador_should_init_sabotaje_routine())
        {
            pthread_mutex_lock(&p_planificador->sabotaje_pos_mx);
            ds_planificador_init_rutina_sabotaje(&p_planificador->sabotaje_pos);
            pthread_mutex_unlock(&p_planificador->sabotaje_pos_mx);
        }

        ds_planificador_admit_from_ready_to_exec();

        ds_synchronizer_execute_next_cicle();
        ds_planificador_check_exec_queue();
        ds_planificador_check_exp_trip_requests();

        U_LOG_INFO("***Fin del ciclo: %d***\n", p_planificador->ciclos_counter ++);
    }
}

private void ds_planificador_check_pausa(void)
{
    pthread_mutex_lock(&p_planificador->pause_mx);
    if(p_planificador->pause)
        pthread_cond_wait(&p_planificador->pause_cond, &p_planificador->pause_mx);
    pthread_mutex_unlock(&p_planificador->pause_mx);
}

private bool ds_planificador_should_init_sabotaje_routine(void)
{
    pthread_mutex_lock(&p_planificador->inicializar_rutina_sabotaje_mx);
    bool should_init = p_planificador->inicializar_rutina_sabotaje;
    pthread_mutex_unlock(&p_planificador->inicializar_rutina_sabotaje_mx);

    return should_init;
}

private void ds_planificador_admit_from_new_to_ready(void)
{
    ds_queue_mt_t* new_queue = ds_queue_manager_hold(DS_QUEUE_NEW);
    tripulante_t* trip = ds_queue_mt_pop(new_queue);

    while(trip != NULL)
    {
        tripulante_change_state(trip, TRIP_STATE_READY);
        
        ds_queue_mt_t* ready_queue = ds_queue_manager_hold(DS_QUEUE_READY);
        ds_queue_mt_push(ready_queue, trip);
        ds_queue_manager_release(DS_QUEUE_READY);

        trip = ds_queue_mt_pop(new_queue);
    }

    ds_queue_manager_release(DS_QUEUE_NEW);
}

private void ds_planificador_admit_from_ready_to_exec(void)
{
    ds_queue_mt_t* exec_queue = ds_queue_manager_hold(DS_QUEUE_EXEC);

    while(!ds_queue_mt_is_full(exec_queue))
    {
        ds_queue_mt_t* ready_queue = ds_queue_manager_hold(DS_QUEUE_READY);
        tripulante_t* trip = ds_queue_mt_pop(ready_queue);
        ds_queue_manager_release(DS_QUEUE_READY);

        if(trip != NULL)
        {
            tripulante_change_state(trip, TRIP_STATE_EXEC);
            ds_queue_mt_push(exec_queue, trip);
        }
        else
            break;
    }

    ds_queue_manager_release(DS_QUEUE_EXEC);
}

private void ds_planificador_check_exec_queue(void)
{
    p_planificador->algorithm();
}

private void ds_planificador_check_exp_trip_requests(void)
{
    pthread_mutex_lock(&p_planificador->expulsar_trip_request_queue_mx);

    while(!queue_is_empty(p_planificador->expulsar_trip_request_queue))
    {
        uint32_t* tid_ptr = queue_pop(p_planificador->expulsar_trip_request_queue);
        ds_planificador_find_and_terminate_trip(*tid_ptr);

        u_free(tid_ptr);
    }

    pthread_mutex_unlock(&p_planificador->expulsar_trip_request_queue_mx);
}

private void ds_planificador_find_and_terminate_trip(uint32_t tid)
{
    ds_planificador_find_and_terminate_from_new(tid);
}

private void ds_planificador_find_and_terminate_from_new(uint32_t tid)
{
    ds_queue_mt_t* new_queue = ds_queue_manager_hold(DS_QUEUE_NEW);
    bool keep_looking = true;

    DS_QUEUE_MT_FOREACH(new_queue)

        tripulante_t* trip = ds_queue_mt_iterator_next(it);
        if(trip->tid == tid)
        {
            ds_queue_mt_iterator_remove(it);
            tripulante_terminate(trip);

            keep_looking = false;

            DS_QUEUE_MT_BREAK_FOREACH
        }

    DS_QUEUE_MT_END_FOREACH

    ds_queue_manager_release(DS_QUEUE_NEW);

    if(keep_looking)
        ds_planificador_find_and_terminate_from_ready(tid);
}

private void ds_planificador_find_and_terminate_from_ready(uint32_t tid)
{
    ds_queue_mt_t* ready_queue = ds_queue_manager_hold(DS_QUEUE_READY);
    bool keep_looking = true;

    DS_QUEUE_MT_FOREACH(ready_queue)

        tripulante_t* trip = ds_queue_mt_iterator_next(it);
        if(trip->tid == tid)
        {
            ds_queue_mt_iterator_remove(it);
            tripulante_terminate(trip);

            keep_looking = false;

            DS_QUEUE_MT_BREAK_FOREACH
        }

    DS_QUEUE_MT_END_FOREACH

    ds_queue_manager_release(DS_QUEUE_READY);

    if(keep_looking)
        ds_planificador_find_and_terminate_from_exec(tid);
}

private void ds_planificador_find_and_terminate_from_exec(uint32_t tid)
{
    ds_queue_mt_t* exec_queue = ds_queue_manager_hold(DS_QUEUE_EXEC);
    bool keep_looking = true;

    DS_QUEUE_MT_FOREACH(exec_queue)

        tripulante_t* trip = ds_queue_mt_iterator_next(it);
        if(trip->tid == tid)
        {
            ds_queue_mt_iterator_remove(it);
            tripulante_terminate(trip);

            keep_looking = false;

            DS_QUEUE_MT_BREAK_FOREACH
        }

    DS_QUEUE_MT_END_FOREACH

    ds_queue_manager_release(DS_QUEUE_EXEC);

    if(keep_looking)
        ds_planificador_find_and_terminate_from_block_sabotage(tid);
}

private void ds_planificador_find_and_terminate_from_block_sabotage(uint32_t tid)
{
    ds_queue_mt_t* sabotage_queue = ds_queue_manager_hold(DS_QUEUE_SABOTAGE);
    bool keep_looking = true;

    DS_QUEUE_MT_FOREACH(sabotage_queue)

        tripulante_t* trip = ds_queue_mt_iterator_next(it);
        if(trip->tid == tid)
        {
            ds_queue_mt_iterator_remove(it);
            tripulante_terminate(trip);

            keep_looking = false;

            DS_QUEUE_MT_BREAK_FOREACH
        }

    DS_QUEUE_MT_END_FOREACH

    ds_queue_manager_release(DS_QUEUE_SABOTAGE);

    if(keep_looking)
        io_find_and_terminate_from_block(tid);
}

// =======================
// ***Rutina Sabotaje***
// =======================
private void ds_planificador_init_rutina_sabotaje(const u_pos_t* pos)
{
    uint32_t pos_unificado_sabotaje = pos->x + pos->y;

    pthread_mutex_lock(&p_planificador->inicializar_rutina_sabotaje_mx);
    p_planificador->inicializar_rutina_sabotaje = false;
    pthread_mutex_unlock(&p_planificador->inicializar_rutina_sabotaje_mx);

    ds_planificador_move_from_ready_to_blocked_by_sabotage();

    tripulante_t* tripulante_elegido = ds_planificador_find_tripulante_closer_to_sabotage(pos_unificado_sabotaje);

    ds_planificador_move_choosen_tripulante_to_ready(tripulante_elegido);
}

private void ds_planificador_move_from_ready_to_blocked_by_sabotage()
{
    ds_queue_mt_t* ready = ds_queue_manager_hold(DS_QUEUE_READY);

    while(ds_queue_mt_get_size(ready) != 0)
    {
        tripulante_t* trip = ds_queue_mt_pop(ready);

        ds_queue_mt_t* bloqueo_sabotaje = ds_queue_manager_hold(DS_QUEUE_SABOTAGE);
        ds_queue_mt_push(bloqueo_sabotaje, trip);
        ds_queue_manager_release(DS_QUEUE_SABOTAGE);
    }

    ds_queue_manager_release(DS_QUEUE_READY);
}

private tripulante_t* ds_planificador_find_tripulante_closer_to_sabotage(const uint32_t pos_unificado_sabotaje) 
{
    uint32_t pos_minima;
    uint32_t tid;

    ds_queue_mt_t* bloqueo_sabotaje = ds_queue_manager_hold(DS_QUEUE_SABOTAGE);
    DS_QUEUE_MT_FOREACH(bloqueo_sabotaje)
    {

        tripulante_t* trip = ds_queue_mt_iterator_next(it);
        uint32_t distancia_trip = pos_unificado_sabotaje - (trip->pos.x + trip->pos.y);

        if(pos_minima > distancia_trip) {
            pos_minima = distancia_trip;
            tid = trip->tid;
        }
    }
    DS_QUEUE_MT_END_FOREACH

    tripulante_t* trip_encontrado = ds_queue_mt_pop_by_tid(bloqueo_sabotaje, tid);

    return trip_encontrado;
}

private void ds_planificador_move_choosen_tripulante_to_ready(tripulante_t* tripulante)
{
    ds_queue_manager_release(DS_QUEUE_SABOTAGE);

    tripulante_change_state(tripulante, TRIP_STATE_READY); //NO SE SI TENDRÍA QUE TENER OTRO ESTADO POR PASAR A READY PERO POR SABOTAJE
    ds_queue_mt_t* ready_queue = ds_queue_manager_hold(DS_QUEUE_READY);

    ds_queue_mt_push(ready_queue, tripulante);

    ds_queue_manager_release(DS_QUEUE_READY);
}