# Define some constants used throughout the Makefile.
# First line is used to avoid trouble on systems where the SHELL variable 
SHELL = /bin/sh
CC = g++
CFLAGS = -std=c++17 -Wall -g -I. 
OBJ = utilityfunc.o parent-child-IPC.o 
EXE = ipc
# SRC = main.c
# '$@' is used to represent the target 
# '$<' is used to represent the dependency 

$(EXE): $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $(OBJ)

# We can also use the below line as it generates the object file corresponding to their source.
%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ) $(EXE)

run:
	./$(EXE)
