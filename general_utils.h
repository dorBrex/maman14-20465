#pragma once
#ifndef FINAL_MAMAN14_GENERAL_UTILS_H
#define FINAL_MAMAN14_GENERAL_UTILS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

/* 16 Known operations in assembly such as - mov, jmp, hlt.. */
#define NUM_OPS 16
/* extern pointer to the operation name */
extern char *operations_names[NUM_OPS];
/* extern integer value of the current line */
extern int line_number;
/*as mention in page 23 - max line length is 81 characters with '\n' ending */
#define MAX_LINE_LENGTH 81
/* The address of the first word in the code */
#define START_OF_CODE_ADDRESS 100
/* The maximal length of label (including '\0' at the end) */
#define SYMBOL_LEN 30
/* a const for the slash zero '\0' single index in a string/ char*/
#define SLASH_ZERO_TO_END_STRING 1
/* boolean enumerate for true / false*/
enum bool {
    FALSE = 0, TRUE = 1
};
/* integered enumerate for ok (valid) or error code output */
enum status {
    ERROR = -1, OK = 0
};
/* operations and their decimal values */
enum reserved_words_num {
    MOV, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP,
    BNE, GET, PRN, JSR, RTS, HLT,
    DATA, STRING, STRUCT, ENTRY, EXTERN
};
/* 4 addressing methods */
enum addressing_methods {
    IMMEDIATE_ADDRESS, DIRECT_ADDRESS, ACCESS_TO_RECORD_ADDRESS, DIRECT_REGISTER_ADDRESS
};
/**
 * Gets the addressing method of the operand, stored in the argument parameter
 * @param argument the given operand in the instruction line
 * @return the number of the addressing method, according to the addressing_methods enum,
 *         or ERROR if it's given an invalid operand
 */
int get_address_method(char *are_argument);

/**
 * removing the white spaces in the current line
 * @param clean_result_str the output string
 * @param length_of_str len of the input string
 * @param line_to_clean the input string
 * @return the output string without white spaces
 */
size_t remove_white_spaces(char *clean_result_str, size_t length_of_str, const char *line_to_clean);
/**
 * wrapper for strncpy function
 * @param src the input string
 * @param dest the output string
 * @param start_index first index
 * @param end last index
 * @return none
 */
void slice(const char *src, char *dest, size_t start_index, size_t end);
/**
 * skipping the spaces in the current line
 * @param s the input string
 * @return the output string without spaces
 */
char *skip_spaces(char *s);

/**
 * Prints to stderr a custom error message plus the file name and the line number where the error is
 * @param the_error The error message to be printed
 * @return the value ERROR
 */
int print_error(char *the_error);

/**
 * Prints to stderr a custom error message (without file name and line number)
 * @param the_error The error to be printed
 * @return the value ERROR
 */
int print_error_only_message(char *the_error);

/**
 * Function to remove all spaces from a given string
 * @param str the string we want to remove the spaces from
 */
void remove_spaces(char *str);

/**
 * Classified if the given name (which supposed to be an operation or definition) is valid or not
 * @param name the name of the possible operation or definition
 * @return ERROR if the word is not an operation or definition, and a number representing the name otherwise
 */
int is_operation_name_or_declared_word(char *name);

/**
 The method receives a line number and data to convert it into binary and returns the binary of both as 32bit string ascii.
  @param num line number that will be converted to binary.
  @param converted_data data array (object) that will contain the converted to binary data.
*/
void convert_decimal_32bit(int num, char converted_data[]);
/**
 gets a file the return a pointer to the opened file
  @param name name of the file to be opened
  @param file_ending the file suffix
*/
FILE *get_file(char *name, char *file_ending);

#endif

