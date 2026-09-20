#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"


/*
 * Read one shell word.
 *
 * Supports:
 *      echo hello
 *      echo "hello world"
 *      echo 'hello world'
 *
 * Quotes are removed from the final argument.
 */
static char *get_next_word(char **input)
{
    char *p = *input;

    while (*p == ' ' || *p == '\t')
        p++;

    if (*p == '\0' || *p == '\n')
    {
        *input = p;
        return NULL;
    }

    char buffer[1024];
    int index = 0;

    if (*p == '"' || *p == '\'')
    {
        char quote = *p;
        p++;

        while (*p != '\0' && *p != quote)
        {
            if (index < (int)sizeof(buffer) - 1)
                buffer[index++] = *p;

            p++;
        }

        if (*p == quote)
            p++;
    }
    else
    {
        while (*p != '\0' &&
               *p != ' ' &&
               *p != '\t' &&
               *p != '\n' &&
               *p != '<' &&
               *p != '>' &&
               *p != '&' &&
               *p != '|')
        {
            if (index < (int)sizeof(buffer) - 1)
                buffer[index++] = *p;

            p++;
        }
    }

    buffer[index] = '\0';

    *input = p;

    return strdup(buffer);
}


/*
 * Parse one command.
 */
Command *parse_command(char *input)
{
    Command *cmd = malloc(sizeof(Command));

    if (cmd == NULL)
        return NULL;

    cmd->argc = 0;
    cmd->command = NULL;
    cmd->input = NULL;
    cmd->output = NULL;
    cmd->append = 0;
    cmd->background = 0;

    char *p = input;

    while (*p != '\0' &&
           *p != '\n' &&
           cmd->argc < MAX_ARGS - 1)
    {
        while (*p == ' ' || *p == '\t')
            p++;

        if (*p == '\0' || *p == '\n')
            break;

        /*
         * Input redirection
         */
        if (*p == '<')
        {
            p++;

            while (*p == ' ' || *p == '\t')
                p++;

            cmd->input = get_next_word(&p);

            continue;
        }

        /*
         * Output redirection
         */
        if (*p == '>')
        {
            p++;

            if (*p == '>')
            {
                cmd->append = 1;
                p++;
            }
            else
            {
                cmd->append = 0;
            }

            while (*p == ' ' || *p == '\t')
                p++;

            cmd->output = get_next_word(&p);

            continue;
        }

        /*
         * Background
         */
        if (*p == '&')
        {
            cmd->background = 1;
            p++;
            continue;
        }

        /*
         * Pipe belongs to the pipeline parser.
         */
        if (*p == '|')
        {
            break;
        }

        char *word = get_next_word(&p);

        if (word == NULL)
            break;

        cmd->args[cmd->argc] = word;
        cmd->argc++;
    }

    cmd->args[cmd->argc] = NULL;

    if (cmd->argc > 0)
    {
        cmd->command = strdup(cmd->args[0]);
    }

    return cmd;
}


/*
 * Parse multiple commands separated by |.
 */
Pipeline *parse_pipeline(char *input)
{
    Pipeline *pipeline = malloc(sizeof(Pipeline));

    if (pipeline == NULL)
        return NULL;

    pipeline->count = 0;

    char *start = input;

    int in_single_quote = 0;
    int in_double_quote = 0;

    char *p = input;

    while (*p != '\0')
    {
        if (*p == '\'' && !in_double_quote)
            in_single_quote = !in_single_quote;

        else if (*p == '"' && !in_single_quote)
            in_double_quote = !in_double_quote;

        else if (*p == '|' &&
                 !in_single_quote &&
                 !in_double_quote)
        {
            char saved = *p;
            *p = '\0';

            Command *cmd = parse_command(start);

            *p = saved;

            if (cmd != NULL)
            {
                pipeline->commands[pipeline->count] = *cmd;
                free(cmd);

                pipeline->count++;
            }

            start = p + 1;
        }

        p++;
    }

    if (*start != '\0' &&
        pipeline->count < MAX_COMMANDS)
    {
        Command *cmd = parse_command(start);

        if (cmd != NULL)
        {
            pipeline->commands[pipeline->count] = *cmd;
            free(cmd);

            pipeline->count++;
        }
    }

    return pipeline;
}


/*
 * Free one command.
 */
void free_command(Command *cmd)
{
    if (cmd == NULL)
        return;

    for (int i = 0; i < cmd->argc; i++)
    {
        free(cmd->args[i]);
    }

    free(cmd->command);
    free(cmd->input);
    free(cmd->output);

    free(cmd);
}


/*
 * Free complete pipeline.
 */
void free_pipeline(Pipeline *pipeline)
{
    if (pipeline == NULL)
        return;

    for (int i = 0; i < pipeline->count; i++)
    {
        Command *cmd = &pipeline->commands[i];

        for (int j = 0; j < cmd->argc; j++)
        {
            free(cmd->args[j]);
        }

        free(cmd->command);
        free(cmd->input);
        free(cmd->output);
    }

    free(pipeline);
}
