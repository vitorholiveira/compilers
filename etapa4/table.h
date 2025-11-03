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

table_t* table_new(void);

void table_free(table_t* table);

symbol_t* table_add_symbol(table_t* table, symbol_t* symbol);

symbol_t* table_get_symbol(table_t* table, const char* label);

symbol_t* symbol_new(nature_t nature, data_type_t data_type, lex_value_t* lex_value);

void symbol_free(symbol_t* symbol);

void symbol_add_parameter(symbol_t* symbol, param_t* param);

void symbol_table_debug_print(table_t* table);

param_t* parameter_new(const char* label, data_type_t data_type);

void parameter_free(param_t* param);

#endif  // TABLE_H
