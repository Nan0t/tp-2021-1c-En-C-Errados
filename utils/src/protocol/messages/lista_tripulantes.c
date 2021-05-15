#include "utils/protocol/messages/lista_tripulantes.h"
#include "utils/memory/allocator.h"

u_msg_lista_tripulantes_t* u_msg_lista_tripulantes_crear(void)
{
    u_msg_lista_tripulantes_t* this = u_malloc(sizeof(u_msg_lista_tripulantes_t));
    this->tripulantes = list_create();

    return this;
}

void u_msg_lista_tripulantes_agregar(u_msg_lista_tripulantes_t* this, u_tripulante_info_t trip)
{
    u_tripulante_info_t* new_trip = u_malloc(sizeof(u_tripulante_info_t));

    new_trip->pid    = trip.pid;
    new_trip->tid    = trip.tid;
    new_trip->estado = trip.estado;

    list_add(this->tripulantes, new_trip);
}

u_buffer_t* u_msg_lista_tripulantes_serialize(const u_msg_lista_tripulantes_t* this)
{
    u_buffer_t* buffer = u_buffer_create();

    void _write_in_buffer(const u_tripulante_info_t* trip) {
        u_buffer_write(buffer, &trip->pid, sizeof(uint32_t));
        u_buffer_write(buffer, &trip->tid, sizeof(uint32_t));
        u_buffer_write(buffer, &trip->estado, sizeof(char));
    };

    u_buffer_write(buffer, &this->tripulantes->elements_count, sizeof(uint32_t));
    list_iterate(this->tripulantes, (void*)_write_in_buffer);

    return buffer;
}

u_msg_lista_tripulantes_t* u_msg_lista_tripulantes_deserialize(const u_buffer_t* buffer)
{
    u_msg_lista_tripulantes_t* this = u_msg_lista_tripulantes_crear();

    uint64_t offset = 0;
    uint32_t cant_trip;

    u_buffer_read(buffer, &cant_trip, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);

    for(uint32_t i = 0; i < cant_trip; i ++)
    {
        u_tripulante_info_t* trip_info = u_malloc(sizeof(u_tripulante_info_t));

        u_buffer_read(buffer, &trip_info->pid, sizeof(uint32_t), offset);
        offset += sizeof(uint32_t);

        u_buffer_read(buffer, &trip_info->tid, sizeof(uint32_t), offset);
        offset += sizeof(uint32_t);

        u_buffer_read(buffer, &trip_info->estado, sizeof(char), offset);
        offset += sizeof(char);

        list_add(this->tripulantes, trip_info);
    }

    return this;
}

void u_msg_lista_tripulantes_eliminar(u_msg_lista_tripulantes_t* this)
{
    list_destroy_and_destroy_elements(this->tripulantes, u_free);
    u_free(this);
}

char* u_msg_lista_tripulantes_to_string(const u_msg_lista_tripulantes_t* this)
{
    char* msg_str = string_duplicate("MSG_LISTA_TRIPULANTES: { Tripulantes: { ");

    bool first_iterate = true;

    void _add_trip_to_string(const u_tripulante_info_t* trip) {
        if(first_iterate)
        {
            string_append_with_format(&msg_str,
                "Trip: { PID: %d | TID: %d | Estado: %c }",
                trip->pid, trip->tid, trip->estado);

            first_iterate = false;
        }
        else
            string_append_with_format(&msg_str,
                ", Trip: { PID: %d | TID: %d | Estado: %c }",
                trip->pid, trip->tid, trip->estado);
    };

    list_iterate(this->tripulantes, (void*)_add_trip_to_string);

    return msg_str;
}