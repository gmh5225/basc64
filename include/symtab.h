#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdio.h>
#include "list.h"

typedef enum SymbolType {
    SYM_FUNCTION,
    SYM_VAR
} SymbolType;

typedef struct {
    SymbolType type;
    size_t ln, col;
    char *scope;
    union {
        struct {
            char *name, *type;
            List *params;
        } function;

        struct {
            char *name, *type;
        } var;
    };
} Symbol;

void symtab_init();

void symtab_del();

Symbol *symtab_search(SymbolType type, char *name, char *scope);

Symbol *sym_init(SymbolType type, size_t ln, size_t col, char *scope);

void sym_add(Symbol *sym);

#endif
