#include "server.h"
#include "client_handler.h"
#include <utils/utils.h>
#include <pthread.h>
#include <errno.h>

private void ds_server_loop(int32_t* server_sock);

int ds_server_init(const char* port)
{
    int32_t* server_sock = u_malloc(sizeof(int32_t));
    u_sock_err_t* err = u_sock_err_create();

    *server_sock = u_socket_listen(port, 128, err);

    if(*server_sock == -1)
    {
        U_LOG_ERROR("%s", u_sock_err_get_description(err));
        u_sock_err_delete(err);

        return -1;
    }

    pthread_t server_thread;
    U_ASSERT(pthread_create(&server_thread, NULL, (void*)ds_server_loop, server_sock) != -1,
        "No se pudo crear un nuevo hilo para el servidor: %s", strerror(errno));
    pthread_detach(server_thread);

        U_LOG_INFO("Servidor listo y escuchando en puerto %s...", port);

    return 0;
}

private void ds_server_loop(int32_t* server_sock)
{
    while(1)
    {
        u_sock_err_t* err = u_sock_err_create();
        int32_t new_client = u_socket_accept(*server_sock, err);

        if(new_client == -1)
            U_LOG_ERROR("%s", u_sock_err_get_description(err));
        else
            client_handler_new_connection(new_client);
    }
}