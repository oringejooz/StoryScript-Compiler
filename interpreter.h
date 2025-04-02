#ifndef INTERPRETER_H
#define INTERPRETER_H

#define MAX_ITEMS 50
#define MAX_CHARS 20

typedef struct {
    char *label;
    long position;
} LabelMap;

typedef struct {
    char *name;
    int value;
} Variable;

typedef struct {
    char *name;
    char *items[MAX_ITEMS];
    int count;
} Inventory;

typedef struct {
    char *name;
    char *emotion;
    char *description;
    char *status;
} Character;

void run_interpreter(const char *target_filename);

#endif