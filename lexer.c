#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "utils.h"

#define MAX_BUFFER_SIZE 1024
static char buffer[MAX_BUFFER_SIZE];
static int buffer_pos = 0;
static int buffer_size = 0;
static FILE *source;

static struct {
    char *keyword;
    TokenType type;
} keywords[] = {
    {"title", TOKEN_TITLE}, {"say", TOKEN_SAY}, {"narrate", TOKEN_NARRATE}, {"pause", TOKEN_PAUSE},
    {"choice", TOKEN_CHOICE}, {"input", TOKEN_INPUT}, {"confirm", TOKEN_CONFIRM}, {"goto", TOKEN_GOTO},
    {"end", TOKEN_END}, {"return", TOKEN_RETURN}, {"story_restart", TOKEN_RESTART},
    {"if", TOKEN_IF}, {"ifelse", TOKEN_IFELSE}, {"switch", TOKEN_SWITCH}, {"assign", TOKEN_ASSIGN},
    {"increase", TOKEN_INCREASE}, {"decrease", TOKEN_DECREASE}, {"scale", TOKEN_SCALE},
    {"divide_by", TOKEN_DIVIDE_BY}, {"randomize", TOKEN_RANDOMIZE}, {"both", TOKEN_BOTH},
    {"either", TOKEN_EITHER}, {"invert", TOKEN_INVERT}, {"combine", TOKEN_COMBINE},
    {"length_of", TOKEN_LENGTH}, {"substring_in", TOKEN_SUBSTRING_IN}, {"uppercase", TOKEN_UPPERCASE},
    {"lowercase", TOKEN_LOWERCASE}, {"format_text", TOKEN_FORMAT_TEXT}, {"create_inventory", TOKEN_CREATE_INV},
    {"add_to_inventory", TOKEN_ADD_ITEM}, {"remove_item", TOKEN_REMOVE_ITEM}, {"has_item", TOKEN_HAS_ITEM},
    {"count_inventory", TOKEN_COUNT_INV}, {"clear_inventory", TOKEN_CLEAR_INV}, {"show_inventory", TOKEN_SHOW_INV},
    {"set_time_of_day", TOKEN_SET_TIME}, {"check_time_of_day", TOKEN_CHECK_TIME}, {NULL, TOKEN_EOF}
};

void init_lexer(FILE *file) {
    source = file;
    buffer_pos = 0;
    buffer_size = fread(buffer, 1, MAX_BUFFER_SIZE - 1, file);
    buffer[buffer_size] = '\0';
}

static void skip_whitespace() {
    while (buffer_pos < buffer_size && isspace(buffer[buffer_pos])) {
        buffer_pos++;
    }
}

static Token match_keyword_or_identifier(char *str) {
    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(str, keywords[i].keyword) == 0) {
            return (Token){keywords[i].type, strdup(str)};
        }
    }
    return (Token){TOKEN_IDENTIFIER, strdup(str)};
}

Token get_next_token() {
    skip_whitespace();
    if (buffer_pos >= buffer_size || buffer[buffer_pos] == '\0') {
        return (Token){TOKEN_EOF, NULL};
    }

    if (buffer[buffer_pos] == '"') {
        buffer_pos++;
        int start = buffer_pos;
        while (buffer_pos < buffer_size && buffer[buffer_pos] != '"' && buffer[buffer_pos] != '\0') {
            buffer_pos++;
        }
        if (buffer[buffer_pos] == '"') {
            int length = buffer_pos - start;
            char *value = strndup(buffer + start, length); // Using custom strndup from utils.c
            buffer_pos++;
            return (Token){TOKEN_STRING, value};
        }
    }

    if (isalpha(buffer[buffer_pos])) {
        int start = buffer_pos;
        while (buffer_pos < buffer_size && (isalnum(buffer[buffer_pos]) || buffer[buffer_pos] == '_')) {
            buffer_pos++;
        }
        int length = buffer_pos - start;
        char *value = strndup(buffer + start, length); // Using custom strndup from utils.c
        if (buffer_pos < buffer_size && buffer[buffer_pos] == ':') {
            buffer_pos++;
            return (Token){TOKEN_LABEL, value};
        }
        return match_keyword_or_identifier(value);
    }

    switch (buffer[buffer_pos]) {
        case '(': buffer_pos++; return (Token){TOKEN_LPAREN, strdup("(")};
        case ')': buffer_pos++; return (Token){TOKEN_RPAREN, strdup(")")};
        case '[': buffer_pos++; return (Token){TOKEN_LBRACKET, strdup("[")};
        case ']': buffer_pos++; return (Token){TOKEN_RBRACKET, strdup("]")};
        case ',': buffer_pos++; return (Token){TOKEN_COMMA, strdup(",")};
        case ':': buffer_pos++; return (Token){TOKEN_COLON, strdup(":")};
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

void free_token(Token token) {
    if (token.value) free(token.value);
}