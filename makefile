EXEC_FILE = Assembler
C_FILES = Assembler_main.c firstScan.c secondScan.c symbolTable.c instructions.c assemblerManager.c dataTable.c outputFiles.c
H_FILES = firstScan.h secondScan.h symbolTable.h instructions.h assemblerManager.h dataTable.h outputFiles.h common.h 

O_FILES = $(C_FILES:.c=.o)

all: $(EXEC_FILE)
$(EXEC_FILE): $(O_FILES) 
	gcc -Wall -ansi -pedantic $(O_FILES) -o $(EXEC_FILE) 
%.o: %.c $(H_FILES)
	gcc -Wall -ansi -pedantic -c -o $@ $<
clean:
	rm -f *.o $(EXEC_FILE)
