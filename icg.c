#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "icg.h"
#include "ast.h"

// File pointer for intermediate code output
FILE *icg_file;

// Generate IR for a single node
void generate_node_ir(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_TITLE:
            fprintf(icg_file, "TITLE \"%s\"\n", node->value);
            break;
        case AST_NARRATE:
            fprintf(icg_file, "NARRATE \"%s\"\n", node->value);
            break;
        case AST_CHOICE:
            fprintf(icg_file, "CHOICE ");
            for (int i = 0; i < node->choice_count; i++) {
                fprintf(icg_file, "\"%s\"", node->choices[i]);
                if (i < node->choice_count - 1) {
                    fprintf(icg_file, ", ");
                }
            }
            fprintf(icg_file, " -> [");
            for (int i = 0; i < node->choice_count; i++) {
                fprintf(icg_file, "%s", node->labels[i]);
                if (i < node->choice_count - 1) {
                    fprintf(icg_file, ", ");
                }
            }
            fprintf(icg_file, "]\n");
            break;
        case AST_GOTO:
            fprintf(icg_file, "GOTO %s\n", node->value);
            break;
        case AST_END:
            fprintf(icg_file, "END\n");
            break;
        case AST_LABEL:
            fprintf(icg_file, "LABEL %s\n", node->value);
            break;
            case AST_CREATE_INV:
            fprintf(icg_file, "CREATE_INVENTORY\n");
            break;

        case AST_ADD_ITEM:
            fprintf(icg_file, "ADD_ITEM \"%s\"\n", node->value);
            break;

        case AST_REMOVE_ITEM:
            fprintf(icg_file, "REMOVE_ITEM \"%s\"\n", node->value);
            break;

        case AST_SHOW_INV:
            fprintf(icg_file, "SHOW_INVENTORY\n");
            break;

        case AST_COMBINE:
            fprintf(icg_file, "COMBINE \"%s\" \"%s\"\n", node->str1, node->str2);
            break;

        case AST_LENGTH:
            fprintf(icg_file, "LENGTH_OF \"%s\"\n", node->value);
            break;

        case AST_UPPERCASE:
            fprintf(icg_file, "UPPERCASE \"%s\"\n", node->value);
            break;

        case AST_LOWERCASE:
            fprintf(icg_file, "LOWERCASE \"%s\"\n", node->value);
            break;

        case AST_FORMAT_TEXT:
            fprintf(icg_file, "FORMAT_TEXT \"%s\" \"%s\"\n", node->str1, node->str2);
            break;

        case AST_SET_TIME:
            fprintf(icg_file, "SET_TIME \"%s\"\n", node->value);
            break;

        case AST_CHECK_TIME:
            fprintf(icg_file, "CHECK_TIME\n");
            break;

        default:
            fprintf(stderr, "Unknown AST Node Type!\n");
            exit(1);
    }
}

// Generate Intermediate Code by Traversing AST
void generate_intermediate_code(AST *ast) {
    if (!ast || !ast->head) {
        fprintf(stderr, "Error: Empty AST. Cannot generate intermediate code.\n");
        return;
    }

    ASTNode *current = ast->head;
    while (current) {
        generate_node_ir(current);
        current = current->next;
    }
}

// Write the generated intermediate code to a file
void write_intermediate_code(AST *ast, const char *filename) {
    icg_file = fopen(filename, "w");
    if (!icg_file) {
        perror("Error opening intermediate code file");
        exit(1);
    }

    // ✅ Pass AST to generate_intermediate_code()
    generate_intermediate_code(ast);

    fclose(icg_file);
    printf("✅ Intermediate code successfully written to '%s'\n", filename);
}

