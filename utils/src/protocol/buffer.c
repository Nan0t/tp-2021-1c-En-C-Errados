#include "utils/protocol/buffer.h"
#include "utils/memory/allocator.h"
#include "utils/diagnostics/assert.h"

struct u_buffer_t
{
    uint32_t size;
    uint32_t capacity;
    void*    data;
};

#define CHECK_FOR_RESIZE(buffer, size)\
    while(buffer->size + size > buffer->capacity)\
        buffer->capacity = buffer->capacity * 3 / 2 + 1;\
    buffer->data = u_realloc(buffer->data, buffer->capacity);

#define CHECK_OUT_OF_BOUND(buffer, size, offset)\
    U_ASSERT(buffer->size >= offset + size, "Trying to access out of bound buffer memory");


u_buffer_t* u_buffer_create(void)
{
    u_buffer_t* this = u_malloc(sizeof(struct u_buffer_t));

    this->size     = 0;
    this->capacity = 0;
    this->data     = NULL;

    return this;
}

u_buffer_t* u_buffer_duplicate(const u_buffer_t* other)
{
    u_buffer_t* this = u_malloc(sizeof(struct u_buffer_t));

    this->size     = other->size;
    this->capacity = other->capacity;
    this->data     = u_malloc(this->capacity);

    memcpy(this->data, other->data, this->capacity);

    return this;
}

void u_buffer_delete(u_buffer_t* this)
{
    if(this != NULL)
    {
        if(this->data != NULL)
            u_free(this->data);

        u_free(this);
    }
}

void u_buffer_write(u_buffer_t* this, const void* data, uint64_t size)
{
    CHECK_FOR_RESIZE(this, size)

    memcpy(this->data + this->size, data, size);
    this->size += size;
}

void u_buffer_read(const u_buffer_t* this, void* data, uint64_t size, uint64_t offset)
{
    CHECK_OUT_OF_BOUND(this, size, offset)
    memcpy(data, this->data + offset, size);
}

uint32_t u_buffer_get_size(u_buffer_t* this)
{
    return this->size;
}

const void* u_buffer_get_content(const u_buffer_t* this)
{
    return this->data;
}