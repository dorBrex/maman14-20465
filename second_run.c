
#include "second_run.h"
#include "general_utils.h"
#include "data.h"

/* TODO: SECOND PASS FUNCTIONS */
int second_pass(FILE *source_file) {

    /* flag to see if there's an error during the second part parsing */
    int error_flag = FALSE;
    /* line object to hold the current line */
    char curr_line[MAX_LINE_LENGTH];
    /* passing through the memory again from instruction counter in index 100 */
    IC = 100; /* Initialize the variable */

    for (line_number = 1; fgets(curr_line, MAX_LINE_LENGTH, source_file); line_number++) {
        if (second_pass_handle_line(curr_line) == ERROR)
            error_flag = TRUE;
    }
    return error_flag;
}


int second_pass_handle_line(char line[]) {
    char *pointer_to_current_line = line;
    char arguments[MAX_LINE_LENGTH];
    char comment_sign = ';';
    int operation_declaration_value;

    /* In the second phase we already passed through the lines' length - and we know if they are all valid (80 characters max)
     * and we already collected the symbols / labels into a label, so no need in doing it again*/

    /* if it's a comment line - skip to the next one  */
    if (*pointer_to_current_line == comment_sign) {
        return OK;
    }
    /* remove white spaces before and after the text of the line*/
    /*   remove_new_line_tab_etc(pointer_to_current_line); */
    remove_white_spaces(pointer_to_current_line, MAX_LINE_LENGTH, pointer_to_current_line);
    /* only spaces in the cur_line - skip to the next line*/
    if (!(*pointer_to_current_line) || (*pointer_to_current_line == ' ') || (*pointer_to_current_line == '\t')) {
        return OK;
    }

    /* the second parsing is like the parse_line_data function
     * the second_parsing function fills the operation/declaration generic_word into operation_declaration_value
     * and the rest of the line after the declaration/ operation to arguments
     * These 2 are used later to fill up the missing memories of the .data .string .struct .entry .extern values */

    second_parsing(pointer_to_current_line, &operation_declaration_value, arguments);

    /* if the operation_declaration_value is .data, .string, .struct ,.extern, .struct  -> return valid */
    if (operation_declaration_value == DATA || operation_declaration_value == STRING ||
        operation_declaration_value == STRUCT || operation_declaration_value == ENTRY ||
        operation_declaration_value == EXTERN)
        return OK;

    /* if the operation_declaration_value is RTS or HLT -> increase IC and return valid */
    if (operation_declaration_value >= RTS) { /* no operands */
        /* there are no operands for hlt or rts - move on to the next lines */
        IC++;
        return OK;
    }
    /* if the operation_declaration_value is RTS or HLT -> increase IC and return valid */
    /* take care of operations - MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, GET, PRN, JSR */
    /* fill the missing memory in the instruction-table of the arguments */
    if (complete_binary_encoding(arguments) == ERROR)
        return ERROR;

    return OK;
}

