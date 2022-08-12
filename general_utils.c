#include "file_handler.h"
#include "general_utils.h"
#include "first_run.h"
#include "data.h"

/* lines tracker / counter */
int line_number = 1;


void slice(const char *src, char *dest, size_t start_index, size_t end) {
    /*  copies up to n characters from the string pointed to, by src to dest. */
    strncpy(dest, src + start_index, end - start_index);
}

char *skip_spaces(char *s) {
    int n;
    for (n = 0; s[n] && isspace(s[n]); n++) {} /* Count how many spaces there are at the beginning */
    return s + n;
}


size_t remove_white_spaces(char *clean_result_str, size_t length_of_str, const char *line_to_clean) {
    /*   a function that removes white spaces such as " ", "\t", "\n" and "\r" from the beginning and the end of the line */
    /*   "\r" is only for windows operating systems */
    const char *end;
    size_t out_size;
    if (length_of_str == 0)
        return OK;

    /* Trim leading space*/
    while (isspace((unsigned char) *line_to_clean)) line_to_clean++;
    /* check if possibly the line is just spaces */
    if (*line_to_clean == 0)
    {
        *clean_result_str = 0;
        return OK;
    }

    /* Trim trailing space*/
    end = line_to_clean + strlen(line_to_clean) - 1;
    while (end > line_to_clean && isspace((unsigned char) *end)) end--;
    end++;

    /* Set output size to minimum of trimmed string length and buffer size minus 1*/
    out_size = (end - line_to_clean) < length_of_str - 1 ? (end - line_to_clean) : length_of_str - 1;

    /* Copy trimmed string and add null terminator*/
    memcpy(clean_result_str, line_to_clean, out_size);
    clean_result_str[out_size] = 0;

    return out_size;
}


void remove_spaces(char *str) {
    int i;
    /* To keep track of non-space character count */
    int count = 0;

    /* Traverse the given string. If current character
     is not space, then place it at index 'count++' */
    for (i = 0; str[i]; i++) {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r')
            str[count++] = str[i]; /* here count is incremented */
    }

    str[count] = '\0'; /* finish the string */
}


/* print error wrapper function - mentioning the line and the specific error*/
int print_error(char *the_error) {
    fprintf(stderr, "Error: in file \"%s\" in line %d: %s.\n", file_name, line_number, the_error);
    return ERROR;
}


int print_error_only_message(char *the_error) {
    fprintf(stderr, "%s.\n", the_error);
    return ERROR;
}



int which_register(char *reg) {
    int i;
    for (i = 0; i < NUM_REGISTERS; i++) { /* scan the registers names array */
        if (!strcmp(reg, registers_names[i]))  /* the name is in the array - we found! */
            return i;
    }
    return ERROR; /* reg is not a register */
}



int get_address_method(char *are_argument) {
    char *argument = malloc(strlen(are_argument) + 1);
    char *field_of_record, clean_res[MAX_LINE_LENGTH];
    long field_number_of_record;
    strcpy(argument, are_argument);
    /*
     * 00 Immediate address - extra generic_word to the operation - 8 bits of the number + ARE 00
     * 01 Direct address - extra generic_word - address from the data table - adding at second pass + ARE 01
     * 10 Access to Record address - two extra words - the address of the record+ARE 10 + generic_word of the digit+00
     * 11 Direct Register address -
     * reminder - data table and data words such as .data .struct .string .entry .extern don't have ARE
     */

    /* checking if the first operand is #digit - if so, it's immediate address*/
    if (argument[0] == '#') {
        /* 00 Immediate address */
        if (!is_valid_digit(argument + 1)) {
            return print_error("ERROR - Invalid number in immediate addressing method");
        }
        return IMMEDIATE_ADDRESS; /* 00 */
    }

    /* checking if the first operand is of a form r0-r7 (register) -> if so, it's register address*/
    if (is_symbol_name_equals_to_saved_names(registers_names, NUM_REGISTERS, argument))
        /* 11 Direct Register address */
        return DIRECT_REGISTER_ADDRESS; /* 11 */

    /* checking if the first operand has a dot in it - if so, it's ACCESS_TO_RECORD_ADDRESS */
    if (strchr(argument, '.')) {
        if (!is_valid_ref_to_address(argument)) {
            return print_error("Invalid label in relative addressing method");
        }
        field_of_record = strtok(argument, ".");
        field_of_record = strtok(NULL, "");
        remove_white_spaces(clean_res, MAX_LINE_LENGTH, field_of_record);
        field_number_of_record = (int) atoi(field_of_record);
        /* making sure the field number is a digit and also is 1 or 2 (the only possible values)
         * if so - return access to record address*/
        if (field_number_of_record == 1 || field_number_of_record == 2) {
            return ACCESS_TO_RECORD_ADDRESS; /* 10 */
        } else if (!isdigit(field_number_of_record)) {
            return print_error("ERROR - the label's field is not a digit - cannot parse it");
        }
    }


    /* if all the checks above didn't work we are in direct addressing method (number 1) */
    if (!is_valid_symbol(argument)) {
        return print_error("ERROR - Invalid label in direct addressing method");
    }
    return DIRECT_ADDRESS; /* 01 Direct address */

}

int is_operation_name_or_declared_word(char *name) {
    int i;
    /* check for operations: */
    for (i = 0; i < NUM_OPS; i++) {
        if (!strcmp(name, operations_names[i])) {
            return i;
        }
    }
    /*0: MOV, 1: CMP, 2: ADD, 3: SUB, 4: NOT, 5: CLR, 6: LEA, 7: INC, 8: DEC, 9: JMP, 10: BNE, 11: GET, 12: PRN,
     * 13: JSR, 14: RTS, 15: HLT, 16: DATA, 17: STRING, 18: STRUCT, 19: ENTRY, 20: EXTERN*/
    /* check for declared data words - .data: */
    for (i = 0; i < DATA_WORDS_DECLARATIONS; i++) {
        if (!strcmp(name, data_string_struct_entry_extern_declarations[i])) {
            return i + DATA;
        }
    }

    return print_error("Undefined operation or definition");
}



char symbol_characters[32] = {'!', '@', '#', '$', '%', '^', '&',
                              '*', '<', '>', 'a', 'b', 'c', 'd',
                              'e', 'f', 'g', 'h', 'i', 'j',
                              'k', 'l', 'm', 'n', 'o', 'p', 'q',
                              'r', 's', 't', 'u', 'v'};



void convert_decimal_32bit(int num, char converted_data[]) {
    char left_value = '!'; /* value zero initialized */
    if (num > 32) {
        converted_data[0] = symbol_characters[(num / 32)];
        converted_data[1] = symbol_characters[(num % 32)];
    } else {
        converted_data[0] = left_value;
        converted_data[1] = symbol_characters[(num % 32)];
    }

}


FILE *get_file(char *name, char *file_ending) {
    FILE *file_pointer;
    char *out_name = (char *) malloc(strlen(name) + strlen(file_ending));
    if (!out_name) { /* Allocates memory failed so we print an error message and exit the program */
        printf("Memory allocation failed");
        exit(1);
    }
    strcpy(out_name, name);
    strcat(out_name, file_ending);

    file_pointer = fopen(out_name, "w"); /* Opens the file, with writing permission */
    free(out_name); /* The file's name isn't needed anymore */
    return file_pointer;
}