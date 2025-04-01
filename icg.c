#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "icg.h"
#include "ast.h"

static FILE *icg_file;

static void generate_node_ir(ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case AST_TITLE: fprintf(icg_file, "TITLE \"%s\"\n", node->value); break;
        case AST_SAY: fprintf(icg_file, "SAY \"%s\"\n", node->value); break;
        case AST_NARRATE: fprintf(icg_file, "NARRATE \"%s\"\n", node->value); break;
        case AST_PAUSE: fprintf(icg_file, "PAUSE %s\n", node->value); break;
        case AST_CHOICE:
            fprintf(icg_file, "CHOICE ");
            for (int i = 0; i < node->choice_count; i++) {
                fprintf(icg_file, "\"%s\"%s", node->choices[i], i < node->choice_count - 1 ? ", " : "");
            }
            fprintf(icg_file, " -> [");
            for (int i = 0; i < node->choice_count; i++) {
                fprintf(icg_file, "%s%s", node->labels[i], i < node->choice_count - 1 ? ", " : "");
            }
            fprintf(icg_file, "]\n");
            break;
        case AST_INPUT: fprintf(icg_file, "INPUT %s\n", node->value); break;
        case AST_CONFIRM: fprintf(icg_file, "CONFIRM \"%s\"\n", node->value); break;
        case AST_GOTO: fprintf(icg_file, "GOTO %s\n", node->value); break;
        case AST_END: fprintf(icg_file, "END\n"); break;
        case AST_RETURN: fprintf(icg_file, "RETURN\n"); break;
        case AST_RESTART: fprintf(icg_file, "RESTART\n"); break;
        case AST_LABEL: fprintf(icg_file, "LABEL %s\n", node->value); break;
        case AST_IF: fprintf(icg_file, "IF %s -> %s\n", node->condition, node->true_label); break;
        case AST_IFELSE: fprintf(icg_file, "IFELSE %s -> [%s, %s]\n", node->condition, node->true_label, node->false_label); break;
        case AST_ASSIGN: fprintf(icg_file, "ASSIGN %s \"%s\"\n", node->value, node->value); break;
        case AST_INCREASE: fprintf(icg_file, "INCREASE %s %s\n", node->value, node->value); break;
        case AST_DECREASE: fprintf(icg_file, "DECREASE %s %s\n", node->value, node->value); break;
        case AST_SCALE: fprintf(icg_file, "SCALE %s %s\n", node->value, node->value); break;
        case AST_DIVIDE_BY: fprintf(icg_file, "DIVIDE_BY %s %s\n", node->value, node->value); break;
        case AST_RANDOMIZE: fprintf(icg_file, "RANDOMIZE %s %s\n", node->value, node->value); break;
        case AST_COMBINE: fprintf(icg_file, "COMBINE \"%s\" \"%s\"\n", node->str1, node->str2); break;
        case AST_LENGTH: fprintf(icg_file, "LENGTH \"%s\"\n", node->value); break;
        case AST_SUBSTRING_IN: fprintf(icg_file, "SUBSTRING_IN \"%s\"\n", node->value); break;
        case AST_UPPERCASE: fprintf(icg_file, "UPPERCASE \"%s\"\n", node->value); break;
        case AST_LOWERCASE: fprintf(icg_file, "LOWERCASE \"%s\"\n", node->value); break;
        case AST_FORMAT_TEXT: fprintf(icg_file, "FORMAT_TEXT \"%s\" \"%s\"\n", node->str1, node->str2); break;
        case AST_CREATE_INV: fprintf(icg_file, "CREATE_INV %s\n", node->value); break;
        case AST_ADD_ITEM: fprintf(icg_file, "ADD_ITEM \"%s\"\n", node->value); break;
        case AST_REMOVE_ITEM: fprintf(icg_file, "REMOVE_ITEM \"%s\"\n", node->value); break;
        case AST_HAS_ITEM: fprintf(icg_file, "HAS_ITEM \"%s\"\n", node->value); break;
        case AST_COUNT_INV: fprintf(icg_file, "COUNT_INV\n"); break;
        case AST_CLEAR_INV: fprintf(icg_file, "CLEAR_INV\n"); break;
        case AST_SHOW_INV: fprintf(icg_file, "SHOW_INV\n"); break;
        case AST_SET_TIME: fprintf(icg_file, "SET_TIME \"%s\"\n", node->value); break;
        case AST_CHECK_TIME: fprintf(icg_file, "CHECK_TIME\n"); break;
        default: fprintf(stderr, "Unknown AST node type: %d\n", node->type); break;
    }
}

void generate_intermediate_code(AST *ast) {
    ASTNode *current = ast->head;
    while (current) {
        generate_node_ir(current);
        current = current->next;
    }
}

void write_intermediate_code(AST *ast, const char *filename) {
    icg_file = fopen(filename, "w");
    if (!icg_file) {
        perror("Error opening ICG file");
        exit(1);
    }
    generate_intermediate_code(ast);
    fclose(icg_file);
}