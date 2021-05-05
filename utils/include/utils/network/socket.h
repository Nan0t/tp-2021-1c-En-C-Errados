#ifndef UTILS_NETWORK_SOCKET_H
#define UTILS_NETWORK_SOCKET_H

/**
 * @FILE: socket.h
 * 
 * @DESC: Header que contiene las operaciones basicas para operar sockets.
 */

#include "utils/typedefs.h"
#include "utils/memory/allocator.h"

/**
 * @NAME: sock_err_type_e
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
} sock_err_type_e;

/**
 * @NAME: u_sock_err_t
 * @DESC: struct que es utilizado por u_sock_connect,
 * u_sock_listen y u_sock_accept para generar una descripcion
 * en caso que ocurriese un error dentro de alguna de estas funciones
 * 
 * @FIELDS:
 *  enum sock_err_type err_type - enumerado con el codigo de error.
 *  char*              desc     - string con la descripcion del error.
 */
typedef struct
{
    sock_err_type_e err_type;
    char*           desc;
} u_sock_err_t;

/**
 * @NAME: u_sock_err_create
 * @DESC: crea un struct u_sock_err_t que puede ser usado en u_sock_connect,
 * u_sock_listen y u_sock_accept para obtener en caso que ocurrira un error,
 * una descripcion del mismo.
 * 
 * @RETURNS: un struct u_sock_err_t
 */
u_sock_err_t* u_sock_err_create(void);

/**
 * @NAME: u_sock_err_delete
 * @DESC: libera la memoria utilizada por un struct u_sock_err_t
 */
void          u_sock_err_delete(u_sock_err_t* this);

/**
 * @NAME: u_socket_connect
 * @DESC: crea un socket y lo conecta al host y puerto especificados por
 * parametro.
 * 
 * @PARAMS:
 *  [in]  const char*    host - direccion ipv4 del host a conectarse.
 *  [in]  const char*    port - puerto del host a conectarse.
 *  [out] u_sock_err_t*  err  - struct que contendra un codigo de error y una
 *                              descripcion en caso de que ocurra un error.
 * 
 * @RETURNS: un file descriptro del socket en caso de que se haya podido realizar
 * la conexion correctamente. En caso de error, se devolvera -1 y err contendra una
 * descripcion del error (la memoria utilizada por err debe ser liberada luego con 
 * u_sock_err_delete).
 */
int32_t u_socket_connect(const char* host, const char* port, u_sock_err_t* err);

/**
 * @NAME: u_socket_listen
 * @DESC: crea un socket y lo bindea al puerto especificado, quedando a la escucha
 * de nuevas conexiones.
 * 
 * @PARAMS:
 *  [in]  const char*    port     - puerto de escucha.
 *  [in]  int32_t        backlogs - cantidad de conexiones maximas a aceptar.
 *  [out] u_sock_err_t*  err      - struct que contendra un codigo de error y una
 *                                  descripcion en caso de que ocurra un error.
 * 
 * @RETURNS: un socket que representa la conexion que se encuentra bindeado al puerto. En caso de
 * error, se devolvera -1 y err contendra una descripcion del error (la memoria utilizada
 * por err debe ser liberada luego con u_sock_err_delete).
 */
int32_t u_socket_listen(const char* port, int32_t backlogs, u_sock_err_t* err);

/**
 * @NAME: u_socket_accept
 * @DESC: acepta una nueva conexion entrante.
 * 
 * @PARAMS:
 *  [in]  int32_t        sock - socket que se encuentra a la escucha de nuevas conexiones.
 *  [out] u_sock_err_t*  err  - struct que contendra un codigo de error y una
 *                              descripcion en caso de que ocurra un error.
 * 
 * @RETURNS: un socket que representa la conexion entrante. En caso de error, se devolvera
 * -1 y err contendra una descripcion del error (la memoria utilizada por err debe ser liberada
 * luego con u_sock_err_delete).
 */
int32_t u_socket_accept(int32_t sock, u_sock_err_t* err);

/**
 * @NAME: u_socket_close
 * @DESC: cierra la conexion (tanto de lectura, como escritura) de un socket
 */
void    u_socket_close(int32_t sock);

/**
 * @NAME: u_socket_send
 * @DESC: envia los datos especificados en data al servidor al que se encuentre
 * conectado el socket.
 * 
 * @PARAMS:
 *  [in] int32_t      sock  --> socket que representa la conexion.
 *  [in] const void*  data  --> datos a enviar.
 *  [in] int64_t      size  --> tamaño en bytes del dato a enviar.
 * 
 * @RETURNS: en caso de que la conexion se pierda, la funcion devolvera false.
 * En caso contrario, true.
 * 
 * @NOTA: en caso que data sea NULL el programa aborta.
 */
bool u_socket_send(int32_t sock, const void* data, int64_t size);

/**
 * @NAME: u_socket_recv
 * @DESC: guarda los datos recibidos desde el servidor en data.
 * 
 * @PARAMS:
 *  [in] int32_t      sock  --> socket que representa la conexion.
 *  [in] void*        data  --> buffer donde se guardaran los datos a recibir.
 *  [in] int64_t      size  --> tamaño en bytes que se esperan recibir.
 * 
 * @RETURNS: en caso de que la conexion se pierda, la funcion devolvera false.
 * En caso contrario, true.
 * 
 * @ASSERT: en caso que data sea NULL.
 */
bool u_socket_recv(int32_t sock, void* data, int64_t size);

#endif