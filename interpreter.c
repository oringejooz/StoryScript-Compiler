#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

#define MAX_LINE_LENGTH 256
#define MAX_LABELS 100

// Structure to store label positions
typedef struct {
    char *label;
    long position;
} LabelMap;

LabelMap labels[MAX_LABELS];
int label_count = 0;

// Helper function to trim whitespace and newlines
void trim(char *str) {
    char *end;
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') str++;
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) end--;
    end[1] = '\0';
}

// Store label position for fast jumps
void register_label(char *label, long position) {
    trim(label);
    if (label_count < MAX_LABELS) {
        labels[label_count].label = strdup(label);
        labels[label_count].position = position;
        label_count++;
    } else {
        fprintf(stderr, "Error: Too many labels!\n");
        exit(1);
    }
}

// Find the position of a label
long get_label_position(char *label) {
    trim(label);
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].label, label) == 0) {
            return labels[i].position;
        }
    }
    fprintf(stderr, "Error: Label '%s' not found!\n", label);
    exit(1);
}

// Run the interpreter
void run_interpreter(const char *target_filename) {
    FILE *target_file = fopen(target_filename, "r");
    if (!target_file) {
        perror("Error opening target file");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    long position;

    // First Pass: Register labels
    while (fgets(line, MAX_LINE_LENGTH, target_file)) {
        trim(line);
        if (strncmp(line, "LABEL", 5) == 0) {
            char label[MAX_LINE_LENGTH];
            sscanf(line, "LABEL %s", label);
            position = ftell(target_file);
            register_label(label, position);
        }
    }

    // Reset file pointer to start
    rewind(target_file);

    // Second Pass: Execute instructions
    while (fgets(line, MAX_LINE_LENGTH, target_file)) {
        trim(line);

        if (strncmp(line, "LOAD", 4) == 0) {
            char title[MAX_LINE_LENGTH];
            sscanf(line, "LOAD \"%[^\"]\"", title);
            printf("\n=== %s ===\n", title);
        }
        else if (strncmp(line, "PRINT", 5) == 0) {
            char text[MAX_LINE_LENGTH];
            sscanf(line, "PRINT \"%[^\"]\"", text);
            printf("%s\n", text);
        }
        else if (strncmp(line, "PROMPT", 6) == 0) {
            char choices_part[MAX_LINE_LENGTH], labels_part[MAX_LINE_LENGTH];
            sscanf(line, "PROMPT \"%[^\"]\" -> [%[^]]", choices_part, labels_part);

            // Split choices and labels
            char *choice_tokens[10];
            char *label_tokens[10];
            int choice_count = 0;

            // Split and process choices
            char *choice_token = strtok(choices_part, ",");
            char *label_token = strtok(labels_part, ",");

            while (choice_token && label_token && choice_count < 10) {
                trim(choice_token);
                trim(label_token);
                choice_tokens[choice_count] = strdup(choice_token);
                label_tokens[choice_count] = strdup(label_token);
                choice_token = strtok(NULL, ",");
                label_token = strtok(NULL, ",");
                choice_count++;
            }

            // Display choices
            printf("Make a choice:\n");
            for (int i = 0; i < choice_count; i++) {
                printf("%d. %s\n", i + 1, choice_tokens[i]);
            }

            // Get user input safely
            int selected_choice = -1;
            char input[MAX_LINE_LENGTH];
            printf("Enter your choice: ");
            fgets(input, MAX_LINE_LENGTH, stdin); // Use fgets to clear input buffer
            sscanf(input, "%d", &selected_choice);

            if (selected_choice < 1 || selected_choice > choice_count) {
                printf("Invalid choice. Ending story.\n");
                break;
            }

            // Jump to selected label
            char target_label[MAX_LINE_LENGTH];
            strcpy(target_label, label_tokens[selected_choice - 1]);
            trim(target_label);
            fseek(target_file, get_label_position(target_label), SEEK_SET);

            // Free allocated memory for choices and labels
            for (int i = 0; i < choice_count; i++) {
                free(choice_tokens[i]);
                free(label_tokens[i]);
            }
        }
        else if (strncmp(line, "JUMP", 4) == 0) {
            char target_label[MAX_LINE_LENGTH];
            sscanf(line, "JUMP %s", target_label);
            trim(target_label);
            fseek(target_file, get_label_position(target_label), SEEK_SET);
        }
        else if (strncmp(line, "LABEL", 5) == 0) {
            // Skip labels in the second pass
            continue;
        }
        else if (strncmp(line, "HALT", 4) == 0) {
            printf("The story ends here.\n");
            break;
        }
        else if (strlen(line) > 0) {
            fprintf(stderr, "Unknown instruction: %s\n", line);
            exit(1);
        }
    }

    fclose(target_file);
    printf("\nStory completed!\n");
}
