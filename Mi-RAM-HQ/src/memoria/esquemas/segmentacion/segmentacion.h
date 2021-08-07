#ifndef ESQUEMA_SEGMENTACION_H
#define ESQUEMA_SEGMENTACION_H

#include "memoria/esquemas/esquema_memoria.h"

typedef struct
{
    uint32_t inicio_segmento;
    uint32_t tamanio_segmento;
    uint32_t tipo_segmento;  // 0 patota, 1 tareas, 2 tripulante, -1 vacio
    uint32_t id_propietario; // patota o tarea: pid, tripulante: tid, vacio: -1
}s_segmento_t;

typedef struct
{
    uint32_t pid;
    t_list* tabla_segmentos;
}s_patota_y_tabla_t;

typedef struct
{
	uint32_t inicio_segmento_pcb;
    uint32_t tamanio_segmento_pcb;
    uint32_t inicio_segmento_tareas;
    uint32_t tamanio_segmento_tareas;
	t_list*  listado_tripulantes;
}s_segmento_patota_t;

typedef struct
{
	uint32_t tid;
	uint32_t inicio_segmento_tcb;
    uint32_t tamanio_segmento_tcb;
}s_segmento_tripulante_t;

void segmentacion_memoria_init(void);

bool segmentacion_memoria_inicializar_patota(uint32_t pid, uint32_t cant_tripulantes, const char* tareas);
bool segmentacion_memoria_inicializar_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);

bool segmentacion_memoria_actualizar_posicion_tripulante(uint32_t pid, uint32_t tid, u_pos_t pos);
bool segmentacion_memoria_actualizar_estado_tripulante(uint32_t pid, uint32_t tid, char estado);

char*          segmentacion_memoria_obtener_proxima_tarea(uint32_t pid, uint32_t tid);
tripulantes_t* segmentacion_memoria_obtener_info_tripulante(uint32_t pid, uint32_t tid);
t_list*        segmentacion_memoria_obtener_todos_los_tripulantes(void);

bool           segmentacion_memoria_expulsar_tripulante(uint32_t pid, uint32_t tid);




#endif
