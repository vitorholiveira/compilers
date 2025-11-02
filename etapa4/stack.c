#include "stack.h"

stack_t* stack_new(void)
{
    stack_t* new_stack = NULL;
    new_stack = calloc(1, sizeof(stack_t));
    if (new_stack != NULL) {
        new_stack->num_tables = 0;
        new_stack->tables = NULL;
    }
    return new_stack;
}

void stack_free(stack_t* stack)
{
    int i;
    for (i = 0; i < stack->num_tables; i++) {
        table_free(stack->tables[i]);
    }
    free(stack->tables);
    free(stack);
}

void stack_push(stack_t* stack)
{
    if (stack == NULL) {
        printf("Error: %s received NULL scope stack = %p.\n", __FUNCTION__, stack);
        return;
    }

    table_t* new_scope = table_new();
    stack->num_tables++;
    stack->tables = realloc(stack->tables, stack->num_tables * sizeof(table_t*));
    stack->tables[stack->num_tables - 1] = new_scope;
}

void stack_pop(stack_t* stack)
{
    if (stack == NULL) {
        printf("Error: %s received NULL scope stack = %p.\n", __FUNCTION__, stack);
        return;
    }

    table_free(stack->tables[stack->num_tables - 1]);
    stack->num_tables--;
    stack->tables = realloc(stack->tables, stack->num_tables * sizeof(table_t*));
}

void stack_declare_symbol(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value)
{
    if (stack == NULL || stack->num_tables == 0) {
        printf("Error: %s called with invalid stack or empty scope stack.\n", __FUNCTION__);
        return;
    }

    table_t* current_scope = stack->tables[stack->num_tables - 1];
    symbol_t* symbol = symbol_new(nature, data_type, lex_value);
    symbol_t* declared_symbol = table_add_symbol(current_scope, symbol);

    // If failed to add to table, symbol was already declared
    if (declared_symbol != NULL) {
        err_print_declared(symbol->lex_value->value, symbol->lex_value->line,
                               declared_symbol->lex_value->line);
        stack_free(stack);
        exit(ERR_DECLARED);
    }
}

void stack_declare_function_parameter(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value)
{
    stack_declare_symbol(stack, nature, data_type, lex_value);

    symbol_t* func_symbol = stack_get_current_function(stack);
    param_t* new_param = parameter_new(lex_value->value, data_type);
    symbol_add_parameter(func_symbol, new_param);
}

symbol_t* stack_get_current_function(stack_t* stack)
{
    if (stack == NULL || stack->num_tables < 2) {
        printf(
            "Error: %s Not enough scopes to find function symbol (expected parent scope below "
            "parameters).\n",
            __FUNCTION__);
        return NULL;
    }

    table_t* function_decl_table = stack->tables[0];
    if (function_decl_table->num_symbols == 0) {
        printf("Error: %s expected at least one symbol in the function declaration scope.\n",
               __FUNCTION__);
        return NULL;
    }

    symbol_t* func_symbol = function_decl_table->symbols[function_decl_table->num_symbols - 1];
    if (func_symbol->nature != FUNCTION) {
        printf("Error: %s expected last symbol in function declaration scope to be a function.\n",
               __FUNCTION__);
        return NULL;
    }

    return func_symbol;
}

symbol_t* stack_get_symbol(stack_t* stack, const char* label, int line)
{
    // Search from innermost to outermost scope
    for (int i = stack->num_tables - 1; i >= 0; i--) {
        symbol_t* declared_symbol = table_get_symbol(stack->tables[i], label);
        if (declared_symbol != NULL) {
            return declared_symbol;
        }
    }

    // Not found: undeclared identifier being used
    err_print_undeclared(label, line);
    stack_free(stack);
    exit(ERR_UNDECLARED);
}

void stack_debug_print(stack_t* stack)
{
    if (!stack) {
        printf("[Debug] Scope stack is NULL.\n");
        return;
    }

    printf("\n\n========== Scope Stack (Top to Bottom) ==========\n");

    for (int i = stack->num_tables - 1; i >= 0; i--) {
        printf("┌── Scope Level %d ───────────────────────────────\n", i);
        symbol_table_debug_print(stack->tables[i]);
        if (i > 0)
            printf(
                "└───────────────────────────────────────────────┐\n"
                "                 ↓\n");
        else
            printf("└───────────────────────────────────────────────┘\n");
    }

    printf("=================================================\n\n");
}
