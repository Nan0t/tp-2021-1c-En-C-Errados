#include "utils/protocol/package.h"
#include "utils/memory/allocator.h"

u_package_t* u_package_crear(u_opcode_e opcode, const u_buffer_t* buffer)
{
    u_package_t* this = u_malloc(sizeof(u_package_t));

    this->opCodeMsg = opcode;
    this->buffer    = u_buffer_duplicate(buffer);

    return this;
}

void u_package_delete(u_package_t* this)
{
    if(this != NULL)
    {
        if(this->buffer != NULL)
            u_buffer_delete(this->buffer);

        u_free(this);
    }
}

u_buffer_t* u_package_serializar(const u_package_t* paquete)
{
    u_buffer_t* buffer = u_buffer_create();
    uint32_t paquete_buffer_size = u_buffer_get_size(paquete->buffer);

    u_buffer_write(buffer, &paquete->opCodeMsg, sizeof(uint32_t));
    u_buffer_write(buffer, &paquete_buffer_size, sizeof(uint32_t));
    u_buffer_write(buffer, u_buffer_get_content(paquete->buffer), u_buffer_get_size(paquete->buffer));

    return buffer;
}