#include "server.h"
#include <utils/utils.h>
#include <utils/entry_point.h>

// Info de la aplicacion que es utilizada por las utils
// Debemos definir esta funcion para mandarle informacion
// a las utils sobre el nombre de la aplicacion, la ruta
// del archivo de configuracion, del log y el nivel de logeo.
app_attr_t get_app_setup(void)
{
    return (app_attr_t) {
        .app_name           = "Server",
        .config_file_path   = "config/server.conf",
        .log_active_console = true,
        .log_file_path      = "Server.log",
        .log_level          = U_LOG_LEVEL_TRACE
    };
}

int entry_point(int argc, char** argv)
{
    (void) argc; // Pequeño hack para no recibir un warning unused_parameters al compilar (ya que nunca usamos estas variables).
    (void) argv; // Pequeño hack para no recibir un warning unused_parameters al compilar (ya que nunca usamos estas variables).

    return server_init(u_config_get_string_value("PORT"));
}