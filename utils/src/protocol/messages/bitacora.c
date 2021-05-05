#include "utils/protocol/messages/bitacora.h"
#include "utils/memory/allocator.h"

u_msg_bitacora_t* u_msg_bitacora_crear(void)
{
	u_msg_bitacora_t* this = u_malloc(sizeof(u_msg_bitacora_t));

	this->movimientos         = list_create();
	this->tareas_asignadas    = list_create();
	this->tareas_finalizadas  = list_create();
	this->sabotajes_asignados = list_create();
	this->sabotajes_resueltos = list_create();

	return this;
}

void u_msg_bitacora_agregar_movimiento(u_msg_bitacora_t* this, u_trip_movimiento_t mov)
{
	u_trip_movimiento_t* new_mov = u_malloc(sizeof(u_trip_movimiento_t));

	new_mov->inicio.x = mov.inicio.x;
	new_mov->inicio.y = mov.inicio.y;
	new_mov->dest.x   = mov.dest.x;
	new_mov->dest.y   = mov.dest.y;

	list_add(this->movimientos, new_mov);
}

void u_msg_bitacora_agregar_tarea_asignada(u_msg_bitacora_t* this, const char* tarea)
{
	list_add(this->tareas_asignadas, strdup(tarea));
}

void u_msg_bitacora_agregar_tarea_finalizadas(u_msg_bitacora_t* this, const char* tarea)
{
	list_add(this->tareas_finalizadas, strdup(tarea));
}

void u_msg_bitacora_agregar_sabotaje_asignado(u_msg_bitacora_t* this, const char* sabotaje)
{
	list_add(this->sabotajes_asignados, strdup(sabotaje));
}

void u_msg_bitacora_agregar_sabotaje_resuelto(u_msg_bitacora_t* this, const char* sabotaje)
{
	list_add(this->sabotajes_resueltos, strdup(sabotaje));
}

u_paquete_t* u_msg_bitacora_serializar(const u_msg_bitacora_t* this)
{
	u_buffer_t* buffer = u_buffer_create();

	void _add_mov_to_buffer(const u_trip_movimiento_t* mov) {
		u_buffer_write(buffer, mov, sizeof(u_trip_movimiento_t));
	};

	void _add_string_to_buffer(const char* string) {
		uint32_t string_length = strlen(string) + 1;

		u_buffer_write(buffer, &string_length, sizeof(uint32_t));
		u_buffer_write(buffer, string, string_length);
	};

	u_buffer_write(buffer, &this->movimientos->elements_count, sizeof(uint32_t));
	list_iterate(this->movimientos, (void*)_add_mov_to_buffer);

	u_buffer_write(buffer, &this->tareas_asignadas->elements_count, sizeof(uint32_t));
	list_iterate(this->tareas_asignadas, (void*)_add_string_to_buffer);

	u_buffer_write(buffer, &this->tareas_finalizadas->elements_count, sizeof(uint32_t));
	list_iterate(this->tareas_finalizadas, (void*)_add_string_to_buffer);

	u_buffer_write(buffer, &this->sabotajes_asignados->elements_count, sizeof(uint32_t));
	list_iterate(this->sabotajes_asignados, (void*)_add_string_to_buffer);

	u_buffer_write(buffer, &this->sabotajes_resueltos->elements_count, sizeof(uint32_t));
	list_iterate(this->sabotajes_resueltos, (void*)_add_string_to_buffer);

	u_paquete_t* paquete = u_paquete_crear(BITACORA, buffer);
	u_buffer_delete(buffer);

	return paquete;
}

