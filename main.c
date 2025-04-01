#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "semantics.h"
#include "ast.h"
#include "icg.h"
#include "codegen.h"
#include "interpreter.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <story_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    init_lexer(file);
    AST *ast = parse();
    print_ast(ast); // This should now link correctly
    check_semantics(ast);
    write_intermediate_code(ast, "output.icg");
    generate_target_code("output.icg", "output.target");
    run_interpreter("output.target");

    fclose(file);
    free_ast(ast);
    return 0;
}