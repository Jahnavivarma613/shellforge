#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "expand.h"
#include "executor.h"

/*
 * Display ShellForge banner.
 */
void print_banner(void)
{
    printf("==========================================\n");
    printf("              Shellforge\n");
    printf("       A Unix Style Shell written in C\n");
    printf("==========================================\n");
}


/*
 * Display tokens exactly in the format
 * shown in the milestone screenshots.
 */
void print_tokens(const char *input)
{
    char buffer[1024];

    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    printf("\n");
    printf("------------- TOKENS -------------\n");

    char *p = buffer;

    int index = 0;

    while (*p != '\0' && *p != '\n')
    {
        while (*p == ' ' || *p == '\t')
            p++;

        if (*p == '\0' || *p == '\n')
            break;

        /*
         * Pipe token
         */
        if (*p == '|')
        {
            printf("%2d : PIPE          |\n", index);
            index++;

            p++;
            continue;
        }

        /*
         * Input redirection
         */
        if (*p == '<')
        {
            printf("%2d : REDIRECT_IN   <\n", index);
            index++;

            p++;
            continue;
        }

        /*
         * Output redirection
         */
        if (*p == '>')
        {
            if (*(p + 1) == '>')
            {
                printf("%2d : APPEND        >>\n", index);
                p += 2;
            }
            else
            {
                printf("%2d : REDIRECT_OUT  >\n", index);
                p++;
            }

            index++;
            continue;
        }

        /*
         * Background
         */
        if (*p == '&')
        {
            printf("%2d : BACKGROUND    &\n", index);
            index++;

            p++;
            continue;
        }

        char word[1024];

        int word_index = 0;

        /*
         * Quoted word
         */
        if (*p == '"' || *p == '\'')
        {
            char quote = *p;

            p++;

            while (*p != '\0' &&
                   *p != quote)
            {
                if (word_index < 1023)
                    word[word_index++] = *p;

                p++;
            }

            if (*p == quote)
                p++;
        }

        /*
         * Normal word
         */
        else
        {
            while (*p != '\0' &&
                   *p != ' ' &&
                   *p != '\t' &&
                   *p != '\n' &&
                   *p != '|' &&
                   *p != '<' &&
                   *p != '>' &&
                   *p != '&')
            {
                if (word_index < 1023)
                    word[word_index++] = *p;

                p++;
            }
        }

        word[word_index] = '\0';

        if (word_index > 0)
        {
            printf("%2d : WORD          %s\n",
                   index,
                   word);

            index++;
        }
    }

    printf("%2d : END           END\n", index);

    printf("----------------------------------\n");
}


/*
 * Display pipeline and commands.
 */
void print_pipeline(Pipeline *pipeline)
{
    printf("\n");
    printf("========== PIPELINE ==========\n\n");

    if (pipeline == NULL)
        return;

    for (int i = 0; i < pipeline->count; i++)
    {
        Command *cmd = &pipeline->commands[i];

        printf("Command %d\n", i + 1);

        printf("------------------------------\n");

        printf("Arguments\n");

        for (int j = 0; j < cmd->argc; j++)
        {
            printf("argv[%d] = %s\n",
                   j,
                   cmd->args[j]);
        }

        if (cmd->input != NULL)
            printf("Input     : %s\n",
                   cmd->input);
        else
            printf("Input     : None\n");

        if (cmd->output != NULL)
            printf("Output    : %s\n",
                   cmd->output);
        else
            printf("Output    : None\n");

        printf("Append    : %s\n",
               cmd->append ? "Yes" : "No");

        printf("Background: %s\n",
               cmd->background ? "Yes" : "No");

        printf("==============================\n");

        /*
         * Do not add an extra blank command.
         */
        if (i < pipeline->count - 1)
            printf("\n");
    }
}


/*
 * Main ShellForge loop.
 */
int main(void)
{
    print_banner();

    char input[1024];

    while (1)
    {
        printf("shellforge$ ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        /*
         * Remove newline.
         */
        input[strcspn(input, "\n")] = '\0';

        /*
         * Empty input.
         */
        if (strlen(input) == 0)
            continue;

        /*
         * Exit ShellForge.
         */
        if (strcmp(input, "exit") == 0)
            break;

        /*
         * Show lexer/token output.
         */
        print_tokens(input);

        /*
         * Parser modifies its input,
         * so make a separate copy.
         */
        char pipeline_input[1024];

        strncpy(pipeline_input,
                input,
                sizeof(pipeline_input) - 1);

        pipeline_input[sizeof(pipeline_input) - 1] = '\0';

        Pipeline *pipeline =
            parse_pipeline(pipeline_input);

        if (pipeline == NULL)
        {
            printf("Parser error\n");
            continue;
        }

        /*
         * Expand environment variables.
         */
        expand_pipeline(pipeline);

        /*
         * Display parsed pipeline.
         */
	print_pipeline(pipeline);
	/* Execute the commands. */	
	execute_pipeline(pipeline);
        /*
         * Free parser memory.
         */
        free_pipeline(pipeline);
    }

    return 0;
}
