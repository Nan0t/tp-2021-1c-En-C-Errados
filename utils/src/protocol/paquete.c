#include "utils/protocol/paquete.h"
#include "utils/memory/allocator.h"

u_paquete_t* u_paquete_crear(u_opcode_e opcode, const u_buffer_t* buffer)
{
    u_paquete_t* this = u_malloc(sizeof(u_paquete_t));

    this->opCodeMsg = opcode;
    this->buffer    = u_buffer_duplicate(buffer);

    return this;
}

void u_paquete_delete(u_paquete_t* this)
{
    if(this != NULL)
    {
        if(this->buffer != NULL)
            u_buffer_delete(this->buffer);

        u_free(this);
    }
}

u_buffer_t* u_paquete_serializar(const u_paquete_t* paquete)
{
    u_buffer_t* buffer = u_buffer_create();
    uint32_t paquete_buffer_size = u_buffer_get_size(paquete->buffer);

    u_buffer_write(buffer, &paquete->opCodeMsg, sizeof(uint32_t));
    u_buffer_write(buffer, &paquete_buffer_size, sizeof(uint32_t));
    u_buffer_write(buffer, u_buffer_get_content(paquete->buffer), u_buffer_get_size(paquete->buffer));

    return buffer;
}