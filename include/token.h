#ifndef TOKEN_H
#define TOKEN_H

#define MAX_TOKEN_LENGTH 100
#define MAX_TOKENS 100

typedef enum {
    TOKEN_WORD,
    TOKEN_END
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
} Token;

void print_token(Token *token, int index);

#endif
