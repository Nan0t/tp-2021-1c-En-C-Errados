#include "fs/file_system.h"
#include "server/server.h"

#include <utils/entry_point.h>

#include <stdlib.h>

app_attr_t get_app_setup(void)
{
    return (app_attr_t) {
        .app_name           = "I-Mongo-Store",
        .config_file_path   = "config/I-Mongo-Store.conf",
        .log_active_console = true,
        .log_file_path      = "I-Mongo-Store.log",
        .log_level          = U_LOG_LEVEL_TRACE
    };
}

private t_queue* get_sabotage_positions(void);

int entry_point(int argc, char** argv)
{
    (void)argc; // --> para evitar el warning unused_parameter
    (void)argv; // --> para evitar el warning unused_parameter

    file_system_init(&(file_system_attr_t) {
        .mount_point        = u_config_get_string_value("PUNTO_MONTAJE"),
        .sabotage_positions = get_sabotage_positions(),
        .sync_time          = u_config_get_int_value("TIEMPO_SINCRONIZACION")
    });

    fs_server_init(u_config_get_string_value("PUERTO"));

    return 0;
}

private t_queue* get_sabotage_positions(void)
{
    char** positions           = u_config_get_array_value("POSICIONES_SABOTAJE");
    t_queue* sabotage_positions = queue_create();

    for(char** pos = positions; *pos != NULL;  pos ++)
    {
        u_pos_t* pos_struct = u_malloc(sizeof(u_pos_t));

        char** pos_values = string_split(*pos, "|");

        pos_struct->x = (uint32_t)atoi(pos_values[0]);
        pos_struct->y = (uint32_t)atoi(pos_values[1]);

        queue_push(sabotage_positions, pos_struct);

        u_free(pos_values[0]);
        u_free(pos_values[1]);
        u_free(pos_values);

        u_free(*pos);
    }

    u_free(positions);

    return sabotage_positions;
}