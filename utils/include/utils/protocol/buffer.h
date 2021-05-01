#ifndef UTILS_PROTOCOL_BUFFER_H
#define UTILS_PROTOCOL_BUFFER_H

#include "utils/typedefs.h"

typedef struct u_buffer_t u_buffer_t;

u_buffer_t* u_buffer_create(void);
void        u_buffer_delete(u_buffer_t* this);

void u_buffer_write(u_buffer_t* this, const void* data, uint64_t size);
void u_buffer_read(u_buffer_t* this, void* data, uint64_t size, uint64_t offset);

uint32_t    u_buffer_get_size(u_buffer_t* this);
const void* u_buffer_get_content(const u_buffer_t* this);

#endif