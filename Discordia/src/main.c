#include <utils/entry_point.h>
#include "discordia/discordia.h"

app_attr_t get_app_setup(void)
{
    return (app_attr_t) {
        .app_name           = "Discordia",
        .config_file_path   = "config/Discordia.conf",
        .log_active_console = true,
        .log_file_path      = "Discordia.log",
        .log_level          = U_LOG_LEVEL_INFO
    };
}

int entry_point(int argc, char** argv)
{
    (void)argc; // --> para evitar el warning unused_parameter
    (void)argv; // --> para evitar el warning unused_parameter

    return discordia_init();
}