#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

void execute_ast(AST *ast) {
    ASTNode *current = ast->head;
    while (current) {
        switch (current->type) {
            case AST_TITLE:
                printf("Title: %s\n", current->value);
                break;
            case AST_NARRATE:
                printf("%s\n", current->value);
                break;
            case AST_CHOICE:
                printf("Make a choice:\n");
                for (int i = 0; i < current->choice_count; i++) {
                    printf("%d. %s\n", i + 1, current->choices[i]);
                }
                break;
            case AST_GOTO:
                printf("Jumping to: %s\n", current->value);
                break;
            case AST_END:
                printf("The story ends here.\n");
                return;
        }
        current = current->next;
    }
}