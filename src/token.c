#include <stdio.h>
#include "token.h"

void print_token(Token *token, int index)
{
    if (token->type == TOKEN_WORD)
    {
        printf("%d : WORD    %s\n", index, token->value);
    }
    else if (token->type == TOKEN_END)
    {
        printf("%d : END     END\n", index);
    }
}
