
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "parser.h"

int execute_pipeline(Pipeline *pipeline)
{
    if (pipeline == NULL || pipeline->count == 0)
        return -1;

    int pipes[MAX_COMMANDS - 1][2];
    pid_t pids[MAX_COMMANDS];

    for (int i = 0; i < pipeline->count - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            return -1;
        }
    }

    for (int i = 0; i < pipeline->count; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            return -1;
        }

        if (pid == 0)
        {
            Command *cmd = &pipeline->commands[i];

            /* Connect input from previous command */
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            /* Connect output to next command */
            if (i < pipeline->count - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            /* Close all pipe descriptors */
            for (int j = 0; j < pipeline->count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            /* Input redirection */
            if (cmd->input != NULL)
            {
                int fd = open(cmd->input, O_RDONLY);

                if (fd == -1)
                {
                    perror(cmd->input);
                    exit(1);
                }

                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            /* Output redirection */
            if (cmd->output != NULL)
            {
                int flags = O_WRONLY | O_CREAT;

                if (cmd->append)
                    flags |= O_APPEND;
                else
                    flags |= O_TRUNC;

                int fd = open(cmd->output, flags, 0644);

                if (fd == -1)
                {
                    perror(cmd->output);
                    exit(1);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            if (cmd->argc == 0)
                exit(0);

            execvp(cmd->command, cmd->args);

            perror(cmd->command);
            exit(127);
        }

        pids[i] = pid;
    }

    /* Parent closes all pipe descriptors */
    for (int i = 0; i < pipeline->count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /* Wait unless command is running in background */
    if (!pipeline->commands[pipeline->count - 1].background)
    {
        for (int i = 0; i < pipeline->count; i++)
        {
            waitpid(pids[i], NULL, 0);
        }
    }

    return 0;
}
