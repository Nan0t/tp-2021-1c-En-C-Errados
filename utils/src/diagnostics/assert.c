#ifndef NDEBUG
#include "utils/diagnostics/assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>

extern void u_logger_vlog(u_log_level_e log_level, const char* message, va_list args);

void _u__assert_impl(const char* file, const char* function, int line, const char* message, ...)
{
    va_list args;
    va_start(args, message);

    if(u_logger_is_init())
    {
        U_LOG_ERROR("U_ASSERT: at file %s, function %s, at line %d", file, function, line);
        u_logger_vlog(U_LOG_LEVEL_ERROR, message, args);
    }
    else
    {
        fprintf(stderr, "U_ASSERT: at file %s, function %s, at line %d\n", file, function, line);
        vfprintf(stderr, message, args);
        puts("");
    }

    va_end(args);
    raise(SIGTRAP);
    exit(-1);
}
#endif