#include "server.h"
#include "client_handler.h"

#include <pthread.h>

private int32_t p_server_sock = -1;

private void fs_server_loop(void);

void fs_server_init(const char* port)
{
    u_sock_err_t* err;

    if(p_server_sock != -1)
        return;

    err           = u_sock_err_create();
    p_server_sock = u_socket_listen(port, 128, err);

    if(p_server_sock == -1)
    {
        U_LOG_ERROR(
            "No se pudo inicializar el servidor: %s", u_sock_err_get_description(err));
        u_sock_err_delete(err);        

        return;
    }
    u_sock_err_delete(err);

    pthread_t server_thread;
    U_ASSERT(pthread_create(&server_thread, NULL, (void*)fs_server_loop, NULL) != -1,
        "No se pudo crear el hilo del servidor");

    pthread_detach(server_thread);
}

private void fs_server_loop(void)
{
    u_sock_err_t* err = u_sock_err_create();
    int32_t* new_client_sock = u_malloc(sizeof(int32_t));
    while(1)
    {
        *new_client_sock = u_socket_accept(p_server_sock, err);

        if(*new_client_sock == -1)
            U_LOG_WARN("No se pudo aceptar una nueva conexion: %s", u_sock_err_get_description(err));
        else
        {
            fs_client_handler_new_connection(new_client_sock);
            new_client_sock = u_malloc(sizeof(int32_t));
        }
    }
}