#include "utils/protocol/messages/bitacora.h"
#include "utils/memory/allocator.h"

u_msg_bitacora_t* u_msg_bitacora_crear(const char* contenido_bitacora)
{
    u_msg_bitacora_t* this = u_malloc(sizeof(u_msg_bitacora_t));
    this->contenido = strdup(contenido_bitacora);

    return this;
}

u_buffer_t* u_msg_bitacora_serializar(const u_msg_bitacora_t* this)
{
    u_buffer_t* buffer = u_buffer_create();
    uint32_t bitacora_length = strlen(this->contenido) + 1;

    u_buffer_write(buffer, &bitacora_length, sizeof(uint32_t));
    u_buffer_write(buffer, this->contenido, bitacora_length);

    return buffer;
}

u_msg_bitacora_t* u_msg_bitacora_deserializar(const u_buffer_t* buffer)
{
    u_msg_bitacora_t* this = u_malloc(sizeof(u_msg_bitacora_t));

    uint64_t offset = 0;
    uint32_t bitacora_length;

    u_buffer_read(buffer, &bitacora_length, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);

    this->contenido = u_malloc(bitacora_length);

    u_buffer_read(buffer, this->contenido, bitacora_length, offset);

    return this;
}

void u_msg_bitacora_eliminar(u_msg_bitacora_t* this)
{
    u_free(this->contenido);
    u_free(this);
}

char* u_msg_bitacora_to_string(const u_msg_bitacora_t* this)
{
    return string_from_format(
        "MSG_BITACORA: { Content: %s }",
        this->contenido
    );
}
