#include "thread_pool.h"
#include <pthread.h>
#include <semaphore.h>

typedef struct
{
    void* user_data;
    void(*func)(void*);
} fs_task_t;

private sem_t           p_task_queue_sem;
private pthread_mutex_t p_task_queue_mx = PTHREAD_MUTEX_INITIALIZER;
private t_queue*        p_task_queue    = NULL;

private void fs_thread_worker(void);

void fs_thread_pool_init(uint16_t amount_workers)
{
    pthread_t thread_id;
    pthread_attr_t thread_attr;

    if(p_task_queue != NULL)
        return;

    p_task_queue = queue_create();
    sem_init(&p_task_queue_sem, 0, 0);
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

    for(uint16_t i = 0; i < amount_workers; i ++)
        U_ASSERT(
            pthread_create(&thread_id, &thread_attr, (void*)fs_thread_worker, NULL) != -1,
            "No se pudo crear un nuevo thread en el sistema"
        );
}

void fs_thread_pool_push_task(void* user_data, void(*task_func)(void* user_data))
{
    fs_task_t* task = u_malloc(sizeof(fs_task_t));

    task->user_data = user_data;
    task->func      = task_func;

    pthread_mutex_lock(&p_task_queue_mx);
    queue_push(p_task_queue, task);
    pthread_mutex_unlock(&p_task_queue_mx);

    sem_post(&p_task_queue_sem);
}

private void fs_thread_worker(void)
{
    fs_task_t* task = NULL;

    while(1)
    {
        sem_wait(&p_task_queue_sem);
        
        pthread_mutex_lock(&p_task_queue_mx);
        task = queue_pop(p_task_queue);
        pthread_mutex_unlock(&p_task_queue_mx);

        task->func(task->user_data);

        u_free(task);
    }
}