CC    = gcc
LEX   = flex
YACC  = bison
CFLAGS = -Wall -g

LEX_SRC  = scanner.l
YACC_SRC = parser.y

LEX_C  = lex.yy.c
YACC_C  = parser.tab.c
YACC_H  = parser.tab.h

OBJS = parser.tab.o lex.yy.o main.o

TARGET = etapa2

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lfl

$(YACC_C) $(YACC_H): $(YACC_SRC)
	$(YACC) -d $<

$(LEX_C): $(LEX_SRC) $(YACC_H)
	$(LEX) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) *.o lex.yy.c parser.tab.c parser.tab.h parser.output
