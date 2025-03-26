#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

#define MAX_LINE_LENGTH 256

// Generate target code from intermediate code (ICG)
void generate_target_code(const char *icg_filename, const char *target_filename) {
    FILE *icg_file = fopen(icg_filename, "r");
    FILE *target_file = fopen(target_filename, "w");

    if (!icg_file) {
        perror("Error opening ICG file");
        exit(1);
    }
    if (!target_file) {
        perror("Error opening target code file");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, icg_file)) {
        char command[MAX_LINE_LENGTH];
        char param1[MAX_LINE_LENGTH], param2[MAX_LINE_LENGTH];
        
        // Parse the ICG command
        if (sscanf(line, "TITLE \"%[^\"]\"", param1) == 1) {
            fprintf(target_file, "LOAD \"%s\"\n", param1);
        }
        else if (sscanf(line, "NARRATE \"%[^\"]\"", param1) == 1) {
            fprintf(target_file, "PRINT \"%s\"\n", param1);
        }
        else if (strncmp(line, "CHOICE", 6) == 0) {
            char *start_choices = strchr(line, '"');  // Start of choices
            char *end_choices = strrchr(line, '"');  // Last occurrence of "
            char *start_labels = strchr(line, '[');
            char *end_labels = strchr(line, ']');

            if (start_choices && end_choices && start_labels && end_labels) {
                *end_choices = '\0';  // Terminate choices part
                *end_labels = '\0';   // Terminate labels part

                char choices_part[MAX_LINE_LENGTH];
                char labels_part[MAX_LINE_LENGTH];
                
                // Extract choices and labels correctly
                strncpy(choices_part, start_choices, end_choices - start_choices + 1);
                choices_part[end_choices - start_choices + 1] = '\0';

                strncpy(labels_part, start_labels + 1, end_labels - start_labels - 1);
                labels_part[end_labels - start_labels - 1] = '\0';

                // Generate correct PROMPT format
                fprintf(target_file, "PROMPT %s -> [%s]\n", choices_part, labels_part);
            } else {
                fprintf(stderr, "Error parsing CHOICE in ICG\n");
            }
        }
        else if (sscanf(line, "GOTO %s", param1) == 1) {
            fprintf(target_file, "JUMP %s\n", param1);
        }
        else if (sscanf(line, "LABEL %s", param1) == 1) {
            // Remove any trailing colon or extra characters
            param1[strcspn(param1, " \n:")] = '\0';
            fprintf(target_file, "LABEL %s\n", param1);
        }
        else if (strncmp(line, "END", 3) == 0) {
            fprintf(target_file, "HALT\n");
        }
        if (sscanf(line, "CREATE_INVENTORY") == 0) {
            fprintf(target_file, "INVENTORY_INIT\n");
        }
        else if (sscanf(line, "ADD_ITEM \"%[^\"]\"", param1) == 1) {
            fprintf(target_file, "INVENTORY_ADD \"%s\"\n", param1);
        }
        else if (sscanf(line, "REMOVE_ITEM \"%[^\"]\"", param1) == 1) {
            fprintf(target_file, "INVENTORY_REMOVE \"%s\"\n", param1);
        }
        else if (strncmp(line, "SHOW_INVENTORY", 14) == 0) {
            fprintf(target_file, "INVENTORY_SHOW\n");
        }
        else if (sscanf(line, "COMBINE \"%[^\"]\" \"%[^\"]\"", param1, param2) == 2) {
            fprintf(target_file, "STRING_COMBINE \"%s\" \"%s\"\n", param1, param2);
        }
        else if (sscanf(line, "LENGTH_OF \"%[^\"]\"", param1) == 1) {
            fprintf(target_file, "STRING_LENGTH \"%s\"\n", param1);
        }
        else if (sscanf(line, "UPPERCASE \"%[^\"]\"", param1) == 1) {
            fprintf(target_file, "STRING_UPPER \"%s\"\n", param1);
        }
        else if (sscanf(line, "LOWERCASE \"%[^\"]\"", param1) == 1) {
            fprintf(target_file, "STRING_LOWER \"%s\"\n", param1);
        }
        else if (sscanf(line, "SET_TIME \"%[^\"]\"", param1) == 1) {
            fprintf(target_file, "SET_TIME \"%s\"\n", param1);
        }
        else if (strncmp(line, "CHECK_TIME", 10) == 0) {
            fprintf(target_file, "GET_TIME\n");
        }

        else {
            fprintf(stderr, "Warning: Unknown ICG command: %s\n", line);
        }
    }

    fclose(icg_file);
    fclose(target_file);
    printf("Target code successfully written to '%s'\n", target_filename);
}
