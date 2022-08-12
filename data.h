#ifndef FINAL_MAMAN14_DATA_H
#define FINAL_MAMAN14_DATA_H

/*----------structs----------*/

/* types of words (all of them will set up the word_to_insert we will use in the data linked-list of the lines) */
typedef struct operation_word {
    unsigned int are: 2; /* default is 00 */
    unsigned int dst: 2;
    unsigned int src: 2;
    unsigned int opcode: 4;
} operation_word;

typedef struct field_num_word {
    unsigned int are: 2; /* should be 00 */
    unsigned int field_num: 8;/* could be 1 or 2 only */
} field_num_word;

/* could be the address of a record or a symbol (ARE could be extern or entry - 10 or 01) */
typedef struct address_word {
    unsigned int are: 2; /* 01 or 10 */
    unsigned int address_in_ic: 8;
} address_word;

typedef struct register_word {
    unsigned int are: 2; /* should be 00 */
    unsigned int dst: 4;
    unsigned int src: 4;
} register_word;

/* Struct of a word in assembly */
typedef struct word {
    unsigned int data: 10;
} word;
/* Union of a word in assembly */
typedef union {
    struct word word;
} word_to_insert;

/*----------tables----------*/

/* linked list */
typedef struct node {
    struct node *next;
    char *label_name;
    int value;
    int is_extern;
    int is_entry;
} node;

/* Pointer to a node in a linked list */
typedef struct node *table;

/* 5 known data words: .data .struct .string .entry .extern .. */
#define DATA_WORDS_DECLARATIONS 5

/* 8 registers r0, r1, r2, r3 ,r4, r5, r6, r7  */
#define NUM_REGISTERS 8
/* The maximal length of the machine code (256 bytes of 10 bits which starts IC from 100) */
#define MAX_INSTRUCTION_AND_DATA_TABLE_LEN 256

/*----------variables----------*/
/* Instruction counter & Data counter */
extern int IC , DC ;
/* Instruction counter & Data counter  final values*/
extern int ICF, DCF;

/* arrays to compare the ops, defs and regs names */
extern char *data_string_struct_entry_extern_declarations[DATA_WORDS_DECLARATIONS];
extern char *registers_names[NUM_REGISTERS];

/* The heads of the lists that contain the symbols: */
extern table data_table_head;
extern table code_table_head;
extern table entry_table_head;
extern table extern_table_head;
extern table refer_to_an_external_symbol_head;
extern table general_symbols_table_head;

/* the data and instruction tables */
extern word_to_insert *data_table[MAX_INSTRUCTION_AND_DATA_TABLE_LEN];
extern word_to_insert *instruction_table[MAX_INSTRUCTION_AND_DATA_TABLE_LEN];



/**
 * initializes the symbol tables heads to NULL
 */
void initialize_symbol_table_heads();

/**
 * allocate memory for specific cell in the instruction_table
 * @param index the index of the cell
 */
void allocate_memory_instruction_table(int index);

/**
 * allocate memory for specific cell in the data_table
 * @param index the index of the cell
 */
void allocate_memory_data_table(int index);

/**
 * allocate memory for new symbol table node
 * @param node the new node we want to allocate memory for
 * @param label_name the symbol name
 */
void allocate_memory_symbol_table(table *node, char *label_name);

/**
 * A function that inserts a symbol to list (given head of the list) while checking if the value exists in one of the tables
 * @param head The head of the list
 * @param label_name the symbol name we want to insert into the list
 * @param label_value the address of the symbol
 * @param is_extern boolean value to show if the symbol is extern
 * @param is_entry boolean value to show if the symbol is entry
 * @return Error if an error occurred and VALID if we added the symbol successfully
 */
int add_to_table(table *head, char *label_name, int label_value, int is_extern, int is_entry);

/**
 * A function that inserts a symbol to a list (given head of the list) without making a check if the value exists already
 * @param head The head of the list
 * @param label_name the symbol name we want to insert into the list
 * @param label_value the address of the symbol
 * @param is_extern boolean value to show if the symbol is extern
 * @param is_entry boolean value to show if the symbol is entry
 * @return Error if an error occurred and VALID if we added the symbol successfully
 */
int add_to_table_with_no_check(table *head, char *label_name, int label_value, int is_extern, int is_entry);


/**
 * Checks whether a given symbol name is on list whose head is given
 * @param head The head of the list
 * @param label_name the symbol name
 * @return True if the symbol is in the list and False otherwise
 */
int is_on_list(table head, char *label_name);

/**
 * Checks whether the symbol we are trying add to a symbol table list is not already on the list,
 * and whether it is "valid" to insert the symbol or is it an error, if there is an error, an error message is printed.
 * @param head the head of the list we are trying add the symbol to
 * @param label_name the symbol name
 * @return ERROR if there is an error and VALID otherwise
 */
int check_if_label_is_in_any_of_the_tables(table *head_pointer_to_the_tables, char *label_name);

/**
 * Increases the address of each symbol with type of data by ICF (line 19 in first_main_run algorithm)
 */
void increase_address_to_data();

/**
 * A function that inserts a symbol to a list (given head of the list) to the symbol table
 * @return None
 */
void add_symbols_from_data_table_to_symbols_table();

/**
 * Enter a number to the data table
 * @param num the number we want to enter
 */
void insert_number_to_data_table(int num);

/**
 * Enter a list of numbers to the data table
 * @param numbers a string of all the number separated by commas only
 */
void insert_numbers_to_data_table(char *numbers);

/**
 * insert every char in the string to the data table
 * @param s the string
 */
void insert_string_to_data_table(char *s);

/**
 * insert every char in the string / integer to the data table
 * uses both function - insert_String.. and insert_numbers
 * @param s the string and integer
 */
void insert_struct_to_data_table(char *s);

/**
 * checks if symbol_name is register, operation or data name (which cannot be used)
 * @param name the name of the variable
 * @return integer - boolean
 */
int is_valid_ref_to_address(char *name);

/**
 * checks if symbol_name is in the given array
 * @param list_of_definitions the names array
 * @param list_size how many cells list_of_definitions have
 * @param symbol_name the symbol_name we are looking for in list_of_definitions
 * @return TRUE if the symbol_name is in the given array and FALSE otherwise
 */
int is_symbol_name_equals_to_saved_names(char *list_of_definitions[], int list_size, char *symbol_name);

#endif
