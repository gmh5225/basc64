#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/token.h"

Token *tok_init(TokenType type, char *value, size_t ln, size_t col) {
    Token *tok = malloc(sizeof(Token));
    tok->type = type;
    tok->value = value;
    tok->ln = ln;
    tok->col = col;
    return tok;
}

char *toktype_to_str(TokenType type) {
    switch (type) {
        case TOK_ID: return "ID";
        case TOK_INT: return "INT";
        case TOK_FLOAT: return "FLOAT";
        case TOK_STRING: return "STRING";
        case TOK_LPAREN: return "LPAREN";
        case TOK_RPAREN: return "RPAREN";
        case TOK_LBRACE: return "LBRACE";
        case TOK_RBRACE: return "RBRACE";
        case TOK_SEMI: return "SEMI";
        case TOK_COMMA: return "COMMA";
        case TOK_EQUAL: return "EQUAL";
        case TOK_EOF: return "EOF";
        default: return "UNDEFINED";
    }
}

char *tok_to_str(Token *tok) {
    const char template[] = "<Token type='%s' value='%s' defined=%zu:%zu>";
    char *type = toktype_to_str(tok->type);
    char *str = calloc(strlen(template) + strlen(type) + strlen(tok->value) + 128, sizeof(char));
    sprintf(str, template, type, tok->value, tok->ln, tok->col);
    return str;
}
