#include "discordia.h"
#include "planificador/planificador.h"
#include "server/server.h"
#include "console/console.h"

#include <commons/temporal.h>

#include <stdio.h>

typedef struct
{
    uint32_t pid_counter;
    uint32_t tid_counter;
} discordia_t;

private discordia_t* p_discordia_instance = NULL;

private uint32_t discordia_obtener_nuevo_pid(void);
private uint32_t discordia_obtener_nuevo_tid(void);

private int32_t discordia_conectar_con_memoria(void);
private int32_t discordia_conectar_con_fs(void);

private void  discordia_eliminar_tripulante_de_memoria(uint32_t pid, uint32_t tid);
private char* discordia_obtener_bitacora_del_fs(int32_t conn);
private char* discordia_obtener_tarea_de_memoria(int32_t conn);
private char* discordia_recibir_tripulantes_de_memoria(int32_t conn);

private char* discordia_lista_tripulantes_to_string(const u_msg_lista_tripulantes_t* msg);

private void discordia_recibir_fail(int32_t conn);

private uint32_t discordia_enviar_patota_a_memoria(uint32_t cant_trips, const char* tareas);
private bool     discordia_recibir_respuesta_memoria(int32_t conn);
private void     discordia_inicializar_tripulantes(uint32_t pid, int32_t cant_trips, t_list* positions);

private char* discordia_leer_archivo_tareas(const char* ruta_tareas);

int discordia_init(void)
{
    if(p_discordia_instance)
        return 0;

    U_LOG_INFO("Modulo Discordia");
    if(ds_server_init(u_config_get_string_value("PUERTO")) == -1)
        return -1;

    p_discordia_instance = u_malloc(sizeof(discordia_t));

    p_discordia_instance->pid_counter = 0;
    p_discordia_instance->tid_counter = 0;

    ds_planificador_init();
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

    u_socket_close(conn);

    return bitacora;
}

char* discordia_obtener_tripulantes(void)
{
    int32_t conn = discordia_conectar_con_memoria();
    char* lista_tripulantes = NULL;

    if(conn == -1)
        return NULL;

    u_opcode_e opcode = OBTENER_TRIPULANTES;

    if(u_socket_send(conn, &opcode, sizeof(uint32_t)))
        lista_tripulantes = discordia_recibir_tripulantes_de_memoria(conn);
    else
        U_LOG_ERROR("No se pudo enviar el mensaje %s. Conexion perdida con la Memoria", u_opcode_to_string(opcode));
    
    u_socket_close(conn);

    return lista_tripulantes;
}

void  discordia_desplazamiento_tripulante(uint32_t tid, const u_pos_t* origen, const u_pos_t* destion)
{
    int32_t conn = discordia_conectar_con_fs();

    if(conn == -1)
        return;

    u_msg_desplazamiento_tripulante_t* msg =
        u_msg_desplazamiento_tripulante_crear(tid, *origen, *destion);

    u_buffer_t* msg_ser     = u_msg_desplazamiento_tripulante_serializar(msg);
    u_package_t* package    = u_package_create(DESPLAZAMIENTO_TRIPULANTE, msg_ser);
    u_buffer_t* package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_desplazamineto_tripulante_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s. Conexion perdida con el FS", msg_str);
        u_free(msg_str);
    }

    u_msg_desplazamiento_tripulante_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    u_socket_close(conn);
}

void  discordia_mover_tripulante(uint32_t pid, uint32_t tid, const u_pos_t* pos)
{
    int32_t conn = discordia_conectar_con_memoria();

    if(conn == -1)
        return;

    u_msg_movimiento_tripulante_t* msg = u_msg_movimiento_tripulante_crear(pid, tid, *pos);

    u_buffer_t*  msg_ser     = u_msg_movimiento_tripulante_serializar(msg);
    u_package_t* package     = u_package_create(MOVIMIENTO_TRIPULANTE, msg_ser);
    u_buffer_t*  package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_movimiento_tripulante_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s. Conexion perdida con la Memoria", msg_str);
        u_free(msg_str);
    }

    discordia_recibir_respuesta_memoria(conn);

    u_msg_movimiento_tripulante_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    u_socket_close(conn);
}

