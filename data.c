
#include "data.h"
#include "first_run.h"
#include "general_utils.h"
#include <stdlib.h>


/* the heads of the symbol table: */
table data_table_head;
table code_table_head;
table entry_table_head;
table extern_table_head;
table refer_to_an_external_symbol_head;
table general_symbols_table_head;

void initialize_symbol_table_heads() {
    /* initializing all the heads to NULL */
    data_table_head = NULL;
    code_table_head = NULL;
    entry_table_head = NULL;
    extern_table_head = NULL;
    refer_to_an_external_symbol_head = NULL;
    general_symbols_table_head = NULL;
}

void allocate_memory_instruction_table(int index) {
    instruction_table[index] = (word_to_insert *) calloc(1, sizeof(word_to_insert)); /* allocate the memory */
    if (!instruction_table[index]) { /* check if the allocation failed */
        print_error_only_message("Memory allocation failed");
        exit(1);
    }
}

void allocate_memory_data_table(int index) {
    data_table[index] = (word_to_insert *) calloc(1, sizeof(word_to_insert)); /* allocate the memory */
    if (!data_table[index]) { /* check if the allocation failed */
        print_error_only_message("Memory allocation failed");
        exit(1);
    }
}

void allocate_memory_symbol_table(table *new_node, char *label_name) {
    /* allocate the new memory */
    (*new_node) = (table) malloc(sizeof(node));
    if (!(*new_node)) {
        printf("Memory allocation failed for the new node of a symbol in the symbol table");
        exit(1);
    }
    /* allocate the memory for the label name's length exactly + 1 for the \0 */
    (*new_node)->label_name = (char *) malloc(strlen(label_name) + 1);
    if (!((*new_node)->label_name)) {
        printf("Memory allocation failed for the symbol's name's length");
        exit(1);
    }
}


int add_to_table(table *head, char *label_name, int label_value, int is_extern, int is_entry) {
    table new_node, p;
    p = *head;

    allocate_memory_symbol_table(&new_node, label_name);

    /* the label is already on the list */
    if (check_if_label_is_in_any_of_the_tables(head, label_name) == ERROR)
        return ERROR;


    strcpy(new_node->label_name, label_name); /* Copying the symbols attributes */
    new_node->value = label_value;
    new_node->next = NULL;
    if (is_extern == TRUE) {
        new_node->is_extern = TRUE; /* 1 */
    } else { new_node->is_extern = FALSE; /* 0 */ }
    if (is_entry == TRUE) {
        new_node->is_entry = TRUE; /* 1 */
    } else { new_node->is_entry = FALSE; /* 0 */ }

    /*self check*/
    if ((is_entry == TRUE) & (is_extern == TRUE)) {
        printf("\nEXTERN AND ENTERN BOTH TOGETHER - %s\n", label_name);
    }

    /* moving head to a new empty node at the end of the list: */
    if (!(*head)) {
        /* If there's no head - create a new one*/
        (*head) = new_node;
        return OK;
    }
        /* If the head is not NULL move on to the last node (it's the empty one to be written over) */
    else {
        while (p->next) { /* moving to the last node */
            p = p->next;
        }
    }
    /* adding the new symbol node to the end of the list */
    p->next = new_node;
    /* finished adding the new symbol node to the linked list table */
    return OK;
}

int add_to_table_with_no_check(table *head, char *label_name, int label_value, int is_extern, int is_entry) {
    table new_node, p;
    p = *head;

    allocate_memory_symbol_table(&new_node, label_name);

    strcpy(new_node->label_name, label_name); /* Copying the symbols attributes */
    new_node->value = label_value;
    new_node->next = NULL;
    if (is_extern == TRUE) {
        new_node->is_extern = TRUE; /* 1 */
    } else { new_node->is_extern = FALSE; /* 0 */ }
    if (is_entry == TRUE) {
        new_node->is_entry = TRUE; /* 1 */
    } else { new_node->is_entry = FALSE; /* 0 */ }

    /*self check*/
    if ((is_entry == TRUE) & (is_extern == TRUE)) {
        printf("\nEXTERN AND ENTERN BOTH TOGETHER - %s\n", label_name);
    }

    /* moving head to a new empty node at the end of the list: */
    if (!(*head)) {
        /* If there's no head - create a new one*/
        (*head) = new_node;
        return OK;
    }
        /* If the head is not NULL move on to the last node (it's the empty one to be written over) */
    else {
        while (p->next) { /* moving to the last node */
            p = p->next;
        }
    }
    /* adding the new symbol node to the end of the list */
    p->next = new_node;
    /* finished adding the new symbol node to the linked list table */
    return OK;
}


int is_on_list(table head, char *label_name) {
    /* going over the linked_list from the head to the last node and making sure the label_name is on the list */
    while (head) {
        /* TODO: REMOVING !STRCMP and replaceing TRUE FALSE*/
        if (!strcmp(head->label_name, label_name)) /* the name is on the list */
            return TRUE;
        head = head->next;
    }
    return FALSE;
}

