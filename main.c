#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main() {
    FILE *file = fopen("story.txt", "r");
    
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Initialize the lexer with the file
    init_lexer(file);

    // Token loop
    Token token;
    do {
        token = get_next_token();
        print_token(token);
        free_token(token);
    } while (token.type != TOKEN_EOF);

    // Close the file
    fclose(file);
    return 0;
}
