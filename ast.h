#ifndef AST_H
#define AST_H

// AST Node Types
typedef enum {
    AST_TITLE,
    AST_NARRATE,
    AST_CHOICE,
    AST_GOTO,
    AST_END,
    AST_LABEL 
} ASTNodeType;

// AST Node Definition
typedef struct ASTNode {
    ASTNodeType type;
    char *value;          // Holds strings (title, narrate text, etc.)
    char **choices;       // Array of choice options
    char **labels;        // Corresponding labels for choices
    int choice_count;     // Number of choices
    struct ASTNode *next; // Pointer to the next node
} ASTNode;

// AST Root
typedef struct {
    ASTNode *head;
    ASTNode *tail;
} AST;

// Function Prototypes
AST *create_ast();
void append_ast_node(AST *ast, ASTNode *node);
ASTNode *create_node(ASTNodeType type, char *value);
ASTNode *create_choice_node(char **choices, char **labels, int choice_count);
void print_ast(AST *ast);
void free_ast(AST *ast); // Added this line

#endif