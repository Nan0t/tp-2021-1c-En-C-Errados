#ifndef UTILS_PROTOCOL_OPCODES_H
#define UTILS_PROTOCOL_OPCODES_H

#include "utils/typedefs.h"

typedef enum
{
    OK,
    FAIL,
    DESPLAZAMIENTO_TRIPULANTE,
	BITACORA,
    MOVIMIENTO_TRIPULANTE,
    INFORMAR_SABOTAJE,
    INICIO_TAREA,
    NUEVA_TAREA,
    FINALIZACION_TAREA,
    ATIENDE_SABOTAJE,
    RESUELVE_SABOTAJE,
    LISTA_TRIPULANTES,
    OBTENER_TRIPULANTES,
    OBTENER_BITACORA,
    ELIMINAR_TRIPULANTE,
    INICIAR_TRIPULANTE,
    INICIAR_PATOTA,
    PROXIMA_TAREA,
    TRIPULANTE_NUEVO_ESTADO,
    FINALIZACION_FSCK
} u_opcode_e;

#define U_MSG_IS_VALID(opcode)\
    ((opcode) >= DESPLAZAMIENTO_TRIPULANTE && (opcode) <= TRIPULANTE_NUEVO_ESTADO)

/**
 * @NAME: u_opcode_to_string.
 * @DESC: devuelve una representacion en string del opcode.
 * @PARAMS:
 *  [in] u_opcode_e opcode - opcode del cual se quiere obtener su representacion en string.
 * 
 * @RETURNS: un string con la representacion del opcode.
 */
const char* u_opcode_to_string(u_opcode_e opcode);

#endif
