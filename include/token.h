#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>

typedef enum TokenType {
    TOK_ID,
    TOK_INT,
    TOK_FLOAT,
    TOK_STRING,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMI,
    TOK_COMMA,
    TOK_EQUAL,
    TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    size_t ln, col;
} Token;

Token *tok_init(TokenType type, char *value, size_t ln, size_t col);

char *toktype_to_str(TokenType type);

char *tok_to_str(Token *tok);

#endif
