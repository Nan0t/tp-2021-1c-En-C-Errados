#include "utils/console/console.h"
#include "utils/memory/allocator.h"

#include <readline/readline.h>

private bool  s_should_restore   = false;
private char* s_last_buffer_data = NULL;
private int   s_last_cursor_pos  = 0;

char* u_console_read(const char* prompt)
{
    return readline(prompt);
}

void u_console_save_state(void)
{
    s_should_restore = (RL_STATE_READCMD & rl_readline_state) > 0;

    if(s_should_restore)
    {
        s_last_cursor_pos  = rl_point;
        s_last_buffer_data = rl_copy_text(0, rl_end);
        rl_save_prompt();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

void u_console_restore_state(void)
{
    if(s_should_restore)
    {
        rl_restore_prompt();
        rl_replace_line(s_last_buffer_data, 0);
        rl_point = s_last_cursor_pos;
        rl_redisplay();
        u_free(s_last_buffer_data);
    }
}