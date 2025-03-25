#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_TITLE,
    TOKEN_SAY,
    TOKEN_NARRATE,
    TOKEN_CHOICE,
    TOKEN_INPUT,
    TOKEN_GOTO,
    TOKEN_END,
    TOKEN_LABEL,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_IF,
    TOKEN_IFELSE,
    TOKEN_SWITCH,
    TOKEN_ASSIGN,
    TOKEN_IDENTIFIER,
    TOKEN_LPAREN,    // (
    TOKEN_RPAREN,    // )
    TOKEN_ARROW,     // ->
    TOKEN_COLON,     // :
    TOKEN_COMMA,     // ,
    TOKEN_LBRACKET,  // [
    TOKEN_RBRACKET,  // ]
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

void init_lexer(FILE *file);
Token get_next_token();
void print_token(Token token);
void free_token(Token token);

#endif