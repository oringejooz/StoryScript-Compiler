#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "utils.h"

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
    node->condition = NULL;
    node->true_label = NULL;
    node->false_label = NULL;
    node->next = NULL;
    return node;
}

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
    node->str1 = NULL;
    node->str2 = NULL;
    node->next = NULL;
    return node;
}

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

void append_ast_node(AST *ast, ASTNode *node) {
    if (!ast || !node) return;
    if (ast->head == NULL) {
        ast->head = ast->tail = node;
    } else {
        ast->tail->next = node;
        ast->tail = node;
    }
}

void print_ast(AST *ast) {
    if (!ast || !ast->head) {
        printf("AST is empty or NULL\n");
        return;
    }
    ASTNode *current = ast->head;
    while (current) {
        switch (current->type) {
            case AST_TITLE: printf("Title: %s\n", current->value); break;
            case AST_SAY: printf("Say: %s\n", current->value); break;
            case AST_NARRATE: printf("Narrate: %s\n", current->value); break;
            case AST_PAUSE: printf("Pause: %s seconds\n", current->value); break;
            case AST_CHOICE:
                printf("Choice:\n");
                for (int i = 0; i < current->choice_count; i++) {
                    printf("  %d. %s -> %s\n", i + 1, current->choices[i], current->labels[i]);
                }
                break;
            case AST_INPUT: printf("Input: %s\n", current->value); break;
            case AST_CONFIRM: printf("Confirm: %s\n", current->value); break;
            case AST_GOTO: printf("Goto: %s\n", current->value); break;
            case AST_END: printf("End of Story\n"); break;
            case AST_RETURN: printf("Return\n"); break;
            case AST_RESTART: printf("Restart Story\n"); break;
            case AST_LABEL: printf("Label: %s\n", current->value); break;
            case AST_IF: printf("If: %s -> %s\n", current->condition, current->true_label); break;
            case AST_IFELSE: printf("IfElse: %s -> [%s, %s]\n", current->condition, current->true_label, current->false_label); break;
            case AST_SWITCH: printf("Switch: %s\n", current->value); break;
            case AST_ASSIGN: printf("Assign: %s\n", current->value); break;
            case AST_INCREASE: printf("Increase: %s\n", current->value); break;
            case AST_DECREASE: printf("Decrease: %s\n", current->value); break;
            case AST_SCALE: printf("Scale: %s\n", current->value); break;
            case AST_DIVIDE_BY: printf("Divide By: %s\n", current->value); break;
            case AST_RANDOMIZE: printf("Randomize: %s\n", current->value); break;
            case AST_BOTH: printf("Both: %s\n", current->value); break;
            case AST_EITHER: printf("Either: %s\n", current->value); break;
            case AST_INVERT: printf("Invert: %s\n", current->value); break;
            case AST_COMBINE: printf("Combine: %s + %s\n", current->str1, current->str2); break;
            case AST_LENGTH: printf("Length: %s\n", current->value); break;
            case AST_SUBSTRING_IN: printf("Substring In: %s\n", current->value); break;
            case AST_UPPERCASE: printf("Uppercase: %s\n", current->value); break;
            case AST_LOWERCASE: printf("Lowercase: %s\n", current->value); break;
            case AST_FORMAT_TEXT: printf("Format Text: %s with %s\n", current->str1, current->str2); break;
            case AST_CREATE_INV: printf("Create Inventory: %s\n", current->value); break;
            case AST_ADD_ITEM: printf("Add Item: %s\n", current->value); break;
            case AST_REMOVE_ITEM: printf("Remove Item: %s\n", current->value); break;
            case AST_HAS_ITEM: printf("Has Item: %s\n", current->value); break;
            case AST_COUNT_INV: printf("Count Inventory\n"); break;
            case AST_CLEAR_INV: printf("Clear Inventory\n"); break;
            case AST_SHOW_INV: printf("Show Inventory\n"); break;
            case AST_SET_TIME: printf("Set Time: %s\n", current->value); break;
            case AST_CHECK_TIME: printf("Check Time\n"); break;
            default: printf("Unknown node type: %d\n", current->type); break;
        }
        current = current->next;
    }
}

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
        if (temp->condition) free(temp->condition);
        if (temp->true_label) free(temp->true_label);
        if (temp->false_label) free(temp->false_label);
        free(temp);
    }
    free(ast);
}