#ifndef INTERPRETER_H
#define INTERPRETER_H

#define MAX_ITEMS 50
#define MAX_CHARS 20

typedef struct LabelMap {
    char *label;
    long position;
} LabelMap;

typedef struct Variable {
    char *name;
    char *value;
} Variable;

typedef struct Inventory {
    char *name;
    char *items[MAX_ITEMS];
    int count;
} Inventory;

typedef struct Character {
    char *name;
    char *emotion;
    char *description;
    char *status;
} Character;

typedef struct Scene {
    char *name;
    char *background;
} Scene;

void interpret(const char *filename);

#endif