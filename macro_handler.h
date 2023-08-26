#pragma once
#ifndef FINAL_MAMAN14_MACRO_HANDLER_H
#define FINAL_MAMAN14_MACRO_HANDLER_H

/*A few IMPORTANT assumptions before diving into the macro_handler job by page 23 in the maman14 notebook:
 * 1. each "macro" has "end_macro" in the .asm file
 * 2. a declaration of a macro will always come before the usage
 * 3. the pre-assembler / macro-handler creates a new file with the unfolded macros inside the .AM code file itself*/
/* More assumptions about he macro_handler job by page 32:
 * 1. There are no errors in the macro declarations !
 * 2. The pre-assembler stage isn't detecting errors - the assembler stages do !
 * by page 36 - the .AM file is going to hold the pre-assembler (macro unfolding) new file.
 * */


/* IMPORTANT CONSTS TO AVOID MAGIC STRINGS / NUMBERS*/

/* line legnth == maximal macro name length */
#define MACRO_NAME_LENGTH 80
#define AM_SUFFIX ".am"
#define SEPARATOR "."
#define START_OF_MACRO "macro"
#define END_OF_MACRO "endmacro"
/* IMPORTANT INCLUDES */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* a struct of the macro obj to save all the macro names in a linked-list*/
typedef struct Macro
{
    char *name;
    char *data;
    struct Macro *next;
} Macro;

/**
  main function to remove macro declaration lines and add the calls for the macros instead
  @param ptr_to_original_asm_file pointer to the .as file with macros
  @param name_of_file pointer to the string of the name of the .as file
*/
void macro_handler(FILE *ptr_to_original_asm_file, char *name_of_file);
/**
  checking that the new macro is valid
  @param head pointer to the head of the linked-list of macros saved already
  @param macro_name pointer to the string of the name of the new macro struct to check if its a valid name
*/
Macro *is_valid_macro(char *macro_name, Macro *head);
/**
  adding a new macro to the macro list
  @param head pointer to the head of the linked-list of macros
  @param newMacro pointer to the new macro struct to add to the linked-list of macros
  @return returns a Macro object to point to the new linked-list
*/
void  addToMacroList(Macro *newMacro,Macro *head);
/**
  releasing the memory saving the macros at the end of the positioning of the data of the macros
  @param head pointer to the head of the linked-list of macros
  @param head pointer to the head of the linked-list of macros
*/
void free_all_macros_linked_list(Macro *head);

#endif
