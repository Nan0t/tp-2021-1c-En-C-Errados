#ifndef INCLUDE_UTILS_PROTOCOL_MESSAGES_BITACORA_H_
#define INCLUDE_UTILS_PROTOCOL_MESSAGES_BITACORA_H_

#include "utils/protocol/paquete.h"

typedef struct
{
	u_pos_t inicio;
	u_pos_t dest;
} u_trip_movimiento_t;

typedef struct
{
	t_list* movimientos;
	t_list* tareas_asignadas;
	t_list* tareas_finalizadas;
	t_list* sabotajes_asignados;
	t_list* sabotajes_resueltos;
} u_msg_bitacora_t;

u_msg_bitacora_t* u_msg_bitacora_crear(void);

void u_msg_bitacora_agregar_movimiento(u_msg_bitacora_t* this, u_trip_movimiento_t mov);
void u_msg_bitacora_agregar_tarea_asignada(u_msg_bitacora_t* this, const char* tarea);
void u_msg_bitacora_agregar_tarea_finalizadas(u_msg_bitacora_t* this, const char* tarea);
void u_msg_bitacora_agregar_sabotaje_asignado(u_msg_bitacora_t* this, const char* sabotaje);
void u_msg_bitacora_agregar_sabotaje_resuelto(u_msg_bitacora_t* this, const char* sabotaje);

u_paquete_t*      u_msg_bitacora_serializar(const u_msg_bitacora_t* this);
u_msg_bitacora_t* u_msg_bitacora_deserializar(const u_buffer_t* buffer);

void u_msg_bitacora_eliminar(u_msg_bitacora_t* this);

char* u_msg_bitacora_to_string(const u_msg_bitacora_t* this);

#endif /* INCLUDE_UTILS_PROTOCOL_MESSAGES_BITACORA_H_ */
