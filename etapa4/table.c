#include "table.h"
#include <stdlib.h>
#include <stdio.h>

table_t* table_new(void)
{
    table_t* table = malloc(sizeof(table_t));
    if (table) {
        table->head = NULL;
        table->num_symbols = 0;
    }
    return table;
}

void table_free(table_t* table)
{
    if (!table) {
        printf("Error: %s received NULL symbol table = %p.\n", __FUNCTION__, table);
        return;
    }

    symbol_t* current = table->head;
    while (current) {
        symbol_t* temp = current;
        current = current->next;
        symbol_free(temp);
    }
    free(table);
}

symbol_t* table_add_symbol(table_t* table, symbol_t* symbol)
{
    if (!table || !symbol) {
        printf("Error: %s received NULL symbol table = %p / %p.\n", __FUNCTION__, table, symbol);
        return NULL;
    }

    /* Search for duplicate symbol name */
    symbol_t* duplicate = table_get_symbol(table, symbol->lex_value->value);
    if (duplicate) {
        return duplicate;
    }

    /* Insert at head of list */
    symbol->next = table->head;
    table->head = symbol;
    table->num_symbols++;
    
    return NULL;
}

symbol_t* table_get_symbol(table_t* table, const char* label)
{
    if (!table) {
        printf("Error: %s received NULL symbol table = %p.\n", __FUNCTION__, table);
        return NULL;
    }

    symbol_t* current = table->head;
    while (current) {
        if (strcmp(current->lex_value->value, label) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

symbol_t* symbol_new(nature_t nature, data_type_t data_type, lex_value_t* lex_value)
{
    symbol_t* symbol = calloc(1, sizeof(symbol_t));
    if (!symbol) {
        return NULL;
    }

    symbol->nature = nature;
    symbol->data_type = data_type;
    symbol->param_list = NULL;
    symbol->param_count = 0;
    symbol->next = NULL;

    /* Create local copy of lexical value */
    symbol->lex_value = malloc(sizeof(lex_value_t));
    if (symbol->lex_value) {
        symbol->lex_value->value = strdup(lex_value->value);
        symbol->lex_value->line = lex_value->line;
    }

    return symbol;
}

void symbol_free(symbol_t* symbol)
{
    if (!symbol) {
        return;
    }

    /* Validate parameter list is only on functions */
    if (symbol->param_list && symbol->nature != FUNCTION) {
        printf("Error: symbol of nature %d has params, but is not a FUNCTION\n",
               symbol->nature);
    }

    /* Free parameter chain */
    param_node_t* current = symbol->param_list;
    while (current) {
        param_node_t* temp = current;
        current = current->next;
        parameter_free(temp);
    }

    /* Free lexical value */
    if (symbol->lex_value) {
        free(symbol->lex_value->value);
        free(symbol->lex_value);
    }

    free(symbol);
}

void symbol_add_parameter(symbol_t* symbol, param_node_t* param)
{
    if (!symbol || !param) {
        printf("Error: %s received NULL symbol or param\n", __FUNCTION__);
        return;
    }

    if (symbol->nature != FUNCTION) {
        printf("Error: %s called on non-function (%d) symbol\n", __FUNCTION__, symbol->nature);
        return;
    }

    /* Append to end of parameter list */
    if (!symbol->param_list) {
        symbol->param_list = param;
    } else {
        param_node_t* tail = symbol->param_list;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = param;
    }
    
    symbol->param_count++;
}

void symbol_table_debug_print(table_t* table)
{
    if (!table || !table->head) {
        printf("  [empty symbol table]\n");
        return;
    }

    symbol_t* current = table->head;
    while (current) {
        printf("  - %s (nature: %d, data_type: %d, line: %d)\n",
               current->lex_value->value,
               (int)current->nature,
               (int)current->data_type,
               current->lex_value->line);

        /* Print parameters if present */
        if (current->param_list) {
            printf("    Parameters:\n");
            param_node_t* param = current->param_list;
            while (param) {
                printf("      • %s (data_type: %d)\n", param->label, (int)param->data_type);
                param = param->next;
            }
        }
        
        current = current->next;
    }
}

param_node_t* parameter_new(const char* label, data_type_t data_type)
{
    param_node_t* param = calloc(1, sizeof(param_node_t));
    if (!param) {
        return NULL;
    }

    param->label = strdup(label);
    param->data_type = data_type;
    param->next = NULL;
    
    return param;
}

void parameter_free(param_node_t* param)
{
    if (param) {
        free(param->label);
        free(param);
    }
}