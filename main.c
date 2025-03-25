#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "semantics.h"
#include "interpreter.h"
#include "ast.h"

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

    // Run the StoryScript using the AST
    printf("\n--- Story Execution ---\n");
    execute_ast(ast);

    fclose(file);
    free_ast(ast); // Clean up memory
    return 0;
}