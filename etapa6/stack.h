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
    /* Escopos que já foram "desempilhados" (blocos internos),
       mas cujas tabelas ainda não foram liberadas. Usado para
       postergar a liberação até stack_free, permitindo que a
       geração de código ainda acesse informações de símbolos. */
    scope_node_t* archived;
    int num_tables;
} stack_t;

/*
 * stack_new function, creates and initializes an empty scope stack.
 */
stack_t* stack_new(void);

/*
 * stack_free function, recursively frees all scope nodes and their symbol tables.
 */
void stack_free(stack_t* stack);

/*
 * stack_push function, creates a new scope level by pushing an empty symbol table onto the stack.
 */
void stack_push(stack_t* stack);

/*
 * stack_pop function, removes and frees the topmost scope level from the stack.
 */
void stack_pop(stack_t* stack);

/*
 * stack_declare_symbol function, declares a new symbol in the current scope's symbol table.
 */
void stack_declare_symbol(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value);

/*
 * stack_declare_function_parameter function, declares a function parameter in the current scope and adds it to the function's parameter list.
 */
void stack_declare_function_parameter(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value);

/*
 * stack_get_function function, retrieves the function symbol from the enclosing scope.
 */
symbol_t* stack_get_function(stack_t* stack);

/*
 * stack_get_symbol function, searches for a symbol by name through all scopes from top to bottom, reporting an error if not found.
 */
symbol_t* stack_get_symbol(stack_t* stack, const char* label, int line);

/*
 * stack_reset_local_offset function, resets the local offset counter when entering a new function scope.
 */
void stack_reset_local_offset(void);

/*
 * stack_get_local_offset function, gets the current local offset and increments it for the next variable.
 */
int stack_get_local_offset(void);

/*
 * stack_get_global_offset function, gets the current global offset and increments it for the next variable.
 */
int stack_get_global_offset(void);

/*
 * stack_is_global_scope function, checks if the current scope is the global scope (bottom of stack).
 */
int stack_is_global_scope(stack_t* stack);

#endif  // STACK_H