#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "utils.h"

static AST *ast;
static Token current_token;

static void advance() {
    free_token(current_token);
    current_token = get_next_token();
}

static void match(TokenType expected) {
    if (current_token.type == expected) {
        advance();
    } else {
        fprintf(stderr, "Syntax Error: Expected %d, got %d (%s)\n", expected, current_token.type, current_token.value);
        exit(1);
    }
}

ASTNode *parse_choice() {
    advance();
    match(TOKEN_LPAREN);
    char **choices = (char **)malloc(sizeof(char *) * 10);
    char **labels = (char **)malloc(sizeof(char *) * 10);
    int choice_count = 0;

    if (current_token.type == TOKEN_STRING) {
        choices[choice_count++] = strdup(current_token.value);
        advance();
    }
    while (current_token.type == TOKEN_COMMA) {
        advance();
        if (current_token.type == TOKEN_STRING) {
            choices[choice_count++] = strdup(current_token.value);
            advance();
        }
    }
    match(TOKEN_RPAREN);
    match(TOKEN_ARROW);
    match(TOKEN_LBRACKET);

    int label_count = 0;
    if (current_token.type == TOKEN_IDENTIFIER) {
        labels[label_count++] = strdup(current_token.value);
        advance();
    }
    while (current_token.type == TOKEN_COMMA) {
        advance();
        if (current_token.type == TOKEN_IDENTIFIER) {
            labels[label_count++] = strdup(current_token.value);
            advance();
        }
    }
    match(TOKEN_RBRACKET);

    if (choice_count != label_count) {
        fprintf(stderr, "Syntax Error: Choices (%d) and labels (%d) mismatch\n", choice_count, label_count);
        exit(1);
    }

    return create_choice_node(choices, labels, choice_count);
}

void parse_statement() {
    ASTNode *node = NULL;
    switch (current_token.type) {
        case TOKEN_TITLE:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_TITLE, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_SAY:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_SAY, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_NARRATE:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_NARRATE, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_PAUSE:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_PAUSE, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_CHOICE:
            node = parse_choice();
            break;
        case TOKEN_INPUT:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) node = create_node(AST_INPUT, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_CONFIRM:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_CONFIRM, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_GOTO:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) node = create_node(AST_GOTO, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_END:
            advance();
            match(TOKEN_LPAREN);
            match(TOKEN_RPAREN);
            node = create_node(AST_END, NULL);
            break;
        case TOKEN_RETURN:
            advance();
            node = create_node(AST_RETURN, NULL);
            break;
        case TOKEN_RESTART:
            advance();
            node = create_node(AST_RESTART, NULL);
            break;
        case TOKEN_LABEL:
            node = create_node(AST_LABEL, current_token.value);
            advance();
            break;
        case TOKEN_IF:
            advance();
            match(TOKEN_LPAREN);
            node = create_node(AST_IF, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            match(TOKEN_ARROW);
            if (current_token.type == TOKEN_IDENTIFIER) node->true_label = strdup(current_token.value);
            advance();
            break;
        case TOKEN_IFELSE:
            advance();
            match(TOKEN_LPAREN);
            node = create_node(AST_IFELSE, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            match(TOKEN_ARROW);
            match(TOKEN_LBRACKET);
            if (current_token.type == TOKEN_IDENTIFIER) node->true_label = strdup(current_token.value);
            advance();
            match(TOKEN_COMMA);
            if (current_token.type == TOKEN_IDENTIFIER) node->false_label = strdup(current_token.value);
            advance();
            match(TOKEN_RBRACKET);
            break;
        case TOKEN_ASSIGN:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) node = create_node(AST_ASSIGN, current_token.value);
            advance();
            match(TOKEN_COMMA);
            if (current_token.type == TOKEN_STRING) node->value = strdup(current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_INCREASE:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) node = create_node(AST_INCREASE, current_token.value);
            advance();
            match(TOKEN_COMMA);
            if (current_token.type == TOKEN_STRING) node->value = strdup(current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_DECREASE:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) node = create_node(AST_DECREASE, current_token.value);
            advance();
            match(TOKEN_COMMA);
            if (current_token.type == TOKEN_STRING) node->value = strdup(current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_SCALE:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) node = create_node(AST_SCALE, current_token.value);
            advance();
            match(TOKEN_COMMA);
            if (current_token.type == TOKEN_STRING) node->value = strdup(current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_DIVIDE_BY:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) node = create_node(AST_DIVIDE_BY, current_token.value);
            advance();
            match(TOKEN_COMMA);
            if (current_token.type == TOKEN_STRING) node->value = strdup(current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_RANDOMIZE:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) node = create_node(AST_RANDOMIZE, current_token.value);
            advance();
            match(TOKEN_COMMA);
            if (current_token.type == TOKEN_STRING) node->value = strdup(current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_COMBINE:
            advance();
            match(TOKEN_LPAREN);
            node = create_node(AST_COMBINE, NULL);
            node->str1 = strdup(current_token.value);
            advance();
            match(TOKEN_COMMA);
            node->str2 = strdup(current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_LENGTH:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_LENGTH, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_SUBSTRING_IN:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_SUBSTRING_IN, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_UPPERCASE:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_UPPERCASE, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_LOWERCASE:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_LOWERCASE, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_FORMAT_TEXT:
            advance();
            match(TOKEN_LPAREN);
            node = create_node(AST_FORMAT_TEXT, NULL);
            node->str1 = strdup(current_token.value);
            advance();
            match(TOKEN_COMMA);
            node->str2 = strdup(current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_CREATE_INV:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_IDENTIFIER) node = create_node(AST_CREATE_INV, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_ADD_ITEM:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_ADD_ITEM, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_REMOVE_ITEM:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_REMOVE_ITEM, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_HAS_ITEM:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_HAS_ITEM, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_COUNT_INV:
            advance();
            match(TOKEN_LPAREN);
            match(TOKEN_RPAREN);
            node = create_node(AST_COUNT_INV, NULL);
            break;
        case TOKEN_CLEAR_INV:
            advance();
            match(TOKEN_LPAREN);
            match(TOKEN_RPAREN);
            node = create_node(AST_CLEAR_INV, NULL);
            break;
        case TOKEN_SHOW_INV:
            advance();
            match(TOKEN_LPAREN);
            match(TOKEN_RPAREN);
            node = create_node(AST_SHOW_INV, NULL);
            break;
        case TOKEN_SET_TIME:
            advance();
            match(TOKEN_LPAREN);
            if (current_token.type == TOKEN_STRING) node = create_node(AST_SET_TIME, current_token.value);
            advance();
            match(TOKEN_RPAREN);
            break;
        case TOKEN_CHECK_TIME:
            advance();
            match(TOKEN_LPAREN);
            match(TOKEN_RPAREN);
            node = create_node(AST_CHECK_TIME, NULL);
            break;
        default:
            fprintf(stderr, "Syntax Error: Unknown token %d\n", current_token.type);
            exit(1);
    }
    if (node) append_ast_node(ast, node);
}

AST *parse() {
    ast = create_ast();
    advance();
    while (current_token.type != TOKEN_EOF) {
        parse_statement();
    }
    free_token(current_token);
    return ast;
}