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
#define MAX_ITEMS 50

typedef struct {
    char *label;
    long position;
} LabelMap;

typedef struct {
    char *name;
    int value;
} Variable;

typedef struct {
    char *items[MAX_ITEMS];
    int count;
} Inventory;

static LabelMap labels[MAX_LABELS];
static int label_count = 0;
static Variable vars[100];
static int var_count = 0;
static Inventory inv;
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

static void init_inventory() {
    inv.count = 0;
}

static void add_item(const char *item) {
    if (inv.count < MAX_ITEMS) {
        inv.items[inv.count++] = strdup(item);
    }
}

static void remove_item(const char *item) {
    for (int i = 0; i < inv.count; i++) {
        if (!strcmp(inv.items[i], item)) {
            free(inv.items[i]);
            inv.items[i] = inv.items[inv.count - 1];
            inv.count--;
            break;
        }
    }
}

static int has_item(const char *item) {
    for (int i = 0; i < inv.count; i++) {
        if (!strcmp(inv.items[i], item)) return 1;
    }
    return 0;
}

static void portable_sleep(int seconds) {
#ifdef _WIN32
    Sleep(seconds * 1000); // Windows uses milliseconds
#else
    sleep(seconds); // Unix uses seconds
#endif
}

void run_interpreter(const char *target_filename) {
    FILE *file = fopen(target_filename, "r");
    if (!file) {
        perror("Error opening target file");
        exit(1);
    }

    init_inventory();
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
        
            // Parse choices, skipping "PROMPT "
            char *token = strtok(choices + 7, ",");
            while (token && choice_count < 10) {
                trim(token);
                // Remove quotes
                if (token[0] == '"') token++;
                int len = strlen(token);
                if (len > 0 && token[len - 1] == '"') token[len - 1] = '\0';
                choice_tokens[choice_count] = strdup(token);
                token = strtok(NULL, ",");
                choice_count++;
            }
        
            // Parse labels
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
            char text[MAX_LINE_LENGTH];
            sscanf(line, "CONFIRM \"%[^\"]\"", text);
            printf("%s (y/n): ", text);
            char response;
            scanf(" %c", &response);
            if (response == 'y' || response == 'Y') {
                // Continue
            } else {
                fgets(line, MAX_LINE_LENGTH, file); // Skip next line
            }
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
            set_var(var, rand() % atoi(range_str));
        } else if (strncmp(line, "COMBINE", 7) == 0) {
            char str1[MAX_LINE_LENGTH], str2[MAX_LINE_LENGTH];
            sscanf(line, "COMBINE \"%[^\"]\" \"%[^\"]\"", str1, str2);
            char result[MAX_LINE_LENGTH * 2];
            snprintf(result, sizeof(result), "%s%s", str1, str2);
            printf("Combined: %s\n", result);
        } else if (strncmp(line, "LENGTH", 6) == 0) {
            char str[MAX_LINE_LENGTH];
            sscanf(line, "LENGTH \"%[^\"]\"", str);
            printf("Length: %lu\n", strlen(str));
        } else if (strncmp(line, "SUBSTRING_IN", 12) == 0) {
            char str[MAX_LINE_LENGTH];
            sscanf(line, "SUBSTRING_IN \"%[^\"]\"", str);
            printf("Substring check not fully implemented\n");
        } else if (strncmp(line, "UPPERCASE", 9) == 0) {
            char str[MAX_LINE_LENGTH];
            sscanf(line, "UPPERCASE \"%[^\"]\"", str);
            for (int i = 0; str[i]; i++) str[i] = toupper(str[i]);
            printf("%s\n", str);
        } else if (strncmp(line, "LOWERCASE", 9) == 0) {
            char str[MAX_LINE_LENGTH];
            sscanf(line, "LOWERCASE \"%[^\"]\"", str);
            for (int i = 0; str[i]; i++) str[i] = tolower(str[i]);
            printf("%s\n", str);
        } else if (strncmp(line, "FORMAT_TEXT", 11) == 0) {
            char fmt[MAX_LINE_LENGTH], val[MAX_LINE_LENGTH];
            sscanf(line, "FORMAT_TEXT \"%[^\"]\" \"%[^\"]\"", fmt, val);
            printf(fmt, val);
            printf("\n");
        } else if (strncmp(line, "CREATE_INV", 10) == 0) {
            char var[MAX_LINE_LENGTH];
            sscanf(line, "CREATE_INV %s", var);
            init_inventory();
        } else if (strncmp(line, "ADD_ITEM", 8) == 0) {
            char item[MAX_LINE_LENGTH];
            sscanf(line, "ADD_ITEM \"%[^\"]\"", item);
            add_item(item);
        } else if (strncmp(line, "REMOVE_ITEM", 11) == 0) {
            char item[MAX_LINE_LENGTH];
            sscanf(line, "REMOVE_ITEM \"%[^\"]\"", item);
            remove_item(item);
        } else if (strncmp(line, "HAS_ITEM", 8) == 0) {
            char item[MAX_LINE_LENGTH];
            sscanf(line, "HAS_ITEM \"%[^\"]\"", item);
            printf("%s: %s\n", item, has_item(item) ? "Yes" : "No");
        } else if (strncmp(line, "COUNT_INV", 9) == 0) {
            printf("Inventory count: %d\n", inv.count);
        } else if (strncmp(line, "CLEAR_INV", 9) == 0) {
            while (inv.count > 0) remove_item(inv.items[0]);
        } else if (strncmp(line, "SHOW_INV", 8) == 0) {
            printf("Inventory: ");
            for (int i = 0; i < inv.count; i++) printf("%s%s", inv.items[i], i < inv.count - 1 ? ", " : "");
            printf("\n");
        } else if (strncmp(line, "SET_TIME", 8) == 0) {
            sscanf(line, "SET_TIME \"%[^\"]\"", current_time);
            printf("Time set to: %s\n", current_time);
        } else if (strncmp(line, "CHECK_TIME", 10) == 0) {
            printf("Current time: %s\n", current_time);
        }
    }
    fclose(file);
}