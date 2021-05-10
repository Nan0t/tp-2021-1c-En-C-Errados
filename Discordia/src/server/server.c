#include "server.h"
#include "client_handler.h"
#include <utils/utils.h>

int ds_server_init(const char* port)
{
    // Socket del servidor
    int32_t server_sock;

    // Objeto que se puede usar para obtener los errores que
    // puedan llegar a suceder al conectarse al servidor.
    u_sock_err_t* err = u_sock_err_create();

    // Conectamos el servidor al puerto especificado
    server_sock = u_socket_listen(port, 128, err);

    // En caso de devolver -1, el objeto err contendra tanto
    // el código de error como una descripción del error, el cual
    // puede ser obtenido con la funcion u_sock_err_get_description.
    if(server_sock == -1)
    {
        U_LOG_ERROR("%s", u_sock_err_get_description(err));
        u_sock_err_delete(err);

        return -1;
    }

    U_LOG_INFO("Server created and listen on port %s...", port);

    // Loop donde el servidor queda a la escucha de nuevas conexiones.
    while(1)
    {
        int32_t new_client = u_socket_accept(server_sock, err);

        if(new_client == -1)
            U_LOG_ERROR("%s", u_sock_err_get_description(err));
        else
        {
            // Agregamos un nuevo cliente al administrador de clientes.
            client_handler_new_connection(new_client);
        }
    }

    return 0;
}
