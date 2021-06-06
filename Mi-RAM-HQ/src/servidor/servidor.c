#include "servidor.h"

private void cliente_thread(int32_t sock_client);
void client_handler_manage_opcode(int32_t sock_client, u_opcode_e op_code, u_buffer_t* buffer);

bool servidor_init(const char* port)
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

    U_LOG_INFO("Mi-RAM-HQ escuchando en el puerto %s...", port);

    // Loop donde el servidor queda a la escucha de nuevas conexiones.
    while(1)
    {
        int32_t new_client = u_socket_accept(server_sock, err);

        if(new_client == -1)
            U_LOG_ERROR("%s", u_sock_err_get_description(err));
        else
        {
            // Agregamos un nuevo cliente al administrador de clientes.
            client_handler_new_conection(new_client);
        }
    }

    return 0;
}

void client_handler_new_conection(int32_t nuevo_cliente){
    U_LOG_TRACE("Nuevo cliente conectado");
    
   // int32_t* new_client_sock_instance = u_malloc(sizeof(int32_t));
    //*new_client_sock_instance = nuevo_cliente;
    

    // Creamos el hilo del cliente.
    // En caso de que pthread_create devuelva -1, se logea un mensaje de error y
    // se aborta el programa. Para mas info sobre el macro U_ASSERT, ver el archivo
    // que se encuentra en utils/include/utils/diagnostics/assert.h
    pthread_t client_thread_id;
    U_ASSERT(
        pthread_create(&client_thread_id, NULL, (void*)cliente_thread, (void*)nuevo_cliente) != -1,
        "No se pudo crear un nuevo thread en el sistema!");

    pthread_detach(client_thread_id); 
}

private void cliente_thread(int32_t sock_client)
{
    uint32_t op_code; 
    uint32_t msg_length;

    // Quedamos a la escucha de que el cliente envie un
    // mensaje. En caso de que u_socket_recv devuelva false, significara que se
    // perdio la conexion con el cliente, por lo que el hilo del cliente terminará.

    if(u_socket_recv(sock_client, &op_code, sizeof(uint32_t)))  //if en lugar de while
    {

        if(op_code == OBTENER_TRIPULANTES)
            client_handler_manage_opcode(sock_client, op_code, NULL);
        else
        {
            // Si el primer recv tiene éxito, habremos obtenido el op_code
            // Volvemos a hacer otro recv para recibir el tamaño del msj.
            u_socket_recv(sock_client, &msg_length, sizeof(uint32_t));

            //Recibido el msj lo pasamos a una estructura buffer para deserializarlo
            
            void* msg = u_malloc(msg_length);
            if(u_socket_recv(sock_client, msg, msg_length)){
            //    u_socket_recv(sock_client, &msg, msg_length);
                u_buffer_t* buffer = u_buffer_create();
                u_buffer_write(buffer, msg, msg_length);
                u_free(msg);
                client_handler_manage_opcode(sock_client, op_code, buffer);
            }
        }
    }
    u_socket_close(sock_client);
}

