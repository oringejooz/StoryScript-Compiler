#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

#ifndef _MSC_VER
char *strndup(const char *s, size_t n) {
    char *result;
    size_t len = strnlen(s, n);
    result = (char *)malloc(len + 1);
    if (!result) {
        return NULL;
    }
    strncpy(result, s, len);
    result[len] = '\0';
    return result;
}
#endif

#define MAX_BUFFER_SIZE 1024
char buffer[MAX_BUFFER_SIZE];
int buffer_pos = 0;
int buffer_size = 0;
FILE *source;

typedef struct {
    char *keyword;
    TokenType type;
} Keyword;

// Keyword keywords[] = {
//     {"title", TOKEN_TITLE},
//     {"narrate", TOKEN_NARRATE},
//     {"choice", TOKEN_CHOICE},
//     {"input", TOKEN_INPUT},
//     {"goto", TOKEN_GOTO},
//     {"end", TOKEN_END},
//     {"if", TOKEN_IF},
//     {"ifelse", TOKEN_IFELSE},
//     {"switch", TOKEN_SWITCH},
//     {"assign", TOKEN_ASSIGN},
//     {NULL, TOKEN_EOF}
// };

Keyword keywords[] = {
    {"title", TOKEN_TITLE},
    {"narrate", TOKEN_NARRATE},
    {"say", TOKEN_SAY},
    {"pause", TOKEN_PAUSE},
    {"choice", TOKEN_CHOICE},
    {"goto", TOKEN_GOTO},
    {"story_restart", TOKEN_RESTART},
    {"end", TOKEN_END},
    {"assign", TOKEN_ASSIGN},
    {"increase", TOKEN_INCREASE},
    {"decrease", TOKEN_DECREASE},
    {"if", TOKEN_IF},
    {"ifelse", TOKEN_IFELSE},
    {"switch", TOKEN_SWITCH},
    {"createInventory", TOKEN_CREATE_INV},
    {"addItem", TOKEN_ADD_ITEM},
    {"removeItem", TOKEN_REMOVE_ITEM},
    {"hasItem", TOKEN_HAS_ITEM},
    {"countInventory", TOKEN_COUNT_INV},
    {"clearInventory", TOKEN_CLEAR_INV},
    {"showInventory", TOKEN_SHOW_INV},
    {"combine", TOKEN_COMBINE},
    {"lengthOf", TOKEN_LENGTH},
    {"substring_in", TOKEN_SUBSTRING},
    {"uppercase", TOKEN_UPPERCASE},
    {"lowercase", TOKEN_LOWERCASE},
    {"format_text", TOKEN_FORMAT_TEXT},
    {"set_time_of_day", TOKEN_SET_TIME},
    {"check_time_of_day", TOKEN_CHECK_TIME},
    {"checkStatus", TOKEN_CHECK_STATUS},
    {"characterStatus", TOKEN_CHAR_STATUS},
    {NULL, TOKEN_EOF}
};


void init_lexer(FILE *file) {
    buffer_pos = 0;
    buffer_size = fread(buffer, 1, MAX_BUFFER_SIZE, file);
    buffer[buffer_size] = '\0';

    printf("Buffer Content:\n%s\n", buffer);

    if (buffer_size == 0) {
        fprintf(stderr, "Error: File is empty or could not be read.\n");
        exit(1);
    }
}

void skip_whitespace() {
    while (isspace(buffer[buffer_pos])) {
        buffer_pos++;
    }
}

Token match_keyword_or_identifier(char *str) {
    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(str, keywords[i].keyword) == 0) {
            return (Token){keywords[i].type, strdup(str)};
        }
    }
    return (Token){TOKEN_IDENTIFIER, strdup(str)};
}

Token get_next_token() {
    skip_whitespace();

    if (buffer[buffer_pos] == '\0') {
        return (Token){TOKEN_EOF, NULL};
    }

    if (buffer[buffer_pos] == '"') {
        buffer_pos++;
        int start = buffer_pos;
        while (buffer[buffer_pos] != '"' && buffer[buffer_pos] != '\0') {
            buffer_pos++;
        }
        if (buffer[buffer_pos] == '"') {
            int length = buffer_pos - start;
            char *value = strndup(&buffer[start], length);
            buffer_pos++;
            return (Token){TOKEN_STRING, value};
        }
    }

    if (isalpha(buffer[buffer_pos])) {
        int start = buffer_pos;
        while (isalnum(buffer[buffer_pos]) || buffer[buffer_pos] == '_') {
            buffer_pos++;
        }
        int length = buffer_pos - start;
        char *value = strndup(&buffer[start], length);

        if (buffer[buffer_pos] == ':') {
            buffer_pos++;
            return (Token){TOKEN_LABEL, value};
        }
        return match_keyword_or_identifier(value);
    }

    switch (buffer[buffer_pos]) {
        case '(':
            buffer_pos++;
            return (Token){TOKEN_LPAREN, strdup("(")};
        case ')':
            buffer_pos++;
            return (Token){TOKEN_RPAREN, strdup(")")};
        case '[':
            buffer_pos++;
            return (Token){TOKEN_LBRACKET, strdup("[")};
        case ']':
            buffer_pos++;
            return (Token){TOKEN_RBRACKET, strdup("]")};
        case ',':
            buffer_pos++;
            return (Token){TOKEN_COMMA, strdup(",")};
        case ':':
            buffer_pos++;
            return (Token){TOKEN_COLON, strdup(":")};
        case '-':
            if (buffer[buffer_pos + 1] == '>') {
                buffer_pos += 2;
                return (Token){TOKEN_ARROW, strdup("->")};
            }
            break;
    }

    fprintf(stderr, "Unexpected character: %c\n", buffer[buffer_pos]);
    exit(1);
}

void print_token(Token token) {
    printf("Token Type: %d, Value: %s\n", token.type, token.value ? token.value : "NULL");
}

void free_token(Token token) {
    if (token.value) {
        free(token.value);
    }
}