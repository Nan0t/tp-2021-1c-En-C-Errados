#ifndef DISCORDIA_H
#define DISCORDIA_H

#include <utils/utils.h>

/**
 * @NAME: discordia_init
 * @DESC: inicia el modulo Discordiador
 */
int discordia_init(void);

/**
 * @NAME: discordia_obtener_bitacora.
 * @DESC: dado el id de un tripulante, obtiene su bitacora.
 * @PARAMS:
 *  [in] uint32_t tid - id del tripulante
 * 
 * @RETURNS: la bitacora del tripulante, o NULL en caso de que no exista la bitacora del tripulante.
 */
char* discordia_obtener_bitacora(uint32_t tid);

/**
 * @NAME: discordia_obtener_tripulantes
 * @DESC: consulta el estado de los tripulantes en el sistema
 * 
 * @RETURNS: un string con la informacion del estado de los tripulantes.
 */
char* discordia_obtener_tripulantes(void);

/**
 * @NAME: discordia_desplazamiento_tripulante
 * @DESC: logea el movimiento de un tripulante en su bitacora.
 * @PARAMS:
 *  [in] uint32_t       tid     - id del tripulante
 *  [in] const u_pos_t* origen  - posicion de origen del tripulante.
 *  [in] const u_pos_t* destion - posicion a donde se movera el tripulante.
 */
void  discordia_desplazamiento_tripulante(uint32_t tid, const u_pos_t* origen, const u_pos_t* destion);

/**
 * @NAME: discordia_mover_tripulante
 * @DESC: actualiza la posicion de un tripulante.
 * @PARAMS:
 *  [in] uint32_t pid       - id de la patota del tripulante.
 *  [in] uint32_t tid       - id del tripulante.
 *  [in] const u_pos_t* pos - posicion a la cual se movera el tripulante.
 */
void  discordia_mover_tripulante(uint32_t pid, uint32_t tid, const u_pos_t* pos);

/**
 * @NAME: discordia_iniciar_tarea
 * @DESC: logea e inicializa la tarea de un tripulante.
 * @PARAMS:
 *  [in] uint32_t    tid   - id del tripulante.
 *  [in] const char* tarea - tarea a realiazar por el tripulante.
 */
void  discordia_iniciar_tarea(uint32_t tid, const char* tarea);

/**
 * @NAME: discordia_finalizar_tarea
 * @DESC: logea en la bitacora del tripulante que finalizo una tarea
 * @PARAMS:
 *  [in] uint32_t    tid   - id del tripulante.
 *  [in] const char* tarea - tarea que finalizo el tripulante.
 */
void  discordia_finalizar_tarea(uint32_t tid, const char* tarea);

/**
 * @NAME: discordia_tripulante_atiende_sabotaje
 * @DESC: logea y notifica al file system que un tripulante atendio una tarea. Esto implica la inicializacion
 *          de la rutina FSCK en el file system.
 * @PARAMS:
 *  [in] uint32_t tid - id del tripulante.
 */
void  discordia_tripulante_atiende_sabotaje(uint32_t tid);

/**
 * @NAME: discordia_tripulante_resuelve_sabotaje
 * @DESC: logea en la bitacora del tripulante que termino de resolver el sabotaje.
 * @PARAMS:
 *  [in] uint32_t tid - id del tripulante
 */
void  discordia_tripulante_resuelve_sabotaje(uint32_t tid);

/**
 * @NAME: discordia_obtener_proxima_tarea
 * @DESC: solicita la proxima tarea de un tripulante dado.
 * @PARAMS:
 *  [in] uint32_t pid - id de la patota del tripulante.
 *  [in] uint32_t tid - id del tripulante.
 * 
 * @RETURNS: la proxima tarea a realizar por el tripulante, un string vacio en caso de que no haya mas tareas
 *              o NULL en caso de error.
 */
char* discordia_obtener_proxima_tarea(uint32_t pid, uint32_t tid);

/**
 * @NAME: discordia_tripulante_nuevo_estado
 * @DESC: actualiza el estado de un tripulante.
 * @PARAMS:
 *  [in] uint32_t pid    - id de la patota del tripulante.
 *  [in] uint32_t tid    - id del tripulante.
 *  [in] char     estado - el nuevo estado del tripulante.
 */
void  discordia_tripulante_nuevo_estado(uint32_t pid, uint32_t tid, char estado);

/**
 * @NAME: discordia_inicializar_patota
 * @DESC: inicializa una nueva patota en el sistema.
 * @PARAMS:
 *  [in] const char* ruta_tareas     - ruta del archivo de tareas de la patota.
 *  [in] uint32_t    cant_trip       - cantidad de tripulantes de la patota.
 *  [in] t_list*     posiciones_trip - posiciones de cada uno de los tripulantes.
 */
void  discordia_inicializar_patota(const char* ruta_tareas, uint32_t cant_trip, t_list* posiciones_trip);

/**
 * @NAME: discordia_expulsar_tripulante
 * @DESC: solicita la expulsion de un tripulante dado.
 * @PARAMS:
 *  [in] uint32_t tid - id del tripulante.
 */
void  discordia_expulsar_tripulante(uint32_t tid);

/**
 * @NAME: discordia_eliminar_tripulante
 * @DESC: elimina y libera los recursos de un tripulante del sistema.
 * @PARAMS:
 *  [in] uint32_t pid - id de la patota del tripulante.
 *  [in] uint32_t tid - id del tripulante.
 */
void  discordia_eliminar_tripulante(uint32_t pid, uint32_t tid);

/**
 * @NAME: discordia_iniciar_planificacion.
 * @DESC: inicializa el planificador del Discordiador.
 */
void  discordia_iniciar_planificacion(void);

/**
 * @NAME: discordia_pausar_planificacion
 * @DESC: detiene el planificador del Discordiador.
 */
void  discordia_pausar_planificacion(void);

/**
 * @NAME: discordia_notificar_sabotaje
 * @DESC: notifica al Discordiador la ocurrencia de un sabotaje.
 * @PARAMS:
 *  [in] const u_pos_t* pos - posicion donde ocurrio el sabotaje.
 */
void  discordia_notificar_sabotaje(const u_pos_t* pos);

/**
 * @NAME: discordia_finaliza_fsck
 * @DESC: notifica al Discordiador que el File System finalizo la rutina FSCK
 */
void  discordia_finaliza_fsck(void);

#endif