#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "utils.h"

void check_labels(AST *ast) {
    ASTNode *current = ast->head;
    char *defined_labels[100];
    int label_count = 0;

    // First pass: Collect defined labels from AST_LABEL nodes
    while (current) {
        if (current->type == AST_LABEL) {
            if (label_count >= 100) {
                error("Too many labels defined (max 100)");
            }
            defined_labels[label_count++] = current->value;
        }
        current = current->next;
    }

    // Second pass: Check goto and choice labels against defined labels
    current = ast->head;
    while (current) {
        if (current->type == AST_GOTO) {
            int found = 0;
            for (int j = 0; j < label_count; j++) {
                if (strcmp(current->value, defined_labels[j]) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                fprintf(stderr, "Error: Undefined label '%s' in goto\n", current->value);
                exit(1);
            }
        } else if (current->type == AST_CHOICE) {
            for (int i = 0; i < current->choice_count; i++) {
                int found = 0;
                for (int j = 0; j < label_count; j++) {
                    if (strcmp(current->labels[i], defined_labels[j]) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    fprintf(stderr, "Error: Undefined label '%s' in choice\n", current->labels[i]);
                    exit(1);
                }
            }
        }
        current = current->next;
    }
}

void check_semantics(AST *ast) {
    check_labels(ast);
}