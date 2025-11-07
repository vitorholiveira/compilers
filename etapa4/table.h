
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
table_t* table_new(void);
void table_free(table_t* table);

/* Symbol operations */
symbol_t* table_add_symbol(table_t* table, symbol_t* symbol);
symbol_t* table_get_symbol(table_t* table, const char* label);

/* Symbol creation/destruction */
symbol_t* symbol_new(nature_t nature, data_type_t data_type, lex_value_t* lex_value);
void symbol_free(symbol_t* symbol);
void symbol_add_parameter(symbol_t* symbol, param_node_t* param);

/* Parameter operations */
param_node_t* parameter_new(const char* label, data_type_t data_type);
void parameter_free(param_node_t* param);

#endif  // TABLE_H