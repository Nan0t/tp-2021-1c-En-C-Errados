#include "queue_manager.h"

typedef struct
{
    ds_queue_mt_t* queues[5];
} ds_queue_manager_t;

private ds_queue_manager_t* p_queue_manager_instances = NULL;

extern void ds_queue_mt_hold(ds_queue_mt_t* queue);
extern void ds_queue_mt_release(ds_queue_mt_t* queue);

void ds_queue_manager_init(void)
{
    if(p_queue_manager_instances)
        return;

    uint32_t cpu_count = u_config_get_int_value("GRADO_MULTITAREA");

    p_queue_manager_instances = u_malloc(sizeof(ds_queue_manager_t));

    p_queue_manager_instances->queues[DS_QUEUE_NEW]      = ds_queue_mt_create(0);
    p_queue_manager_instances->queues[DS_QUEUE_EXEC]     = ds_queue_mt_create(cpu_count);
    p_queue_manager_instances->queues[DS_QUEUE_READY]    = ds_queue_mt_create(0);
    p_queue_manager_instances->queues[DS_QUEUE_BLOCK]    = ds_queue_mt_create(0);
    p_queue_manager_instances->queues[DS_QUEUE_SABOTAGE] = ds_queue_mt_create(0);
}

ds_queue_mt_t* ds_queue_manager_hold(ds_queue_type_e queue_type)
{
    ds_queue_mt_t* queue = p_queue_manager_instances->queues[queue_type];
    ds_queue_mt_hold(queue);

    return queue;
}

void ds_queue_manager_release(ds_queue_type_e queue_type)
{
    ds_queue_mt_release(p_queue_manager_instances->queues[queue_type]);
}