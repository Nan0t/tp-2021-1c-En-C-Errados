#include "utils/protocol/messages/nueva_tarea.h"
#include "utils/memory/allocator.h"

u_msg_nueva_tarea_t* u_msg_nueva_tarea_crear(const char* tarea)
{
    u_msg_nueva_tarea_t* this = u_malloc(sizeof(u_msg_nueva_tarea_t));
    this->tarea = strdup(tarea);

    return this;
}

u_paquete_t* u_msg_nueva_tarea_serializar(const u_msg_nueva_tarea_t* this)
{
    u_buffer_t* buffer = u_buffer_create();
    uint32_t tarea_length = strlen(this->tarea) + 1;

    u_buffer_write(buffer, &tarea_length, sizeof(uint32_t));
    u_buffer_write(buffer, this->tarea, tarea_length);

    u_paquete_t* paquete = u_paquete_crear(NUEVA_TAREA, buffer);
    u_buffer_delete(buffer);

    return paquete;
}

u_msg_nueva_tarea_t* u_msg_nueva_tarea_deserializar(const u_buffer_t* buffer)
{
    u_msg_nueva_tarea_t* this = u_malloc(sizeof(u_msg_nueva_tarea_t));

    uint64_t offset = 0;
    uint32_t tarea_length;

    u_buffer_read(buffer, &tarea_length, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);

    this->tarea = u_malloc(tarea_length);

    u_buffer_read(buffer, this->tarea, tarea_length, offset);

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