#ifndef AST_H
#define AST_H

typedef enum {
    AST_TITLE,
    AST_SAY,
    AST_NARRATE,
    AST_PAUSE,
    AST_CHOICE,
    AST_INPUT,
    AST_CONFIRM,
    AST_GOTO,
    AST_END,
    AST_RETURN,
    AST_RESTART,
    AST_LABEL,
    AST_IF,
    AST_IFELSE,
    AST_SWITCH,
    AST_ASSIGN,
    AST_INCREASE,
    AST_DECREASE,
    AST_SCALE,
    AST_DIVIDE_BY,
    AST_RANDOMIZE,
    AST_BOTH,
    AST_EITHER,
    AST_INVERT,
    AST_COMBINE,
    AST_LENGTH,
    AST_SUBSTRING_IN,
    AST_UPPERCASE,
    AST_LOWERCASE,
    AST_FORMAT_TEXT,
    AST_CREATE_INV,
    AST_ADD_ITEM,
    AST_REMOVE_ITEM,
    AST_HAS_ITEM,
    AST_COUNT_INV,
    AST_CLEAR_INV,
    AST_SHOW_INV,
    AST_SET_TIME,
    AST_CHECK_TIME
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    char *value;
    char **choices;
    char **labels;
    int choice_count;
    char *str1;
    char *str2;
    char *condition;
    char *true_label;
    char *false_label;
    struct ASTNode *next;
} ASTNode;

typedef struct {
    ASTNode *head;
    ASTNode *tail;
} AST;

AST *create_ast();
void append_ast_node(AST *ast, ASTNode *node);
ASTNode *create_node(ASTNodeType type, char *value);
ASTNode *create_choice_node(char **choices, char **labels, int choice_count);
ASTNode *create_combine_node(char *str1, char *str2);
ASTNode *create_format_node(char *format_str, char *template);
void print_ast(AST *ast); // Declare print_ast here
void free_ast(AST *ast);

#endif