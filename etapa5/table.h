#ifndef TABLE_H
#define TABLE_H

#include <string.h>
#include "asd.h"

/* Function parameter descriptor */
typedef struct param_node {
    char* label;
    data_type_t data_type;
    struct param_node* next;
} param_node_t;

/* Symbol entry */
typedef struct symbol_node {
    nature_t nature;
    data_type_t data_type;
    lex_value_t* lex_value;
    param_node_t* param_list;
    int param_count;
    struct symbol_node* next;
} symbol_t;

/* Symbol table using linked list */
typedef struct {
    symbol_t* head;
    int num_symbols;
} table_t;

/* Table management */

/*
 * table_new function, creates and initializes an empty symbol table.
 */
table_t* table_new(void);

/*
 * table_free function, recursively frees the symbol table and all its symbols.
 */
void table_free(table_t* table);

/* Symbol operations */

/*
 * table_add_symbol function, adds a symbol to the table and returns a pointer to it.
 */
symbol_t* table_add_symbol(table_t* table, symbol_t* symbol);

/*
 * table_get_symbol function, searches for a symbol by label and returns it, or NULL if not found.
 */
symbol_t* table_get_symbol(table_t* table, const char* label);

/* Symbol creation/destruction */

/*
 * symbol_new function, creates a new symbol with the given nature, data type, and lexical value.
 */
symbol_t* symbol_new(nature_t nature, data_type_t data_type, lex_value_t* lex_value);

/*
 * symbol_free function, frees a symbol and its associated parameter list.
 */
void symbol_free(symbol_t* symbol);

/*
 * symbol_add_parameter function, appends a parameter to the symbol's parameter list.
 */
void symbol_add_parameter(symbol_t* symbol, param_node_t* param);

/* Parameter operations */

/*
 * parameter_new function, creates a new parameter node with the given label and data type.
 */
param_node_t* parameter_new(const char* label, data_type_t data_type);

/*
 * parameter_free function, recursively frees a parameter node and all subsequent parameters in the list.
 */
void parameter_free(param_node_t* param);

#endif  // TABLE_H