#include "print.h"
#include <stdio.h>

extern void u_console_save_state(void);
extern void u_console_restore_state(void);

void ds_print(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    u_console_save_state();
    vprintf(format, args);
    u_console_restore_state();

    va_end(args);
}