int check_if_label_is_in_any_of_the_tables(table *head_pointer_to_the_tables, char *label_name) {

    /* check if the label is in the data or code tables */
    int on_the_tables_already = is_on_list(data_table_head, label_name) ||
                                is_on_list(code_table_head, label_name);
    if (on_the_tables_already) {
        return print_error("ERROR - Multiple definitions of the same label");
    }
    if (head_pointer_to_the_tables == &extern_table_head && is_on_list(entry_table_head, label_name)) {
        /* It is forbidden to declare a label in both entry and extern */
        return print_error("ERROR - %s in both extern and entry", label_name);
    }

    on_the_tables_already = is_on_list(entry_table_head, label_name) ||
                            is_on_list(extern_table_head, label_name);
    if (on_the_tables_already) {
        printf("\nInfo: %s is on entry OR entry tables\n", label_name);
    }

    return OK;
}


void increase_address_to_data() {
    table p = data_table_head;
    while (p) { /* Scanning the symbol_data_table */
        p->value += ICF; /* increase the value */
        p = p->next;
    }
}

void add_symbols_from_data_table_to_symbols_table() {
    int is_entry = 0;
    int is_extern = 0;
    table p = data_table_head;
    while (p) { /* Scanning the symbol_data_table */
        if (is_on_list(entry_table_head, p->label_name)) {
            is_entry = 1;
        }
        if (is_on_list(extern_table_head, p->label_name)) {
            is_extern = 1;
        }
        add_to_table_with_no_check(&general_symbols_table_head, p->label_name, p->value + ICF, is_extern,
                                       is_entry);
        p = p->next;
        is_entry = 0;
        is_extern = 0;
    }
}


void insert_number_to_data_table(int num) {
    allocate_memory_data_table(DC); /* allocating memory for the generic_word */
    data_table[DC]->word.data = num; /* inserting the number to the data table */
    DC++;
}

void insert_numbers_to_data_table(char *numbers) {
    char *temp; /* just for using strtol */
    char *token;
    token = strtok(numbers, ",");

    while (token != NULL) { /* Go through all the numbers */
        if (token[0] == '+')
            token++;
        insert_number_to_data_table(strtol(token, &temp, 10)); /* insert each number to the table */
        token = strtok(NULL, ","); /* passing to the nest number */
    }
}

void insert_string_to_data_table(char *s) {
    int i;
    for (i = 1; s[i + 1]; i++) { /* scan the string without the first and last char (they are " ) */
        insert_number_to_data_table(s[i]); /* insert all the chars to the data table */
    }
    insert_number_to_data_table('\0'); /* the last char in a string is '\0' and we need to enter him as well */
}

/* char is a string of data of the .struct representing an integer and a string */
void insert_struct_to_data_table(char *s) {
    /* by page 25 .struct will always have 2 fields - 1. integer, 2. string, encoded same as .data and .string*/
    char *the_integer, *the_string;
    /*getting the integer seperated from the .struct object */
    the_integer = strtok(s, ",");
    the_string = strtok(NULL, "");
    /* maybe remove empty spaces before and after the data */
    /* make sure the first argument is a number */
    if (the_integer != NULL) {
        if (!is_valid_digit(the_integer + 1)) {
            printf("Invalid number in immediate addressing method");
        }
        /* inserting the integer to the data table */
        insert_numbers_to_data_table(the_integer);
    }
    if (the_string != NULL) {
        /* make sure the second  argument is a string */
        insert_string_to_data_table(the_string);
    }


}


int is_symbol_name_equals_to_saved_names(char *list_of_definitions[], int list_size, char *symbol_name) {
    int i;
    /* This function goes in a for loop through each word in the list and checks if the symbol name == array[i]*/
    for (i = 0; i < list_size; i++) {
        if (!strcmp(symbol_name, list_of_definitions[i]))
            return TRUE;
    }
    return FALSE; /* the symbol_name is not un the array */
}



int is_valid_ref_to_address(char *name) {
    /* The first character is a letter */
    if (!isalpha(name[0]))
        return FALSE;
    /* The maximum length of a symbol is 30 */
    if (strlen(name) > SYMBOL_LEN)
        return FALSE;

    /* The symbol cannot be an operation, or a register or a declared data generic_word */
    if (is_symbol_name_equals_to_saved_names(operations_names, NUM_OPS, name))
        return FALSE;
    if (is_symbol_name_equals_to_saved_names(data_string_struct_entry_extern_declarations, DATA_WORDS_DECLARATIONS,
                                             name))
        return FALSE;
    if (is_symbol_name_equals_to_saved_names(registers_names, NUM_REGISTERS, name))
        return FALSE;
    /* The symbol is valid */
    return TRUE;
}