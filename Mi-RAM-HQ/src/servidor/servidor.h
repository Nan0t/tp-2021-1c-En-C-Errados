#ifndef MEMORIA_SERVIDOR_H
#define MEMORIA_SERVIDOR_H

#include <utils/utils.h>
#include <pthread.h>
#include "../memoria/admin_memoria.h"


bool servidor_init(const char* port);
void client_handler_new_conection(int32_t);

#endif