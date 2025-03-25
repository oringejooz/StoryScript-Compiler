#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "utils.h"

AST *ast;
Token current_token;

void advance() {
    current_token = get_next_token();
    printf("DEBUG: Advanced to token: Type=%d, Value=%s\n", 
           current_token.type, current_token.value ? current_token.value : "NULL");
}

void match(TokenType expected) {
    if (current_token.type == expected) {
        advance();
    } else {
        fprintf(stderr, "Syntax Error: Expected token of type %d but found %d (%s)\n", 
                expected, current_token.type, current_token.value ? current_token.value : "NULL");
        exit(1);
    }
}

ASTNode *parse_choice() {
    printf("DEBUG: Parsing choice statement\n");
    
    advance();
    match(TOKEN_LPAREN);

    char **choices = (char **)malloc(sizeof(char *) * 10);
    char **labels = (char **)malloc(sizeof(char *) * 10);
    int choice_count = 0;

    if (current_token.type == TOKEN_STRING) {
        choices[choice_count] = strdup(current_token.value);
        advance();
        choice_count++;
    } else {
        fprintf(stderr, "Syntax Error: Expected string in choice\n");
        exit(1);
    }
    
    while (current_token.type == TOKEN_COMMA) {
        advance();
        if (current_token.type == TOKEN_STRING) {
            choices[choice_count] = strdup(current_token.value);
            advance();
            choice_count++;
        } else {
            fprintf(stderr, "Syntax Error: Expected string after comma in choice\n");
            exit(1);
        }
    }
    
    match(TOKEN_RPAREN);
    match(TOKEN_ARROW);
    match(TOKEN_LBRACKET);

    int label_count = 0;
    
    if (current_token.type == TOKEN_IDENTIFIER) {
        labels[label_count] = strdup(current_token.value);
        advance();
        label_count++;
    } else {
        fprintf(stderr, "Syntax Error: Expected identifier in choice labels\n");
        exit(1);
    }
    
    while (current_token.type == TOKEN_COMMA) {
        advance();
        if (current_token.type == TOKEN_IDENTIFIER) {
            labels[label_count] = strdup(current_token.value);
            advance();
            label_count++;
        } else {
            fprintf(stderr, "Syntax Error: Expected identifier after comma in choice labels\n");
            exit(1);
        }
    }
    
    if (choice_count != label_count) {
        fprintf(stderr, "Syntax Error: Number of choices (%d) doesn't match number of labels (%d)\n", 
                choice_count, label_count);
        exit(1);
    }
    
    match(TOKEN_RBRACKET);
    
    printf("DEBUG: Created choice node with %d choices\n", choice_count);
    return create_choice_node(choices, labels, choice_count);
}

void parse_block() {
    printf("DEBUG: Parsing block\n");
    
    while (current_token.type != TOKEN_EOF && 
           current_token.type != TOKEN_LABEL) {
        parse_statement();
    }
}

void parse_statement() {
    printf("DEBUG: Parsing statement, token type: %d, value: %s\n", 
           current_token.type, current_token.value ? current_token.value : "NULL");

    ASTNode *node = NULL;
    
    switch (current_token.type) {
        case TOKEN_TITLE:
            printf("DEBUG: Parsing title\n");
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) {
                node = create_node(AST_TITLE, current_token.value);
                advance();
            } else {
                fprintf(stderr, "Syntax Error: Expected string in title\n");
                exit(1);
            }
            match(TOKEN_RPAREN);
            break;
            
        case TOKEN_NARRATE:
            printf("DEBUG: Parsing narrate\n");
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) {
                node = create_node(AST_NARRATE, current_token.value);
                advance();
            } else {
                fprintf(stderr, "Syntax Error: Expected string in narrate\n");
                exit(1);
            }
            match(TOKEN_RPAREN);
            break;
            
        case TOKEN_CHOICE:
            printf("DEBUG: Parsing choice\n");
            node = parse_choice();
            break;
            
        case TOKEN_GOTO:
            printf("DEBUG: Parsing goto\n");
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) {
                node = create_node(AST_GOTO, current_token.value);
                advance();
            } else {
                fprintf(stderr, "Syntax Error: Expected identifier in goto\n");
                exit(1);
            }
            match(TOKEN_RPAREN);
            break;
            
        case TOKEN_END:
            printf("DEBUG: Parsing end\n");
            advance();
            match(TOKEN_LPAREN);
            match(TOKEN_RPAREN);
            node = create_node(AST_END, NULL);
            break;
            
        case TOKEN_LABEL:
            printf("DEBUG: Parsing label: %s\n", current_token.value);
            node = create_node(AST_LABEL, current_token.value);
            advance();
            break;
            
        default:
            fprintf(stderr, "Syntax Error: Unexpected token type %d (%s)\n", 
                    current_token.type, current_token.value ? current_token.value : "NULL");
            exit(1);
    }
    
    if (node) {
        printf("DEBUG: Adding node to AST, type: %d, value: %s\n", 
               node->type, node->value ? node->value : "NULL");
        append_ast_node(ast, node);
    }
}

AST *parse() {
    printf("DEBUG: Starting parsing\n");
    
    ast = create_ast();
    if (!ast) {
        fprintf(stderr, "Error: Failed to create AST\n");
        exit(1);
    }
    
    advance();
    
    while (current_token.type != TOKEN_EOF) {
        parse_statement();
    }
    
    printf("DEBUG: Finished parsing. Returning AST.\n");
    return ast;
}