int complete_binary_encoding(char *arguments) {
    int symbol_value, second_operand_are;
    char *current_operand = strtok(arguments, ",");
    char * temp, *second_operand;
    int addressing_method, passed_first_operand = 0;
    int flag_first_register = 0;
    second_operand = strtok(NULL,",");
    if (second_operand){
        second_operand_are = get_address_method(second_operand);
    }
    /* skipping the operation word into its arguments (memory spaces)*/
    IC++;

    /* the loop will run the number of times equal to the number of the operands after this operation / declaration */
    while (current_operand) {
        if (is_on_list(extern_table_head, current_operand)){
            add_to_table(&refer_to_an_external_symbol_head, current_operand, IC, TRUE,FALSE);
        }
        remove_spaces(current_operand);
        addressing_method = get_address_method(current_operand);

        if (addressing_method == IMMEDIATE_ADDRESS) {
            ;  /* let the last function do the IC++ for it */
        }
        else if (addressing_method == DIRECT_REGISTER_ADDRESS) {
            if (flag_first_register == 0) {
                if (second_operand_are== DIRECT_REGISTER_ADDRESS){
                    flag_first_register = 1;
                }else{
                    IC++;
                }
            } else {
                /* The first operand was already taking place of this memory section - and the second register operand
                 * is concatenating to the same memory space (doesn't need to move on another IC) */
                flag_first_register = 0;
                /*move on from the */
            }
        } else if (addressing_method == DIRECT_ADDRESS) { /* ARE == 01 */

            /* allocate_memory_instruction_table(IC);  allocate memory for the cell */

            /* it means this memory space (word) in the instruction table wasn't initialized yet and waits for data from the data table*/
            /* filling this empty memory space */
            address_word address_word_to_save;
            address_word_to_save.are = 01; /* ARE == 01 */

            symbol_value = find_label_value(current_operand);
            if (symbol_value == ERROR) {
                return print_error("ERROR - Indefinite label operand");
            }
            if(symbol_value != 0){
                /* it means the value is not external and from the file itself. so ARE is 10 after all!*/
                address_word_to_save.are = 2; /* ARE == 10 */
            }
            /* if symbol value is zero - it means the operand is external (added from outside file) */
            address_word_to_save.address_in_ic = symbol_value;
            memset(instruction_table[IC], 0, sizeof(address_word_to_save));
            allocate_memory_instruction_table(IC);
            memcpy(instruction_table[IC], &address_word_to_save, sizeof(address_word_to_save));
            if((second_operand != NULL) & (second_operand != current_operand)){
                IC++;
            }
        } else if (addressing_method == ACCESS_TO_RECORD_ADDRESS)/* ARE==10 (2)*/{
            address_word address_word_to_save;
            address_word_to_save.are = 2; /* ARE == 10 */
            temp = strtok(current_operand, ".");
            symbol_value = find_label_value(temp);

            if (symbol_value == ERROR) {
                return print_error("ERROR - Indefinite label operand");
            }
            address_word_to_save.address_in_ic = symbol_value;
            memset(instruction_table[IC], 0, sizeof(address_word_to_save));
            allocate_memory_instruction_table(IC);
            memcpy(instruction_table[IC], &address_word_to_save, sizeof(address_word_to_save));

            /* skipping the .1 field in the memory and up to the next memory space to check */
            IC=IC+2;
        }
        if ((second_operand != NULL) & (passed_first_operand != 1)){
            passed_first_operand = 1;
            current_operand = second_operand;
        }else {
            /* after we re-matched the memory space with the right value, moving on to the next IC pointer*/
            IC++;
            return OK;
        }
    }
    return OK;
}


int find_label_value(char *label_name) {
    table p = code_table_head;
    while (p) { /* search for the label in the symbol table */
        if (!strcmp(p->label_name, label_name)) /* find the label */
            return p->value;
        p = p->next;
    }

    /* not in code_table_head */

    p = data_table_head;
    while (p) { /* search for the label in the symbol table */
        if (!strcmp(p->label_name, label_name)) /* find the label */
            return p->value;
        p = p->next;
    }

    if (is_on_list(extern_table_head, label_name))
        return OK;

    return ERROR; /* did not find the label in the symbol table */

}

/* extracting the operation / declaration word and the operands into arguments variable */
void second_parsing(char *line, int *operation_declaration_val, char arguments[]) {
    char *token;
    char *arguments_part;

    /* search for ':' in order to remove it from line and have <operation  ...  rest of the line> without symbol / label*/
    token = strchr(line, ':');
    if (token != NULL)
        line = token + 1;

    /*token holds the words after the symbol (such as MAIN: <words that token holds >) */
    token = strtok(line, " \t\n");
    if(!token){
        printf("\nempty line - %s\n", line);
    }
    /* check is operation or declaration and save it to operation_declaration_val variable */
    *operation_declaration_val = is_operation_name_or_declared_word(token);

    arguments_part = token + strlen(token) + 1;
    remove_spaces(arguments_part);
    /* Saving the argument string */
    strcpy(arguments, arguments_part);
}
