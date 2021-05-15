#include "utils/console/console.h"
#include "utils/memory/allocator.h"

#include <readline/readline.h>
#include <readline/history.h>

private bool   s_should_restore   = false;
private char*  s_last_buffer_data = NULL;
private int    s_last_cursor_pos  = 0;
private const char** s_commands   = NULL;

private char*  u_command_generator(const char* text, int count);
private char** u_fileman_completion(const char* text, int start, int end);
private char*  u_gets(const char* prompt);

char* u_console_read(const char* prompt)
{
    if(s_commands)
        rl_attempted_completion_function = u_fileman_completion;

    return u_gets(prompt);
}

void u_console_set_commands(const char** commands)
{
    s_commands = commands;
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

private char*  u_command_generator(const char* text, int count)
{
    static int list_index, len;
    const char *name;

    if(!count)
    {
        list_index = 0;
        len = strlen(text);
    }

    while((name = s_commands[list_index]))
    {
        list_index++;

        if(strncmp(name, text, len) == 0)
            return strdup(name);
    }

    return NULL;
}

private char** u_fileman_completion(const char* text, int start, int end)
{
	char **matches = NULL;

	if (start == 0)
		matches = rl_completion_matches(text, u_command_generator);

	return matches;
}

private char* u_gets(const char* prompt)
{
	char *line = readline (prompt);

	if(!line)
		return NULL;

    char* line_cpy = strdup(line);
	add_history (line_cpy);

    return line;
}