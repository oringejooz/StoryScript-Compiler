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
    printf("DEBUG: Created new AST\n");
    return ast;
}

// Create a new AST Node
ASTNode *create_node(ASTNodeType type, char *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode)); // Fixed cast from AST* to ASTNode*
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for ASTNode\n");
        return NULL;
    }
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->choices = NULL;
    node->labels = NULL;
    node->choice_count = 0;
    node->next = NULL;
    printf("DEBUG: Created node of type %d with value %s\n", 
           type, value ? value : "NULL");
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
    printf("DEBUG: Created choice node with %d choices\n", choice_count);
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
        ast->head = node;
        ast->tail = node;
    } else {
        ast->tail->next = node;
        ast->tail = node;
    }
    
    printf("DEBUG: Appended node to AST, type: %d, value: %s\n", 
           node->type, node->value ? node->value : "NULL");
}

// Print the AST structure
void print_ast(AST *ast) {
    if (!ast) {
        printf("AST is NULL\n");
        return;
    }
    
    if (!ast->head) {
        printf("AST is empty\n");
        return;
    }
    
    ASTNode *current = ast->head;
    int indent = 0;
    
    printf("AST Structure:\n");
    
    while (current) {
        // Print indentation
        for (int i = 0; i < indent; i++) {
            printf("  ");
        }
        
        // Print node based on type
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
                    for (int j = 0; j < indent + 1; j++) {
                        printf("  ");
                    }
                    printf("%d. %s -> %s\n", i + 1, 
                           current->choices[i], current->labels[i]);
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
                indent++; // Increase indentation for statements in this label
                break;
                
            default:
                printf("Unknown node type: %d\n", current->type);
                break;
        }
        
        // Move to next node
        current = current->next;
        
        // If next node is a label, decrease indentation
        if (current && current->type == AST_LABEL) {
            indent = 0; // Reset indentation for new label
        }
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
        
        free(temp);
    }
    
    free(ast);
}