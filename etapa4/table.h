#ifndef TABLE_H
#define TABLE_H

#include <string.h>

#include "asd.h"

typedef struct {
    char* label;
    data_type_t data_type; /* (INTEGER ou DECIMAL) */
} param_t;

typedef struct {
    int num_params;       
    param_t** params;
} params_t;

typedef struct {
    nature_t nature;                /* (IDENTIFIER, LITERAL ou FUNCTION) */
    data_type_t data_type;  /* (INTEGER ou DECIMAL) */
    params_t* params;           /* Parâmetros da função (NULL se for um número) */
    lex_value_t* lex_value;
} symbol_t;

typedef struct {
    symbol_t** symbols;
    int num_symbols;
} table_t;

/**
 * table_new function, creates and initializes an empty symbol table.
 */
table_t* table_new(void);

/**
 * table_free, frees the symbol table and all associated symbols.
 */
void table_free(table_t* table);

/**
 * table_add_symbol, adds a symbol to the symbol table if not already declared.
 * If a symbol with the same label exists, returns a pointer to it.
 * Otherwise, adds the new symbol and returns NULL.
 */
symbol_t* table_add_symbol(table_t* table, symbol_t* symbol);

/**
 * table_get_symbol function, searches for a symbol by label in the symbol table.
 */
symbol_t* table_get_symbol(table_t* table, const char* label);

/**
 * symbol_new function, creates a new symbol with a deep copy of the lexical value.
 */
symbol_t* symbol_new(nature_t nature, data_type_t data_type, lex_value_t* lex_value);

/**
 * symbol_free function, frees the memory associated with a symbol.
 */
void symbol_free(symbol_t* symbol);

/**
 * symbol_add_parameter function, adds a parameter to a function symbol's parameter list.
 */
void symbol_add_parameter(symbol_t* symbol, param_t* param);

/**
 * symbol_table_debug_print function, prints the symbol table contents for debugging.
 */
void symbol_table_debug_print(table_t* table);

/**
 * parameter_new function, creates a new function parameter.
 * Allocates and initializes a parameter with the given label and nature.
 */
param_t* parameter_new(const char* label, data_type_t data_type);

/**
 * parameter_free function, frees a function parameter.
 */
void parameter_free(param_t* param);

#endif  // TABLE_H
