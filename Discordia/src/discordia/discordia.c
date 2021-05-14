#include "discordia.h"
#include "server/server.h"
#include "console/console.h"

#include <stdio.h>

typedef struct
{
    uint32_t pid;
    uint32_t tid;
} discordia_t;

private discordia_t* p_discordia_instance = NULL;

private uint32_t discordia_obtener_nuevo_pid(void);
private uint32_t discordia_obtener_nuevo_tid(void);

private int32_t discordia_conectar_con_memoria(void);
private int32_t discordia_conectar_con_fs(void);

private void  discordia_eliminar_tripulante_de_memoria(uint32_t tid);
private void  discordia_eliminar_tripulante_de_fs(uint32_t tid);
private char* discordia_obtener_bitacora_del_fs(int32_t conn);

private bool discordia_enviar_patota_a_memoria(const char* tareas);
private bool discordia_enviar_tripulantes_a_memoria(int32_t cant_trips, t_list* positions);

private char* discordia_leer_archivo_tareas(const char* ruta_tareas);

int discordia_init(void)
{
    if(p_discordia_instance)
        return 0;

    U_LOG_TRACE("Modulo Discordia");
    if(ds_server_init(u_config_get_string_value("PUERTO")) == -1)
        return -1;

    p_discordia_instance = u_malloc(sizeof(discordia_t));

    p_discordia_instance->pid = 0;
    p_discordia_instance->tid = 0;

    ds_console_init();
    return 0;
}

char* discordia_obtener_bitacora(uint32_t tid)
{
    int32_t conn = discordia_conectar_con_fs();
    char* bitacora = NULL;

    if(conn == -1)
        return NULL;

    u_msg_obtener_bitacora_t* msg         = u_msg_obtener_bitacora_crear(tid);
    u_buffer_t*               msg_ser     = u_msg_obtener_bitacora_serializar(msg);
    u_package_t*              package     = u_package_create(OBTENER_BITACORA, msg_ser);
    u_buffer_t*               package_ser = u_package_serialize(package);

    if(u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
        bitacora = discordia_obtener_bitacora_del_fs(conn);

    u_msg_obtener_bitacora_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    return bitacora;
}

char* discordia_obtener_tripulantes(void)
{
    return NULL;
}

void discordia_inicializar_patota(const char* ruta_tareas, uint32_t cant_trip, t_list* posiciones_trip)
{
    char* tareas = discordia_leer_archivo_tareas(ruta_tareas);

    if(tareas == NULL)
    {
        U_LOG_ERROR("No existe el archivo de tareas especificado: %s", ruta_tareas);
        return;
    }

    if(!discordia_enviar_patota_a_memoria(tareas))
        discordia_enviar_tripulantes_a_memoria(cant_trip, posiciones_trip);

    // TODO: inicializar planficador
}

void discordia_expulsar_tripulante(uint32_t tid)
{
    discordia_eliminar_tripulante_de_memoria(tid);
    discordia_eliminar_tripulante_de_fs(tid);
}

void  discordia_iniciar_planificacion(void)
{
    U_LOG_TRACE("Iniciar Planificador");
}

void  discordia_pausar_planificacion(void)
{
    U_LOG_TRACE("Pausar Planificador");
}

void  discordia_notificar_sabotaje(const u_pos_t* pos)
{
    U_LOG_TRACE("Nuevo Sabotaje: { X: %d | Y:%d }", pos->x, pos->y);
}

void  discordia_finaliza_fsck(void)
{
    U_LOG_TRACE("Finalización del FSCK. Se reanuda la planificacion");
}

// ======================================================
//              *** Funciones Privadas ****
// ======================================================

private uint32_t discordia_obtener_nuevo_pid(void)
{
    uint32_t new_pid = ++ p_discordia_instance->pid;
    return new_pid;
}

private uint32_t discordia_obtener_nuevo_tid(void)
{
    uint32_t new_tid = ++ p_discordia_instance->tid;
    return new_tid;
}

private int32_t discordia_conectar_con_memoria(void)
{
    u_sock_err_t* err = u_sock_err_create();
    int32_t conn = u_socket_connect(
        u_config_get_string_value("IP_MI_RAM_HQ"),
        u_config_get_string_value("PUERTO_MI_RAM_HQ"),
        err
    );

    if(conn == -1)
        U_LOG_ERROR(
            "No se pudo establecer conexion con la Memoria: %s",
            u_sock_err_get_description(err));

    return conn;
}

private int32_t discordia_conectar_con_fs(void)
{
    u_sock_err_t* err = u_sock_err_create();
    int32_t conn = u_socket_connect(
        u_config_get_string_value("IP_I_MONGO_STORE"),
        u_config_get_string_value("PUERTO_I_MONGO_STORE"),
        err
    );

    if(conn == -1)
        U_LOG_ERROR(
            "No se pudo establecer conexion con el FS: %s",
            u_sock_err_get_description(err));

    return conn;
}

private void discordia_eliminar_tripulante_de_memoria(uint32_t tid)
{
    int32_t conn = discordia_conectar_con_memoria();

    if(conn == -1)
        return;

    u_msg_eliminar_tripulante_t* msg         = u_msg_eliminar_tripulante_crear(tid);
    u_buffer_t*                  msg_ser     = u_msg_eliminar_tripulante_serializar(msg);
    u_package_t*                 package     = u_package_create(ELIMINAR_TRIPULANTE, msg_ser);
    u_buffer_t*                  package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_eliminar_tripulante_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s a la Memoria", msg_str);
        u_free(msg_str);
    }
}

