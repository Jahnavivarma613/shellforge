#include <stdio.h>
#include <string.h>

#include "token.h"
#include "lexer.h"

int main()
{
    char input[1000];
    Token tokens[MAX_TOKENS];

    printf("====================================\n");
    printf("            Shellforge\n");
    printf("    A Unix Style Shell written in C\n");
    printf("====================================\n");

    while (1)
    {
        printf("shellforge$ ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
        {
            break;
        }

        if (strlen(input) == 0)
        {
            continue;
        }

        int token_count = lexer(input, tokens);

        printf("\n------------ TOKENS ------------\n");

        for (int i = 0; i < token_count; i++)
        {
            print_token(&tokens[i], i);
        }

        printf("--------------------------------\n");
    }

    return 0;
}
