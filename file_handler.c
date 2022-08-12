/* handler function to handle many files one after another */

#include "file_handler.h"
#include "macro_handler.h"
#include "first_run.h"
#include "general_utils.h"
#include "second_run.h"
char *file_name;
void open_many_files(int argc, char *argv[]) {
    int i;
    /* Starting from the second argc (the first argc is the program name)*/
    for (i = 1; i < argc; i++) {
        open_single_file(argv[i]);
    }
}


void open_single_file(char *name) {
    /*PRE-ASSEMBLER PART*/
    FILE *source_file, *am_file;

    printf("\n\t\t\tSTART ASSEMBLER\n");

    /* Taking the file name and adding the.AS suffix to it in order to open it */
    /* Assigns memory for the name of the file and copies the name of the file, including the ".as" ending, to it */
    file_name = (char *) malloc(strlen(name) + 3); /* Allocate memory for the full file name (strlen(".as") = 3) */
    printf("\nOPEN SINGLE .AS FILE");

    /* Assigns memory for the name of the file and copies the name of the file, including the ".as" ending, to it */
    file_name = (char *) malloc(strlen(name) + 3); /* Allocate memory for the full file name (strlen(".as") = 3) */

    /* If allocating memory failed -> program is shutdown*/
    if (!file_name) {
        printf("\nMemory allocation failed");
        exit(1);
    }

    strcpy(file_name, name);
    strcat(file_name, AS_SUFFIX); /* file_name stores the file name as it is on the computer */

    /* Opening the .AS file and running the macro_handler */
    source_file = fopen(file_name, "r");

    if (!source_file) { /* If the file wasn't found, or it isn't allowed for reading, the file pointer is NULL */
        fprintf(stderr, "\nCouldn't open file %s.\n", file_name);
        return;
    }
    /*Macro Handling*/
    printf("\nNOW I HANDLE MACRO");
    macro_handler(source_file, file_name);
    printf("\nFINISHED HANDLE MACRO");


    /* Starts the first and second passes, and if they are finished successfully, writes the output files */
    /* Closes the file after reading and frees the file_name string for the next file name */
    fclose(source_file);
    printf("\nCLOSED .AS FILE");
    /* If allocating memory failed -> program is shutdown*/
    /* Opening the unfolded macros file we just created */
    memset(file_name, 0, strlen(file_name));
    strcpy(file_name, name);
    strcat(file_name, AM_SUFFIX);

    /* Opening the .AM file and running the macro_handler */
    am_file = fopen(file_name, "r");
    if (!am_file) { /* If the file wasn't found, or it isn't allowed for reading, the file pointer is NULL */
        fprintf(stderr, "\nCouldn't open file %s.\n", file_name);
        return;
    }


    /*SECOND PART*/

    /* Starts the first and second passes, and if they are finished successfully, writes the output files
     * the returned value of the first_main_run is error_flag boolean variable which
     * it's equal to zero/ false - we succeeded
     * 
     * if the first main run returns the error_flag == -1 -> the program will stop the run and wouldn't continue to the second stage and to create files
     * if the first main run returns the error_flag == 0 -> the program will continue to the second pass, it it runs correctly the program will try to create output files, if not - the program will shut 
     * anyways when there are errors collected during the runs - they will be printed to the terminal / console at the end of the run */

    printf("\nOPEN .AM FILE");
    if (!first_main_run(am_file)) {
        printf("\nSUCCESS first stage\n");
           /* Starts the first and second passes, and if they are finished successfully, writes the output files */
        if (!second_pass(am_file)) {
            printf("\nSUCCESS second stage\n");
                write_output_files(name);
        } else{
            printf("\nERRORS in first stage\n");
        }
    } else{
        printf("\nERRORS in first stage\n");
    }
 
    /* Closes the file after reading and frees the file_name string for the next file name */
    fclose(am_file);
    free(file_name);
    printf("\nCLOSED .AM FILE");
    printf("\n\t\t\tFINISHED ASSEMBLER SUCCESSFUL\n");


}


void write_output_files(char *name) {
    /* Calling to the methods that write each output file, with the name of the given file: */
    write_ob_file(name);
    write_ent_file(name);
    write_ext_file(name);
    printf("\nCreated the relevant files - .ob .ent .ext");
}



void write_ob_file(char *name) {
    int i;
    FILE *ob_file = get_file(name, ".ob"); /* Gets the file with ".ob" ending and writing permission */
    char data_table_size[3];
    char instruction_table_size[3];
    char ten_bit_word_in_base_thirty_two[3];
    char line_index[3];

    convert_decimal_32bit(ICF - START_OF_CODE_ADDRESS, instruction_table_size);
    convert_decimal_32bit(DCF, data_table_size);

    fprintf(ob_file, "\t %c %c \n", instruction_table_size[1],
            data_table_size[1]); /* Prints the number of instruction and data words */

    for (i = 0; i < ICF - START_OF_CODE_ADDRESS; i++) { /* First, prints each instruction generic_word */
        convert_decimal_32bit(i + START_OF_CODE_ADDRESS, line_index);
        convert_decimal_32bit(instruction_table[START_OF_CODE_ADDRESS + i]->word.data, ten_bit_word_in_base_thirty_two);
        fprintf(ob_file, "%s \t\t %s\n", line_index, ten_bit_word_in_base_thirty_two);
    }
    for (i = 0; i < DCF; i++) { /* Prints each data generic_word */
        convert_decimal_32bit(i + ICF, line_index);
        convert_decimal_32bit(data_table[i]->word.data, ten_bit_word_in_base_thirty_two);
        fprintf(ob_file, "%s \t\t %s\n", line_index, ten_bit_word_in_base_thirty_two);
    }
    fclose(ob_file); /* Frees the file pointer after use */
}

void write_ent_file(char *name) {
    FILE *ent_file_pointer;
    table p = general_symbols_table_head;
    char line_index[3];

    if (!entry_table_head)
        return;

    ent_file_pointer = get_file(name, ".ent"); /* Gets the file with ".ent" ending and writing permission */
    while (p) { /* Scanning the table */
        if (p->is_entry) {
            convert_decimal_32bit(p->value, line_index);
            fprintf(ent_file_pointer, "%s \t\t %s\n", p->label_name, line_index);
        }
        p = p->next;
    }
    fclose(ent_file_pointer); /* Frees the file pointer after use */
}


void write_ext_file(char *name) {
    FILE *ext_file_pointer;
    table p = refer_to_an_external_symbol_head;
    char line_index[3];

    if (!refer_to_an_external_symbol_head)
        return;

    ext_file_pointer = get_file(name, ".ext"); /* Gets the file with ".ext" ending and writing permission */
    while (p) { /* Scanning the table */
        if (p->is_extern) {
            convert_decimal_32bit(p->value, line_index);
            fprintf(ext_file_pointer, "%s \t\t %s\n", p->label_name, line_index);
        }
        p = p->next;
    }
    fclose(ext_file_pointer); /* Frees the file pointer after use */
}