#include "fs/file_system.h"
#include "client_handler.h"

#include <pthread.h>

private void fs_client_handler_task(int32_t* client_sock);
private void fs_client_handler_check_opcode(int32_t client_sock, u_opcode_e opcode);
private void fs_client_handler_get_msg(int32_t client_sock, u_opcode_e opcode, uint32_t msg_length);
private void fs_client_handler_deserialize_msg(int32_t client_sock, u_opcode_e opcode, const u_buffer_t* buffer);

private void fs_client_handler_msg_desplazamiento_tripulante(const u_buffer_t* buffer);
private void fs_client_handler_msg_inicio_tarea(const u_buffer_t* buffer);
private void fs_client_handler_msg_finalizacion_tarea(const u_buffer_t* buffer);
private void fs_client_handler_msg_atiende_sabotaje(const u_buffer_t* buffer);
private void fs_client_handler_msg_resuelve_sabotaje(const u_buffer_t* buffer);
private void fs_client_handler_msg_obtener_bitacora(int32_t client_sock, const u_buffer_t* buffer);

private bool fs_client_handler_is_valid_opcode(u_opcode_e opcode);

void fs_client_handler_new_connection(int32_t* client_sock)
{
    pthread_t client_thread;
    U_ASSERT(pthread_create(&client_thread, NULL, (void*)fs_client_handler_task, client_sock) != -1,
        "No se pudo crear un hilo para atender a una conexion");
    pthread_detach(client_thread);
}

private void fs_client_handler_task(int32_t* client_sock)
{
    u_opcode_e opcode;
    
    while(u_socket_recv(*client_sock, &opcode, sizeof(uint32_t)))
        fs_client_handler_check_opcode(*client_sock, opcode);

    u_socket_close(*client_sock);
    u_free(client_sock);
}

private void fs_client_handler_check_opcode(int32_t client_sock, u_opcode_e opcode)
{
    if(fs_client_handler_is_valid_opcode(opcode))
    {
        uint32_t msg_length;

        if(u_socket_recv(client_sock, &msg_length, sizeof(uint32_t)))
            fs_client_handler_get_msg(client_sock, opcode, msg_length);
    }
}

private void fs_client_handler_get_msg(int32_t client_sock, u_opcode_e opcode, uint32_t msg_length)
{
    void* msg = u_malloc(msg_length);

    if(u_socket_recv(client_sock, msg, msg_length))
    {
        u_buffer_t* buffer = u_buffer_create();
        u_buffer_write(buffer, msg, msg_length);
        u_free(msg);

        fs_client_handler_deserialize_msg(client_sock, opcode, buffer);
        u_buffer_delete(buffer);
    }
}

private void fs_client_handler_deserialize_msg(int32_t client_sock, u_opcode_e opcode, const u_buffer_t* buffer)
{
    switch(opcode)
    {
    case DESPLAZAMIENTO_TRIPULANTE:
        fs_client_handler_msg_desplazamiento_tripulante(buffer);
        break;

    case INICIO_TAREA:
        fs_client_handler_msg_inicio_tarea(buffer);
        break;

    case FINALIZACION_TAREA:
        fs_client_handler_msg_finalizacion_tarea(buffer);
        break;

    case ATIENDE_SABOTAJE:
        fs_client_handler_msg_atiende_sabotaje(buffer);
        break;

    case RESUELVE_SABOTAJE:
        fs_client_handler_msg_resuelve_sabotaje(buffer);
        break;

    default:
        fs_client_handler_msg_obtener_bitacora(client_sock, buffer);
    }
}

private void fs_client_handler_msg_desplazamiento_tripulante(const u_buffer_t* buffer)
{
    u_msg_desplazamiento_tripulante_t* msg =
        u_msg_desplazamiento_tripulante_deserializar(buffer);

    file_system_desplazamiento_tripulante(msg->tripulante_id, &msg->origen, &msg->destino);

    u_msg_desplazamiento_tripulante_eliminar(msg);
}

private void fs_client_handler_msg_inicio_tarea(const u_buffer_t* buffer)
{
    u_msg_inicio_tarea_t* msg = u_msg_inicio_tarea_deserializar(buffer);
    file_system_inicio_tarea(msg->tripulante_id, msg->tarea);

    u_msg_inicio_tarea_eliminar(msg);
}

private void fs_client_handler_msg_finalizacion_tarea(const u_buffer_t* buffer)
{
    u_msg_finalizacion_tarea_t* msg = u_msg_finalizacion_tarea_deserializar(buffer);
    file_system_finalizacion_tarea(msg->tripulante_id, msg->tarea);

    u_msg_finalizacion_tarea_eliminar(msg);
}

private void fs_client_handler_msg_atiende_sabotaje(const u_buffer_t* buffer)
{
    u_msg_atender_sabotaje_t* msg = u_msg_atiende_sabotaje_deserializar(buffer);
    file_system_atiende_sabotaje(msg->tripulante_id);
    
    u_msg_atiende_sabotaje_eliminar(msg);
}

private void fs_client_handler_msg_resuelve_sabotaje(const u_buffer_t* buffer)
{
    u_msg_resuelve_sabotaje_t* msg = u_msg_resuelve_sabotaje_deserializar(buffer);
    file_system_resuelve_sabotaje(msg->tripulante_id);

    u_msg_resuelve_sabotaje_eliminar(msg);
}

private void fs_client_handler_msg_obtener_bitacora(int32_t client_sock, const u_buffer_t* buffer)
{
    u_msg_obtener_bitacora_t* msg = u_msg_obtener_bitacora_deserializar(buffer);
    char* bitacora_content = file_system_obtener_bitacora(msg->tripulante_id);

    u_buffer_t* package_ser = NULL;

    if(bitacora_content != NULL)
    {
        u_msg_bitacora_t* bitacora = u_msg_bitacora_crear(bitacora_content);
        u_buffer_t*  bitacora_ser = u_msg_bitacora_serializar(bitacora);
        u_package_t* package      = u_package_create(BITACORA, bitacora_ser);

        package_ser  = u_package_serialize(package);

        u_msg_bitacora_eliminar(bitacora);
        u_buffer_delete(bitacora_ser);
        u_package_delete(package);
        u_free(bitacora_content);
    }
    else
    {
        u_msg_fail_t* fail = u_msg_fail_crear("No existe el tripulante %d en el FileSystem", msg->tripulante_id);
        u_buffer_t* fail_ser = u_msg_fail_serializar(fail);
        u_package_t* package = u_package_create(FAIL, fail_ser);

        package_ser = u_package_serialize(package);

        u_msg_fail_eliminar(fail);
        u_buffer_delete(fail_ser);
        u_package_delete(package);
    }

    u_socket_send(client_sock, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser));

    u_msg_obtener_bitacora_eliminar(msg);
    u_buffer_delete(package_ser);
}

private bool fs_client_handler_is_valid_opcode(u_opcode_e opcode)
{
    return  opcode == DESPLAZAMIENTO_TRIPULANTE ||
            opcode == INICIO_TAREA              ||
            opcode == FINALIZACION_TAREA        ||
            opcode == ATIENDE_SABOTAJE          ||
            opcode == RESUELVE_SABOTAJE         ||
            opcode == ELIMINAR_TRIPULANTE       ||
            opcode == OBTENER_BITACORA;
}