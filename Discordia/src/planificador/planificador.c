#include "discordia/discordia.h"
#include "planificador.h"
#include "queues/new_queue.h"
#include "cpu.h"

typedef struct
{
    uint32_t cant_cpu;
    uint32_t quantum;
    uint32_t duracion_sabotaje;
    uint32_t retardo_ciclo_cpu;
    char*    algoritmo;

    pthread_mutex_t esta_pausado_mx;
    bool            esta_pausado;
} planificador_t;

private planificador_t* p_planificador = NULL;

void planificador_init(void)
{
    if(p_planificador)
        return;

    p_planificador = u_malloc(sizeof(planificador_t));

    p_planificador->cant_cpu = u_config_get_int_value("GRADO_MULTITAREA");
    p_planificador->quantum  = u_config_get_int_value("QUANTUM");
    p_planificador->duracion_sabotaje = u_config_get_int_value("DURACION_SABOTAJE");
    p_planificador->retardo_ciclo_cpu = u_config_get_int_value("RETARDO_CICLO_CPU");
    p_planificador->algoritmo = u_config_get_string_value("ALGORITMO");

    pthread_t thread_cpu;
    
    for(uint32_t i = 0; i < p_planificador->cant_cpu; i ++)
    {
        U_ASSERT(pthread_create(&thread_cpu, NULL, (void*)cpu_init, NULL) != -1,
            "No se pudo crear el hilo de la CPU");

        pthread_detach(thread_cpu);
    }
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
    pthread_mutex_lock(&p_planificador->esta_pausado_mx);
    bool pausar = p_planificador->esta_pausado;
    pthread_mutex_unlock(&p_planificador->esta_pausado_mx);

    return pausar;
}