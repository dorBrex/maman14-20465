
#include "first_run.h"


/* the data and instruction linked-lists */
word_to_insert *data_table[MAX_INSTRUCTION_AND_DATA_TABLE_LEN];
word_to_insert *instruction_table[MAX_INSTRUCTION_AND_DATA_TABLE_LEN];
/* Instruction Counter and Data Counter */
int IC = 100, DC = 0;
/* Instruction Counter and Data Counter last value (At the end of the run) */
int ICF, DCF;

char *data_string_struct_entry_extern_declarations[DATA_WORDS_DECLARATIONS] = {".data", ".string", ".struct", ".entry",
                                                                               ".extern"};

char *registers_names[NUM_REGISTERS] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

int first_main_run(FILE *source_file) {
    /* A flag to check if any validation function returns error */
    int error_flag = FALSE;
    /* asm line holder */
    char line[MAX_LINE_LENGTH];
    /* Instructions counter starts from memory space 100, data counter starts from 0*/
    IC = 100, DC = 0;

    /* Each line in the .am file should end with an \n - this is how we move on to the next one */
    line[MAX_LINE_LENGTH - 2] = '\n';
    /* foes over the lines */
    for (line_number = 1; fgets(line, MAX_LINE_LENGTH, source_file); line_number++) {
        /* Validating the current line and adding its values to the relevant data tables */
        if (check_if_line_is_valid(line) == ERROR)
            /* If there was an error found in one of the lines - raise en error_flag */
            error_flag = TRUE;
    }

    /* updating the ICF, DCF to the final values of the IC and DC) */
    ICF = IC;
    DCF = DC;

    add_symbols_from_data_table_to_symbols_table();
    /* Appending the data table to the instructions table (to the end of it) */
    increase_address_to_data();
    /* Rewinds the file for the second run and raise the exceptions caught
     * The main idea is to pass over the file later on and adding the
     * missing addresses to the instruction table*/
    rewind(source_file);

    return error_flag;
}

