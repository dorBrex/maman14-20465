#pragma once
#ifndef FINAL_MAMAN14_FIRST_RUN_H
#define FINAL_MAMAN14_FIRST_RUN_H
/*----------first pass----------*/

#include <stdio.h>
#include "parsing_lines_to_binary.h"
#include "file_handler.h"
#include "general_utils.h"
#include "data.h"
/**
 * Preforms the first pass on the given source file, searching for most of the syntax errors,
 * and translating some of the .as file into machine code.
 *
 * @param source_file - a pointer to the given file
 * @return TRUE or FALSE, according to the correctness of the file's syntax (FALSE if no error is found)
 */
int first_main_run(FILE *source_file);

/**
 * Executes the first pass algorithm on the current cur_line, searching for the syntax errors in the cur_line,
 * and translating some of the .as file into machine code.
 *
 * @param cur_line the current cur_line
 * @return ERROR or VALID, according to the correctness of the cur_line
 */
int check_if_line_is_valid(char cur_line[]);


/**
 * A function that receives a line, divides it into parts (for example symbol_name and variables)
 * and checks if it has syntax errors, if there are errors printed error message accordingly.
 *
 * @param line The line we check errors in (and parse)
 * @param symbol_name The label name to be passed to the first pass function
 * @param operation_or_decalred_word_holder A number representing the operation or definition in the line
 * @param variables The variables string to be passed to the first pass function
 * @return ERROR if the line is with errors and VALID otherwise
 */
int parse_line_data(char *line, char symbol_name[], int *operation_or_decalred_word_holder, char variables[]);

/*----------validation tests and checks for the first main run ----------*/



/**
 * Checks if the given label is valid or not
 * @param name the name of the possible label
 * @return TRUE if the label is valid and FALSE otherwise
 */
int is_valid_symbol(char *name);

/**
 * Checks whether the given string is an integer
 * @param s the given string
 * @return TRUE if s is an integer and FALSE otherwise
 */
int is_valid_digit(char *s);

/**
 * A function that checks commas errors in the arguments part in a line.
 * In addition, the function counts the number of arguments
 * if there is an error we print an appropriate error message
 *
 * @param s The string we check (the arguments)
 * @param count_arguments count how many arguments there are in this variable
 * @return ERROR if we found a comma error and VALID otherwise
 */
int is_valid_commas(char *s, int *count_arguments);

/**
 * Checks whether the part of the line after the .string definition is a valid string
 * In addition if the string is not valid then an appropriate error message is printed
 *
 * @param s the part of the line after the .string definition
 * @return ERROR if s is not valid and VALID otherwise
 */
int is_valid_string(char *s);

/**
 * check if the line has ':' in it (and that the ':' is not inside a string)
 * @param s the line
 * @return TRUE if we found ':' in the line (not inside a string) and FALSE otherwise
 */
int check_if_colon_in_line(char *s);

#endif