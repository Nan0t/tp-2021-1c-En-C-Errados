#include "utils/diagnostics/log.h"
#include "utils/diagnostics/assert.h"

#include <commons/log.h>
#include <pthread.h>

#include <stdarg.h>

typedef struct
{
    pthread_mutex_t  _log_mx;
    t_log*           _log;
} u_logger_t;

extern void u_console_save_state(void);
extern void u_console_restore_state(void);

private u_logger_t s_logger_instances = { 0 };

void u_logger_init(const char* file_name, const char* program, bool log_to_console, u_log_level_e log_level)
{
    if(s_logger_instances._log)
        return;

    s_logger_instances._log = log_create((char*)file_name, (char*)program, log_to_console, log_level);
    pthread_mutex_init(&s_logger_instances._log_mx, NULL);
}

void u_logger_terminate(void)
{
    if(s_logger_instances._log)
    {
        log_destroy(s_logger_instances._log);
        pthread_mutex_destroy(&s_logger_instances._log_mx);

        s_logger_instances._log = NULL;
    }
}

void u_logger_log(u_log_level_e log_level, const char* message, ...)
{
    U_ASSERT(u_logger_is_init(), "The logger is not initialized");

    va_list args;
    va_start(args, message);

    pthread_mutex_lock(&s_logger_instances._log_mx);
    u_console_save_state();

    switch (log_level)
    {
    case U_LOG_LEVEL_TRACE:
        vlog_trace(s_logger_instances._log, message, args);
        break;

    case U_LOG_LEVEL_DEBUG:
        vlog_debug(s_logger_instances._log, message, args);
        break;

    case U_LOG_LEVEL_INFO:
        vlog_info(s_logger_instances._log, message, args);
        break;

    case U_LOG_LEVEL_WARN:
        vlog_warning(s_logger_instances._log, message, args);
        break;

    case U_LOG_LEVEL_ERROR:
        vlog_error(s_logger_instances._log, message, args);
        break;

    default:
        break;
    }

    va_end(args);

    u_console_restore_state();
    pthread_mutex_unlock(&s_logger_instances._log_mx);
}

bool u_logger_is_init(void)
{
    return s_logger_instances._log != NULL;
}

// Function to be used by assert

#ifndef NDEBUG
void u_logger_vlog(u_log_level_e log_level, const char* message, va_list args)
{
    pthread_mutex_lock(&s_logger_instances._log_mx);
    u_console_save_state();

    switch (log_level)
    {
    case U_LOG_LEVEL_TRACE:
        vlog_trace(s_logger_instances._log, message, args);
        break;

    case U_LOG_LEVEL_DEBUG:
        vlog_debug(s_logger_instances._log, message, args);
        break;

    case U_LOG_LEVEL_INFO:
        vlog_info(s_logger_instances._log, message, args);
        break;

    case U_LOG_LEVEL_WARN:
        vlog_warning(s_logger_instances._log, message, args);
        break;

    case U_LOG_LEVEL_ERROR:
        vlog_error(s_logger_instances._log, message, args);
        break;

    default:
        break;
    }

    u_console_restore_state();
    pthread_mutex_unlock(&s_logger_instances._log_mx);    
}
#endif