u_msg_bitacora_t* u_msg_bitacora_deserializar(const u_buffer_t* buffer)
{
	u_msg_bitacora_t* this = u_msg_bitacora_crear();

	uint64_t offset = 0;

	uint32_t cant_movimientos;
	uint32_t cant_tareas_asignadas;
	uint32_t cant_tareas_finalizadas;
	uint32_t cant_sabotajes_asignadas;
	uint32_t cant_sabotajes_resueltos;

	u_buffer_read(buffer, &cant_movimientos, sizeof(uint32_t), offset);
	offset += sizeof(uint32_t);

	for(uint32_t i = 0; i < cant_movimientos; i ++)
	{
		u_trip_movimiento_t mov = { 0 };

		u_buffer_read(buffer, &mov, sizeof(u_trip_movimiento_t), offset);
		offset += sizeof(u_trip_movimiento_t);

		u_msg_bitacora_agregar_movimiento(this, mov);
	}

	u_buffer_read(buffer, &cant_tareas_asignadas, sizeof(uint32_t), offset);
	offset += sizeof(uint32_t);

	for(uint32_t i = 0; i < cant_tareas_asignadas; i ++)
	{
		uint32_t largo_nombre_tarea;
		char* tarea;

		u_buffer_read(buffer, &largo_nombre_tarea, sizeof(uint32_t), offset);
		offset += sizeof(uint32_t);

		tarea = u_malloc(largo_nombre_tarea);

		u_buffer_read(buffer, tarea, largo_nombre_tarea, offset);
		offset += largo_nombre_tarea;

		u_msg_bitacora_agregar_tarea_asignada(this, tarea);
		u_free(tarea);
	}

	u_buffer_read(buffer, &cant_tareas_finalizadas, sizeof(uint32_t), offset);
	offset += sizeof(uint32_t);

	for(uint32_t i = 0; i < cant_tareas_finalizadas; i ++)
	{
		uint32_t largo_nombre_tarea;
		char* tarea;

		u_buffer_read(buffer, &largo_nombre_tarea, sizeof(uint32_t), offset);
		offset += sizeof(uint32_t);

		tarea = u_malloc(largo_nombre_tarea);

		u_buffer_read(buffer, tarea, largo_nombre_tarea, offset);
		offset += largo_nombre_tarea;

		u_msg_bitacora_agregar_tarea_finalizadas(this, tarea);
		u_free(tarea);
	}

	u_buffer_read(buffer, &cant_sabotajes_asignadas, sizeof(uint32_t), offset);
	offset += sizeof(uint32_t);

	for(uint32_t i = 0; i < cant_sabotajes_asignadas; i ++)
	{
		uint32_t largo_nombre_sabotaje;
		char* sabotaje;

		u_buffer_read(buffer, &largo_nombre_sabotaje, sizeof(uint32_t), offset);
		offset += sizeof(uint32_t);

		sabotaje = u_malloc(largo_nombre_sabotaje);

		u_buffer_read(buffer, sabotaje, largo_nombre_sabotaje, offset);
		offset += largo_nombre_sabotaje;

		u_msg_bitacora_agregar_sabotaje_asignado(this, sabotaje);
		u_free(sabotaje);
	}

	u_buffer_read(buffer, &cant_sabotajes_resueltos, sizeof(uint32_t), offset);
	offset += sizeof(uint32_t);

	for(uint32_t i = 0; i < cant_sabotajes_resueltos; i ++)
	{
		uint32_t largo_nombre_sabotaje;
		char* sabotaje;

		u_buffer_read(buffer, &largo_nombre_sabotaje, sizeof(uint32_t), offset);
		offset += sizeof(uint32_t);

		sabotaje = u_malloc(largo_nombre_sabotaje);

		u_buffer_read(buffer, sabotaje, largo_nombre_sabotaje, offset);
		offset += largo_nombre_sabotaje;

		u_msg_bitacora_agregar_sabotaje_resuelto(this, sabotaje);
		u_free(sabotaje);
	}

	return this;
}

void u_msg_bitacora_eliminar(u_msg_bitacora_t* this)
{
	list_destroy_and_destroy_elements(this->movimientos, (void*)u_free);
	list_destroy_and_destroy_elements(this->tareas_asignadas, (void*)u_free);
	list_destroy_and_destroy_elements(this->tareas_finalizadas, (void*)u_free);
	list_destroy_and_destroy_elements(this->sabotajes_asignados, (void*)u_free);
	list_destroy_and_destroy_elements(this->sabotajes_resueltos, (void*)u_free);

	u_free(this);
}

char* u_msg_bitacora_to_string(const u_msg_bitacora_t* this)
{
	char* bitacora_str;
	bool first_iteration;

	void _add_mov_to_string(const u_trip_movimiento_t* mov) {
		if(first_iteration)
		{
			string_append_with_format(&bitacora_str,
					"Movimiento: { Inicio: { X_POS: %d | Y_POS: %d }, Dest: { X_POS: %d | Y_POS: %d } }",
					mov->inicio.x, mov->inicio.y, mov->dest.x, mov->dest.y);
			first_iteration = false;
		}
		else
			string_append_with_format(&bitacora_str,
					", Movimiento: { Inicio: { X_POS: %d | Y_POS: %d }, Dest: { X_POS: %d | Y_POS: %d } }",
					mov->inicio.x, mov->inicio.y, mov->dest.x, mov->dest.y);
	};

	void _add_string_content_to_string(const char* string) {
		if(first_iteration)
		{
			string_append_with_format(&bitacora_str,
					"Sabotaje: %s", string);
			first_iteration = false;
		}
		else
			string_append_with_format(&bitacora_str,
					", Sabotaje: %s", string);
	};

	bitacora_str = string_duplicate("MSG_BITACORA: { Mensajes: { ");

	first_iteration = true;
	list_iterate(this->movimientos, (void*)_add_mov_to_string);

	string_append(&bitacora_str, " }, Tareas_Asignadas: { ");

	first_iteration = true;
	list_iterate(this->tareas_asignadas, (void*)_add_string_content_to_string);

	string_append(&bitacora_str, " }, Tareas_Terminadas: { ");

	first_iteration = true;
	list_iterate(this->tareas_finalizadas, (void*)_add_string_content_to_string);

	string_append(&bitacora_str, " }, Sabotajes_Asignados: { ");

	first_iteration = true;
	list_iterate(this->sabotajes_asignados, (void*)_add_string_content_to_string);

	string_append(&bitacora_str, " }, Sabotajes_Resueltos: { ");

	first_iteration = true;
	list_iterate(this->sabotajes_resueltos, (void*)_add_string_content_to_string);

	string_append(&bitacora_str, " }");

	return bitacora_str;
}
