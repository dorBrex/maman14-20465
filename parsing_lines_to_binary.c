#include "parsing_lines_to_binary.h"
#include "general_utils.h"

int two_operands(int operation, char *arguments) {
    /*
     * Constants for classification addressing methods
     * 00 Immediate address - the operands starts with '#' and number afterwords such as mov #-1 ...
     * 01 Direct address - the operand is label / symbol starting with a dot such as - .string .data .struct .extern
     * 10 Access to Record address - the operand is a symbol/ label referring to a .struct and later 1 or 2 as refer to the field
     * 11 Direct Register address - using r0...r7 as operands
     * addressing_methods {IMMEDIATE_ADDRESS, DIRECT_ADDRESS, ACCESS_TO_RECORD_ADDRESS, DIRECT_REGISTER_ADDRESS };
     */
    /*initializing operation_word_to_save to be empty */
    operation_word operation_word_to_save;
    int addressing_method;

    /* temp - receiving from strtol the int as char */
    char *temp, *first_word, *second_word;

    /* holding the operand till the ',' */
    char *current_operand;

    /* L counter to count the number of words created from this asm line*/
    int L, first_operand_are, second_operand_are;
    long digit;
    char clean_res[MAX_LINE_LENGTH];

    first_word = NULL;
    current_operand = strtok(arguments, ",");
    L = 0, first_operand_are = 0, second_operand_are = 0;
    /* starting to process the 2 operands after the opcode in order to find the ARE values
     * and build the binary encoding of the words */
    remove_spaces(current_operand);
    /* checking for the addressing method */
    first_word = current_operand;
    second_word = strtok(NULL, ",");
    addressing_method = get_address_method(first_word);
    /* if no valid addressing method - throw exception */
    if (addressing_method == ERROR)
        return ERROR;

    if (addressing_method == IMMEDIATE_ADDRESS) { /*ARE==00*/
        first_operand_are = IMMEDIATE_ADDRESS;
        L++; /* it's going to be an extra generic_word */
    } else if (addressing_method == DIRECT_REGISTER_ADDRESS) { /*ARE==11 */
        first_operand_are = DIRECT_REGISTER_ADDRESS;
        /*reminder - the register generic_word itself would have 4 bits for src, 4 bit for dst, 2 bits or ARE == 00*/
        L++;
    } else if (addressing_method == ACCESS_TO_RECORD_ADDRESS) { /*ARE==10 */
        first_operand_are = ACCESS_TO_RECORD_ADDRESS;
        /* This method is using extra 2 words  - one for the address itself and the second for the field */
        L += 2;
    } else {
        first_operand_are = DIRECT_ADDRESS; /* 01 */
        L++;
    }

    /*----------the second operand----------*/

    /* isolating the second operand from the arguments part */
    remove_spaces(second_word);
    addressing_method = get_address_method(second_word);

    if (addressing_method == ERROR)
        return ERROR;

    if (addressing_method == IMMEDIATE_ADDRESS) { /*ARE==00*/
        second_operand_are = IMMEDIATE_ADDRESS;
        L++; /* it's going to be an extra generic_word */
    } else if (addressing_method == DIRECT_REGISTER_ADDRESS) { /*ARE==11 */
        second_operand_are = DIRECT_REGISTER_ADDRESS;
        /*reminder - the register generic_word itself would have 4 bits for src, 4 bit for dst, 2 bits or ARE == 00*/
        L++;
    } else if (addressing_method == ACCESS_TO_RECORD_ADDRESS) { /*ARE==10 */
        second_operand_are = ACCESS_TO_RECORD_ADDRESS;
        /* This method is using extra 2 words  - one for the address itself and the second for the field */
        L += 2;
    } else {
        second_operand_are = DIRECT_ADDRESS; /* 01 */
        L++;
    }

    /* TODO : INSTRUCTION WORD */
    /* nullify the operation_word and then inserting values and copy it with memcpy to the IC table in the IC index*/
    allocate_memory_instruction_table(IC);
    operation_word_to_save.opcode = 0000;
    operation_word_to_save.src = 00;
    operation_word_to_save.dst = 00;
    operation_word_to_save.are = 00;

    /*filling the first instruction operation_word_to_save from the asm code instructions */
    operation_word_to_save.opcode = operation;
    operation_word_to_save.src = first_operand_are;
    operation_word_to_save.dst = second_operand_are;
    /* are is always 00*/
    operation_word_to_save.are = 00;

    memcpy(instruction_table[IC], &operation_word_to_save, sizeof(operation_word_to_save));
    /* moving on to the next generic_word */
    IC++;

    /* TODO : FIRST OPERAND */
    /* checking if the first operand is a #digit */
    /* nullify the operation_word and then inserting values and copy it with memcpy to the IC table in the IC index*/
    allocate_memory_instruction_table(IC);


    if (first_operand_are == IMMEDIATE_ADDRESS) {
        /*filling the first operand generic_word from the asm code instructions */
        field_num_word field_number_to_save;
        digit = strtol(clean_res, &first_word, 10);
        field_number_to_save.field_num = digit;
        /* are is always 00*/
        field_number_to_save.are = 00;
        memcpy(instruction_table[IC], &field_number_to_save, sizeof(field_number_to_save));
    }


        /* checking if the first operand is a register */
    else if (first_operand_are == DIRECT_REGISTER_ADDRESS) {
        /*filling the first operand generic_word from the asm code instructions */
        register_word register_word_to_save;
        digit = which_register(first_word);
        register_word_to_save.src = digit;
        /* zeroing the second register place */
        register_word_to_save.dst = 0000;
        /* if the second operand is a register - add it to the same generic_word */
        if (second_operand_are == DIRECT_REGISTER_ADDRESS) {
            digit = which_register(second_word);
            register_word_to_save.dst = digit;
        }
        /* are is always 00*/
        register_word_to_save.are = 00;
        memcpy(instruction_table[IC], &register_word_to_save, sizeof(register_word_to_save));

    } else if (first_operand_are == DIRECT_ADDRESS) {
        /* leaving it empty for the second phase*/
        address_word address_word_to_save;
        address_word_to_save.address_in_ic = 00000000;
        address_word_to_save.are = 00;
        memcpy(instruction_table[IC], &address_word_to_save, sizeof(address_word_to_save));

    }


        /* the last option is that the first operand is a ACCESS_TO_RECORD_ADDRESS which means we need to save 2 lines
         * one for the address in the second phase and one for the field itself */
    else {
        /*initialising the second generic_word after the operation generic_word */
        address_word address_word_to_save;
        field_num_word field_number_to_save;
        /* leaving it empty for the second phase*/
        address_word_to_save.address_in_ic = 00000000;
        address_word_to_save.are = 2; /* 10 is the ARE of ref to a memory address*/
        /* calling the IC once again in order to open the field generic_word*/

        memcpy(instruction_table[IC], &address_word_to_save, sizeof(address_word_to_save));

        IC++;

        allocate_memory_instruction_table(IC);
        /* moving on to the next generic_word and extracting the field number */
        temp = strtok(first_word, ".");
        temp = strtok(NULL, "");

        digit = atoi(temp);

        field_number_to_save.field_num = digit;
        field_number_to_save.are = 00;

        memcpy(instruction_table[IC], &field_number_to_save, sizeof(field_number_to_save));


    }

    /* finished with the first generic_word - going on to the second generic_word (operand) */

    /* moving on to the next generic_word */
    if ((first_operand_are != DIRECT_REGISTER_ADDRESS) || (second_operand_are != DIRECT_REGISTER_ADDRESS)) {
        IC++;
        allocate_memory_instruction_table(IC);
    }
    /* TODO : SECOND WORD */

    /* checking if the first operand is a #digit */
    if (second_operand_are == IMMEDIATE_ADDRESS) {
        /*filling the first operand generic_word from the asm code instructions */
        field_num_word field_number_to_save;

        digit = strtol(clean_res, &second_word, 10);
        field_number_to_save.field_num = digit;
        /* are is always 00*/
        field_number_to_save.are = 00;

        memcpy(instruction_table[IC], &field_number_to_save, sizeof(field_number_to_save));

    } else if ((second_operand_are == DIRECT_REGISTER_ADDRESS) & (first_operand_are == DIRECT_REGISTER_ADDRESS)) {
        /*continue without creating new memory space */
    }
        /* checking if the second operand is a register and that the first one isn't also */
    else if ((second_operand_are == DIRECT_REGISTER_ADDRESS) & (first_operand_are != DIRECT_REGISTER_ADDRESS)) {
        register_word register_word_to_save;
        /*filling the first operand generic_word from the asm code instructions */
        digit = which_register(second_word);
        /* zeroing the first register place */
        register_word_to_save.src = 0000;
        register_word_to_save.dst = digit;
        register_word_to_save.are = 00;

        memcpy(instruction_table[IC], &register_word_to_save, sizeof(register_word_to_save));


    } else if (second_operand_are == DIRECT_ADDRESS) {
        /* leaving it empty for the second phase*/
        address_word address_word_to_save;
        address_word_to_save.address_in_ic = 00000000;
        address_word_to_save.are = 00;

        memcpy(instruction_table[IC], &address_word_to_save, sizeof(address_word_to_save));

    }

        /* the last option is that the first operand is a ACCESS_TO_RECORD_ADDRESS which means we need to save 2 lines
         * one for the address in the second phase and one for the field itself */
    else {
        field_num_word field_number_to_save;
        /* calling the IC once again in order to open the field generic_word*/
        IC++;
        /*initialising the second generic_word after the operation generic_word */
        allocate_memory_instruction_table(IC);

        /* moving on to the next generic_word and extracting the field number */
        digit = strtol(clean_res, &first_word, 10);
        field_number_to_save.field_num = digit;
        /* are is always 00*/
        field_number_to_save.are = 00;

        memcpy(instruction_table[IC], &field_number_to_save, sizeof(field_number_to_save));
    }

    if ((second_operand_are != 2) & (first_operand_are != 2)) {
        if (L == 2) {
            IC++;
            return OK;
        }
    } else if ((first_operand_are == 2 )|| (second_operand_are == 2)) {
        if (L >= 3) {
            IC++;
            return OK;
        }
    }
    print_error("ERROR - finished two operands - WITH ERRORS");
    return ERROR;

}

