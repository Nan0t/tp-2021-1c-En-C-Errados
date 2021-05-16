#include "utils/protocol/messages/nueva_tarea.h"
#include "utils/memory/allocator.h"

u_msg_nueva_tarea_t* u_msg_nueva_tarea_crear(const char* tarea)
{
    u_msg_nueva_tarea_t* this = u_malloc(sizeof(u_msg_nueva_tarea_t));
    
    if(tarea)
    {
        this->tarea = strdup(tarea);
        this->hay_tarea = true;
    }
    else
    {
        this->tarea = NULL;
        this->hay_tarea = false;
    }

    return this;
}

u_buffer_t* u_msg_nueva_tarea_serializar(const u_msg_nueva_tarea_t* this)
{
    u_buffer_t* buffer = u_buffer_create();
    
    u_buffer_write(buffer, &this->hay_tarea, sizeof(uint8_t));

    if(this->hay_tarea)
    {
        uint32_t tarea_length = strlen(this->tarea) + 1;
        u_buffer_write(buffer, &tarea_length, sizeof(uint32_t));
        u_buffer_write(buffer, this->tarea, tarea_length);
    }

    return buffer;
}

u_msg_nueva_tarea_t* u_msg_nueva_tarea_deserializar(const u_buffer_t* buffer)
{
    u_msg_nueva_tarea_t* this = u_malloc(sizeof(u_msg_nueva_tarea_t));

    uint64_t offset = 0;

    u_buffer_read(buffer, &this->hay_tarea, sizeof(uint8_t), offset);
    offset += sizeof(uint8_t);

    if(this->hay_tarea)
    {
        uint32_t tarea_length;

        u_buffer_read(buffer, &tarea_length, sizeof(uint32_t), offset);
        offset += sizeof(uint32_t);

        this->tarea = u_malloc(tarea_length);

        u_buffer_read(buffer, this->tarea, tarea_length, offset);
    }
    else
        this->tarea = NULL;

    return this;
}

void u_msg_nueva_tarea_eliminar(u_msg_nueva_tarea_t* this)
{
    u_free(this->tarea);
    u_free(this);
}

char* u_msg_nueva_tarea_to_string(const u_msg_nueva_tarea_t* this)
{
    return string_from_format(
        "MSG_NUEVA_TAREA: { Tarea: %s }",
        this->tarea
    );
}