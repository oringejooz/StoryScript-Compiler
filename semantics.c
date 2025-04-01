#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "utils.h"

void check_semantics(AST *ast) {
    char *labels[100];
    int label_count = 0;
    ASTNode *current = ast->head;

    while (current) {
        if (current->type == AST_LABEL) {
            labels[label_count++] = current->value;
        }
        current = current->next;
    }

    current = ast->head;
    while (current) {
        if (current->type == AST_GOTO || current->type == AST_IF || current->type == AST_IFELSE) {
            char *target = current->type == AST_GOTO ? current->value : current->true_label;
            int found = 0;
            for (int i = 0; i < label_count; i++) {
                if (!strcmp(target, labels[i])) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                fprintf(stderr, "Error: Undefined label '%s'\n", target);
                exit(1);
            }
            if (current->type == AST_IFELSE) {
                found = 0;
                for (int i = 0; i < label_count; i++) {
                    if (!strcmp(current->false_label, labels[i])) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    fprintf(stderr, "Error: Undefined label '%s'\n", current->false_label);
                    exit(1);
                }
            }
        } else if (current->type == AST_CHOICE) {
            for (int i = 0; i < current->choice_count; i++) {
                int found = 0;
                for (int j = 0; j < label_count; j++) {
                    if (!strcmp(current->labels[i], labels[j])) {
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