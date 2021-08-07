#include "sabotage_notifier.h"
#include <signal.h>

private t_queue* p_sabotaje_positions = NULL;

private void fs_sabotaje_notifier_sig_handler(int sig);
private int32_t fs_sabotage_notifier_connect_to_discordia(void);

void fs_sabotage_notifier_init(t_queue* sabotage_positions)
{
    if(p_sabotaje_positions)
        return;

    p_sabotaje_positions = sabotage_positions;

    signal(SIGUSR1, fs_sabotaje_notifier_sig_handler);
}

private void fs_sabotaje_notifier_sig_handler(int sig)
{
    U_LOG_TRACE("Sig Usr 1");
    
    int32_t conn = fs_sabotage_notifier_connect_to_discordia();
    (void)sig;

    if(conn == -1)
        return;

    if(queue_is_empty(p_sabotaje_positions))
    {
        U_LOG_WARN("No hay mas posiciones de sabotaje disponibles");
        return;
    }
    u_pos_t* next_sabotage_pos = queue_pop(p_sabotaje_positions);

    u_msg_posicion_sabotaje_t* msg         = u_msg_sabotaje_crear(next_sabotage_pos->x, next_sabotage_pos->y);
    u_buffer_t*                msg_ser     = u_msg_sabotaje_serializar(msg);
    u_package_t*               package     = u_package_create(INFORMAR_SABOTAJE, msg_ser);
    u_buffer_t*                package_ser = u_package_serialize(package);

    if(!u_socket_send(conn, u_buffer_get_content(package_ser), u_buffer_get_size(package_ser)))
        U_LOG_ERROR("No se pudo informar del sabotaje al Discordia");

    u_msg_informar_sabotaje_eliminar(msg);
    u_buffer_delete(msg_ser);
    u_buffer_delete(package_ser);
    u_package_delete(package);

    u_free(next_sabotage_pos);

    u_socket_close(conn);

    U_LOG_INFO("Nuevo sabotaje en el FS: { x: %d | y: %d }", next_sabotage_pos->x, next_sabotage_pos->y);
}

private int32_t fs_sabotage_notifier_connect_to_discordia(void)
{
    u_sock_err_t* err = u_sock_err_create();
    int32_t conn = u_socket_connect(
        u_config_get_string_value("IP_DISCORDIA"),
        u_config_get_string_value("PUERTO_DISCORDIA"),
        err
    );

    if(conn == -1)
        U_LOG_ERROR("No se pudo conectar al Discordia: %s", u_sock_err_get_description(err));

    u_sock_err_delete(err);
    
    return conn;
}