void  discordia_iniciar_tarea(uint32_t tid, const char* tarea)
{
    int32_t conn = discordia_conectar_con_fs();

    if(conn == -1)
        return;

    u_msg_inicio_tarea_t* msg = u_msg_inicio_tarea_crear(tid, tarea);

    u_buffer_t*  msg_ser     = u_msg_inicio_tarea_serializar(msg);
    u_package_t* package     = u_package_create(INICIO_TAREA, msg_ser);
    u_buffer_t*  package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_inicio_tarea_to_string(msg);
        U_LOG_ERROR("No se pudo mandar el mensaje %s. Conexion perdida con el FS", msg_str);
        u_free(msg_str);
    }

    u_msg_inicio_tarea_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    u_socket_close(conn);
}

void  discordia_finalizar_tarea(uint32_t tid, const char* tarea)
{
    int32_t conn = discordia_conectar_con_fs();

    if(conn == -1)
        return;

    u_msg_finalizacion_tarea_t* msg = u_msg_finalizacion_tarea_crear(tid, tarea);

    u_buffer_t*  msg_ser     = u_msg_finalizacion_tarea_serializar(msg);
    u_package_t* package     = u_package_create(FINALIZACION_TAREA, msg_ser);
    u_buffer_t*  package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_finalizacion_tarea_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s. Conexion perdida con el FS", msg_str);
        u_free(msg_str);
    }

    u_msg_finalizacion_tarea_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    u_socket_close(conn);
}

void  discordia_tripulante_atiende_sabotaje(uint32_t tid)
{
    int32_t conn = discordia_conectar_con_fs();

    if(conn == -1)
        return;

    u_msg_atender_sabotaje_t* msg = u_msg_atiende_sabotaje_crear(tid);

    u_buffer_t*  msg_ser     = u_msg_atiende_sabotaje_serializar(msg);
    u_package_t* package     = u_package_create(ATIENDE_SABOTAJE, msg_ser);
    u_buffer_t*  package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_atiende_sabotaje_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s. Conexion perdida con el FS", msg_str);
        u_free(msg_str);
    }

    u_msg_atiende_sabotaje_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    u_socket_close(conn);

}

void  discordia_tripulante_resuelve_sabotaje(uint32_t tid)
{
    int32_t conn = discordia_conectar_con_fs();

    if(conn == -1)
        return;

    u_msg_resuelve_sabotaje_t* msg = u_msg_resuelve_sabotaje_crear(tid);

    u_buffer_t*  msg_ser     = u_msg_resuelve_sabotaje_serializar(msg);
    u_package_t* package     = u_package_create(ATIENDE_SABOTAJE, msg_ser);
    u_buffer_t*  package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_resuelve_sabotaje_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s. Conexion perdida con el FS", msg_str);
        u_free(msg_str);
    }

    u_msg_resuelve_sabotaje_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    u_socket_close(conn);
}

char* discordia_obtener_proxima_tarea(uint32_t pid, uint32_t tid)
{
    int32_t conn = discordia_conectar_con_memoria();
    char* tarea = NULL;

    if(conn == -1)
        return NULL;

    u_msg_proxima_tarea_t* msg = u_msg_proxima_tarea_crear(pid, tid);
    
    u_buffer_t*  msg_ser     = u_msg_proxima_tarea_serializar(msg);
    u_package_t* package     = u_package_create(PROXIMA_TAREA, msg_ser);
    u_buffer_t*  package_ser = u_package_serialize(package);

    if(u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
        tarea = discordia_obtener_tarea_de_memoria(conn);
    else
        U_LOG_ERROR(
            "No se pudo obtener la proxima tarea para el tripulante %d. Conexion perdida con la Memoria",
            tid);

    u_msg_proxima_tarea_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    u_socket_close(conn);

    return tarea;
}

void  discordia_tripulante_nuevo_estado(uint32_t pid, uint32_t tid, char estado)
{
    int32_t conn = discordia_conectar_con_memoria();

    if(conn == -1)
        return;

    u_msg_tripulante_nuevo_estado_t* msg = u_msg_tripulante_nuevo_estado_crear(
        pid,
        tid,
        estado
    );

    u_buffer_t*  msg_ser     = u_msg_tripulante_nuevo_estado_serializar(msg);
    u_package_t* package     = u_package_create(TRIPULANTE_NUEVO_ESTADO, msg_ser);
    u_buffer_t*  package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_tripulante_nuevo_estado_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s. Conexion perdida con la Memoria", msg_str);
        u_free(msg_str);
    }

    discordia_recibir_respuesta_memoria(conn);

    u_msg_tripulante_nuevo_estado_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    u_socket_close(conn);
}

