#include <utils/entry_point.h>
#include "servidor/servidor.h"
#include <utils/utils.h>
#include "memoria/esquemas/esquema_memoria.h"
#include "map/map.h"

app_attr_t get_app_setup(void)
{
    return (app_attr_t) {
        .app_name           = "Mi-RAM-HQ",
        .config_file_path   = "config/Mi-RAM-HQ.conf",
        .log_active_console = false,
        .log_file_path      = "Mi-RAM-HQ.log",
        .log_level          = U_LOG_LEVEL_TRACE
    };
}

__attribute__((noreturn))
private void setup()
{
    esquema_memoria_init();
    //map_init();

    if(servidor_init(u_config_get_string_value("PUERTO")))
        exit(-1);

    while(1);
}

int entry_point(int argc, char** argv)
{
    (void)argc; // --> para evitar el warning unused_parameter
    (void)argv; // --> para evitar el warning unused_parameter

    U_LOG_INFO("Modulo Mi-RAM-HQ iniciado");
    setup();
    
    return 0;
}