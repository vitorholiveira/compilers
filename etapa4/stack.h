#ifndef STACK_H
#define STACK_H

#include "errors.h"
#include "table.h"

typedef struct {
    table_t** tables; /* uma tabela por escopo */
    int num_tables;
} stack_t;

/**
 * stack_new function, creates and initializes an empty scope stack.
 */
stack_t* stack_new(void);

/**
 * stack_free function, frees the scope stack and all contained symbol tables.
 */
void stack_free(stack_t* stack);

/**
 * scope_push function, pushes a new empty scope onto the stack.
 */
void stack_push(stack_t* stack);

/**
 * stack_pop, pops and frees the top scope from the stack.
 */
void stack_pop(stack_t* stack);

/**
 * stack_declare_symbol function, declares a new symbol in the current (top) scope.
 * Exits with ERR_DECLARED if the symbol is already declared in this scope.
 */
void stack_declare_symbol(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value);

/**
 * stack_declare_function_parameter function, declares a function parameter for the
 * function currently being defined.
 *
 * Assumes the function symbol is in the scope below the current one.
 * Adds the parameter symbol to both the current scope and the function's parameter list.
 */
void stack_declare_function_parameter(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value);

/**
 * stack_get_current_function function, retrieves the function symbol of the function whose scope is currently active.
 *
 * Assumes that the function symbol is stored in the scope immediately below the current scope,
 * and is the last symbol declared in that scope.
 */
symbol_t* stack_get_current_function(stack_t* stack);

/**
 * stack_get_symbol function, searches for a symbol by label in the scope stack.
 *
 * Searches from the innermost scope outward.
 *
 */
symbol_t* stack_get_symbol(stack_t* stack, const char* label, int line);

/**
 * stack_debug_print function, prints the scope stack and symbols for debugging.
 *
 * Shows each scope level and its contained symbols.
 */
void stack_debug_print(stack_t* stack);

#endif  // STACK_H
