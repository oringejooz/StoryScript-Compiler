#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "utils.h"

// Create a new AST
AST *create_ast() {
    AST *ast = (AST *)malloc(sizeof(AST));
    if (!ast) {
        fprintf(stderr, "Error: Memory allocation failed for AST\n");
        return NULL;
    }
    ast->head = NULL;
    ast->tail = NULL;
    return ast;
}

// Create a new AST Node
ASTNode *create_node(ASTNodeType type, char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for ASTNode\n");
        return NULL;
    }
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->choices = NULL;
    node->labels = NULL;
    node->choice_count = 0;
    node->str1 = NULL;
    node->str2 = NULL;
    node->next = NULL;
    return node;
}

// Create a Choice Node
ASTNode *create_choice_node(char **choices, char **labels, int choice_count) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for choice node\n");
        return NULL;
    }
    node->type = AST_CHOICE;
    node->value = NULL;
    node->choices = choices;
    node->labels = labels;
    node->choice_count = choice_count;
    node->next = NULL;
    return node;
}

// Create a node for combine operation
ASTNode *create_combine_node(char *str1, char *str2) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for combine node\n");
        return NULL;
    }
    node->type = AST_COMBINE;
    node->str1 = strdup(str1);
    node->str2 = strdup(str2);
    node->next = NULL;
    return node;
}

// Create a node for format_text operation
ASTNode *create_format_node(char *format_str, char *template) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for format_text node\n");
        return NULL;
    }
    node->type = AST_FORMAT_TEXT;
    node->str1 = strdup(format_str);
    node->str2 = strdup(template);
    node->next = NULL;
    return node;
}

// Append Node to AST
void append_ast_node(AST *ast, ASTNode *node) {
    if (!ast) {
        fprintf(stderr, "Error: AST is NULL\n");
        return;
    }
    if (!node) {
        fprintf(stderr, "Error: Node is NULL\n");
        return;
    }
    if (ast->head == NULL) {
        ast->head = ast->tail = node;
    } else {
        ast->tail->next = node;
        ast->tail = node;
    }
}

// Print the AST structure
void print_ast(AST *ast) {
    if (!ast || !ast->head) {
        printf("AST is empty or NULL\n");
        return;
    }
    ASTNode *current = ast->head;
    while (current) {
        switch (current->type) {
            case AST_TITLE:
                printf("Title: %s\n", current->value);
                break;
            case AST_NARRATE:
                printf("Narrate: %s\n", current->value);
                break;
            case AST_CHOICE:
                printf("Choice:\n");
                for (int i = 0; i < current->choice_count; i++) {
                    printf("  %d. %s -> %s\n", i + 1, current->choices[i], current->labels[i]);
                }
                break;
            case AST_GOTO:
                printf("Goto: %s\n", current->value);
                break;
            case AST_END:
                printf("End of Story\n");
                break;
            case AST_LABEL:
                printf("Label: %s\n", current->value);
                break;
            case AST_COMBINE:
                printf("Combine: %s + %s\n", current->str1, current->str2);
                break;
            case AST_FORMAT_TEXT:
                printf("Format Text: %s with %s\n", current->str1, current->str2);
                break;
            default:
                printf("Unknown node type: %d\n", current->type);
                break;
        }
        current = current->next;
    }
}

// Free AST Memory
void free_ast(AST *ast) {
    if (!ast) return;
    ASTNode *current = ast->head;
    while (current) {
        ASTNode *temp = current;
        current = current->next;

        if (temp->value) free(temp->value);

        if (temp->type == AST_CHOICE) {
            for (int i = 0; i < temp->choice_count; i++) {
                if (temp->choices[i]) free(temp->choices[i]);
                if (temp->labels[i]) free(temp->labels[i]);
            }
            free(temp->choices);
            free(temp->labels);
        }

        if (temp->str1) free(temp->str1);
        if (temp->str2) free(temp->str2);

        free(temp);
    }
    free(ast);
}
