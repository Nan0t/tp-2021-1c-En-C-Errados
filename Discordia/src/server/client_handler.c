#include "client_handler.h"
#include "../discordia/discordia.h"
#include <pthread.h>
#include <stdlib.h>

// Helper functions. Se describen mas abajo.
private void client_thread(int32_t* sock_client);
private void discordiador_client_handler_manage_buffer_sabotaje(int32_t _sock_client);
private void discordiador_client_handler_manage_opcode(int32_t _sock_client, u_opcode_e _opcode);

void client_handler_new_connection(int32_t new_client)
{
    U_LOG_TRACE("New client connected");

    int32_t* new_client_sock_instance = u_malloc(sizeof(int32_t));
    *new_client_sock_instance = new_client;

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

    default:
        U_LOG_ERROR("Opcode incorrecto o invalido: #(%d)", _opcode);
    }
}

private void discordiador_client_handler_manage_buffer_sabotaje(int32_t _sock_client){
    uint32_t tamanio_buffer;
        if(u_socket_recv(_sock_client, &(tamanio_buffer),sizeof(uint32_t))){
            void* stream= malloc(tamanio_buffer);
            if(u_socket_recv(_sock_client, stream, tamanio_buffer)){
                u_buffer_t* buffer = u_buffer_create();
                u_buffer_write(buffer, stream, tamanio_buffer);

                u_msg_posicion_sabotaje_t* msg = u_msg_informar_sabotaje_deserializar(buffer);

                discordia_notificar_sabotaje(&(u_pos_t){
                    .x = msg->pos_x,
                    .y = msg->pos_y
                });
                
                u_msg_informar_sabotaje_eliminar(msg);
            }
            free(stream);
        }
}