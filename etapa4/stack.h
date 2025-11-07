#ifndef STACK_H
#define STACK_H

#include "errors.h"
#include "table.h"

/* Scope node in linked stack */
typedef struct scope_node {
    table_t* table;
    struct scope_node* below;
} scope_node_t;

typedef struct {
    scope_node_t* top;
    int num_tables;
} stack_t;

stack_t* stack_new(void);

void stack_free(stack_t* stack);

void stack_push(stack_t* stack);

void stack_pop(stack_t* stack);

void stack_declare_symbol(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value);

void stack_declare_function_parameter(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value);

symbol_t* stack_get_function(stack_t* stack);

symbol_t* stack_get_symbol(stack_t* stack, const char* label, int line);

#endif  // STACK_H