#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

#define MAX_LINE_LENGTH 256

void generate_target_code(const char *icg_filename, const char *target_filename) {
    FILE *icg_file = fopen(icg_filename, "r");
    FILE *target_file = fopen(target_filename, "w");
    if (!icg_file || !target_file) {
        perror("Error opening files");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, icg_file)) {
        char cmd[MAX_LINE_LENGTH], param1[MAX_LINE_LENGTH], param2[MAX_LINE_LENGTH];
        if (sscanf(line, "%s", cmd) == 1) {
            if (!strcmp(cmd, "TITLE")) {
                sscanf(line, "TITLE \"%[^\"]\"", param1);
                fprintf(target_file, "LOAD \"%s\"\n", param1);
            } else if (!strcmp(cmd, "SAY") || !strcmp(cmd, "NARRATE")) {
                sscanf(line, "%*s \"%[^\"]\"", param1);
                fprintf(target_file, "PRINT \"%s\"\n", param1);
            } else if (!strcmp(cmd, "PAUSE")) {
                sscanf(line, "PAUSE %s", param1);
                fprintf(target_file, "PAUSE %s\n", param1);
            } else if (!strcmp(cmd, "CHOICE")) {
                char choices[1024], labels[1024];
                sscanf(line, "CHOICE %[^->] -> [%[^]]", choices, labels);
                fprintf(target_file, "PROMPT %s -> [%s]\n", choices, labels);
            } else if (!strcmp(cmd, "INPUT")) {
                sscanf(line, "INPUT %s", param1);
                fprintf(target_file, "INPUT %s\n", param1);
            } else if (!strcmp(cmd, "CONFIRM")) {
                sscanf(line, "CONFIRM \"%[^\"]\"", param1);
                fprintf(target_file, "CONFIRM \"%s\"\n", param1);
            } else if (!strcmp(cmd, "GOTO")) {
                sscanf(line, "GOTO %s", param1);
                fprintf(target_file, "JUMP %s\n", param1);
            } else if (!strcmp(cmd, "END")) {
                fprintf(target_file, "HALT\n");
            } else if (!strcmp(cmd, "RETURN")) {
                fprintf(target_file, "RETURN\n");
            } else if (!strcmp(cmd, "RESTART")) {
                fprintf(target_file, "RESTART\n");
            } else if (!strcmp(cmd, "LABEL")) {
                sscanf(line, "LABEL %s", param1);
                fprintf(target_file, "LABEL %s\n", param1);
            } else if (!strcmp(cmd, "IF")) {
                sscanf(line, "IF %s -> %s", param1, param2);
                fprintf(target_file, "IF %s -> %s\n", param1, param2);
            } else if (!strcmp(cmd, "IFELSE")) {
                char true_label[256], false_label[256];
                sscanf(line, "IFELSE %s -> [%[^,], %[^]]", param1, true_label, false_label);
                fprintf(target_file, "IFELSE %s -> [%s, %s]\n", param1, true_label, false_label);
            } else if (!strcmp(cmd, "ASSIGN")) {
                sscanf(line, "ASSIGN %s \"%[^\"]\"", param1, param2);
                fprintf(target_file, "ASSIGN %s \"%s\"\n", param1, param2);
            } else if (!strcmp(cmd, "INCREASE")) {
                sscanf(line, "INCREASE %s %s", param1, param2);
                fprintf(target_file, "INCREASE %s %s\n", param1, param2);
            } else if (!strcmp(cmd, "DECREASE")) {
                sscanf(line, "DECREASE %s %s", param1, param2);
                fprintf(target_file, "DECREASE %s %s\n", param1, param2);
            } else if (!strcmp(cmd, "SCALE")) {
                sscanf(line, "SCALE %s %s", param1, param2);
                fprintf(target_file, "SCALE %s %s\n", param1, param2);
            } else if (!strcmp(cmd, "DIVIDE_BY")) {
                sscanf(line, "DIVIDE_BY %s %s", param1, param2);
                fprintf(target_file, "DIVIDE_BY %s %s\n", param1, param2);
            } else if (!strcmp(cmd, "RANDOMIZE")) {
                sscanf(line, "RANDOMIZE %s %s", param1, param2);
                fprintf(target_file, "RANDOMIZE %s %s\n", param1, param2);
            } else if (!strcmp(cmd, "COMBINE")) {
                sscanf(line, "COMBINE \"%[^\"]\" \"%[^\"]\"", param1, param2);
                fprintf(target_file, "COMBINE \"%s\" \"%s\"\n", param1, param2);
            } else if (!strcmp(cmd, "LENGTH")) {
                sscanf(line, "LENGTH \"%[^\"]\"", param1);
                fprintf(target_file, "LENGTH \"%s\"\n", param1);
            } else if (!strcmp(cmd, "SUBSTRING_IN")) {
                sscanf(line, "SUBSTRING_IN \"%[^\"]\"", param1);
                fprintf(target_file, "SUBSTRING_IN \"%s\"\n", param1);
            } else if (!strcmp(cmd, "UPPERCASE")) {
                sscanf(line, "UPPERCASE \"%[^\"]\"", param1);
                fprintf(target_file, "UPPERCASE \"%s\"\n", param1);
            } else if (!strcmp(cmd, "LOWERCASE")) {
                sscanf(line, "LOWERCASE \"%[^\"]\"", param1);
                fprintf(target_file, "LOWERCASE \"%s\"\n", param1);
            } else if (!strcmp(cmd, "FORMAT_TEXT")) {
                sscanf(line, "FORMAT_TEXT \"%[^\"]\" \"%[^\"]\"", param1, param2);
                fprintf(target_file, "FORMAT_TEXT \"%s\" \"%s\"\n", param1, param2);
            } else if (!strcmp(cmd, "CREATE_INV")) {
                sscanf(line, "CREATE_INV %s", param1);
                fprintf(target_file, "CREATE_INV %s\n", param1);
            } else if (!strcmp(cmd, "ADD_ITEM")) {
                sscanf(line, "ADD_ITEM \"%[^\"]\"", param1);
                fprintf(target_file, "ADD_ITEM \"%s\"\n", param1);
            } else if (!strcmp(cmd, "REMOVE_ITEM")) {
                sscanf(line, "REMOVE_ITEM \"%[^\"]\"", param1);
                fprintf(target_file, "REMOVE_ITEM \"%s\"\n", param1);
            } else if (!strcmp(cmd, "HAS_ITEM")) {
                sscanf(line, "HAS_ITEM \"%[^\"]\"", param1);
                fprintf(target_file, "HAS_ITEM \"%s\"\n", param1);
            } else if (!strcmp(cmd, "COUNT_INV")) {
                fprintf(target_file, "COUNT_INV\n");
            } else if (!strcmp(cmd, "CLEAR_INV")) {
                fprintf(target_file, "CLEAR_INV\n");
            } else if (!strcmp(cmd, "SHOW_INV")) {
                fprintf(target_file, "SHOW_INV\n");
            } else if (!strcmp(cmd, "SET_TIME")) {
                sscanf(line, "SET_TIME \"%[^\"]\"", param1);
                fprintf(target_file, "SET_TIME \"%s\"\n", param1);
            } else if (!strcmp(cmd, "CHECK_TIME")) {
                fprintf(target_file, "CHECK_TIME\n");
            }
        }
    }
    fclose(icg_file);
    fclose(target_file);
}