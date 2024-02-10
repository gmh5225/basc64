#include <stdio.h>
#include <stdlib.h>
#include "../include/ast.h"
#include "../include/list.h"

AST *ast_init(ASTType type, size_t ln, size_t col, char *scope) {
    AST *ast = malloc(sizeof(AST));
    ast->type = type;
    ast->ln = ln;
    ast->col = col;
    ast->scope = scope;
    return ast;
}

char *asttype_to_str(ASTType type) {
    switch (type) {
        case AST_FUNCTION: return "FUNCTION";
        case AST_CALL: return "CALL";
        case AST_VAR_DEF: return "VAR_DEF";
        case AST_VAR_ASSIGN: return "VAR_ASSIGN";
        case AST_VAR: return "VAR";
        case AST_RETURN: return "RETURN";
        case AST_INT: return "INT";
        case AST_FLOAT: return "FLOAT";
        case AST_STRING: return "STRING";
        default: return "UNDEFINED";
    }
}

void ast_del(AST *ast) {
    switch (ast->type) {
        case AST_FUNCTION:
            free(ast->function.name);
            free(ast->function.type);
            ast_del_lst(ast->function.params);
            ast_del_lst(ast->function.block);
            break;
        case AST_CALL:
            free(ast->call.name);
            ast_del_lst(ast->call.args);
            break;
        case AST_VAR_DEF:
            free(ast->var_def.name);
            free(ast->var_def.type);
            if (ast->var_def.value != NULL)
                ast_del(ast->var_def.value);
            break;
        case AST_VAR_ASSIGN:
            free(ast->var_assign.name);
            if (ast->var_assign.value != NULL)
                ast_del(ast->var_assign.value);
            break;
        case AST_VAR: free(ast->var.name); break;
        case AST_RETURN:
            if (ast->return_.value != NULL)
                ast_del(ast->return_.value);
            break;
        case AST_INT:
        case AST_FLOAT:
        case AST_STRING: free(ast->constant.value); break;
        default: break;
    }
    free(ast);
}

void ast_del_lst(List *lst) {
    for (size_t i = 0; i < lst->size; i++)
        ast_del(lst->items[i]);
    free(lst->items);
    free(lst);
}
