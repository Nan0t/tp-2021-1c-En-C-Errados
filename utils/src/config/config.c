#include "utils/config/config.h"
#include "utils/diagnostics/assert.h"
#include <commons/config.h>

#define CHECK_CONFIG_INSTANCES U_ASSERT(s_config_instances != NULL, "Config Instances is not initialized!");

private t_config* s_config_instances = NULL;

bool u_config_init(const char* file_path)
{
    if(s_config_instances)
        return true;

    s_config_instances = config_create((char*)file_path);

    return (s_config_instances == NULL) ? false : true;
}

void u_config_terminate(void)
{
    if(u_config_is_init())
    {
        config_destroy(s_config_instances);
        s_config_instances = NULL;
    }
}

bool u_config_is_init(void)
{
    return s_config_instances != NULL;
}

const char* u_config_get_string_value(const char* key)
{
    CHECK_CONFIG_INSTANCES
    return config_get_string_value(s_config_instances, (char*)key);
}

char** u_config_get_array_value(const char* key)
{
    CHECK_CONFIG_INSTANCES
    return config_get_array_value(s_config_instances, (char*)key);
}

int u_config_get_int_value(const char* key)
{
    CHECK_CONFIG_INSTANCES
    return config_get_int_value(s_config_instances, (char*)key);
}

float u_config_get_float_value(const char* key)
{
    CHECK_CONFIG_INSTANCES
    return (float)config_get_double_value(s_config_instances, (char*)key);
}

double u_config_get_double_value(const char* key)
{
    CHECK_CONFIG_INSTANCES
    return config_get_double_value(s_config_instances, (char*)key);
}
