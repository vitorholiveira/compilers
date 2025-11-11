# Compiler and flags
CC     = gcc
LEX    = flex
YACC   = bison
CFLAGS = -g -fsanitize=address -Wall -Werror

# Source files
LEX_SRC  = scanner.l
YACC_SRC = parser.y

# Generated files
LEX_C  = lex.yy.c
YACC_C = parser.tab.c
YACC_H = parser.tab.h

# All C sources (including generated)
SRC = main.c asd.c stack.c table.c errors.c iloc.c $(YACC_C) $(LEX_C)

# Object files
OBJ = $(SRC:.c=.o)

# Executable
TARGET = etapa5

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lfl

# Bison parser generation
$(YACC_C) $(YACC_H): $(YACC_SRC)
	$(YACC) -d -o $(YACC_C) $(YACC_SRC)

# Flex lexer generation
$(LEX_C): $(LEX_SRC) $(YACC_H)
	$(LEX) -o $(LEX_C) $(LEX_SRC)

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(TARGET) *.o $(LEX_C) $(YACC_C) $(YACC_H) parser.output

# Create a tarball
tgz:
	tar -czvf etapa5.tgz main.c parser.y scanner.l asd.c asd.h errors.c errors.h stack.c stack.h table.c table.h Makefile
