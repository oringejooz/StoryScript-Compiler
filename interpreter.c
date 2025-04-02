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

static LabelMap labels[MAX_LABELS];
static int label_count = 0;
static Variable vars[100];
static int var_count = 0;
static Inventory invs[10];
static int inv_count = 0;
static Character chars[MAX_CHARS];
static int char_count = 0;
static char current_time[10] = "00:00";

static void trim(char *str) {
    char *start = str;
    while (*start && isspace(*start)) start++;
    char *end = str + strlen(str) - 1;
    while (end >= start && isspace(*end)) *end-- = '\0';
    memmove(str, start, end - start + 2);
}

static void register_label(const char *label, long pos) {
    if (label_count < MAX_LABELS) {
        labels[label_count].label = strdup(label);
        labels[label_count].position = pos;
        label_count++;
    }
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

static void set_var(const char *name, int value) {
    Variable *v = find_var(name);
    if (!v && var_count < 100) {
        vars[var_count].name = strdup(name);
        vars[var_count].value = value;
        var_count++;
    } else if (v) {
        v->value = value;
    }
}

static int get_var(const char *name) {
    Variable *v = find_var(name);
    return v ? v->value : 0;
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

static void portable_sleep(int seconds) {
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

void run_interpreter(const char *target_filename) {
    srand(time(NULL)); // Seed random number generator
    FILE *file = fopen(target_filename, "r");
    if (!file) {
        perror("Error opening target file");
        exit(1);
    }

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

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        trim(line);
        if (!strlen(line)) continue;

        if (strncmp(line, "LOAD", 4) == 0) {
            char title[MAX_LINE_LENGTH];
            sscanf(line, "LOAD \"%[^\"]\"", title);
            printf("\n=== %s ===\n", title);
        } else if (strncmp(line, "SAY", 3) == 0) {
            char text[MAX_LINE_LENGTH];
            sscanf(line, "SAY \"%[^\"]\"", text);
            printf("\"%s\"\n", text);
        } else if (strncmp(line, "PRINT", 5) == 0) {
            char text[MAX_LINE_LENGTH];
            sscanf(line, "PRINT \"%[^\"]\"", text);
            printf("%s\n", text);
        } else if (strncmp(line, "PAUSE", 5) == 0) {
            int seconds;
            sscanf(line, "PAUSE %d", &seconds);
            portable_sleep(seconds);
        } else if (strncmp(line, "PROMPT", 6) == 0) {
            char choices[MAX_LINE_LENGTH], labels[MAX_LINE_LENGTH];
            sscanf(line, "PROMPT %[^->] -> [%[^]]", choices, labels);
            char *choice_tokens[10], *label_tokens[10];
            int choice_count = 0;

            char *token = strtok(choices + 7, ",");
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
        } else if (strncmp(line, "INPUT", 5) == 0) {
            char var[MAX_LINE_LENGTH];
            sscanf(line, "INPUT %s", var);
            printf("Enter value for %s: ", var);
            int val;
            scanf("%d", &val);
            set_var(var, val);
        } else if (strncmp(line, "CONFIRM", 7) == 0) {
            char text[MAX_LINE_LENGTH], true_label[MAX_LINE_LENGTH], false_label[MAX_LINE_LENGTH];
            sscanf(line, "CONFIRM \"%[^\"]\" -> [%[^,], %[^]]", text, true_label, false_label);
            printf("%s (y/n): ", text);
            char response;
            scanf(" %c", &response);
            if (response == 'y' || response == 'Y') {
                fseek(file, get_label_position(true_label), SEEK_SET);
            } else {
                fseek(file, get_label_position(false_label), SEEK_SET);
            }
        } else if (strncmp(line, "WAIT_FOR_KEY", 12) == 0) {
            printf("Press Enter to continue...\n");
            getchar(); // Clear buffer
            getchar();
        } else if (strncmp(line, "JUMP", 4) == 0) {
            char label[MAX_LINE_LENGTH];
            sscanf(line, "JUMP %s", label);
            fseek(file, get_label_position(label), SEEK_SET);
        } else if (strncmp(line, "HALT", 4) == 0) {
            printf("The story ends here.\n");
            break;
        } else if (strncmp(line, "RETURN", 6) == 0) {
            // Placeholder for nested calls
        } else if (strncmp(line, "RESTART", 7) == 0) {
            rewind(file);
        } else if (strncmp(line, "IF", 2) == 0) {
            char cond[MAX_LINE_LENGTH], label[MAX_LINE_LENGTH];
            sscanf(line, "IF %s -> %s", cond, label);
            if (get_var(cond)) fseek(file, get_label_position(label), SEEK_SET);
        } else if (strncmp(line, "IFELSE", 6) == 0) {
            char cond[MAX_LINE_LENGTH], true_label[MAX_LINE_LENGTH], false_label[MAX_LINE_LENGTH];
            sscanf(line, "IFELSE %s -> [%[^,], %[^]]", cond, true_label, false_label);
            fseek(file, get_label_position(get_var(cond) ? true_label : false_label), SEEK_SET);
        } else if (strncmp(line, "SWITCH", 6) == 0) {
            char var[MAX_LINE_LENGTH], cases_labels[MAX_LINE_LENGTH];
            sscanf(line, "SWITCH %s -> [%[^]]", var, cases_labels);
            char *case_tokens[10], *label_tokens[10];
            int case_count = 0;
            int value = get_var(var);

            char *token = strtok(cases_labels, ",");
            while (token && case_count < 10) {
                char case_val[MAX_LINE_LENGTH], label[MAX_LINE_LENGTH];
                sscanf(token, "%[^:]:%s", case_val, label);
                trim(case_val);
                trim(label);
                case_tokens[case_count] = strdup(case_val);
                label_tokens[case_count++] = strdup(label);
                token = strtok(NULL, ",");
            }

            for (int i = 0; i < case_count; i++) {
                if (!strcmp(case_tokens[i], "default") || atoi(case_tokens[i]) == value) {
                    fseek(file, get_label_position(label_tokens[i]), SEEK_SET);
                    break;
                }
            }

            for (int i = 0; i < case_count; i++) {
                free(case_tokens[i]);
                free(label_tokens[i]);
            }
        } else if (strncmp(line, "ASSIGN", 6) == 0) {
            char var[MAX_LINE_LENGTH], val_str[MAX_LINE_LENGTH];
            sscanf(line, "ASSIGN %s \"%[^\"]\"", var, val_str);
            set_var(var, atoi(val_str));
        } else if (strncmp(line, "INCREASE", 8) == 0) {
            char var[MAX_LINE_LENGTH], val_str[MAX_LINE_LENGTH];
            sscanf(line, "INCREASE %s %s", var, val_str);
            set_var(var, get_var(var) + atoi(val_str));
        } else if (strncmp(line, "DECREASE", 8) == 0) {
            char var[MAX_LINE_LENGTH], val_str[MAX_LINE_LENGTH];
            sscanf(line, "DECREASE %s %s", var, val_str);
            set_var(var, get_var(var) - atoi(val_str));
        } else if (strncmp(line, "SCALE", 5) == 0) {
            char var[MAX_LINE_LENGTH], val_str[MAX_LINE_LENGTH];
            sscanf(line, "SCALE %s %s", var, val_str);
            set_var(var, get_var(var) * atoi(val_str));
        } else if (strncmp(line, "DIVIDE_BY", 9) == 0) {
            char var[MAX_LINE_LENGTH], val_str[MAX_LINE_LENGTH];
            sscanf(line, "DIVIDE_BY %s %s", var, val_str);
            int divisor = atoi(val_str);
            if (divisor) set_var(var, get_var(var) / divisor);
        } else if (strncmp(line, "RANDOMIZE", 9) == 0) {
            char var[MAX_LINE_LENGTH], range_str[MAX_LINE_LENGTH];
            sscanf(line, "RANDOMIZE %s %s", var, range_str);
            int range = atoi(range_str);
            if (range > 0) set_var(var, rand() % range);
        } else if (strncmp(line, "BOTH", 4) == 0) {
            char var1[MAX_LINE_LENGTH], var2[MAX_LINE_LENGTH];
            sscanf(line, "BOTH %s %s", var1, var2);
            set_var(var1, get_var(var1) && get_var(var2));
        } else if (strncmp(line, "EITHER", 6) == 0) {
            char var1[MAX_LINE_LENGTH], var2[MAX_LINE_LENGTH];
            sscanf(line, "EITHER %s %s", var1, var2);
            set_var(var1, get_var(var1) || get_var(var2));
        } else if (strncmp(line, "INVERT", 6) == 0) {
            char var[MAX_LINE_LENGTH];
            sscanf(line, "INVERT %s", var);
            set_var(var, !get_var(var));
        } else if (strncmp(line, "COMBINE", 7) == 0) {
            char var[MAX_LINE_LENGTH], str1[MAX_LINE_LENGTH], str2[MAX_LINE_LENGTH];
            sscanf(line, "COMBINE %s \"%[^\"]\" \"%[^\"]\"", var, str1, str2);
            char result[MAX_LINE_LENGTH * 2];
            snprintf(result, sizeof(result), "%s%s", str1, str2);
            set_var(var, atoi(result)); // Numeric for simplicity
        } else if (strncmp(line, "LENGTH", 6) == 0) {
            char var[MAX_LINE_LENGTH], str[MAX_LINE_LENGTH];
            sscanf(line, "LENGTH %s \"%[^\"]\"", var, str);
            set_var(var, strlen(str));
        } else if (strncmp(line, "SUBSTRING_IN", 12) == 0) {
            char var[MAX_LINE_LENGTH], substr[MAX_LINE_LENGTH];
            sscanf(line, "SUBSTRING_IN %s \"%[^\"]\"", var, substr);
            set_var(var, strstr(var, substr) != NULL);
        } else if (strncmp(line, "UPPERCASE", 9) == 0) {
            char var[MAX_LINE_LENGTH], str[MAX_LINE_LENGTH];
            sscanf(line, "UPPERCASE %s \"%[^\"]\"", var, str);
            for (int i = 0; str[i]; i++) str[i] = toupper(str[i]);
            set_var(var, atoi(str)); // Numeric for simplicity
        } else if (strncmp(line, "LOWERCASE", 9) == 0) {
            char var[MAX_LINE_LENGTH], str[MAX_LINE_LENGTH];
            sscanf(line, "LOWERCASE %s \"%[^\"]\"", var, str);
            for (int i = 0; str[i]; i++) str[i] = tolower(str[i]);
            set_var(var, atoi(str)); // Numeric for simplicity
        } else if (strncmp(line, "FORMAT_TEXT", 11) == 0) {
            char var[MAX_LINE_LENGTH], fmt[MAX_LINE_LENGTH];
            sscanf(line, "FORMAT_TEXT %s \"%[^\"]\"", var, fmt);
            printf("%s\n", fmt); // Simple output for now
        } else if (strncmp(line, "CREATE_INV", 10) == 0) {
            char name[MAX_LINE_LENGTH];
            sscanf(line, "CREATE_INV %s", name);
            if (inv_count < 10) {
                invs[inv_count].name = strdup(name);
                invs[inv_count].count = 0;
                inv_count++;
            }
        } else if (strncmp(line, "ADD_ITEM", 8) == 0) {
            char inv_name[MAX_LINE_LENGTH], item[MAX_LINE_LENGTH];
            sscanf(line, "ADD_ITEM %s \"%[^\"]\"", inv_name, item);
            Inventory *inv = find_inventory(inv_name);
            if (inv && inv->count < MAX_ITEMS) {
                inv->items[inv->count++] = strdup(item);
            }
        } else if (strncmp(line, "REMOVE_ITEM", 11) == 0) {
            char inv_name[MAX_LINE_LENGTH], item[MAX_LINE_LENGTH];
            sscanf(line, "REMOVE_ITEM %s \"%[^\"]\"", inv_name, item);
            Inventory *inv = find_inventory(inv_name);
            if (inv) {
                for (int i = 0; i < inv->count; i++) {
                    if (!strcmp(inv->items[i], item)) {
                        free(inv->items[i]);
                        inv->items[i] = inv->items[--inv->count];
                        break;
                    }
                }
            }
        } else if (strncmp(line, "HAS_ITEM", 8) == 0) {
            char inv_name[MAX_LINE_LENGTH], item[MAX_LINE_LENGTH];
            sscanf(line, "HAS_ITEM %s \"%[^\"]\"", inv_name, item);
            Inventory *inv = find_inventory(inv_name);
            int has = 0;
            if (inv) {
                for (int i = 0; i < inv->count; i++) {
                    if (!strcmp(inv->items[i], item)) {
                        has = 1;
                        break;
                    }
                }
            }
            set_var(inv_name, has);
        } else if (strncmp(line, "COUNT_INV", 9) == 0) {
            char inv_name[MAX_LINE_LENGTH];
            sscanf(line, "COUNT_INV %s", inv_name);
            Inventory *inv = find_inventory(inv_name);
            set_var(inv_name, inv ? inv->count : 0);
        } else if (strncmp(line, "CLEAR_INV", 9) == 0) {
            char inv_name[MAX_LINE_LENGTH];
            sscanf(line, "CLEAR_INV %s", inv_name);
            Inventory *inv = find_inventory(inv_name);
            if (inv) {
                while (inv->count > 0) free(inv->items[--inv->count]);
            }
        } else if (strncmp(line, "SHOW_INV", 8) == 0) {
            char inv_name[MAX_LINE_LENGTH];
            sscanf(line, "SHOW_INV %s", inv_name);
            Inventory *inv = find_inventory(inv_name);
            if (inv) {
                printf("Inventory %s: ", inv_name);
                for (int i = 0; i < inv->count; i++) {
                    printf("%s%s", inv->items[i], i < inv->count - 1 ? ", " : "");
                }
                printf("\n");
            }
        } else if (strncmp(line, "CREATE_SCENE", 12) == 0) {
            char name[MAX_LINE_LENGTH];
            sscanf(line, "CREATE_SCENE %s", name);
            // Placeholder for scene creation
        } else if (strncmp(line, "RANDOM_EVENT", 12) == 0) {
            printf("A random event occurs!\n");
        } else if (strncmp(line, "SET_BACKGROUND", 14) == 0) {
            char name[MAX_LINE_LENGTH], bg[MAX_LINE_LENGTH];
            sscanf(line, "SET_BACKGROUND %s \"%[^\"]\"", name, bg);
            printf("Background set to: %s\n", bg);
        } else if (strncmp(line, "TRIGGER_SCENE", 13) == 0) {
            char name[MAX_LINE_LENGTH];
            sscanf(line, "TRIGGER_SCENE %s", name);
            printf("Scene %s triggered.\n", name);
        } else if (strncmp(line, "ADD_CHAR", 8) == 0) {
            char name[MAX_LINE_LENGTH];
            sscanf(line, "ADD_CHAR \"%[^\"]\"", name);
            if (char_count < MAX_CHARS) {
                chars[char_count].name = strdup(name);
                chars[char_count].emotion = NULL;
                chars[char_count].description = NULL;
                chars[char_count].status = NULL;
                char_count++;
            }
        } else if (strncmp(line, "REMOVE_CHAR", 11) == 0) {
            char name[MAX_LINE_LENGTH];
            sscanf(line, "REMOVE_CHAR \"%[^\"]\"", name);
            Character *c = find_character(name);
            if (c) {
                free(c->name);
                if (c->emotion) free(c->emotion);
                if (c->description) free(c->description);
                if (c->status) free(c->status);
                chars[char_count - 1] = *c;
                char_count--;
            }
        } else if (strncmp(line, "SET_CHAR_EMOTION", 16) == 0) {
            char name[MAX_LINE_LENGTH], emotion[MAX_LINE_LENGTH];
            sscanf(line, "SET_CHAR_EMOTION \"%[^\"]\" \"%[^\"]\"", name, emotion);
            Character *c = find_character(name);
            if (c) {
                if (c->emotion) free(c->emotion);
                c->emotion = strdup(emotion);
            }
        } else if (strncmp(line, "CHANGE_NAME", 11) == 0) {
            char old_name[MAX_LINE_LENGTH], new_name[MAX_LINE_LENGTH];
            sscanf(line, "CHANGE_NAME \"%[^\"]\" \"%[^\"]\"", old_name, new_name);
            Character *c = find_character(old_name);
            if (c) {
                free(c->name);
                c->name = strdup(new_name);
            }
        } else if (strncmp(line, "SET_CHAR_DESC", 13) == 0) {
            char name[MAX_LINE_LENGTH], desc[MAX_LINE_LENGTH];
            sscanf(line, "SET_CHAR_DESC \"%[^\"]\" \"%[^\"]\"", name, desc);
            Character *c = find_character(name);
            if (c) {
                if (c->description) free(c->description);
                c->description = strdup(desc);
            }
        } else if (strncmp(line, "CHAR_STATUS", 11) == 0) {
            char name[MAX_LINE_LENGTH], status[MAX_LINE_LENGTH];
            sscanf(line, "CHAR_STATUS \"%[^\"]\" \"%[^\"]\"", name, status);
            Character *c = find_character(name);
            if (c) {
                if (c->status) free(c->status);
                c->status = strdup(status);
            }
        } else if (strncmp(line, "CHECK_STATUS", 12) == 0) {
            char name[MAX_LINE_LENGTH];
            sscanf(line, "CHECK_STATUS \"%[^\"]\"", name);
            Character *c = find_character(name);
            printf("%s status: %s\n", name, c && c->status ? c->status : "None");
        } else if (strncmp(line, "SET_TIME", 8) == 0) {
            sscanf(line, "SET_TIME \"%[^\"]\"", current_time);
            printf("Time set to: %s\n", current_time);
        } else if (strncmp(line, "CHECK_TIME", 10) == 0) {
            printf("Current time: %s\n", current_time);
        }
    }

    // Cleanup
    for (int i = 0; i < label_count; i++) free(labels[i].label);
    for (int i = 0; i < var_count; i++) free(vars[i].name);
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
    fclose(file);
}