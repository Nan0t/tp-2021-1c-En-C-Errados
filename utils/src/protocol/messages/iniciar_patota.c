#include "utils/protocol/messages/iniciar_patota.h"
#include "utils/memory/allocator.h"

u_msg_iniciar_patota_t* u_msg_iniciar_patota_crear(uint32_t pid, const char* lista_tareas)
{
    u_msg_iniciar_patota_t* this = u_malloc(sizeof(u_msg_iniciar_patota_t));

    this->pid = pid;
    this->lista_tareas = strdup(lista_tareas);

    return this;
}

u_paquete_t* u_msg_iniciar_patota_serializar(const u_msg_iniciar_patota_t* this)
{
    u_buffer_t* buffer = u_buffer_create();
    uint32_t lista_tarea_length = strlen(this->lista_tareas) + 1;

    u_buffer_write(buffer, &this->pid, sizeof(uint32_t));
    u_buffer_write(buffer, &lista_tarea_length, sizeof(uint32_t));
    u_buffer_write(buffer, this->lista_tareas, lista_tarea_length);

    u_paquete_t* paquete = u_paquete_crear(INICIAR_PATOTA, buffer);
    u_buffer_delete(buffer);

    return paquete;
}

u_msg_iniciar_patota_t* u_msg_iniciar_patota_deserializar(const u_buffer_t* buffer)
{
    u_msg_iniciar_patota_t* this = u_malloc(sizeof(u_msg_iniciar_patota_t));

    uint64_t offset = 0;
    uint32_t lista_tarea_length;

    u_buffer_read(buffer, &this->pid, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);

    u_buffer_read(buffer, &lista_tarea_length, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);

    this->lista_tareas = u_malloc(lista_tarea_length);

    u_buffer_read(buffer, this->lista_tareas, lista_tarea_length, offset);

    return this;
}

void u_msg_iniciar_patota_eliminar(u_msg_iniciar_patota_t* this)
{
    u_free(this->lista_tareas);
    u_free(this);
}

char* u_msg_iniciar_patota_to_string(const u_msg_iniciar_patota_t* this)
{
    return string_from_format(
        "MSG_INICIAR_PATOTA: { PID: %d | Lista_Tareas: %s }",
        this->pid, this->lista_tareas
    );
}