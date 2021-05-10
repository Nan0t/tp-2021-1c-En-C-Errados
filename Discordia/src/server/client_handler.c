#include "client_handler.h"
#include "../discordia/discordia.h"
#include <pthread.h>
#include <stdlib.h>

// Lista de clientes conectados al servidor. Como se puede acceder
// a la lista desde distintos hilos, necesitamos un mutex para la lista
// de clientes.
private t_list*         client_list    = NULL;
private pthread_mutex_t client_list_mx = PTHREAD_MUTEX_INITIALIZER;

// Helper functions. Se describen mas abajo.
private void client_thread(int32_t* sock_client);
private void client_delete_from_list(int32_t sock_client);
private void discordiador_client_handler_manage_buffer_sabotaje(int32_t _sock_client);
private void discordiador_client_handler_manage_opcode(int32_t _sock_client, u_opcode_e _opcode);

void client_handler_new_connection(int32_t new_client)
{
    // Si la lista esta vacia, inicializarla.
    if(client_list == NULL)
        client_list = list_create();

    U_LOG_TRACE("New client connected");

    // Tenemos que reservar memoria para guardar el socket del nuevo cliente
    // dentro de la lista de conexiones, ya que los t_list de las commons solo
    // pueden guardar punteros.
    int32_t* new_client_sock_instance = u_malloc(sizeof(int32_t));
    *new_client_sock_instance = new_client;

    // Agregamos la nueva conexion a la lista.
    pthread_mutex_lock(&client_list_mx);
    list_add(client_list, new_client_sock_instance);
    pthread_mutex_unlock(&client_list_mx);

    // Creamos el hilo del cliente.
    // En caso de que pthread_create devuelva -1, se logea un mensaje de error y
    // se aborta el programa. Para mas info sobre el macro U_ASSERT, ver el archivo
    // que se encuentra en utils/include/utils/diagnostics/assert.h
    pthread_t client_thread_id;
    U_ASSERT(
        pthread_create(&client_thread_id, NULL, (void*)client_thread, new_client_sock_instance) != -1,
        "No se pudo crear un nuevo thread en el sistema!");

    pthread_detach(client_thread_id);
}

private void client_thread(int32_t* sock_client)
{
    u_opcode_e opcode;

    if(u_socket_recv(*sock_client, &opcode, sizeof(uint32_t))){
        discordiador_client_handler_manage_opcode(*sock_client, opcode);
    }

    client_delete_from_list(*sock_client);
}

private void client_delete_from_list(int32_t sock_client)
{
    // Funcion que se usa para comparar cada uno de los sockets
    // en la lista de conexiones con el sock_client.
    bool _find_client_to_delete(const int32_t* client) {
        return *client == sock_client;
    };

    // Funcion para eliminar una conexion.
    void _client_delete(int32_t* client) {
        u_socket_close(*client);
        u_free(client);
    };

    // Buscamos el socket_cliente y lo eliminamos.
    pthread_mutex_lock(&client_list_mx);
    list_remove_and_destroy_by_condition(client_list, (void*)_find_client_to_delete, (void*)_client_delete);
    pthread_mutex_unlock(&client_list_mx);
}


private void discordiador_client_handler_manage_opcode(int32_t _sock_client, u_opcode_e _opcode){
    switch (_opcode)
    {
    case FINALIZACION_FSCK : 
        U_LOG_TRACE("EL FSCK FINALIZO");
        break;
    
    case INFORMAR_SABOTAJE :
        discordiador_client_handler_manage_buffer_sabotaje(_sock_client);
        break;
    }
}

private void discordiador_client_handler_manage_buffer_sabotaje(int32_t _sock_client){
    uint32_t tamanio_buffer;
        if(u_socket_recv(_sock_client, &(tamanio_buffer),sizeof(uint32_t))){
            void* stream= malloc(sizeof(tamanio_buffer));
            if(u_socket_recv(_sock_client, stream, tamanio_buffer)){
                u_msg_posicion_sabotaje_t* msg = u_msg_informar_sabotaje_deserializar(stream);
                u_pos_t* posicion_sabotaje = malloc(sizeof(u_pos_t));
                posicion_sabotaje->x = msg->pos_x;
                posicion_sabotaje->y = msg->pos_y;
                discordia_notificar_sabotaje(posicion_sabotaje);
                free(posicion_sabotaje);
                u_msg_informar_sabotaje_eliminar(msg);
            }
            free(stream);
        }
}