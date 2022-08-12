#include "macro_handler.h"
#include "general_utils.h"


char *operations_names[NUM_OPS] = {"mov", "cmp", "add", "sub", "not", "clr", "lea",
                                   "inc", "dec", "jmp", "bne", "get",
                                   "prn", "jsr", "rts", "hlt"};

/* data handler - receives the file itself and a string representing the name of the file*/
void macro_handler(FILE *ptr_to_asm_file, char *name_of_file) {
    FILE *file_without_macros;
        /* Declaring 3 data */
    Macro *new_macro, *macroToApply, *ptr_to_macro_link;
    char line[MAX_LINE_LENGTH], trimmed_macro_name[MACRO_NAME_LENGTH];
    char *find_macro_declaration_string, *name, *macro_data_collector, *the_macro_name;
    int macro_flag, macro_lines_counter, its_the_first_macro;
    macro_flag = 0,macro_lines_counter = 1, its_the_first_macro=1;
    the_macro_name = (char *) malloc(sizeof(MACRO_NAME_LENGTH));

    /* TODO: MAKE SURE FOR EACH LINE IT DOESN'T PASS THE 80 CHARACTERS */



    new_macro = NULL;
    macroToApply = NULL;
    ptr_to_macro_link = NULL;
    file_without_macros = fopen(strcat(strtok(name_of_file, SEPARATOR), AM_SUFFIX), "w");

    /* searching file for macros and saving a processed file*/
    while ((fgets(line, MAX_LINE_LENGTH, ptr_to_asm_file) != NULL)) {
        /*TODO: FIND A WAY TO CHECK IF THE LINE IS 80+ CHARACTERS - THROW AN ERROR*/
        /*The flag is turned off at the beginning of the file reading.
         * It turns on if it finds a line declaring a data*/
        if (!macro_flag) {
            /*search for data declaration string in a line*/
            find_macro_declaration_string = strstr(line, START_OF_MACRO);

            /* if the current line is not a declaration of a new macro_data_collector - check if the current line is a known macro_data_collector*/
            if (!find_macro_declaration_string) {
                if ((macroToApply = is_valid_macro(line, ptr_to_macro_link)))
                    fputs(macroToApply->data, file_without_macros);
                else
                    fputs(line, file_without_macros);
            } else {
                /* turn on macro_data_collector flag */
                macro_flag = 1;
                macro_data_collector = (char *) calloc(MAX_LINE_LENGTH, sizeof(char));

                /*strtok splits the 'macro_data_collector' declaration from the macro_data_collector name */
                strcpy(the_macro_name, strtok(line, " "));
                /* strtok with NULL chooses the second word after the word 'macro_data_collector' on the same line which
                 * represents the macro_data_collector's name*/
                strcpy(the_macro_name, strtok(NULL, " "));

                if (the_macro_name == NULL) {
                    printf("\nERROR: There was a data declaration without a name - can't use it\n");
                    continue;
                }
                /*removing \r\n from the name of the data*/
                remove_white_spaces(trimmed_macro_name, MACRO_NAME_LENGTH, the_macro_name);



                /*TODO: BEFORE ADDING THE MACRO TO THE MACRO_LIST - MAKE SURE IT DOES NOT EXIST IN THE LIST ALREADY
                 * IF SO - THROW AN ERROR AND END THE PROGRAM (REMEMBER TO COMMENT AND MENTION IT)
                 * ALSO CHECK HERE IF THE MACRO IS NOT USING AN OPERATION NAME INSTEAD IN THE
                 * IS_VALID_MACRO(...) FUNCTION*/



                /* creating a new data*/
                new_macro = (Macro *) malloc(sizeof(Macro));
                /* saving the data name into the data struct */
                name = (char *) malloc(sizeof(trimmed_macro_name));
                strcpy(name, trimmed_macro_name);
                new_macro->name = name;

                if (its_the_first_macro) {
                    ptr_to_macro_link = new_macro;
                    its_the_first_macro = 0;
                } else
                    addToMacroList(new_macro, ptr_to_macro_link);
            }
        } else {
            /*search for endmacro string in a line*/
            find_macro_declaration_string = strstr(line, END_OF_MACRO);
            if (!find_macro_declaration_string) {
                macro_data_collector = (char *) realloc(macro_data_collector, MAX_LINE_LENGTH * macro_lines_counter);
                strcat(macro_data_collector, line);
                macro_lines_counter++;
            } else {
                macro_flag = 0;
                new_macro->data = macro_data_collector;
                new_macro->next = NULL;
                macro_lines_counter = 1;
            }
        }
    }
    fclose(file_without_macros);
    free_all_macros_linked_list(ptr_to_macro_link);
}

Macro *is_valid_macro(char *macro_name, Macro *head) {
    int i;
    char word_to_cmp[MACRO_NAME_LENGTH];
    /*checking that the data doesn't exist in the macros' list yet*/
    while (head) {
        /* clearing white spaces before comparing the string to the saved macros*/
        remove_white_spaces(word_to_cmp, MACRO_NAME_LENGTH, macro_name);
        if (strcmp(word_to_cmp, "\0") == 0) {
            head = head->next;
            continue;
        }
        if (strcmp(word_to_cmp, head->name) == 0)
            return head;
        head = head->next;
    }

    /* Turning macro_name into lowercase in order to compare with all the operations names*/
    for (; *word_to_cmp; ++*word_to_cmp) *word_to_cmp = tolower(*word_to_cmp);
    /* checking that the data isn't using a command like name (mov, jmp, hlt...)*/
    for (i = 0; i < NUM_OPS; i++) {
        if (strcmp(macro_name, operations_names[i]) == 0) {
            printf("\nERROR: The data name %s is an operation name ! - can't use it\n", macro_name);
        }
    }
    return NULL;
}

void addToMacroList(Macro *newMacro, Macro *head) {
    while (head->next != NULL)
        head = head->next;

    head->next = newMacro;
}

void free_all_macros_linked_list(Macro *head) {
    if (!head)
        return;
    if (head->next)
        free_all_macros_linked_list(head->next);

    free(head);
}

