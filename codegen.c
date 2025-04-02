#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

#define MAX_LINE_LENGTH 256

void generate_target_code(const char *icg_filename, const char *target_filename) {
    FILE *icg_file = fopen(icg_filename, "r");
    if (!icg_file) {
        perror("Error opening ICG file");
        exit(1);
    }
    FILE *target_file = fopen(target_filename, "w");
    if (!target_file) {
        perror("Error opening target file");
        fclose(icg_file);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, icg_file)) {
        char cmd[MAX_LINE_LENGTH], param1[MAX_LINE_LENGTH] = "", param2[MAX_LINE_LENGTH] = "", param3[MAX_LINE_LENGTH] = "";
        sscanf(line, "%s", cmd);

        if (!strcmp(cmd, "LOAD")) {
            sscanf(line, "LOAD \"%[^\"]\"", param1);
            fprintf(target_file, "LOAD \"%s\"\n", param1);
        } else if (!strcmp(cmd, "SAY") || !strcmp(cmd, "PRINT")) {
            sscanf(line, "%*s \"%[^\"]\"", param1);
            fprintf(target_file, "PRINT \"%s\"\n", param1);
        } else if (!strcmp(cmd, "PAUSE")) {
            sscanf(line, "PAUSE %s", param1);
            fprintf(target_file, "PAUSE %s\n", param1);
        } else if (!strcmp(cmd, "PROMPT")) {
            char choices[1024], labels[1024];
            sscanf(line, "PROMPT %[^->] -> [%[^]]", choices, labels);
            fprintf(target_file, "PROMPT %s -> [%s]\n", choices, labels);
        } else if (!strcmp(cmd, "INPUT")) {
            sscanf(line, "INPUT %s", param1);
            fprintf(target_file, "INPUT %s\n", param1);
        } else if (!strcmp(cmd, "CONFIRM")) {
            char true_label[256], false_label[256];
            sscanf(line, "CONFIRM \"%[^\"]\" -> [%[^,], %[^]]", param1, true_label, false_label);
            fprintf(target_file, "CONFIRM \"%s\" -> [%s, %s]\n", param1, true_label, false_label);
        } else if (!strcmp(cmd, "WAIT_FOR_KEY")) {
            fprintf(target_file, "WAIT_FOR_KEY\n");
        } else if (!strcmp(cmd, "JUMP")) {
            sscanf(line, "JUMP %s", param1);
            fprintf(target_file, "JUMP %s\n", param1);
        } else if (!strcmp(cmd, "HALT")) {
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
        } else if (!strcmp(cmd, "SWITCH")) {
            char cases_labels[1024];
            sscanf(line, "SWITCH %s -> [%[^]]", param1, cases_labels);
            fprintf(target_file, "SWITCH %s -> [%s]\n", param1, cases_labels);
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
        } else if (!strcmp(cmd, "BOTH")) {
            sscanf(line, "BOTH %s %s", param1, param2);
            fprintf(target_file, "BOTH %s %s\n", param1, param2);
        } else if (!strcmp(cmd, "EITHER")) {
            sscanf(line, "EITHER %s %s", param1, param2);
            fprintf(target_file, "EITHER %s %s\n", param1, param2);
        } else if (!strcmp(cmd, "INVERT")) {
            sscanf(line, "INVERT %s", param1);
            fprintf(target_file, "INVERT %s\n", param1);
        } else if (!strcmp(cmd, "COMBINE")) {
            sscanf(line, "COMBINE %s \"%[^\"]\" \"%[^\"]\"", param1, param2, param3);
            fprintf(target_file, "COMBINE %s \"%s\" \"%s\"\n", param1, param2, param3);
        } else if (!strcmp(cmd, "LENGTH")) {
            sscanf(line, "LENGTH %s \"%[^\"]\"", param1, param2);
            fprintf(target_file, "LENGTH %s \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "SUBSTRING_IN")) {
            sscanf(line, "SUBSTRING_IN \"%[^\"]\" \"%[^\"]\"", param1, param2);
            fprintf(target_file, "SUBSTRING_IN \"%s\" \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "UPPERCASE")) {
            sscanf(line, "UPPERCASE %s \"%[^\"]\"", param1, param2);
            fprintf(target_file, "UPPERCASE %s \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "LOWERCASE")) {
            sscanf(line, "LOWERCASE %s \"%[^\"]\"", param1, param2);
            fprintf(target_file, "LOWERCASE %s \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "FORMAT_TEXT")) {
            sscanf(line, "FORMAT_TEXT \"%[^\"]\" \"%[^\"]\"", param1, param2);
            fprintf(target_file, "FORMAT_TEXT \"%s\" \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "CREATE_INV")) {
            sscanf(line, "CREATE_INV %s", param1);
            fprintf(target_file, "CREATE_INV %s\n", param1);
        } else if (!strcmp(cmd, "ADD_ITEM")) {
            sscanf(line, "ADD_ITEM %s \"%[^\"]\"", param1, param2);
            fprintf(target_file, "ADD_ITEM %s \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "REMOVE_ITEM")) {
            sscanf(line, "REMOVE_ITEM %s \"%[^\"]\"", param1, param2);
            fprintf(target_file, "REMOVE_ITEM %s \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "HAS_ITEM")) {
            sscanf(line, "HAS_ITEM %s \"%[^\"]\"", param1, param2);
            fprintf(target_file, "HAS_ITEM %s \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "COUNT_INV")) {
            sscanf(line, "COUNT_INV %s", param1);
            fprintf(target_file, "COUNT_INV %s\n", param1);
        } else if (!strcmp(cmd, "CLEAR_INV")) {
            sscanf(line, "CLEAR_INV %s", param1);
            fprintf(target_file, "CLEAR_INV %s\n", param1);
        } else if (!strcmp(cmd, "SHOW_INV")) {
            sscanf(line, "SHOW_INV %s", param1);
            fprintf(target_file, "SHOW_INV %s\n", param1);
        } else if (!strcmp(cmd, "CREATE_SCENE")) {
            sscanf(line, "CREATE_SCENE %s", param1);
            fprintf(target_file, "CREATE_SCENE %s\n", param1);
        } else if (!strcmp(cmd, "RANDOM_EVENT")) {
            fprintf(target_file, "RANDOM_EVENT\n");
        } else if (!strcmp(cmd, "SET_BACKGROUND")) {
            sscanf(line, "SET_BACKGROUND %s \"%[^\"]\"", param1, param2);
            fprintf(target_file, "SET_BACKGROUND %s \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "TRIGGER_SCENE")) {
            sscanf(line, "TRIGGER_SCENE %s", param1);
            fprintf(target_file, "TRIGGER_SCENE %s\n", param1);
        } else if (!strcmp(cmd, "ADD_CHAR")) {
            sscanf(line, "ADD_CHAR \"%[^\"]\"", param1);
            fprintf(target_file, "ADD_CHAR \"%s\"\n", param1);
        } else if (!strcmp(cmd, "REMOVE_CHAR")) {
            sscanf(line, "REMOVE_CHAR \"%[^\"]\"", param1);
            fprintf(target_file, "REMOVE_CHAR \"%s\"\n", param1);
        } else if (!strcmp(cmd, "SET_CHAR_EMOTION")) {
            sscanf(line, "SET_CHAR_EMOTION \"%[^\"]\" \"%[^\"]\"", param1, param2);
            fprintf(target_file, "SET_CHAR_EMOTION \"%s\" \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "CHANGE_NAME")) {
            sscanf(line, "CHANGE_NAME \"%[^\"]\" \"%[^\"]\"", param1, param2);
            fprintf(target_file, "CHANGE_NAME \"%s\" \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "SET_CHAR_DESC")) {
            sscanf(line, "SET_CHAR_DESC \"%[^\"]\" \"%[^\"]\"", param1, param2);
            fprintf(target_file, "SET_CHAR_DESC \"%s\" \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "CHAR_STATUS")) {
            sscanf(line, "CHAR_STATUS \"%[^\"]\" \"%[^\"]\"", param1, param2);
            fprintf(target_file, "CHAR_STATUS \"%s\" \"%s\"\n", param1, param2);
        } else if (!strcmp(cmd, "CHECK_STATUS")) {
            sscanf(line, "CHECK_STATUS \"%[^\"]\"", param1);
            fprintf(target_file, "CHECK_STATUS \"%s\"\n", param1);
        } else if (!strcmp(cmd, "SET_TIME")) {
            sscanf(line, "SET_TIME \"%[^\"]\"", param1);
            fprintf(target_file, "SET_TIME \"%s\"\n", param1);
        } else if (!strcmp(cmd, "CHECK_TIME")) {
            fprintf(target_file, "CHECK_TIME\n");
        } else {
            fprintf(target_file, "%s", line); // Pass unrecognized lines as-is
        }
    }
    fclose(icg_file);
    fclose(target_file);
    printf("Target code successfully written to '%s'\n", target_filename);
}