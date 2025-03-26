#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "semantics.h"
#include "ast.h"
#include "icg.h"
#include "codegen.h"
int main() {
    FILE *file = fopen("story.txt", "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Initialize the lexer with the file
    init_lexer(file);

    // Parse StoryScript and generate AST
    AST *ast = parse();

    // Print the AST to confirm parsing works correctly
    printf("\n--- AST Structure ---\n");
    print_ast(ast);

    // Perform Semantic Analysis
    check_semantics(ast);

    // ✅ Generate Intermediate Code
    printf("\n--- Generating Intermediate Code ---\n");
    write_intermediate_code(ast, "output.icg");

    // ✅ Generate Target Code from ICG
    printf("\n--- Generating Target Code ---\n");
    generate_target_code("output.icg", "output.target");

    fclose(file);
    free_ast(ast); // Clean up memory
    return 0;
}