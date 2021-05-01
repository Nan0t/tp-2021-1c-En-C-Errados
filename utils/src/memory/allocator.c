#include "utils/memory/allocator.h"
#include "utils/diagnostics/assert.h"
#include <stdlib.h>

void* u_malloc(uint64_t size)
{
    void* mem = malloc(size);
    U_ASSERT(mem != NULL, "malloc returns NULL");
    return mem;
}

void* u_realloc(void* ptr, uint64_t size)
{
    void* mem = realloc(ptr, size);
    U_ASSERT(mem != NULL, "realloc returns NULL");

    return mem;
}

void  u_free(void* ptr)
{
    U_ASSERT(ptr != NULL, "ptr is NULL");
    free(ptr);
}