private void discordia_eliminar_tripulante_de_fs(uint32_t tid)
{
    int32_t conn = discordia_conectar_con_fs();

    if(conn == -1)
        return;

    u_msg_eliminar_tripulante_t* msg         = u_msg_eliminar_tripulante_crear(tid);
    u_buffer_t*                  msg_ser     = u_msg_eliminar_tripulante_serializar(msg);
    u_package_t*                 package     = u_package_create(ELIMINAR_TRIPULANTE, msg_ser);
    u_buffer_t*                  package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_eliminar_tripulante_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s al FS", msg_str);
        u_free(msg_str);
    }
}

// Eww, to many ifs
private char* discordia_obtener_bitacora_del_fs(int32_t conn)
{
    u_opcode_e opcode;
    char* bitacora = NULL;

    if(u_socket_recv(conn, &opcode, sizeof(uint32_t)))
    {
        if(opcode != BITACORA)
        {
            U_LOG_ERROR("Se recibio un opcode invalido como respuesta a OBTENER_BITACORA");
            U_LOG_ERROR("Opcode recibido: %d", opcode);
            u_socket_close(conn);

            return NULL;
        }

        uint32_t msg_length;

        if(u_socket_recv(conn, &msg_length, sizeof(uint32_t)))
        {
            void* msg = u_malloc(msg_length);
            if(u_socket_recv(conn, msg, msg_length))
            {
                u_buffer_t* buffer = u_buffer_create();
                u_buffer_write(buffer, msg, msg_length);

                u_msg_bitacora_t* msg = u_msg_bitacora_deserializar(buffer);
                bitacora = strdup(msg->contenido);

                u_msg_bitacora_eliminar(msg);
                u_buffer_delete(buffer);
            }
            else
                U_LOG_ERROR("No se pudo recibir la bitacora. Conexion perdida con el FS");

            u_free(msg);
        }
        else
            U_LOG_ERROR("No se pudo recibir la bitacora. Conexion perdida con el FS");
    }
    else
        U_LOG_ERROR("No se pudo recibir la bitacora. Conexion perdida con el FS");

    u_socket_close(conn);

    return bitacora;
}

private bool discordia_enviar_patota_a_memoria(const char* tareas)
{
    bool result = true;
    int32_t conn = discordia_conectar_con_memoria();

    if(conn == -1)
        return false;

    uint32_t pid = discordia_obtener_nuevo_pid();

    u_msg_iniciar_patota_t* msg         = u_msg_iniciar_patota_crear(pid, tareas);
    u_buffer_t*             msg_ser     = u_msg_iniciar_patota_serializar(msg);
    u_package_t*            package     = u_package_create(INICIAR_PATOTA, msg_ser);
    u_buffer_t*             package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_iniciar_patota_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s a la memoria", msg_str);
        u_free(msg_str);

        result = false;
    }

    u_msg_iniciar_patota_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    return result;
}

private bool discordia_enviar_tripulantes_a_memoria(int32_t cant_trips, t_list* positions)
{
    // TODO: enviar iniciar tripulante (proxima correccion en utils)
    return true;
}

private char* discordia_leer_archivo_tareas(const char* ruta_tareas)
{
    FILE* file = fopen(ruta_tareas, "r");

    if(file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    uint64_t file_size = ftell(file) / sizeof(char);
    fseek(file, 0, SEEK_SET);

    char* content = u_malloc(file_size);
    fread(content, sizeof(char), file_size, file);
    fclose(file);

    return content;
}