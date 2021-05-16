#ifndef DISCORDIA_H
#define DISCORDIA_H

#include <utils/utils.h>

int discordia_init(void);

char* discordia_obtener_bitacora(uint32_t tid);
char* discordia_obtener_tripulantes(void);
void  discordia_desplazamiento_tripulante(uint32_t tid, const u_pos_t* origen, const u_pos_t* destion);
void  discordia_mover_tripulante(uint32_t tid, const u_pos_t* pos);
void  discordia_iniciar_tarea(uint32_t tid, const char* tarea);
void  discordia_finalizar_tarea(uint32_t tid, const char* tarea);
void  discordia_tripulante_atiende_sabotaje(uint32_t tid);
void  discordia_tripulante_resuelve_sabotaje(uint32_t tid);
char* discordia_obtener_proxima_tarea(uint32_t tid);
void  discordia_tripulante_nuevo_estado(uint32_t tid, char estado);

void  discordia_inicializar_patota(const char* ruta_tareas, uint32_t cant_trip, t_list* posiciones_trip);
void  discordia_expulsar_tripulante(uint32_t tid);
void  discordia_iniciar_planificacion(void);
void  discordia_pausar_planificacion(void);
void  discordia_notificar_sabotaje(const u_pos_t* pos);
void  discordia_finaliza_fsck(void);

#endif