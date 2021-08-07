#include "connection_handler.h"
#include <pthread.h>

typedef struct
{
    uint32_t tid;
    uint32_t ram_conn;
    uint32_t fs_conn;
} ds_conn_t;

static pthread_mutex_t p_connections_mx = PTHREAD_MUTEX_INITIALIZER;
static t_list*         p_connections    = NULL;

private void ds_connection_destroy(ds_conn_t* conn);

void ds_connection_handler_init(void)
{
    if(p_connections)
        return;

    p_connections = list_create();
}

void ds_connection_handler_create_connection(uint32_t tid)
{
    ds_conn_t* conn = u_malloc(sizeof(ds_conn_t));
    u_sock_err_t* err = u_sock_err_create();

    conn->tid = tid;

    conn->fs_conn =
        u_socket_connect(u_config_get_string_value("IP_I_MONGO_STORE"), u_config_get_string_value("PUERTO_I_MONGO_STORE"), err);

    conn->ram_conn =
        u_socket_connect(u_config_get_string_value("IP_MI_RAM_HQ"), u_config_get_string_value("PUERTO_MI_RAM_HQ"), err);

    pthread_mutex_lock(&p_connections_mx);
    list_add(p_connections, conn);
    pthread_mutex_unlock(&p_connections_mx);
}

void ds_connection_handler_close_connection(uint32_t tid)
{
    bool _trip_buscado(ds_conn_t* conn) {
        return conn->tid == tid;
    }

    pthread_mutex_lock(&p_connections_mx);
    list_remove_and_destroy_by_condition(p_connections, (void*)_trip_buscado, (void*)ds_connection_destroy);
    pthread_mutex_unlock(&p_connections_mx);
}

bool ds_connection_handler_send(uint32_t tid, ds_module_e module, const void* data, uint64_t data_length)
{
    bool result = false;

    bool _trip_buscado(ds_conn_t* conn) {
        return conn->tid == tid;
    }

    pthread_mutex_lock(&p_connections_mx);
    ds_conn_t* conn = list_find(p_connections, (void*)_trip_buscado);

    if(conn)
    {
        if(module == MI_RAM)
            result = u_socket_send(conn->ram_conn, data, data_length);
        else
            result = u_socket_send(conn->fs_conn, data, data_length);
    }
    pthread_mutex_unlock(&p_connections_mx);

    return result;
}

bool ds_connection_handler_recv(uint32_t tid, ds_module_e module, void* data, uint64_t data_length)
{
    bool result = false;

    bool _trip_buscado(ds_conn_t* conn) {
        return conn->tid == tid;
    }

    pthread_mutex_lock(&p_connections_mx);
    ds_conn_t* conn = list_find(p_connections, (void*)_trip_buscado);

    if(conn)
    {
        if(module == MI_RAM)
            result = u_socket_recv(conn->ram_conn, data, data_length);
        else
            result = u_socket_recv(conn->fs_conn, data, data_length);
    }
    pthread_mutex_unlock(&p_connections_mx);

    return result;
}

private void ds_connection_destroy(ds_conn_t* conn)
{
    u_socket_close(conn->fs_conn);
    u_socket_close(conn->ram_conn);

    u_free(conn);
}