#ifndef STACK_H
#define STACK_H

#include "errors.h"
#include "table.h"

typedef struct {
    table_t** tables; /* uma tabela por escopo */
    int num_tables;
} stack_t;

stack_t* stack_new(void);

void stack_free(stack_t* stack);

void stack_push(stack_t* stack);

void stack_pop(stack_t* stack);

void stack_declare_symbol(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value);

void stack_declare_function_parameter(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value);

symbol_t* stack_get_current_function(stack_t* stack);

symbol_t* stack_get_symbol(stack_t* stack, const char* label, int line);

void stack_debug_print(stack_t* stack);

#endif  // STACK_H
