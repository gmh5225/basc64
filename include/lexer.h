#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"

typedef struct {
    char *filename, *src, ch;
    size_t i, ln, col;
} Lexer;

Lexer *lex_init(char *filename);

Token *lex_next(Lexer *lex);

void _lex_debug(char *filename);

#endif
