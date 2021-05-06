#include "utils/protocol/messages/movimiento_tripulante.h"
#include "utils/memory/allocator.h"

u_msg_movimiento_tripulante_t* u_msg_movimiento_tripulante_crear(uint32_t tid, u_pos_t pos)
{
    u_msg_movimiento_tripulante_t* this = u_malloc(sizeof(u_msg_movimiento_tripulante_t));

    this->tid = tid;
    this->pos.x = pos.x;
    this->pos.y = pos.y;

    return this;
}

u_buffer_t* u_msg_movimiento_tripulante_serializar(const u_msg_movimiento_tripulante_t* this)
{
    u_buffer_t* buffer = u_buffer_create();

    u_buffer_write(buffer, &this->tid, sizeof(uint32_t));
    u_buffer_write(buffer, &this->pos, sizeof(u_pos_t));

    return buffer;
}

u_msg_movimiento_tripulante_t* u_msg_movimiento_tripulante_deserializar(const u_buffer_t* buffer)
{
    u_pos_t pos = { 0 };
    uint32_t tid;

    uint64_t offset = 0;

    u_buffer_read(buffer, &tid, sizeof(uint32_t), offset);
    offset += sizeof(uint32_t);

    u_buffer_read(buffer, &pos, sizeof(u_pos_t), offset);

    return u_msg_movimiento_tripulante_crear(tid, pos);
}

void u_msg_movimiento_tripulante_eliminar(u_msg_movimiento_tripulante_t* this)
{
    u_free(this);
}

char* u_msg_movimiento_tripulante_to_string(const u_msg_movimiento_tripulante_t* this)
{
    return string_from_format(
        "MSG_MOVIMIENTO_TRIPULANTE: { TID: %d | Pos_X: %d | Pos_Y: %d }",
        this->tid, this->pos.x, this->pos.y
    );
}