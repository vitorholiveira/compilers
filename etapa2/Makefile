# Compilador
CC   = gcc
LEX  = flex
YACC = bison
CFLAGS = -Wall -g

# Fontes
LEX_SRC  = $(wildcard *.l)
YACC_SRC = $(wildcard *.y)

# Arquivos gerados
LEX_C  = $(LEX_SRC:.l=.yy.c)
YACC_C = $(YACC_SRC:.y=.tab.c)
YACC_H = $(YACC_SRC:.y=.tab.h)

OBJS = $(LEX_C:.c=.o) $(YACC_C:.c=.o)

# Nome do executável
TARGET = etapa2

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) -o $@ $^ -lfl

# Regras para gerar código a partir do Bison e Flex
%.tab.c %.tab.h: %.y
	@$(YACC) -d $<

%.yy.c: %.l $(YACC_H)
	@$(LEX) -o $@ $<

# Regra para compilar os .c
%.o: %.c
	@$(CC) $(CFLAGS) -c $<

clean:
	@rm -f $(TARGET) *.o *.yy.c *.tab.c *.tab.h
