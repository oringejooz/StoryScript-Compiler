#ifndef AST_H
#define AST_H

// AST Node Types
typedef enum {
    AST_TITLE,
    AST_NARRATE,
    AST_CHOICE,
    AST_GOTO,
    AST_END,
    AST_LABEL,
    AST_CREATE_INV,
    AST_ADD_ITEM,
    AST_REMOVE_ITEM,
    AST_SHOW_INV,
    AST_COMBINE,
    AST_LENGTH,
    AST_UPPERCASE,
    AST_LOWERCASE,
    AST_FORMAT_TEXT,
    AST_SET_TIME,
    AST_CHECK_TIME
} ASTNodeType;

// AST Node Definition
typedef struct ASTNode {
    ASTNodeType type;
    char *value;          // Holds strings (title, narrate text, etc.)
    char **choices;       // Array of choice options
    char **labels;        // Corresponding labels for choices
    int choice_count;     // Number of choices
    struct ASTNode *next; // Pointer to the next node
    // Additional for combine and format nodes
    char *str1; // First string in combine/format nodes
    char *str2; // Second string in combine/format nodes
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
ASTNode *create_combine_node(char *str1, char *str2);
ASTNode *create_format_node(char *format_str, char *template);
void print_ast(AST *ast);
void free_ast(AST *ast);

#endif
