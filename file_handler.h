#pragma once
#ifndef FINAL_MAMAN14_FILE_HANDLER_H
#define FINAL_MAMAN14_FILE_HANDLER_H

extern char *file_name /* The name of the current file - extern in order to be used multiply times over the code */;

/**
 * open many files from argc argv while running the program ./
 * @param argc number of parameters passed to the program
 * @param argv values passed to the program in the terminal command line
 */
void open_many_files(int argc, char *argv[]);

/**
 * open single file
 * @param name the name of the file
 */
void open_single_file(char *name);

/**
 * creates and writes the output file ending with suffix .ent
 * @param name the name of the file
 */
void write_ent_file(char *name);
/**
 * creates and writes the output file ending with suffix .ext
 * @param index the name of the file
 */
void write_ext_file(char *name);
/**
 * creates and writes the output file ending with suffix .ob
 * @param name the name of the file
 */
void write_ob_file(char *name);
/**
 * creates and writes all of the 3 output files (run 3 different function underneath it)
 * @param name the name of the file
 */
void write_output_files(char *name);

#define AS_SUFFIX ".as"

#endif
