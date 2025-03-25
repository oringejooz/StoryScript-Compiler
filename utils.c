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
    if (copy == NULL) {
        error("Memory allocation failed in strdup");
    }
    strcpy(copy, s);
    return copy;
}