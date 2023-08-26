#pragma once
#ifndef FINAL_MAMAN14_PARSING_LINES_TO_BINARY_H
#define FINAL_MAMAN14_PARSING_LINES_TO_BINARY_H

/* IMPORTANT INCLUDES */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

#include "first_run.h"
#include "file_handler.h"
#include "data.h"

/*----------arguments----------*/

/**
 * Creating machine code for an operation with zero operands (only one word)
 * @param operation a number which represents the operation (can be only RTS or STOP)
 * @return ERROR or VALID, according to the correctness of the operand and addressing method
 */
void zero_operands(int operation);

/**
 * Creating machine code for an operation with one operand (only first word and word of immediate address)
 * @param operation a number which represents the operation
 * @param operand the operand
 * @return ERROR or VALID, according to the correctness of the operand and addressing method
 */
int one_operand(int operation, char *operand);

/**
 * Creating machine code for an operation with 2 operands (only first word and words of immediate address)
 * @param operation a number which represents the operation
 * @param arguments the 2 operands separated by commas only
 * @return ERROR or VALID, according to the correctness of the operands and addressing method
 */
int two_operands(int operation, char *arguments);

/**
 * A function that receives a register's name and returns its number
 *
 * @param reg the register name
 * @return the register number, or ERROR if reg is not one of the registers
 */
int which_register(char *reg);


#endif
