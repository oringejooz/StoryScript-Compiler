#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "interpreter.h"
#include "utils.h"

#define MAX_LINE_LENGTH 256
#define MAX_LABELS 100
#define MAX_VARIABLES 100
#define MAX_INVENTORIES 10
#define MAX_ITEMS 50  // Define MAX_ITEMS since it’s used in Inventory
#define MAX_CHARS 20  // Define MAX_CHARS since it’s used in Character
#define MAX_SCENES 20

// Static data structures (no redefinition, using types from interpreter.h)
static LabelMap labels[MAX_LABELS];
static int label_count = 0;
static Variable vars[MAX_VARIABLES];
static int var_count = 0;
static Inventory invs[MAX_INVENTORIES];
static int inv_count = 0;
static Character chars[MAX_CHARS];
static int char_count = 0;
static Scene scenes[MAX_SCENES];
static int scene_count = 0;
static char current_time[10] = "00:00";

static void trim(char *str) {
    char *start = str;
    while (*start && isspace(*start)) start++;
    char *end = str + strlen(str) - 1;
    while (end >= start && isspace(*end)) *end-- = '\0';
    memmove(str, start, end - start + 2);
}

static void register_label(const char *label, long pos) {
    if (label_count >= MAX_LABELS) error("Too many labels");
    labels[label_count].label = strdup(label);
    labels[label_count].position = pos;
    label_count++;
}

static long get_label_position(const char *label) {
    char trimmed_label[MAX_LINE_LENGTH];
    strncpy(trimmed_label, label, MAX_LINE_LENGTH - 1);
    trimmed_label[MAX_LINE_LENGTH - 1] = '\0';
    trim(trimmed_label);
    for (int i = 0; i < label_count; i++) {
        if (!strcmp(labels[i].label, trimmed_label)) return labels[i].position;
    }
    fprintf(stderr, "Error: Label '%s' not found\n", trimmed_label);
    exit(1);
}

static Variable *find_var(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (!strcmp(vars[i].name, name)) return &vars[i];
    }
    return NULL;
}

static void set_var(const char *name, const char *value) {
    Variable *v = find_var(name);
    if (!v && var_count < MAX_VARIABLES) {
        vars[var_count].name = strdup(name);
        vars[var_count].value = strdup(value);
        var_count++;
    } else if (v) {
        free(v->value);
        v->value = strdup(value);
    }
}

static char *get_var(const char *name) {
    Variable *v = find_var(name);
    return v ? v->value : "";
}

static Inventory *find_inventory(const char *name) {
    for (int i = 0; i < inv_count; i++) {
        if (!strcmp(invs[i].name, name)) return &invs[i];
    }
    return NULL;
}

static Character *find_character(const char *name) {
    for (int i = 0; i < char_count; i++) {
        if (!strcmp(chars[i].name, name)) return &chars[i];
    }
    return NULL;
}

static Scene *find_scene(const char *name) {
    for (int i = 0; i < scene_count; i++) {
        if (!strcmp(scenes[i].name, name)) return &scenes[i];
    }
    return NULL;
}

static int evaluate_condition(const char *cond) {
    if (!cond || !strlen(cond)) return 0;
    char *endptr;
    long val = strtol(cond, &endptr, 10);
    if (*endptr == '\0') return val != 0;
    return 1; // Non-empty string is true
}