int one_operand(int operation, char *operand) {
    operation_word operation_word_to_save;
    int addressing_method;
    /* temp - receiving from strtol the int as char */
    char *first_operand = operand;
    /* holding the operand till the ',' */
    char *current_operand = operand;/* current_operand = "#1" */
    /* L counter to count the number of words created from this asm line*/
    int L = 0, first_operand_are = 0;
    long digit;
    char clean_res[MAX_LINE_LENGTH];

    /* checking for the addressing method */
    addressing_method = get_address_method(current_operand);
    /* if no valid addressing method - throw exception */
    if (addressing_method == ERROR)
        return ERROR;

    /*
     * Constants for classification addressing methods
     * 00 Immediate address '#'
     * 01 Direct address - label / symbol starting with a dot - .string .data .struct .extern
     * 10 Access to Record address - the operand is a symbol/ label referring to a .struct and later 1 or 2 as refer to the field
     * 11 Direct Register address - using r0...r7 as operands
     * addressing_methods {IMMEDIATE_ADDRESS, DIRECT_ADDRESS, ACCESS_TO_RECORD_ADDRESS, DIRECT_REGISTER_ADDRESS };
     */

    if (addressing_method == IMMEDIATE_ADDRESS) { /*ARE==00*/
        first_operand_are = IMMEDIATE_ADDRESS;
        L++; /* it's going to be an extra generic_word */
    } else if (addressing_method == DIRECT_REGISTER_ADDRESS) { /*ARE==11 */
        first_operand_are = DIRECT_REGISTER_ADDRESS;
        /*reminder - the register generic_word itself would have 4 bits for src, 4 bit for dst, 2 bits or ARE == 00*/
        L++;
    } else if (addressing_method == ACCESS_TO_RECORD_ADDRESS) { /*ARE==10 */
        first_operand_are = ACCESS_TO_RECORD_ADDRESS;
        /* This method is using extra 2 words  - one for the address itself and the second for the field */
        L += 2;
    } else {
        first_operand_are = DIRECT_ADDRESS; /* 01 */
        L++;
    }


    /* TODO : INSTRUCTION WORD */
    allocate_memory_instruction_table(IC);

    operation_word_to_save.opcode = 0000;
    operation_word_to_save.src = 00;
    operation_word_to_save.dst = 00;
    operation_word_to_save.are = 00;

    /*filling the first instruction generic_word from the asm code instructions */
    operation_word_to_save.opcode = operation;
    operation_word_to_save.src = 00;
    operation_word_to_save.dst = first_operand_are;
    /* are is always 00*/
    operation_word_to_save.are = 00;

    memcpy(instruction_table[IC], &operation_word_to_save, sizeof(operation_word_to_save));
    /* moving on to the next generic_word */
    IC++;

    /* TODO : FIRST OPERAND */
    /* checking if the first operand is a #digit */
    /* nullify the operation_word and then inserting values and copy it with memcpy to the IC table in the IC index*/
    allocate_memory_instruction_table(IC);

    /* TODO : FIRST OPERAND */
    /* checking if the first operand is a #digit */
    if (first_operand_are == IMMEDIATE_ADDRESS) {
        /*filling the first operand generic_word from the asm code instructions */
        field_num_word field_number_to_save;

        remove_spaces(first_operand);
        first_operand = strtok(first_operand, "#");

        digit = atoi(first_operand);
        field_number_to_save.field_num = digit;
        /* are is always 00*/
        field_number_to_save.are = 00;
        memcpy(instruction_table[IC], &field_number_to_save, sizeof(field_number_to_save));

    }
        /* checking if the first operand is a register */
    else if (first_operand_are == DIRECT_REGISTER_ADDRESS) {
        /*filling the first operand generic_word from the asm code instructions */
        register_word register_word_to_save;

        digit = which_register(first_operand);
        /* zeroing the first register place */
        register_word_to_save.src = 0000;
        register_word_to_save.dst = digit;
        register_word_to_save.are = 00;
        memcpy(instruction_table[IC], &register_word_to_save, sizeof(register_word_to_save));

    } else if (first_operand_are == DIRECT_ADDRESS) {
        address_word address_word_to_save;
        address_word_to_save.address_in_ic = 00000000;
        address_word_to_save.are = 00;
        memcpy(instruction_table[IC], &address_word_to_save, sizeof(address_word_to_save));

    }

        /* the last option is that the first operand is a ACCESS_TO_RECORD_ADDRESS which means we need to save 2 lines
         * one for the address in the second phase and one for the field itself */
    else {
        field_num_word field_number_to_save;
        /* calling the IC once again in order to open the field generic_word*/
        IC++;
        /*initialising the second generic_word after the operation generic_word */
        allocate_memory_instruction_table(IC);
        /* moving on to the next generic_word and extracting the field number */
        digit = strtol(clean_res, &first_operand, 10);
        field_number_to_save.field_num = digit;
        field_number_to_save.are = 00;

        memcpy(instruction_table[IC], &field_number_to_save, sizeof(field_number_to_save));
    }

    /* finished with the first and only generic_word */

    /*TODO: Check for INVALID METHODS USAGE*/
    if (L) {
        IC++;
        return OK;
    }
    print_error("ERROR - finished one operand - WITH ERRORS");
    return ERROR;

}

void zero_operands(int operation) {
    if (operation == RTS || operation == HLT) {
        operation_word operation_word_to_save;
        allocate_memory_instruction_table(IC);
        operation_word_to_save.opcode = 0000;
        operation_word_to_save.src = 00;
        operation_word_to_save.dst = 00;
        operation_word_to_save.are = 00;
        /*filling the operation only */
        operation_word_to_save.opcode = operation;
        memcpy(instruction_table[IC], &operation_word_to_save, sizeof(operation_word_to_save));
        IC++;
    }
    /*else - it's a symbol with zero operands which got to this stage
     * and nothing has to be done (example - .entry LOOP or .extern W */
}
