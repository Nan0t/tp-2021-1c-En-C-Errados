#ifndef UTILS_NETWORK_SOCK_ERR_H
#define UTILS_NETWORK_SOCK_ERR_H

#include "utils/typedefs.h"

typedef struct u_sock_err_t u_sock_err_t;

/**
 * @NAME: u_sock_err_type_e
 * @DESC: enumerados que representan los posibles codigos de error que pueden
 * devolver algunas de las funciones de sockets.
 */
typedef enum
{
    U_SOCK_NO_ERROR,
    U_SOCK_GET_ADDR_ERROR,
    U_SOCK_CONNECT_ERROR,
    U_SOCK_BIND_ERROR,
    U_SOCK_LISTEN_ERROR,
    U_SOCK_ACCEPT_ERROR
} u_sock_err_type_e;

/**
 * @NAME: u_sock_err_create
 * @DESC: crea un struct u_sock_err_t que puede ser usado en u_sock_connect,
 * u_sock_listen y u_sock_accept para obtener en caso que ocurrira un error,
 * una descripcion del mismo.
 * 
 * @RETURNS: un struct u_sock_err_t
 * 
 * @NOTA: el struct u_sock_err_t debe ser liberado posteriormente con u_sock_err_delete.
 */
u_sock_err_t* u_sock_err_create(void);

/**
 * @NAME: u_sock_err_delete
 * @DESC: libera la memoria utilizada por un struct u_sock_err_t
 */
void u_sock_err_delete(u_sock_err_t* err);

/**
 * @NAME: u_sock_err_get_status
 * @DESC: devuelve el ultimo codigo de error almacenado dentro de la estructura.
 * 
 * @RETURNS: un enum u_sock_err_type_e describiendo el ultimo error almacenado en
 *           la estructura.
 */
u_sock_err_type_e u_sock_err_get_status(const u_sock_err_t* err);

/**
 * @NAME: u_sock_err_get_description
 * @DESC: devuelve una descripcion del ultimo error almacendado dentro de la estructura
 * 
 * @RETURNS: un string con la descripcion del ultimo error almacenado en la estructura
 */
const char* u_sock_err_get_description(const u_sock_err_t* err);


#endif