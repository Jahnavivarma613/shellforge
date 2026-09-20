#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 64
#define MAX_COMMANDS 16

typedef struct
{
    char *args[MAX_ARGS];
    int argc;

    char *command;

    char *input;
    char *output;

    int append;
    int background;

} Command;

typedef struct
{
    Command commands[MAX_COMMANDS];
    int count;

} Pipeline;

Command *parse_command(char *input);

Pipeline *parse_pipeline(char *input);

void free_command(Command *cmd);

void free_pipeline(Pipeline *pipeline);

#endif
