#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/symtab.h"
#include "../include/list.h"

List *symtab;
extern char *scope;

void symtab_init() {
    symtab = lst_init(sizeof(Symbol *));
}

void symtab_del() {
    for (size_t i = 0; i < symtab->size; i++)
        free(symtab->items[i]);

    free(symtab->items);
    free(symtab);
    free(scope);
}

Symbol *symtab_search(SymbolType type, char *name, char *scope) {
    for (size_t i = 0; i < symtab->size; i++) {
        Symbol *sym = symtab->items[i];
        if (type != sym->type)
            continue;

        switch (type) {
            case SYM_FUNCTION:
                if (strcmp(name, sym->function.name) == 0 && strcmp(scope, sym->scope) == 0)
                    return sym;
                break;
            case SYM_VAR:
                if (strcmp(name, sym->var.name) == 0 && (strcmp(scope, sym->scope) == 0 || strlen(scope) < strlen(sym->scope))) // higher len scope = nested
                    return sym;
                break;
            default: break;
        }
    }

    return NULL;
}

Symbol *sym_init(SymbolType type, size_t ln, size_t col, char *scope) {
    Symbol *sym = malloc(sizeof(Symbol));
    sym->type = type;
    sym->ln = ln;
    sym->col = col;
    sym->scope = scope;
    return sym;
}

void sym_add(Symbol *sym) {
    lst_push(symtab, sym);
}