int check_if_line_is_valid(char cur_line[]) {
    char *pointer_to_current_line = cur_line;
    char line_without_backslash_zero[MAX_LINE_LENGTH - 1];
    char comment_sign = ';';

    /* symbol_name - holds the name of the symbol of the current line */
    char symbol_name[SYMBOL_LEN];
    char operands_used[MAX_LINE_LENGTH];
    int operation_or_declaration_word;
    /* A flag to indicate if there's a symbol in this line */
    int symbol_flag = FALSE;

    /* if the current line doesn't end with \0 as it should be or doesn't have \n it means that the current line is too long*/
    if (cur_line[MAX_LINE_LENGTH - 2] != '\0' && cur_line[MAX_LINE_LENGTH - 2] != '\n') {
        cur_line[MAX_LINE_LENGTH - 2] = '\n';
        /* raise exception and error flag */
        return print_error("The length of the line is greater than 80"); /* print the error message */

    }
    /* if the current line is at a good length - moves on to the next validation checks before parsing it*/

    /* remove all commented out lines starting with ';' */
    if (*pointer_to_current_line == ';') {
        return OK;
    }
    /* remove white spaces before and after the text of the line*/
    pointer_to_current_line = skip_spaces(pointer_to_current_line);

    /* only spaces in the cur_line - skip line*/
    if (!(*pointer_to_current_line)) {
        return OK;
    }

    /* Remove the first character from the line to check if there's another ';' char in the middle of the line*/
    slice(cur_line, line_without_backslash_zero, 1, strlen(cur_line));

    /* Checks if there's an error in the middle of the cur_line */
    if (strchr(line_without_backslash_zero, comment_sign)) {
        return print_error("ERROR: a comment is announced using ';' at the middle of the cur_line");
    }
    /* if the symbol_name is not going to change - the '\0' will stay the same during the run over the line */
    symbol_name[0] = '\0';

    /*checks for symbols, operations, number of operands and that there are no errors in the syntax of the assembly */
    printf("STARTING TO PARSE LINE DATA - %s", pointer_to_current_line);
    if (parse_line_data(pointer_to_current_line, symbol_name, &operation_or_declaration_word, operands_used) == ERROR)
        return ERROR;

    /* if the symbol_name has changed from '\0' to a different string - turn on the flag in order to let the program know
     * there's been a symbol declared that should be saved */
    if (symbol_name[0])
        symbol_flag = TRUE; /* cur_line 3 in the algorithm */


    /* if there is a symbol of the .data or .string declaration - keep it in the data table also */
    if (operation_or_declaration_word == DATA || operation_or_declaration_word == STRING ||
        operation_or_declaration_word == STRUCT) {
        if (symbol_flag) { /* insert the symbol into the data symbol table */
            if (add_to_table(&data_table_head, symbol_name, DC, FALSE, FALSE) == ERROR) {
                printf("\n COULDN'T ADD THE SYMBOL TO THE TABLE");
                return ERROR;
            }
        }
        if (operation_or_declaration_word == DATA) {
            insert_numbers_to_data_table(operands_used);
        } else if (operation_or_declaration_word == STRING) {
            /* the definition is .string */
            insert_string_to_data_table(operands_used);
        } else {
            /* MAKE SURE IT IS CORRECT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
            insert_struct_to_data_table(operands_used);
        }
        return OK;
    }

    /* By pages 24-25 if there is a symbol of .entry or .extern declaration - it is useless, we should remove and ignore
     * it from the IC DC structures and keep them only for the .ext and .ent output files*/
    if (operation_or_declaration_word == ENTRY || operation_or_declaration_word == EXTERN) {
        if (symbol_flag) {
            /* In pages 24-25 we are asked to print a warning message on the symbol_name referred by .entry or .extern */
            printf("Warning: in file \"%s\" in cur_line %d: Label before entry or extern definitions.\n", file_name,
                   line_number);
        }
        if (operation_or_declaration_word == ENTRY) {
            /* If the declaration is .entry - checking it doesn't exist in one of the tables already*/
            if (is_on_list(entry_table_head, operands_used)) {
                /* symbol is in table already */
                return OK;
            }
            /*if the .entry symbol is in .extern table - it's an error */
            if (is_on_list(extern_table_head, operands_used)) {
                /* symbol is in table already */
                printf("The .entry label is already mentioned as .extern - it's an error");
                return ERROR;
            }
                /* If the .entry declaration is not on .entry or .extern table already - add it to the .entry table*/
            else {
                /* adding to the table: */
                if (add_to_table(&entry_table_head, operands_used, IC, FALSE, TRUE) == ERROR)
                    return ERROR;
            }
        }
        if (operation_or_declaration_word == EXTERN) {
            /* If the declaration is .extern - checking it doesn't exist in one of the tables already*/
            if (is_on_list(extern_table_head, operands_used)) {
                /* symbol is in table already */
                return OK;
            }
            /*if the .extern symbol is in .entry table - it's an error */
            if (is_on_list(entry_table_head, operands_used)) {
                /* symbol is in table already */
                printf("The .entry label is already mentioned as .extern - it's an error");
                return ERROR;
            }
                /* If the .extern declaration is not on .entry or .extern table already - add it to the .extern table*/
            else {
                /* adding to the table: */
                if (add_to_table(&extern_table_head, operands_used, IC, TRUE, FALSE) == ERROR)
                    return ERROR;
            }
            return OK;
        }

    }


    /* If all the functions above aren't finding data words (.data, .struct, .string, .entry, .extern ) it means */
    /* This is an instruction (operation) line - we need to save it to the instruction/code table (IC) */
    if (symbol_flag & (operation_or_declaration_word != EXTERN) & (operation_or_declaration_word != ENTRY)) {
        if(is_on_list(entry_table_head,symbol_name)){
            if (add_to_table(&general_symbols_table_head, symbol_name, IC, FALSE, TRUE) == ERROR)
                return ERROR;
        }
        else if (is_on_list(extern_table_head,symbol_name)){
            if (add_to_table(&general_symbols_table_head, symbol_name, IC, FALSE, TRUE) == ERROR)
                return ERROR;
        }
        else {
            if (add_to_table(&general_symbols_table_head, symbol_name, IC, FALSE, FALSE) == ERROR)
                return ERROR;
        }
    }



    /* check if the operations are using the right amount of operands afterwards, and create words out of the lines
     * in the instructions table */
    if (operation_or_declaration_word <= LEA) { /* 0: MOV, 1: CMP, 2: ADD, 3: SUB, 6: LEA */
        /* the 4th and 5h operations - NOT and CLR are not accepting 2 operands !  */
        if (operation_or_declaration_word != 4 && operation_or_declaration_word != 5) {
            if (two_operands(operation_or_declaration_word, operands_used) == ERROR)
                return ERROR;
        }
    } else if (operation_or_declaration_word <=
               JSR) { /* 4: NOT, 5: CLR, 7: INC, 8: DEC, 9: JMP, 10: BNE, 11: GET, 12: PRN, 13: JSR */
        if (one_operand(operation_or_declaration_word, operands_used) == ERROR)
            return ERROR;
    } else { /*14: RTS, 15: HLT */
        zero_operands(operation_or_declaration_word);
    }

/* The parsing of the current cur_line ended without errors */
    return OK;
}


int parse_line_data(char *line, char symbol_name[], int *operation_or_decalred_word_holder, char variables[]) {
    char *word_holder;
    char *values_or_operands_of_line;
    int variable_counter = 0;

    if (check_if_colon_in_line(line)) {
        /* have : colon in the line - the generic_word before it must be the symbol  */
        word_holder = strtok(line, ":");
        /* Check if the symbol isn't a saved / declared generic_word already and that it is in the correct form */
        if (is_valid_symbol(word_holder) == FALSE) {
            printf("Invalid symbol");
            return OK;
        }
        /* saving the symbol in a new variable */
        strcpy(symbol_name, word_holder);
        /* cutting out the next generic_word - operation or variable */
        word_holder = strtok(NULL, " \t\n");
    }

        /* the first generic_word is not a symbol */
    else {/* operation or variable */
        word_holder = strtok(line, " \t\n");
    }

    if (!word_holder) { /* there is no usage of a known operation or known declarations */
        return TRUE;
    }

    /* check the operation or declaration: */

    /*TODO IMPORTANT REMOVE SPACES*/
    remove_spaces(word_holder);

    *operation_or_decalred_word_holder = is_operation_name_or_declared_word(word_holder);
    if (*operation_or_decalred_word_holder == ERROR)
        return ERROR;

    /* The rest of the line processing after the symbol and operation/declaration: */
    /* values_or_operands_of_line - holds the line without the first generic_word (operation / declaration) */
    values_or_operands_of_line = word_holder + strlen(word_holder) + SLASH_ZERO_TO_END_STRING;

    /* check if the number of commas between each operand is valid and exact */
    /* if the first generic_word (without the symbol) is not .string - check the commas */

    if (*operation_or_decalred_word_holder != STRING &&
        is_valid_commas(values_or_operands_of_line, &variable_counter) == ERROR)
        return ERROR;

    /* copying the variables part without the spaces to - variables
     * if the data generic_word/ operation is not a .string - remove spaces and copy the operands to variables*/
    if (*operation_or_decalred_word_holder != STRING) {
        remove_spaces(values_or_operands_of_line);
        strcpy(variables, values_or_operands_of_line);
    }

    /* This switch makes sure the number of words that comes after the declaration or the operation are valid ! */
    switch (*operation_or_decalred_word_holder) {

        /* operations using 2 operands */
        case MOV: /* copy value from left operand to right operand*/
        case CMP: /* compare values between left operand and right operand*/
        case ADD: /* add the value of the left operand to the right operand*/
        case SUB: /* subtract the value of the left operand from the right operand, and keeps it on the right one*/
        case LEA: /* load effective address - takes the address of the left operand and saves it to the right operand*/
            /* Checks if the number of variables (operands in this case) is 2 */
            if (variable_counter != 2) {
                return print_error("There should be 2 operands after this operation - not more, not less ");
            }
            break;

            /* operations using 1 operand */
        case NOT: /* swaps the bits values - for example: from 1010 to 0101 */
        case CLR: /* clears/ cleans the operands value  */
        case INC: /* increase operand value by +1 */
        case DEC: /* decrease operand value by -1 */
        case JMP: /* makes the PC jump to a different/ new asm line elsewhere in the memory */
        case BNE:  /* branch if not equal to zero - if PSW register == 0 than the PC will jump to the new memory */
        case GET: /* read char from stdin to an operand */
        case PRN: /* print char from the operand to stdout */
        case JSR: /* call subroutine to PC to use and return value to the stack using RTS operation */
            /* Checks if the number of variable (operand in this case) is 1 */
            if (variable_counter != 1) {
                return print_error("There should be only 1 operand after this operation");
            }
            break;

            /* operations without operands */
        case RTS: /* returns the value from the subroutine(JSR) to the stack */
        case HLT: /* STOPS THE PROGRAM RUN */
            if (variable_counter > 0) {
                return print_error("This operation doesn't accept any operands at all !");
            }
            break;

            /* the second generic_word is a declaration of .data , .string , .struct , .entry or .extern */

        case DATA:
            /* if no variables - return error*/
            if (!variable_counter) {
                return print_error("No number(s) after .data");
            }
            /* if there are numbers - we're checking they are valid digits */
            word_holder = strtok(NULL, ",");
            while (word_holder != NULL) { /* check all the numbers */
                if (!is_valid_digit(word_holder)) {
                    return print_error("Invalid number");
                }
                word_holder = strtok(NULL, ",");
            }
            /*finished processing the .data digits well - returning out of the function*/
            break;

        case STRING:
            /* removing the spaces from the corners */
            values_or_operands_of_line = skip_spaces(values_or_operands_of_line);
            /* Checking it's a valid string with \" at the beginning and end */
            is_valid_string(values_or_operands_of_line);
            printf("Checked if the string after %d is valid string - %s\n", *operation_or_decalred_word_holder, line);
            /* strrchr - searches for the last occurrence of the character \"
             * variable_counter holds the number of characters to be copied from values_or_operands_of_line to
             * variables including \" at the beginning and end, +1 for \0 */
            variable_counter = strrchr(values_or_operands_of_line, '\"') - values_or_operands_of_line + 1;
            /* copying variable_counter (number) of characters from the variable-values_or_operands_of_line to variables*/
            strncpy(variables, values_or_operands_of_line, (variable_counter));
            /* adding the finishing \0 char */
            variables[variable_counter] = '\0';
            /*finished processing the .string characters well - returning out of the function*/
            break;

        case STRUCT:
            if (variable_counter != 2) {
                if (!variable_counter) /* no variables */{
                    return print_error("Missing values after .entry or .extern definition");
                }
                return print_error("2 arguments are needed after .struct declaration");
            }
            break;

        case ENTRY:
        case EXTERN:
            if (variable_counter != 1) {
                if (!variable_counter) /* no variables */{
                    return print_error("Missing values after .entry or .extern definition");
                }
                return print_error("More than 1 argument after .entry or .extern definition");
            }
            /* Check if the symbol isn't a saved/ declared generic_word already and that it is in the correct form */
            if (is_valid_symbol(values_or_operands_of_line) ==
                FALSE) /* check the symbol after the .extern or .entry */{
                return print_error("Invalid symbol after .entry or .extern definition");
            }
            break;


    }
    /* line is valid - no errors found */
    return OK; /* everything is good with the line */
}

int check_if_colon_in_line(char *s) {
    int i;

    /* check for ':' before a " (because if ':' comes after " then it is considered a part of a string): */
    for (i = 0; s[i] && s[i] != '\"'; i++) {
        if (s[i] == ':')
            return TRUE;
    }
    return FALSE;
}


int is_valid_symbol(char *name) {
        int i;
        /* The first character is a letter */
        if (!isalpha(name[0]))
            return FALSE;
        /* The maximum length of a symbol is 30 */
        if (strlen(name) > SYMBOL_LEN)
            return FALSE;

        for (i = 0; name[i]; i++) {
            /* The other characters have to be letters or digits */
            if (!(isalpha(name[i]) || isdigit(name[i])))
                return FALSE;
        }
        /* The symbol cannot be an operation, or a register or a declared data generic_word */
        if (is_symbol_name_equals_to_saved_names(operations_names, NUM_OPS, name))
            return FALSE;
        if (is_symbol_name_equals_to_saved_names(data_string_struct_entry_extern_declarations, DATA_WORDS_DECLARATIONS,
                                                 name))
            return FALSE;
        if (is_symbol_name_equals_to_saved_names(registers_names, NUM_REGISTERS, name))
            return FALSE;
        /* The symbol is valid */
        return TRUE; /* The label is good */
    }






int is_valid_digit(char *s) {
    int i;
    if (s[0] == '-' || s[0] == '+')
        s++; /* we do not need the '+' or the '-' at the beginning of the number */
    for (i = 0; s[i]; i++) { /* Scans the number and checks that it is indeed all digits */
        if (!isdigit(s[i]))
            return FALSE;
    }
    return TRUE;
}

/*
 * This function checks the existence of commas and counts the number of operands/ values
 * if there is a syntax error - it will b e raised
 * */
int is_valid_commas(char *s, int *count_arguments) {
    int i;
    /* in_word is a flag to mention if there's a new operand that will be expecting a comma later
     * had_comma is a counter for the number of commas we counted between words */
    int in_word = 0, had_comma = 1;

    /* removing all spaces from the line*/
    s = skip_spaces(s);
    remove_spaces(s);
    /* if there are no operands left after spaces cleaning - return valid*/
    if (!(*s))
        return OK;
    /* if there is a comma after an operation or declaration - raise an error */
    if (s[0] == ',') {
        printf("Invalid comma after operation or declaration");
        return OK;
    }
    if (s[strlen(s) - 1] == ',')
        return printf("comma can't be the last character in a line");

    /* Going over each char of the line - if there's a tab, new line or space - continue
     * if there's a comma - count it */
    for (i = 0; s[i]; i++) {
        /* if the current char is space, tab to \n - we're not in_word it's turned off to 0 -> we're between 2 words
         *had_comma stays turned off */
        if (s[i] == ' ' || s[i] == '\t' || s[i] == '\n') {
            in_word = 0;
            continue;
        }

        /* If the current char is comma - we check if there was a comma before without generic_word - if so, it's an error
         * else - in_word is turned off and had_comma is raised by 1*/
        if (s[i] == ',') {
            if (had_comma) /* multiple commas */
            {
                return print_error("counted 2 commas with no words in between them - invalid syntax ");
            }
            in_word = 0;
            had_comma = 1;
        }
            /* if we're not in a generic_word, yet we didn't collect any comma - it means there's a missing comma bet */
        else if (!in_word) {
            /* missing comma between 2 operands (only spaces between them) */
            if (!had_comma) {
                return print_error("Missing comma between 2 operands/ values");
            }
            in_word = 1;
            had_comma = 0;
            /* counting the number of words*/
            (*count_arguments)++;
        }
    }
    return OK;
}

int is_valid_string(char *s) {
    int i;
    /* the last character in the string */
    char *last = strrchr(s, '\"');
    /* the first char must be \" */
    if (s[0] != '\"') {
        return OK;
    }

    for (i = 1; last[i]; i++) {
        if (!isspace(last[i])) {
            return OK;
        }
    }

    return OK;
}

