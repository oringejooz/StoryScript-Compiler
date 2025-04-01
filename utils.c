#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}

char *strdup(const char *s) {
    char *copy = (char *)malloc(strlen(s) + 1);
    if (!copy) error("Memory allocation failed");
    strcpy(copy, s);
    return copy;
}

char *strndup(const char *s, size_t n) {
    size_t len = strlen(s);
    if (len > n) len = n;
    char *copy = (char *)malloc(len + 1);
    if (!copy) error("Memory allocation failed");
    strncpy(copy, s, len);
    copy[len] = '\0';
    return copy;
}