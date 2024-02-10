#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/token.h"
#include "../include/lexer.h"
#include "../include/ast.h"
#include "../include/list.h"
#include "../include/symtab.h"

char *scope;

Parser *prs_init(Lexer *lex) {
    Parser *prs = malloc(sizeof(Parser));
    prs->lex = lex;
    prs->tok = lex_next(lex);
    return prs;
}

void prs_eat(Parser *prs, TokenType type) {
    if (type != prs->tok->type) {
        fprintf(stderr, "error: parser: %s:%zu:%zu: expected token type '%s', found '%s'\n", prs->lex->filename, prs->tok->ln, prs->tok->col, toktype_to_str(type), toktype_to_str(prs->tok->type));
        exit(1);
    }

    char *str = tok_to_str(prs->tok);
    printf("%s\n", str);
    free(str);

    free(prs->tok->value);
    free(prs->tok);
    prs->tok = lex_next(prs->lex);
}

AST *prs_alpha(Parser *prs) {
    char *id = calloc(strlen(prs->tok->value) + 1, sizeof(char));
    strcpy(id, prs->tok->value);
    size_t ln = prs->lex->ln, col = prs->lex->col;
    prs_eat(prs, TOK_ID);

    AST *ast;
    Symbol *sym;

    if (strcmp(id, "return") == 0) {
        free(id);
        ast = ast_init(AST_RETURN, ln, col, scope);
        ast->return_.value = prs->tok->type == TOK_SEMI ? NULL : prs_statement(prs);
        return ast;
    }

    switch (prs->tok->type) {
        case TOK_ID: {
            char *id2 = calloc(strlen(prs->tok->value) + 1, sizeof(char));
            strcpy(id2, prs->tok->value);
            prs_eat(prs, TOK_ID);

            if (prs->tok->type == TOK_LPAREN) {
                ast = ast_init(AST_FUNCTION, ln, col, scope);
                ast->function.name = id2;
                ast->function.type = id;
                ast->function.params = prs_params(prs);

                char *prev_scope = calloc(strlen(scope) + 1, sizeof(char));
                strcpy(prev_scope, scope);
                scope = realloc(scope, (strlen(id2) + 1) * sizeof(char));
                strcpy(scope, id2);

                ast->function.block = prs_block(prs);

                scope = realloc(scope, (strlen(prev_scope) + 1) * sizeof(char));
                strcpy(scope, prev_scope);
                free(prev_scope);

                sym = sym_init(SYM_FUNCTION, ln, col, scope);
                sym->function.name = id2;
                sym->function.type = id;
                sym->function.params = ast->function.params;
            }
            else if (prs->tok->type == TOK_EQUAL || prs->tok->type == TOK_SEMI) {
                ast = ast_init(AST_VAR_DEF, ln, col, scope);
                ast->var_def.name = id2;
                ast->var_def.type = id;

                if (prs->tok->type == TOK_EQUAL) {
                    prs_eat(prs, TOK_EQUAL);
                    ast->var_def.value = prs_statement(prs);
                }
                else
                    ast->var_def.value = NULL;

                sym = sym_init(SYM_VAR, ln, col, scope);
                sym->var.name = id2;
                sym->var.type = id;
            }
            else {
                fprintf(stderr, "error: parser: %s:%zu:%zu: unexpected statement of type '%s' following an ID, expected function or variable declaration\n", prs->lex->filename, prs->tok->ln, prs->tok->col, toktype_to_str(prs->tok->type));
                exit(1);
            }

            sym_add(sym);
            break;
        }
        case TOK_LPAREN:
            ast = ast_init(AST_CALL, ln, col, scope);
            ast->call.name = id;
            ast->call.args = prs_args(prs);
            break;
        default:
            ast = ast_init(AST_VAR, ln, col, scope);
            ast->var.name = id;
            break;
    }

    return ast;
}