void client_handler_manage_opcode(int32_t sock_client, u_opcode_e op_code, u_buffer_t* buffer){
    switch(op_code){
        case INICIAR_PATOTA: ;
            u_msg_iniciar_patota_t* patota = u_msg_iniciar_patota_deserializar(buffer);
            
            U_LOG_TRACE("Recibido msg INICIAR_PATOTA");
            U_LOG_TRACE("pid: %d, lista_tareas: %s", patota->pid, patota->lista_tareas);//para probar que llego correcto
            
            if(admin_memoria_iniciar_patota(patota->pid, patota->lista_tareas))
            {
                u_opcode_e opcode = OK;
                u_socket_send(sock_client, &opcode, sizeof(uint32_t));
            }
            else
            {
                u_msg_fail_t* msg         = u_msg_fail_crear("No se pudo crear el PCB de patota %d en la memoria", patota->pid);
                u_buffer_t*   msg_ser     = u_msg_fail_serializar(msg);
                u_package_t*  package     = u_package_create(FAIL, msg_ser);
                u_buffer_t*   package_ser = u_package_serialize(package);

                u_socket_send(sock_client, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser));

                u_msg_fail_eliminar(msg);
                u_buffer_delete(msg_ser);
                u_buffer_delete(package_ser);
                u_package_delete(package);
            }
            
            u_msg_iniciar_patota_eliminar(patota);
            

            break;

        case INICIAR_TRIPULANTE: ;
            u_msg_iniciar_tripulante_t* tripulante = u_msg_iniciar_tripulante_deserializar(buffer);
            U_LOG_TRACE("Recibido msg INICIAR_TRIPULANTE");
            U_LOG_TRACE("pid: %d, tid: %d, posx: %d, posy: %d", tripulante->patota_id, tripulante->tripulante_id, tripulante->posicion.x, tripulante->posicion.y);
            
            if(admin_memoria_iniciar_tripulante(tripulante->patota_id, tripulante->tripulante_id, tripulante->posicion))
            {
                u_opcode_e opcode = OK;
                u_socket_send(sock_client, &opcode, sizeof(uint32_t));
            }
            else
            {
                u_msg_fail_t* msg =
                    u_msg_fail_crear("No se pudo crear el TCB para el tripulante %d en la memoria", tripulante->tripulante_id);

                u_buffer_t*  msg_ser     = u_msg_fail_serializar(msg);
                u_package_t* package     = u_package_create(FAIL, msg_ser);
                u_buffer_t*  package_ser = u_package_serialize(package);

                u_socket_send(sock_client, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser));
            }

            u_msg_iniciar_tripulante_eliminar(tripulante);
            break;

        case MOVIMIENTO_TRIPULANTE: ;
            u_msg_movimiento_tripulante_t* movimiento = u_msg_movimiento_tripulante_deserializar(buffer);

            U_LOG_TRACE("Recibido msg MOVIMIENTO_TRIPULANTE");
            U_LOG_TRACE("tid: %d, posx: %d, posy: %d", movimiento->tid, movimiento->pos.x, movimiento->pos.y);

            admin_memoria_mover_tripulante(movimiento->tid, movimiento->pos);
            u_msg_movimiento_tripulante_eliminar(movimiento);

            break;

        case PROXIMA_TAREA:
        {
            u_msg_proxima_tarea_t* tarea_msg = u_msg_proxima_tarea_deserializar(buffer);
            
            U_LOG_TRACE("Recibido msg PROXIMA_TAREA");
            U_LOG_TRACE("tid: %d", tarea_msg->tripulante_id);

            char* tarea = admin_memoria_obtener_proxima_tarea(tarea_msg->tripulante_id);
            u_msg_proxima_tarea_eliminar(tarea_msg);

            u_msg_nueva_tarea_t* msg = u_msg_nueva_tarea_crear(tarea);
            
            u_buffer_t* buffer      = u_msg_nueva_tarea_serializar(msg);
            u_package_t* package    = u_package_create(NUEVA_TAREA, buffer);
            u_buffer_t* package_ser = u_package_serialize(package);

            u_socket_send(sock_client, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser));

            u_msg_nueva_tarea_eliminar(msg);
            u_buffer_delete(buffer);
            u_buffer_delete(package_ser);
            u_package_delete(package);

            u_free(tarea);

            break;
        }

        case TRIPULANTE_NUEVO_ESTADO: ;
            u_msg_tripulante_nuevo_estado_t* nuevo_estado = u_msg_tripulante_nuevo_estado_deserializar(buffer);
            
            U_LOG_TRACE("Recibido msg TRIPULANTE_NUEVO_ESTADO");
            U_LOG_TRACE("tid: %d, estado nuevo: %c", nuevo_estado->tripulante_id, nuevo_estado->nuevo_estado);

            admin_memoria_tripulante_nuevo_estado(nuevo_estado->tripulante_id, nuevo_estado->nuevo_estado);
            u_msg_tripulante_nuevo_estado_eliminar(nuevo_estado);

            break; 

        case ELIMINAR_TRIPULANTE: ;
            u_msg_eliminar_tripulante_t* eliminar = u_msg_eliminar_tripulante_deserializar(buffer);
 
            U_LOG_TRACE("Recibido msg ELIMINAR_TRIPULANTE");
            U_LOG_TRACE("tid: %d", eliminar->tripulante_id);
            
            admin_memoria_eliminar_tripulante(eliminar->tripulante_id);
            u_msg_eliminar_tripulante_eliminar(eliminar);

            break;

        case OBTENER_TRIPULANTES:
        {
            t_list* tripulantes = admin_memoria_obtener_tripulantes();

            t_list_iterator* it = list_iterator_create(tripulantes);
            u_msg_lista_tripulantes_t* msg = u_msg_lista_tripulantes_crear();

            while(list_iterator_has_next(it))
            {
                tripulantes_t* trip = (tripulantes_t*)list_iterator_next(it);
                u_tripulante_info_t trip_info = {
                    .tid = trip->tid,
                    .pid = trip->pid,
                    .estado = trip->estado
                };

                u_msg_lista_tripulantes_agregar(msg, trip_info);
            }

            u_buffer_t*  buffer      = u_msg_lista_tripulantes_serialize(msg);
            u_package_t* package     = u_package_create(LISTA_TRIPULANTES, buffer);
            u_buffer_t*  package_ser = u_package_serialize(package);

            u_socket_send(sock_client, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser));

            u_msg_lista_tripulantes_eliminar(msg);
            u_buffer_delete(buffer);
            u_buffer_delete(package_ser);
            u_package_delete(package);

            list_destroy_and_destroy_elements(tripulantes, u_free);

            break;
        }

        default:
        {
            U_LOG_ERROR("no se recibio op_code aceptable");

            u_msg_fail_t* msg         = u_msg_fail_crear("[Memoria]: Opcode #(%d) erroneo o desconocido", op_code);
            u_buffer_t*   msg_ser     = u_msg_fail_serializar(msg);
            u_package_t*  package     = u_package_create(FAIL, msg_ser);
            u_buffer_t*   package_ser = u_package_serialize(package);

            u_socket_send(sock_client, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser));
        }
    } 

}