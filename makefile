prog1: main.o second_run.o parsing_lines_to_binary.o macro_handler.o general_utils.o first_run.o file_handler.o data.o
	gcc -g -Wall -ansi -pedantic main.o data.o file_handler.o first_run.o general_utils.o macro_handler.o parsing_lines_to_binary.o second_run.o -o prog1

main.o: main.c file_handler.h
	gcc -c -Wall -ansi -pedantic main.c -o main.o

second_run.o: second_run.c second_run.h general_utils.h data.h
	gcc -c -Wall -ansi -pedantic second_run.c -o second_run.o

parsing_lines_to_binary.o: parsing_lines_to_binary.c parsing_lines_to_binary.h general_utils.h first_run.h file_handler.h data.h
	gcc -c -Wall -ansi -pedantic parsing_lines_to_binary.c -o parsing_lines_to_binary.o

macro_handler.o: macro_handler.c macro_handler.h general_utils.h
	gcc -c -Wall -ansi -pedantic macro_handler.c -o macro_handler.o

general_utils.o: general_utils.c file_handler.h general_utils.h first_run.h data.h
	gcc -c -Wall -ansi -pedantic general_utils.c -o general_utils.o

first_run.o: first_run.c first_run.h parsing_lines_to_binary.h file_handler.h general_utils.h data.h
	gcc -c -Wall -ansi -pedantic first_run.c -o first_run.o

file_handler.o: file_handler.c file_handler.h macro_handler.h first_run.h general_utils.h second_run.h 
	gcc -c -Wall -ansi -pedantic file_handler.c -o file_handler.o

data.o: data.c data.h general_utils.h first_run.h
	gcc -c -Wall -ansi -pedantic data.c -o data.o

clean:
	rm main.o data.o file_handler.o first_run.o general_utils.o macro_handler.o parsing_lines_to_binary.o second_run.o