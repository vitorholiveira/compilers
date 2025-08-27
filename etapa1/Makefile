TARGET = etapa1

CC = gcc

LEX_SRC = scanner.l
LEX_GEN = lex.yy.c
LEX_OBJ = lex.yy.o
MAIN_SRC = main.c
MAIN_OBJ = main.o

all: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(LEX_OBJ)
	@$(CC) $(MAIN_OBJ) $(LEX_OBJ) -o $(TARGET)

$(MAIN_OBJ): $(MAIN_SRC)
	@$(CC) -c $(MAIN_SRC)

$(LEX_OBJ): $(LEX_SRC)
	flex $(LEX_SRC)
	@$(CC) -c $(LEX_GEN)

clean:
	@rm -f $(TARGET) $(MAIN_OBJ) $(LEX_OBJ) $(LEX_GEN)
