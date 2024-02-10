#include <stdio.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/list.h"
#include "../include/ast.h"
#include "../include/symtab.h"

void print_help(char *prog) {
    printf("usage: %s <filename> [-o <outname>, -h]\n", prog);
    printf("flags:\n");
    printf("\t-o <outname> - set the output executable name to <outname>, otherwise use 'out.out'\n");
    printf("\t-h           - show compiler usage\n");
}

int main(int argc, char **argv) {   
    if (argc < 2) {
        fprintf(stderr, "error: expected <filename>\nuse '%s -h' for help\n", argv[0]);
        return 1;
    }

    char outname[512];
    strcpy(outname, "out.out");

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 ) {
            print_help(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-o") == 0) {
            if (i == argc - 1) {
                fprintf(stderr, "error: flag '-o' expected <outname>\nuse '%s -h' for help\n", argv[0]);
                return 1;
            }

            strncpy(outname, argv[i + 1], 511);
            outname[511] = '\0';
            i++;
        }
    }

    (void)outname;
    List *ast = prs_to_ast_lst(argv[1]);
    ast_del_lst(ast);
    symtab_del();
    return 0;
}
