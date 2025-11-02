#include "table.h"

table_t* table_new(void)
{
    table_t* new_table = NULL;
    new_table = calloc(1, sizeof(table_t));
    if (new_table != NULL) {
        new_table->num_symbols = 0;
        new_table->symbols = NULL;
    }
    return new_table;
}

void table_free(table_t* table)
{
    if (table == NULL) {
        printf("Error: %s received NULL symbol table = %p.\n", __FUNCTION__, table);
        return;
    }

    int i;
    for (i = 0; i < table->num_symbols; i++) {
        symbol_free(table->symbols[i]);
    }
    free(table->symbols);
    free(table);
}

symbol_t* table_add_symbol(table_t* table, symbol_t* symbol)
{
    if (table == NULL || symbol == NULL) {
        printf("Error: %s received NULL symbol table = %p / %p.\n", __FUNCTION__, table, symbol);
        return NULL;
    }

    int i;
    // Check if a symbol with the same name has already been delcared in the current scope
    for (i = 0; i < table->num_symbols; i++) {
        if (strcmp(table->symbols[i]->lex_value->value, symbol->lex_value->value) == 0) {
            return table->symbols[i];
        }
    }
    table->num_symbols++;
    table->symbols = realloc(table->symbols, table->num_symbols * sizeof(symbol_t*));
    table->symbols[table->num_symbols - 1] = symbol;
    return NULL;
}

symbol_t* table_get_symbol(table_t* table, const char* label)
{
    if (table == NULL) {
        printf("Error: %s received NULL symbol table = %p.\n", __FUNCTION__, table);
        return NULL;
    }

    int i;
    // Look for a symbol declared in the table that has the given label/name
    for (i = 0; i < table->num_symbols; i++) {
        if (strcmp(table->symbols[i]->lex_value->value, label) == 0) {
            return table->symbols[i];
        }
    }
    return NULL;
}

symbol_t* symbol_new(nature_t nature, data_type_t data_type, lex_value_t* lex_value)
{
    symbol_t* symbol = NULL;
    symbol = calloc(1, sizeof(symbol_t));
    if (symbol != NULL) {
        symbol->nature = nature;
        symbol->data_type = data_type;
        symbol->params = NULL;

        lex_value_t* local_copy = malloc(sizeof(lex_value_t));
        local_copy->value = strdup(lex_value->value);
        local_copy->line = lex_value->line;
        symbol->lex_value = local_copy;
    }
    return symbol;
}

void symbol_free(symbol_t* symbol)
{
    if (symbol == NULL) {
        return;
    }

    if (symbol->params != NULL) {
        if (symbol->nature != FUNCTION) {
            printf("Error: symbol of nature %d has params, but is not a FUNCTION\n",
                   symbol->nature);
        }

        int i;
        for (i = 0; i < symbol->params->num_params; i++) {
            parameter_free(symbol->params->params[i]);
        }
        free(symbol->params->params);
        free(symbol->params);
    }

    free(symbol->lex_value->value);
    free(symbol->lex_value);
    free(symbol);
}

void symbol_add_parameter(symbol_t* symbol, param_t* param)
{
    if (symbol == NULL || param == NULL) {
        printf("Error: %s received NULL symbol or param\n", __FUNCTION__);
        return;
    }

    if (symbol->nature != FUNCTION) {
        printf("Error: %s called on non-function (%d) symbol\n", __FUNCTION__, symbol->nature);
        return;
    }

    if (symbol->params == NULL) {
        symbol->params = calloc(1, sizeof(params_t));
        if (symbol->params == NULL) {
            printf("Error: failed to allocate params_t\n");
            exit(EXIT_FAILURE);
        }
    }

    symbol->params->num_params++;
    int num_params = symbol->params->num_params;

    symbol->params->params =
        realloc(symbol->params->params, num_params * sizeof(param_t*));
    symbol->params->params[num_params - 1] = param;
}

void symbol_table_debug_print(table_t* table)
{
    if (!table) {
        printf("  [empty symbol table]\n");
        return;
    }

    for (int i = 0; i < table->num_symbols; i++) {
        symbol_t* sym = table->symbols[i];
        printf("  - %s (nature: %d, data_type: %d, line: %d)\n", sym->lex_value->value, (int)sym->nature,
               (int)sym->data_type, sym->lex_value->line);

        // If the symbol has params, err_print them
        if (sym->params != NULL) {
            printf("    Parameters:\n");
            for (int j = 0; j < sym->params->num_params; j++) {
                param_t* param = sym->params->params[j];
                printf("      • %s (data_type: %d)\n", param->label, (int)param->data_type);
            }
        }
    }
}

param_t* parameter_new(const char* label, data_type_t data_type)
{
    param_t* param = NULL;
    param = calloc(1, sizeof(param_t));
    param->label = strdup(label);
    param->data_type = data_type;
    return param;
}

void parameter_free(param_t* param)
{
    if (param != NULL) {
        free(param->label);
    }
    free(param);
}
