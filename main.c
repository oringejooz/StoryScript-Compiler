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

    printf("Initializing lexer...\n");
    init_lexer(file);

    printf("Parsing AST...\n");
    AST *ast = parse();
    if (!ast) {
        fprintf(stderr, "Error: Failed to parse AST\n");
        fclose(file);
        return 1;
    }

    printf("Printing AST...\n");
    print_ast(ast);

    printf("Checking semantics...\n");
    check_semantics(ast);

    printf("Writing intermediate code...\n");
    write_intermediate_code(ast, "output.icg");

    printf("Generating target code...\n");
    generate_target_code("output.icg", "output.target");

    printf("Running interpreter...\n");
    run_interpreter("output.target");

    printf("Cleaning up...\n");
    fclose(file);
    free_ast(ast);
    return 0;
}