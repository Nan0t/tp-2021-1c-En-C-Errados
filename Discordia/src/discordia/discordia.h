#ifndef DISCORDIA_H
#define DISCORDIA_H

#include <utils/utils.h>

char* discordia_obtener_bitacora(uint32_t tid);
char* discordia_obtener_tripulantes(void);
void  discordia_inicializar_patota(const char* ruta_tareas, uint32_t cant_trip, t_list* posiciones_trip);
void  discordia_expulsar_tripulante(uint32_t tid);
void  discordia_iniciar_planificacion(void);
void  discordia_pausar_planificacion(void);
void  discordia_notificar_sabotaje(const u_pos_t* pos);
void  discordia_finaliza_fsck(void);

#endif