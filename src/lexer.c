#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/lexer.h"
#include "../include/token.h"

int lexing_string = 0;

char *lex_read_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "error: lexer: could not open file '%s' for reading\n", filename);
        exit(1);
    }

    char *src = calloc(1, sizeof(char)), ch;
    size_t i = 0;

    while ((ch = fgetc(fp)) != EOF) {
        src = realloc(src, (i + 2) * sizeof(char));
        src[i++] = ch;
    }

    src[i] = '\0';
    fclose(fp);

    return src;
}

Lexer *lex_init(char *filename) {
    Lexer *lex = malloc(sizeof(Lexer));
    lex->filename = filename;
    lex->src = lex_read_file(filename);
    lex->ch = lex->src[0];
    lex->i = 0;
    lex->ln = lex->col = 1;
    return lex;
}

void lex_forward(Lexer *lex) {
    if (lex->i < strlen(lex->src) && lex->ch != '\0') {
        if (lex->ch == '\n') {
            lex->ln++;
            lex->col = 1;
        }
        else
            lex->col++;

        lex->ch = lex->src[++lex->i];
    }
}

char lex_peek(Lexer *lex, int offset) {
    if (lex->i + offset > strlen(lex->src))
        return lex->src[strlen(lex->src) - 1];

    return lex->src[lex->i + offset];
}

void lex_comment(Lexer *lex) {
    lex_forward(lex);
    lex_forward(lex);

    while (lex->ch != '*' && lex_peek(lex, 1) != '/' && lexing_string == 0) {
        if (lex->ch == '"')
            lexing_string = lexing_string == 1 ? 0 : 1;

        lex_forward(lex);
    }

    lex_forward(lex);
    lex_forward(lex);
}

Token *lex_forward_make(Lexer *lex, TokenType type, char *value) {
    char *value_cpy = calloc(strlen(value) + 1, sizeof(char));
    strcpy(value_cpy, value);
    size_t ln = lex->ln, col = lex->col;

    for (size_t i = 0; i < strlen(value); i++)
        lex_forward(lex);

    return tok_init(type, value_cpy, ln, col);
}

Token *lex_alpha(Lexer *lex) {
    char *value = calloc(1, sizeof(char));
    size_t i = 0, ln = lex->ln, col = lex->col;

    while ((isalpha(lex->ch) || lex->ch == '_') || (isdigit(lex->ch) && i > 0)) {
        value = realloc(value, (i + 2) * sizeof(char));
        value[i++] = lex->ch;
        lex_forward(lex);
    }

    value[i] = '\0';

    return tok_init(TOK_ID, value, ln, col);
}

Token *lex_digit(Lexer *lex) {
    char *value = calloc(1, sizeof(char));
    size_t i = 0, ln = lex->ln, col = lex->col;
    int has_decimal = 0;

    while (isdigit(lex->ch) || (lex->ch == '-' && i == 0) || (lex->ch == '.' && has_decimal == 0)) {
        value = realloc(value, (i + 2) * sizeof(char));
        value[i++] = lex->ch;

        if (lex->ch == '.')
            has_decimal = 1;

        lex_forward(lex);
    }

    value[i] = '\0';

    return has_decimal == 1 ? tok_init(TOK_FLOAT, value, ln, col) : tok_init(TOK_INT, value, ln, col);
}

Token *lex_string(Lexer *lex) {
    lex_forward(lex);
    char *value = calloc(1, sizeof(char));
    size_t i = 0, ln = lex->ln, col = lex->col;

    while (lex->ch != '"') {
        value = realloc(value, (i + 2) * sizeof(char));
        value[i++] = lex->ch;
        lex_forward(lex);
    }

    value[i] = '\0';
    lex_forward(lex);

    return tok_init(TOK_STRING, value, ln, col);
}

Token *lex_char(Lexer *lex) {
    lex_forward(lex);
    char *value = calloc(32, sizeof(char));
    size_t ln = lex->ln, col = lex->col;

    if (lex->ch == '\\') {
        lex_forward(lex);
        switch (lex->ch) {
            case 'n': strcpy(value, "10"); break;
            case 't': strcpy(value, "13"); break;
            case 'r': strcpy(value, "9"); break;
            case '0': strcpy(value, "0"); break;
            default:
                fprintf(stderr, "error: lexer: %s:%zu:%zu: unknown escape sequence '\\%c'\n", lex->filename, ln, col, lex->ch);
                exit(1);
        }
    }
    else
        sprintf(value, "%d", lex->ch);

    if (lex->ch != '\'') {
        fprintf(stderr, "error: lexer: %s:%zu:%zu: expected ' to close char, found character '%c'\n", lex->filename, lex->ln, lex->col, lex->ch);
        exit(1);
    }

    lex_forward(lex);

    return tok_init(TOK_INT, value, ln, col);
}

Token *lex_next(Lexer *lex) {
    while (isspace(lex->ch))
        lex_forward(lex);

    if (lex->ch == '/' && lex_peek(lex, 1) == '*')
        lex_comment(lex);

    if (isalpha(lex->ch) || lex->ch == '_')
        return lex_alpha(lex);
    else if (isdigit(lex->ch) || (lex->ch == '-' && isdigit(lex_peek(lex, 1))))
        return lex_digit(lex);

    switch (lex->ch) {
        case '"': return lex_string(lex);
        case '\'': return lex_char(lex);
        case '(': return lex_forward_make(lex, TOK_LPAREN, "(");
        case ')': return lex_forward_make(lex, TOK_RPAREN, ")");
        case '{': return lex_forward_make(lex, TOK_LBRACE, "{");
        case '}': return lex_forward_make(lex, TOK_RBRACE, "}");
        case ';': return lex_forward_make(lex, TOK_SEMI, ";");
        case ',': return lex_forward_make(lex, TOK_COMMA, ",");
        case '=': return lex_forward_make(lex, TOK_EQUAL, "=");
        case '\0': return tok_init(TOK_EOF, 0, lex->ln, lex->col);
        default:
            fprintf(stderr, "error: lexer: %s:%zu:%zu: unknown character '%c'\n", lex->filename, lex->ln, lex->col, lex->ch);
            exit(1);
    }
}

void _lex_debug(char *filename) {
    Lexer *lex = lex_init(filename);
    Token *tok;

    while ((tok = lex_next(lex))->type != TOK_EOF) {
        char *str = tok_to_str(tok);
        printf("%s\n", str);
        free(str);

        free(tok->value);
        free(tok);
    }

    free(tok);
    free(lex->src);
    free(lex);
}
