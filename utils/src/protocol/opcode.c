#include "utils/protocol/opcodes.h"
#include "utils/diagnostics/assert.h"

private const char* OPCODES_STR[] =
{
    "OK",
    "FAIL",
    "DESPLAZAMIENTO_TRIPULANTE",
	"BITACORA",
    "MOVIMIENTO_TRIPULANTE",
    "INFORMAR_SABOTAJE",
    "INICIO_TAREA",
    "NUEVA_TAREA",
    "FINALIZACION_TAREA",
    "ATIENDE_SABOTAJE",
    "RESUELVE_SABOTAJE",
    "OBTENER_BITACORA",
    "ELIMINAR_TRIPULANTE",
    "INICIAR_TRIPULANTE",
    "INICIAR_PATOTA",
    "PROXIMA_TAREA",
    "TRIPULANTE_NUEVO_ESTADO"
};

const char* u_opcode_to_string(u_opcode_e opcode)
{
    U_ASSERT(U_MSG_IS_VALID(opcode), "Invalid opcode!");
    return OPCODES_STR[opcode];
}