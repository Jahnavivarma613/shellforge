#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expand.h"


static char *expand_variable(const char *text)
{
    if (text == NULL)
        return NULL;

    /*
     * Normal argument.
     */
    if (text[0] != '$')
        return strdup(text);

    /*
     * Environment variable.
     */
    const char *name = text + 1;

    const char *value = getenv(name);

    if (value == NULL)
        return strdup("");

    return strdup(value);
}


void expand_command(Command *cmd)
{
    if (cmd == NULL)
        return;

    for (int i = 0; i < cmd->argc; i++)
    {
        char *expanded = expand_variable(cmd->args[i]);

        if (expanded != NULL)
        {
            free(cmd->args[i]);
            cmd->args[i] = expanded;
        }
    }

    if (cmd->argc > 0)
    {
        free(cmd->command);
        cmd->command = strdup(cmd->args[0]);
    }
}


void expand_pipeline(Pipeline *pipeline)
{
    if (pipeline == NULL)
        return;

    for (int i = 0; i < pipeline->count; i++)
    {
        expand_command(&pipeline->commands[i]);
    }
}
