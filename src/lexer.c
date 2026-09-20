#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

int lexer(char *input, Token tokens[])
{
    int count = 0;
    int i = 0;

    while (input[i] != '\0')
    {
        /* Skip spaces */
        while (isspace((unsigned char)input[i]))
        {
            i++;
        }

        if (input[i] == '\0')
        {
            break;
        }

        int j = 0;

        /* Read a token */
        while (input[i] != '\0' &&
               !isspace((unsigned char)input[i]))
        {
            /* Ignore double quotation marks */
            if (input[i] != '"')
            {
                if (j < MAX_TOKEN_LENGTH - 1)
                {
                    tokens[count].value[j++] = input[i];
                }
            }

            i++;
        }

        tokens[count].value[j] = '\0';
        tokens[count].type = TOKEN_WORD;

        count++;

        if (count >= MAX_TOKENS - 1)
        {
            break;
        }
    }

    /* Add END token */
    tokens[count].type = TOKEN_END;
    strcpy(tokens[count].value, "END");

    return count + 1;
}