void discordia_inicializar_patota(const char* ruta_tareas, uint32_t cant_trip, t_list* posiciones_trip)
{
    char* tareas = discordia_leer_archivo_tareas(ruta_tareas);

    if(tareas == NULL)
    {
        U_LOG_ERROR("No existe el archivo de tareas especificado: %s", ruta_tareas);
        return;
    }

    uint32_t pid = discordia_enviar_patota_a_memoria(cant_trip, tareas);
    if(pid)
        discordia_inicializar_tripulantes(pid, cant_trip, posiciones_trip);

    u_free(tareas);
}

void discordia_expulsar_tripulante(uint32_t tid)
{
    ds_planificador_eliminar_tripulante(tid);
}

void discordia_eliminar_tripulante(uint32_t pid, uint32_t tid)
{
    discordia_eliminar_tripulante_de_memoria(pid, tid);
}

void  discordia_iniciar_planificacion(void)
{
    U_LOG_INFO("Iniciar Planificador");
    ds_planificador_iniciar();
}

void  discordia_pausar_planificacion(void)
{
    U_LOG_INFO("Pausar Planificador");
    ds_planificador_pausar();
}

void  discordia_notificar_sabotaje(const u_pos_t* pos)
{
    U_LOG_INFO("Nuevo Sabotaje: { X: %d | Y:%d }", pos->x, pos->y);
    ds_planificador_notificar_sabotaje(pos);
}

void  discordia_finaliza_fsck(void)
{
    U_LOG_INFO("Finalización del FSCK. Se reanuda la planificacion");
}

// ======================================================
//              *** Funciones Privadas ****
// ======================================================

private uint32_t discordia_obtener_nuevo_pid(void)
{
    uint32_t new_pid = ++ p_discordia_instance->pid_counter;
    return new_pid;
}

private uint32_t discordia_obtener_nuevo_tid(void)
{
    uint32_t new_tid = ++ p_discordia_instance->tid_counter;
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

    u_sock_err_delete(err);

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

    u_sock_err_delete(err);

    return conn;
}

private void discordia_eliminar_tripulante_de_memoria(uint32_t pid, uint32_t tid)
{
    int32_t conn = discordia_conectar_con_memoria();

    if(conn == -1)
        return;

    u_msg_eliminar_tripulante_t* msg         = u_msg_eliminar_tripulante_crear(pid, tid);
    u_buffer_t*                  msg_ser     = u_msg_eliminar_tripulante_serializar(msg);
    u_package_t*                 package     = u_package_create(ELIMINAR_TRIPULANTE, msg_ser);
    u_buffer_t*                  package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_eliminar_tripulante_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s. Conexion perdida con la Memoria", msg_str);
        u_free(msg_str);
    }

    u_msg_eliminar_tripulante_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);
}

