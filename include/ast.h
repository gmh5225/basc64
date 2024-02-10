#ifndef AST_H
#define AST_H

#include <stdio.h>
#include "list.h"

typedef enum ASTType {
    AST_FUNCTION,
    AST_CALL,
    AST_VAR_DEF,
    AST_VAR_ASSIGN,
    AST_VAR,
    AST_RETURN,
    AST_INT,
    AST_FLOAT,
    AST_STRING
} ASTType;

typedef struct AST {
    ASTType type;
    size_t ln, col;
    char *scope;
    union {
        struct {
            char *name, *type;
            List *params, *block;
        } function;

        struct {
            char *name;
            List *args;
        } call;

        struct {
            char *name, *type;
            struct AST *value;
        } var_def;

        struct {
            char *name;
            struct AST *value;
        } var_assign;

        struct {
            char *name;
        } var;

        struct {
            struct AST *value;
        } return_;

        struct {
            char *value;
        } constant;
    };
} AST;

AST *ast_init(ASTType type, size_t ln, size_t col, char *scope);

char *asttype_to_str(ASTType type);

void ast_del(AST *ast);

void ast_del_lst(List *lst);

#endif
