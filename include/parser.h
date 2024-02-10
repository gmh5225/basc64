#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "list.h"

typedef struct {
    Token *tok;
    Lexer *lex;
} Parser;

Parser *prs_init(Lexer *lex);

List *prs_params(Parser *prs);

List *prs_args(Parser *prs);

List *prs_block(Parser *prs);

AST *prs_statement(Parser *prs);

List *prs_to_ast_lst(char *filename);

#endif