static void portable_sleep(int seconds) {
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

void interpret(const char *filename) {
    srand(time(NULL));
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening target file");
        exit(1);
    }

    // First pass: register labels
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        trim(line);
        if (strncmp(line, "LABEL", 5) == 0) {
            char label[MAX_LINE_LENGTH];
            sscanf(line, "LABEL %s", label);
            register_label(label, ftell(file));
        }
    }
    rewind(file);

    // Second pass: execute commands
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        trim(line);
        if (!strlen(line)) continue;

        char cmd[MAX_LINE_LENGTH], param1[MAX_LINE_LENGTH] = "", param2[MAX_LINE_LENGTH] = "", param3[MAX_LINE_LENGTH] = "";

        // Core Story Control Functions
        if (sscanf(line, "LOAD \"%[^\"]\"", param1) == 1) {
            printf("\n=== %s ===\n", param1);
        } else if (sscanf(line, "SAY \"%[^\"]\"", param1) == 1 || sscanf(line, "PRINT \"%[^\"]\"", param1) == 1) {
            printf("%s\n", param1);
        } else if (sscanf(line, "PAUSE %s", param1) == 1) {
            int seconds = atoi(param1);
            if (seconds > 0) portable_sleep(seconds);
        } else if (strncmp(line, "PROMPT", 6) == 0) {
            char choices[1024], labels[1024];
            sscanf(line, "PROMPT %[^->] -> [%[^]]", choices, labels);
            char *choice_tokens[10], *label_tokens[10];
            int choice_count = 0;

            char *token = strtok(choices + 6, ",");
            while (token && choice_count < 10) {
                trim(token);
                if (token[0] == '"') token++;
                int len = strlen(token);
                if (len > 0 && token[len - 1] == '"') token[len - 1] = '\0';
                choice_tokens[choice_count] = strdup(token);
                token = strtok(NULL, ",");
                choice_count++;
            }

            int label_count = 0;
            token = strtok(labels, ",");
            while (token && label_count < 10) {
                trim(token);
                label_tokens[label_count++] = strdup(token);
                token = strtok(NULL, ",");
            }

            if (choice_count != label_count) {
                fprintf(stderr, "Error: Mismatch between choices (%d) and labels (%d)\n", choice_count, label_count);
                exit(1);
            }

            printf("Choose an option:\n");
            for (int i = 0; i < choice_count; i++) {
                printf("%d. %s\n", i + 1, choice_tokens[i]);
            }
            int choice;
            printf("Enter your choice: ");
            scanf("%d", &choice);
            if (choice < 1 || choice > choice_count) {
                printf("Invalid choice. Ending story.\n");
                break;
            }
            fseek(file, get_label_position(label_tokens[choice - 1]), SEEK_SET);

            for (int i = 0; i < choice_count; i++) {
                free(choice_tokens[i]);
                free(label_tokens[i]);
            }
        } else if (sscanf(line, "INPUT %s", param1) == 1) {
            printf("Enter value for %s: ", param1);
            char input[256];
            scanf(" %[^\n]", input);
            set_var(param1, input);
        } else if (sscanf(line, "CONFIRM \"%[^\"]\" -> [%[^,], %[^]]", param1, param2, param3) == 3) {
            printf("%s (y/n): ", param1);
            char response;
            scanf(" %c", &response);
            fseek(file, get_label_position(response == 'y' || response == 'Y' ? param2 : param3), SEEK_SET);
        } else if (strncmp(line, "WAIT_FOR_KEY", 12) == 0) {
            printf("Press Enter to continue...\n");
            getchar();
            getchar();
        } else if (sscanf(line, "JUMP %s", param1) == 1) {
            fseek(file, get_label_position(param1), SEEK_SET);
        } else if (strncmp(line, "HALT", 4) == 0) {
            printf("The story ends here.\n");
            break;
        } else if (strncmp(line, "RETURN", 6) == 0) {
            printf("Return not fully implemented.\n");
        } else if (strncmp(line, "RESTART", 7) == 0) {
            rewind(file);
            for (int i = 0; i < var_count; i++) free(vars[i].name), free(vars[i].value);
            for (int i = 0; i < inv_count; i++) {
                for (int j = 0; j < invs[i].count; j++) free(invs[i].items[j]);
                free(invs[i].name);
            }
            for (int i = 0; i < char_count; i++) {
                free(chars[i].name);
                if (chars[i].emotion) free(chars[i].emotion);
                if (chars[i].description) free(chars[i].description);
                if (chars[i].status) free(chars[i].status);
            }
            for (int i = 0; i < scene_count; i++) {
                free(scenes[i].name);
                if (scenes[i].background) free(scenes[i].background);
            }
            var_count = inv_count = char_count = scene_count = 0;
            strcpy(current_time, "00:00");
        }

        // Conditional & Flow Control
        else if (sscanf(line, "IF %s -> %s", param1, param2) == 2) {
            if (evaluate_condition(get_var(param1))) {
                fseek(file, get_label_position(param2), SEEK_SET);
            }
        } else if (sscanf(line, "IFELSE %s -> [%[^,], %[^]]", param1, param2, param3) == 3) {
            fseek(file, get_label_position(evaluate_condition(get_var(param1)) ? param2 : param3), SEEK_SET);
        } else if (sscanf(line, "SWITCH %s -> [%[^]]", param1, param2) == 2) {
            char *case_tokens[10], *label_tokens[10];
            int case_count = 0;
            char *token = strtok(param2, ",");
            while (token && case_count < 10) {
                char case_val[256], label[256];
                sscanf(token, "%[^:]:%s", case_val, label);
                trim(case_val);
                trim(label);
                case_tokens[case_count] = strdup(case_val);
                label_tokens[case_count++] = strdup(label);
                token = strtok(NULL, ",");
            }

            char *var_value = get_var(param1);
            for (int i = 0; i < case_count; i++) {
                if (!strcmp(case_tokens[i], "default") || !strcmp(case_tokens[i], var_value)) {
                    fseek(file, get_label_position(label_tokens[i]), SEEK_SET);
                    break;
                }
            }

            for (int i = 0; i < case_count; i++) {
                free(case_tokens[i]);
                free(label_tokens[i]);
            }
        } else if (sscanf(line, "ASSIGN %s \"%[^\"]\"", param1, param2) == 2) {
            set_var(param1, param2);
        } else if (sscanf(line, "INCREASE %s %s", param1, param2) == 2) {
            char *val = get_var(param1);
            int num = atoi(val) + atoi(param2);
            char result[32];
            snprintf(result, sizeof(result), "%d", num);
            set_var(param1, result);
        } else if (sscanf(line, "DECREASE %s %s", param1, param2) == 2) {
            char *val = get_var(param1);
            int num = atoi(val) - atoi(param2);
            char result[32];
            snprintf(result, sizeof(result), "%d", num);
            set_var(param1, result);
        } else if (sscanf(line, "SCALE %s %s", param1, param2) == 2) {
            char *val = get_var(param1);
            int num = atoi(val) * atoi(param2);
            char result[32];
            snprintf(result, sizeof(result), "%d", num);
            set_var(param1, result);
        } else if (sscanf(line, "DIVIDE_BY %s %s", param1, param2) == 2) {
            char *val = get_var(param1);
            int divisor = atoi(param2);
            if (divisor != 0) {
                int num = atoi(val) / divisor;
                char result[32];
                snprintf(result, sizeof(result), "%d", num);
                set_var(param1, result);
            }
        } else if (sscanf(line, "RANDOMIZE %s %s", param1, param2) == 2) {
            int range = atoi(param2);
            if (range > 0) {
                int num = rand() % range;
                char result[32];
                snprintf(result, sizeof(result), "%d", num);
                set_var(param1, result);
            }
        }

        // Logical Operators & Conditions
        else if (sscanf(line, "BOTH %s %s", param1, param2) == 2) {
            int result = evaluate_condition(get_var(param1)) && evaluate_condition(get_var(param2));
            char result_str[2] = { result ? '1' : '0', '\0' };
            set_var(param1, result_str);
        } else if (sscanf(line, "EITHER %s %s", param1, param2) == 2) {
            int result = evaluate_condition(get_var(param1)) || evaluate_condition(get_var(param2));
            char result_str[2] = { result ? '1' : '0', '\0' };
            set_var(param1, result_str);
        } else if (sscanf(line, "INVERT %s", param1) == 1) {
            int result = !evaluate_condition(get_var(param1));
            char result_str[2] = { result ? '1' : '0', '\0' };
            set_var(param1, result_str);
        }

        // String Manipulation
        else if (sscanf(line, "COMBINE %s \"%[^\"]\" \"%[^\"]\"", param1, param2, param3) == 3) {
            char result[512];
            snprintf(result, sizeof(result), "%s%s", param2, param3);
            set_var(param1, result);
        } else if (sscanf(line, "LENGTH %s \"%[^\"]\"", param1, param2) == 2) {
            char result[32];
            snprintf(result, sizeof(result), "%zu", strlen(param2));
            set_var(param1, result);
        } else if (sscanf(line, "SUBSTRING_IN \"%[^\"]\" \"%[^\"]\"", param1, param2) == 2) {
            int result = strstr(param1, param2) != NULL;
            char result_str[2] = { result ? '1' : '0', '\0' };
            set_var("result", result_str); // Default variable name since no target specified
        } else if (sscanf(line, "UPPERCASE %s \"%[^\"]\"", param1, param2) == 2) {
            char result[256];
            strncpy(result, param2, sizeof(result) - 1);
            result[sizeof(result) - 1] = '\0';
            for (int i = 0; result[i]; i++) result[i] = toupper(result[i]);
            set_var(param1, result);
        } else if (sscanf(line, "LOWERCASE %s \"%[^\"]\"", param1, param2) == 2) {
            char result[256];
            strncpy(result, param2, sizeof(result) - 1);
            result[sizeof(result) - 1] = '\0';
            for (int i = 0; result[i]; i++) result[i] = tolower(result[i]);
            set_var(param1, result);
        } else if (sscanf(line, "FORMAT_TEXT \"%[^\"]\" \"%[^\"]\"", param1, param2) == 2) {
            char result[512];
            char *var_value = get_var(param2);
            snprintf(result, sizeof(result), param1, var_value ? var_value : "");
            printf("%s\n", result);
        }

        // Inventory Management
        else if (sscanf(line, "CREATE_INV %s", param1) == 1) {
            if (inv_count < MAX_INVENTORIES) {
                invs[inv_count].name = strdup(param1);
                invs[inv_count].count = 0;
                inv_count++;
            }
        } else if (sscanf(line, "ADD_ITEM %s \"%[^\"]\"", param1, param2) == 2) {
            Inventory *inv = find_inventory(param1);
            if (inv && inv->count < MAX_ITEMS) {
                inv->items[inv->count++] = strdup(param2);
            }
        } else if (sscanf(line, "REMOVE_ITEM %s \"%[^\"]\"", param1, param2) == 2) {
            Inventory *inv = find_inventory(param1);
            if (inv) {
                for (int i = 0; i < inv->count; i++) {
                    if (!strcmp(inv->items[i], param2)) {
                        free(inv->items[i]);
                        inv->items[i] = inv->items[--inv->count];
                        break;
                    }
                }
            }
        } else if (strncmp(line, "HAS_ITEM", 8) == 0) {
            char var_name[256] = {0};
            if (sscanf(line, "HAS_ITEM %s \"%[^\"]\" %s", param1, param2, var_name) == 3) {
                Inventory *inv = find_inventory(param1);
                int has = 0;
                if (inv) {
                    for (int i = 0; i < inv->count; i++) {
                        if (!strcmp(inv->items[i], param2)) {
                            has = 1;
                            break;
                        }
                    }
                }
                char result[2] = { has ? '1' : '0', '\0' };
                set_var(var_name, result);
            } else if (sscanf(line, "HAS_ITEM %s \"%[^\"]\"", param1, param2) == 2) {
                Inventory *inv = find_inventory(param1);
                int has = 0;
                if (inv) {
                    for (int i = 0; i < inv->count; i++) {
                        if (!strcmp(inv->items[i], param2)) {
                            has = 1;
                            break;
                        }
                    }
                }
                printf("Has %s in %s: %s\n", param2, param1, has ? "Yes" : "No");
            }
        } else if (sscanf(line, "COUNT_INV %s", param1) == 1) {
            Inventory *inv = find_inventory(param1);
            char result[32];
            snprintf(result, sizeof(result), "%d", inv ? inv->count : 0);
            set_var(param1, result);
        } else if (sscanf(line, "CLEAR_INV %s", param1) == 1) {
            Inventory *inv = find_inventory(param1);
            if (inv) {
                while (inv->count > 0) free(inv->items[--inv->count]);
            }
        } else if (sscanf(line, "SHOW_INV %s", param1) == 1) {
            Inventory *inv = find_inventory(param1);
            if (inv) {
                printf("Inventory %s: ", param1);
                for (int i = 0; i < inv->count; i++) {
                    printf("%s%s", inv->items[i], i < inv->count - 1 ? ", " : "");
                }
                printf("\n");
            } else {
                printf("Inventory %s is empty or does not exist.\n", param1);
            }
        }

        // Scene & Event Management
        else if (sscanf(line, "CREATE_SCENE %s", param1) == 1) {
            if (scene_count < MAX_SCENES) {
                scenes[scene_count].name = strdup(param1);
                scenes[scene_count].background = NULL;
                scene_count++;
            }
        } else if (strncmp(line, "RANDOM_EVENT", 12) == 0) {
            printf("A random event occurs!\n");
        } else if (sscanf(line, "SET_BACKGROUND %s \"%[^\"]\"", param1, param2) == 2) {
            Scene *scene = find_scene(param1);
            if (scene) {
                if (scene->background) free(scene->background);
                scene->background = strdup(param2);
                printf("Background for %s set to: %s\n", param1, param2);
            }
        } else if (sscanf(line, "TRIGGER_SCENE %s", param1) == 1) {
            Scene *scene = find_scene(param1);
            if (scene) {
                printf("Scene %s triggered. Background: %s\n", param1, scene->background ? scene->background : "None");
            } else {
                printf("Scene %s does not exist.\n", param1);
            }
        }

        // Character Management
        else if (sscanf(line, "ADD_CHAR \"%[^\"]\"", param1) == 1) {
            if (char_count < MAX_CHARS) {
                chars[char_count].name = strdup(param1);
                chars[char_count].emotion = NULL;
                chars[char_count].description = NULL;
                chars[char_count].status = NULL;
                char_count++;
            }
        } else if (sscanf(line, "REMOVE_CHAR \"%[^\"]\"", param1) == 1) {
            Character *c = find_character(param1);
            if (c) {
                free(c->name);
                if (c->emotion) free(c->emotion);
                if (c->description) free(c->description);
                if (c->status) free(c->status);
                chars[char_count - 1] = *c;
                char_count--;
            }
        } else if (sscanf(line, "SET_CHAR_EMOTION \"%[^\"]\" \"%[^\"]\"", param1, param2) == 2) {
            Character *c = find_character(param1);
            if (c) {
                if (c->emotion) free(c->emotion);
                c->emotion = strdup(param2);
            }
        } else if (sscanf(line, "CHANGE_NAME \"%[^\"]\" \"%[^\"]\"", param1, param2) == 2) {
            Character *c = find_character(param1);
            if (c) {
                free(c->name);
                c->name = strdup(param2);
            }
        } else if (sscanf(line, "SET_CHAR_DESC \"%[^\"]\" \"%[^\"]\"", param1, param2) == 2) {
            Character *c = find_character(param1);
            if (c) {
                if (c->description) free(c->description);
                c->description = strdup(param2);
            }
        } else if (sscanf(line, "CHAR_STATUS \"%[^\"]\" \"%[^\"]\"", param1, param2) == 2) {
            Character *c = find_character(param1);
            if (c) {
                if (c->status) free(c->status);
                c->status = strdup(param2);
            }
        } else if (sscanf(line, "CHECK_STATUS \"%[^\"]\"", param1) == 1) {
            Character *c = find_character(param1);
            printf("%s status: %s\n", param1, c && c->status ? c->status : "None");
        }

        // Time & Day Management
        else if (sscanf(line, "SET_TIME \"%[^\"]\"", param1) == 1) {
            strncpy(current_time, param1, sizeof(current_time) - 1);
            current_time[sizeof(current_time) - 1] = '\0';
            printf("Time set to: %s\n", current_time);
        } else if (strncmp(line, "CHECK_TIME", 10) == 0) {
            printf("Current time: %s\n", current_time);
        }

        else {
            fprintf(stderr, "Unknown command: %s\n", line);
        }
    }

    // Cleanup
    for (int i = 0; i < label_count; i++) free(labels[i].label);
    for (int i = 0; i < var_count; i++) {
        free(vars[i].name);
        free(vars[i].value);
    }
    for (int i = 0; i < inv_count; i++) {
        for (int j = 0; j < invs[i].count; j++) free(invs[i].items[j]);
        free(invs[i].name);
    }
    for (int i = 0; i < char_count; i++) {
        free(chars[i].name);
        if (chars[i].emotion) free(chars[i].emotion);
        if (chars[i].description) free(chars[i].description);
        if (chars[i].status) free(chars[i].status);
    }
    for (int i = 0; i < scene_count; i++) {
        free(scenes[i].name);
        if (scenes[i].background) free(scenes[i].background);
    }
    fclose(file);
}