#pragma once
#ifndef FINAL_MAMAN14_SECOND_RUN_H
#define FINAL_MAMAN14_SECOND_RUN_H

/*----------second pass----------*/

#include <stdio.h>
/**
 * a main function for the second pass
 * @param source_file pointer to the source file
 * @return int to mention if true / false - passed or didn't (with or without errors)
 */
int second_pass(FILE *source_file);
/**
 * a function to process again each line from the .am file
 * @param line a string input - ref to line read
 * @return int to mention if true / false - passed or didn't (with or without errors)
 */
int second_pass_handle_line(char line[]);
/**
 * completing the missing data in the IC (instruction table) by the data and symbol tables
 * @param arguments the args (line with def symbol or def)
 * @return int to mention if true / false - passed or didn't (with or without errors)
 */
int complete_binary_encoding(char *arguments);
/**
 * A function that searches for the right values saved of the sumbols
 * @param label_name the label name
 * @return int to mention if true / false - passed or didn't (with or without errors)
 */
int find_label_value(char *label_name);
/**
 * A function that receives a register's name and returns its number
 * @param line pointer to the current line being processed
 * @param operation_declaration_val the operation or declaration word of the line (first, without the symbol)
 * @param arguments the args of the line without the op or def (first word)
 */
void second_parsing(char *line, int *operation_declaration_val, char arguments[]);

#endif
