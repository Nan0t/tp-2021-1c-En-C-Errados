#include <utils/entry_point.h>

app_attr_t get_app_setup(void)
{
    return (app_attr_t) {
        .app_name           = "Mi-RAM-HQ",
        .config_file_path   = "config/Mi-RAM-HQ.conf",
        .log_active_console = true,
        .log_file_path      = "Mi-RAM-HQ.log",
        .log_level          = U_LOG_LEVEL_TRACE
    };
}

int entry_point(int argc, char** argv)
{
    (void)argc; // --> para evitar el warning unused_parameter
    (void)argv; // --> para evitar el warning unused_parameter

    U_LOG_INFO("Modulo Mi-RAM-HQ");
    return 0;
}