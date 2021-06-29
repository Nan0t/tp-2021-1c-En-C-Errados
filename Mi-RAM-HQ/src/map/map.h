#ifndef MEMORIA_MAP_H
#define MEMORIA_MAP_H

#include <utils/utils.h>

void map_init(void);
void map_add_trip(uint32_t tid, const u_pos_t* pos);
void map_del_trip(uint32_t tid);

#endif