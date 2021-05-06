#include "utils/protocol/messages/fail.h"
#include "utils/memory/allocator.h"

u_msg_fail_t* u_msg_fail_crear(const char* format, ...)
{
    u_msg_fail_t* this = u_malloc(sizeof(u_msg_fail_t));

    va_list args;
    va_start(args, format);

    this->description = string_from_vformat(format, args);

    va_end(args);

    return this;
}

u_buffer_t* u_msg_fail_serializar(const u_msg_fail_t* this)
{
    u_buffer_t* buffer = u_buffer_create();

    uint32_t desc_length = strlen(this->description) + 1;

    u_buffer_write(buffer, &desc_length, sizeof(uint32_t));
    u_buffer_write(buffer, this->description, desc_length);

    return buffer;
}

u_msg_fail_t* u_msg_fail_deserializar(const u_buffer_t* buffer)
{
    u_msg_fail_t* this = u_malloc(sizeof(u_msg_fail_t));

    uint64_t offset = 0;
    uint32_t desc_length = 0;

    u_buffer_read(buffer, &desc_length, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);

    this->description = u_malloc(desc_length);

    u_buffer_read(buffer, this->description, desc_length, offset);

    return this;
}

void u_msg_fail_eliminar(u_msg_fail_t* this)
{
    u_free(this->description);
    u_free(this);
}

char* u_msg_fail_to_string(const u_msg_fail_t* this)
{
    return string_from_format(
        "MSG_FAIL: { Description: %s }",
        this->description
    );
}