AST *prs_digit(Parser *prs) {
    AST *ast = prs->tok->type == TOK_INT ? ast_init(AST_INT, prs->tok->ln, prs->tok->col, scope) : ast_init(AST_FLOAT, prs->tok->ln, prs->tok->col, scope);
    ast->constant.value = calloc(strlen(prs->tok->value) + 1, sizeof(char));
    strcpy(ast->constant.value, prs->tok->value);
    prs_eat(prs, prs->tok->type);

    return ast;
}

AST *prs_string(Parser *prs) {
    AST *ast = ast_init(AST_STRING, prs->tok->ln, prs->tok->col, scope);
    ast->constant.value = calloc(strlen(prs->tok->value) + 1, sizeof(char));
    strcpy(ast->constant.value, prs->tok->value);
    prs_eat(prs, prs->tok->type);

    return ast;
}

List *prs_params(Parser *prs) {
    List *params = lst_init(sizeof(AST *));
    prs_eat(prs, TOK_LPAREN);

    while (prs->tok->type != TOK_RPAREN) {
        if (params->size > 0)
            prs_eat(prs, TOK_COMMA);

        AST *param = prs_statement(prs);
        if (param->type != AST_VAR_DEF) {
            fprintf(stderr, "error: parser: %s:%zu:%zu: expected type VAR_DEF for parameter, found type '%s'\n", prs->lex->filename, prs->tok->ln, prs->tok->col, asttype_to_str(param->type));
            exit(1);
        }

        lst_push(params, param);
    }

    prs_eat(prs, TOK_RPAREN);

    return params;
}

List *prs_args(Parser *prs) {
    List *args = lst_init(sizeof(AST *));
    prs_eat(prs, TOK_LPAREN);

    while (prs->tok->type != TOK_RPAREN) {
        if (args->size > 0)
            prs_eat(prs, TOK_COMMA);

        AST *arg = prs_statement(prs);
        if (arg->type != AST_VAR && arg->type != AST_INT && arg->type != AST_FLOAT && arg->type != AST_STRING) {
            fprintf(stderr, "error: parser: %s:%zu:%zu: expected type VAR, INT, FLOAT or STRING for argument, found type '%s'\n", prs->lex->filename, prs->tok->ln, prs->tok->col, asttype_to_str(arg->type));
            exit(1);
        }

        lst_push(args, arg);
    }

    prs_eat(prs, TOK_RPAREN);

    return args;
}

List *prs_block(Parser *prs) {
    List *block = lst_init(sizeof(AST *));
    prs_eat(prs, TOK_LBRACE);

    while (prs->tok->type != TOK_RBRACE) {
        lst_push(block, prs_statement(prs));
        
        if (prs->tok->type != TOK_SEMI) {
            fprintf(stderr, "error: parser: %s:%zu:%zu: expected ';' after statement, found token type '%s'\n", prs->lex->filename, prs->tok->ln, prs->tok->col, toktype_to_str(prs->tok->type));
            exit(1);
        }

        prs_eat(prs, TOK_SEMI);
    }

    prs_eat(prs, TOK_RBRACE);

    return block;
}

AST *prs_statement(Parser *prs) {
    switch (prs->tok->type) {
        case TOK_ID: return prs_alpha(prs);
        case TOK_INT:
        case TOK_FLOAT: return prs_digit(prs);
        case TOK_STRING: return prs_string(prs);
        default:
            fprintf(stderr, "error: parser: %s:%zu:%zu: unexpected statement of type '%s'\n", prs->lex->filename, prs->tok->ln, prs->tok->col, toktype_to_str(prs->tok->type));
            exit(1);
    }
}

List *prs_to_ast_lst(char *filename) {
    Lexer *lex = lex_init(filename);
    Parser *prs = prs_init(lex);
    List *ast = lst_init(sizeof(AST *));

    scope = calloc(7, sizeof(char));
    strcpy(scope, "global");

    symtab_init();

    while (prs->tok->type != TOK_EOF)
        lst_push(ast, prs_statement(prs));

    free(prs->tok->value);
    free(prs->tok);
    free(prs);
    free(lex->src);
    free(lex);

    return ast;
}
