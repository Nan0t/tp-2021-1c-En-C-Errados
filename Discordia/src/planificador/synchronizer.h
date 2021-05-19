#ifndef PLANIFICADOR_SYNCHRONIZER_H
#define PLANIFICADOR_SYNCHRONIZER_H

#include <utils/utils.h>

void     ds_synchronizer_init(uint32_t devices_count);
uint32_t ds_synchronizer_get_device_id(void);
void     ds_synchronizer_wait_for_next_cicle(uint32_t device_id);
void     ds_synchronizer_notify_end_of_cicle(void);

#endif