private char* discordia_obtener_bitacora_del_fs(int32_t conn)
{
    u_opcode_e opcode;
    char* bitacora = NULL;

    if(u_socket_recv(conn, &opcode, sizeof(uint32_t)))
    {
        if(opcode == FAIL)
        {
            discordia_recibir_fail(conn);
            return NULL;
        }
        else if(opcode != BITACORA)
        {
            U_LOG_ERROR("Se recibio un opcode invalido como respuesta a OBTENER_BITACORA");
            U_LOG_ERROR("Opcode recibido: %d", opcode);
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

    return bitacora;
}

private char* discordia_obtener_tarea_de_memoria(int32_t conn)
{
    u_opcode_e opcode;
    char* tarea = NULL;

    if(u_socket_recv(conn, &opcode, sizeof(uint32_t)))
    {
        if(opcode == FAIL)
        {
            discordia_recibir_fail(conn);
            return NULL;
        }
        else if(opcode != NUEVA_TAREA)
        {
            U_LOG_ERROR("Se recibio un opcode invalido como respuesta a PROXIMA_TAREA");
            U_LOG_ERROR("Opcode recibido: %d", opcode);
            return NULL;
        }
        uint32_t msg_length;
        void*    msg_buffer;

        if(u_socket_recv(conn, &msg_length, sizeof(uint32_t)))
        {
            msg_buffer = u_malloc(msg_length);

            if(u_socket_recv(conn, msg_buffer, msg_length))
            {
                u_buffer_t* buffer = u_buffer_create();
                u_buffer_write(buffer, msg_buffer, msg_length);

                u_msg_nueva_tarea_t* msg = u_msg_nueva_tarea_deserializar(buffer);
                
                if(msg->hay_tarea)
                    tarea = string_duplicate(msg->tarea);

                u_msg_nueva_tarea_eliminar(msg);
                u_buffer_delete(buffer);
            }
            else
                U_LOG_ERROR("No se pudo recibir una nueva tarea. Conexion perdida con la Memoria");

            u_free(msg_buffer);
        }
        else
            U_LOG_ERROR("No se pudo recibir una nueva tarea. Conexion perdida con la Memoria");
    }
    else
        U_LOG_ERROR("No se pudo recibir una nueva tarea. Conexion perdida con la Memoria");

    return tarea;
}

private char* discordia_recibir_tripulantes_de_memoria(int32_t conn)
{
    u_opcode_e opcode;
    char* lista_tripulantes_str = NULL;

    if(u_socket_recv(conn, &opcode, sizeof(uint32_t)))
    {
        uint32_t msg_length;
        void*    msg_buffer;

        if(opcode == FAIL)
        {
            discordia_recibir_fail(conn);
            return NULL;
        }
        else if(opcode != LISTA_TRIPULANTES)
        {
            U_LOG_ERROR("Se recibio un opcode invalido como respuesta a OBTENER_TRIPULANTES");
            U_LOG_ERROR("Opcode recibido: %d", opcode);
            return NULL;
        }

        if(u_socket_recv(conn, &msg_length, sizeof(uint32_t)))
        {
            msg_buffer = u_malloc(msg_length);

            if(u_socket_recv(conn, msg_buffer, msg_length))
            {
                u_buffer_t* buffer = u_buffer_create();
                u_buffer_write(buffer, msg_buffer, msg_length);

                u_msg_lista_tripulantes_t* msg = u_msg_lista_tripulantes_deserialize(buffer);
                
                lista_tripulantes_str = discordia_lista_tripulantes_to_string(msg);

                u_msg_lista_tripulantes_eliminar(msg);
                u_buffer_delete(buffer);
            }
            else
                U_LOG_ERROR("No se pudo obtener la lista de tripulantes. Conexion perdida con la Memoria");

            u_free(msg_buffer);
        }
        else
            U_LOG_ERROR("No se pudo obtener la lista de tripulantes. Conexion perdida con la Memoria");
    }
    else
        U_LOG_ERROR("No se pudo obtener la lista de tripulantes. Conexion perdida con la Memoria");

    return lista_tripulantes_str;
}

private char* discordia_lista_tripulantes_to_string(const u_msg_lista_tripulantes_t* msg)
{
    char* lista_tripulantes = string_new();
    t_list_iterator* it = list_iterator_create(msg->tripulantes);

    while(list_iterator_has_next(it))
    {
        u_tripulante_info_t* trip = (u_tripulante_info_t*)list_iterator_next(it);

         string_append_with_format(&lista_tripulantes,
            "Tripulante: %d\tPatota: %d\tStatus: %s\n",
            trip->tid, trip->pid, (trip->estado == 'E')
                ? "EXEC"
                : (trip->estado == 'B')
                ? "BLOCKED I/O"
                : (trip->estado == 'N')
                ? "NEW"
                :"READY"
        );
    }

    list_iterator_destroy(it);

    char* current_time = temporal_get_string_time("%d/%m/%y %H:%M:%S");
    char* reporte_estado = string_from_format(
        "Estado de la Nave: %s\n%s",
        current_time,
        lista_tripulantes
    );

    u_free(lista_tripulantes);
    u_free(current_time);

    return reporte_estado;
}

private void discordia_recibir_fail(int32_t conn)
{
    uint32_t msg_length;
    void*    msg_buffer;

    if(u_socket_recv(conn, &msg_length, sizeof(uint32_t)))
    {
        msg_buffer = u_malloc(msg_length);
        
        if(u_socket_recv(conn, msg_buffer, msg_length))
        {
            u_buffer_t* buffer = u_buffer_create();
            u_buffer_write(buffer, msg_buffer, msg_length);

            u_msg_fail_t* msg = u_msg_fail_deserializar(buffer);

            U_LOG_ERROR("Se recibio el siguiente mensaje de error");
            U_LOG_ERROR("%s", msg->description);

            u_msg_fail_eliminar(msg);
            u_buffer_delete(buffer);
        }

        u_free(msg_buffer);
    }
}

private uint32_t discordia_enviar_patota_a_memoria(uint32_t cant_trips, const char* tareas)
{
    int32_t conn = discordia_conectar_con_memoria();

    if(conn == -1)
        return 0;

    uint32_t pid = discordia_obtener_nuevo_pid();

    u_msg_iniciar_patota_t* msg         = u_msg_iniciar_patota_crear(pid, cant_trips, tareas);
    u_buffer_t*             msg_ser     = u_msg_iniciar_patota_serializar(msg);
    u_package_t*            package     = u_package_create(INICIAR_PATOTA, msg_ser);
    u_buffer_t*             package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
    {
        char* msg_str = u_msg_iniciar_patota_to_string(msg);
        U_LOG_ERROR("No se pudo enviar el mensaje %s. Conexion perdida con la Memoria", msg_str);
        u_free(msg_str);

        pid = 0;
    }
    else if(!discordia_recibir_respuesta_memoria(conn))
        pid = 0;

    u_msg_iniciar_patota_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    return pid;
}

private bool discordia_recibir_respuesta_memoria(int32_t conn)
{
    u_opcode_e opcode;

    if(u_socket_recv(conn, &opcode, sizeof(uint32_t)))
    {
        if(opcode == OK)
            return true;

        uint32_t msg_length;
        void*    msg_buffer;

        if(u_socket_recv(conn, &msg_length, sizeof(uint32_t)))
        {
            msg_buffer = u_malloc(msg_length);

            if(u_socket_recv(conn, msg_buffer, msg_length))
            {
                u_buffer_t* buffer = u_buffer_create();
                u_buffer_write(buffer, msg_buffer, msg_length);

                u_msg_fail_t* msg = u_msg_fail_deserializar(buffer);

                U_LOG_ERROR("Memoria contesto con el siguiente error");
                U_LOG_ERROR("%s", msg->description);

                u_msg_fail_eliminar(msg);
                u_buffer_delete(buffer);
                u_free(msg_buffer);
            }
            else
                U_LOG_ERROR("Memoria contesto FAIL. No se pudo obtener una descripcion");
        }
        else
            U_LOG_ERROR("Memoria contesto FAIL. No se pudo obtener una descripcion");
    }
    else
        U_LOG_ERROR("No se pudo obtener una respuesta. Conexion perdida con la Memoria");

    return false;
}

private void discordia_inicializar_tripulantes(uint32_t pid, int32_t cant_trips, t_list* positions)
{
    for(int32_t i = 0; i < cant_trips; i ++)
    {
        int32_t conn = discordia_conectar_con_memoria();

        if(conn == -1)
            continue;

        u_pos_t default_pos = { 0 };
        const u_pos_t* pos = &default_pos;

        if(positions && i < list_size(positions))
            pos = list_get(positions, i);

        uint32_t tid = discordia_obtener_nuevo_tid();

        u_msg_iniciar_tripulante_t* msg = u_msg_iniciar_tripulante_crear(
            pid,
            tid,
            *pos
        );

        u_buffer_t* msg_ser     = u_msg_iniciar_tripulante_serializar(msg);
        u_package_t* package    = u_package_create(INICIAR_TRIPULANTE, msg_ser);
        u_buffer_t* package_ser = u_package_serialize(package);

        if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
        {
            char* msg_str = u_msg_iniciar_tripulante_to_string(msg);
            U_LOG_ERROR("No se pudo mandar el mensaje %s. Conexion perdida con la Memoria", msg_str);
            u_free(msg_str);
        }

        discordia_recibir_respuesta_memoria(conn);
        u_msg_iniciar_tripulante_eliminar(msg);

        u_buffer_delete(msg_ser);
        u_buffer_delete(package_ser);
        u_package_delete(package);

        ds_planificador_iniciar_tripulante(pid, tid, pos);
    }
}

private char* discordia_leer_archivo_tareas(const char* ruta_tareas)
{
    FILE* file = fopen(ruta_tareas, "r");

    if(file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    uint64_t file_size = ftell(file) / sizeof(char);
    fseek(file, 0, SEEK_SET);

    char* content = u_malloc(file_size + 1);
    fread(content, sizeof(char), file_size, file);
    content[file_size] = '\0';
    fclose(file);

    return content;
}