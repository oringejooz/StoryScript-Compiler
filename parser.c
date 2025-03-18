#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

Token current_token;

// Advance to Next Token
void advance() {
    current_token = get_next_token();
}

// Match and Consume Expected Token
void match(TokenType expected) {
    if (current_token.type == expected) {
        advance();
    } else {
        fprintf(stderr, "Syntax Error: Expected token of type %d but found %d\n", expected, current_token.type);
        exit(1);
    }
}

// Parse StoryScript Statements
void parse_statement() {
    switch (current_token.type) {
        case TOKEN_TITLE:
            advance();
            match(TOKEN_LPAREN);
            match(TOKEN_STRING);
            match(TOKEN_RPAREN);
            break;
        case TOKEN_NARRATE:
            advance();
            match(TOKEN_LPAREN);
            match(TOKEN_STRING);
            match(TOKEN_RPAREN);
            break;
        case TOKEN_CHOICE:
            advance();
            match(TOKEN_LPAREN);
            match(TOKEN_STRING);
            while (current_token.type == TOKEN_COMMA) {
                advance();
                match(TOKEN_STRING);
            }
            match(TOKEN_RPAREN);
            match(TOKEN_ARROW);
            match(TOKEN_LBRACKET);
            match(TOKEN_LABEL);
            while (current_token.type == TOKEN_COMMA) {
                advance();
                match(TOKEN_LABEL);
            }
            match(TOKEN_RBRACKET);
            break;
        case TOKEN_END:
            advance();
            break;
        default:
            fprintf(stderr, "Unknown statement!\n");
            exit(1);
    }
}

// Main Parse Function
void parse() {
    advance();
    while (current_token.type != TOKEN_EOF) {
        parse_statement();
    }
}
