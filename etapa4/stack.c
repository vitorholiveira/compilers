#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

stack_t* stack_new(void)
{
    stack_t* stack = calloc(1, sizeof(stack_t));
    if (stack) {
        stack->top = NULL;
        stack->num_tables = 0;
    }
    return stack;
}

void stack_free(stack_t* stack)
{
    if (!stack) {
        return;
    }

    scope_node_t* current = stack->top;
    while (current) {
        scope_node_t* temp = current;
        current = current->below;
        table_free(temp->table);
        free(temp);
    }
    free(stack);
}

void stack_push(stack_t* stack)
{
    if (!stack) {
        printf("Error: %s received NULL scope stack = %p.\n", __FUNCTION__, stack);
        return;
    }

    scope_node_t* new_scope = malloc(sizeof(scope_node_t));
    if (!new_scope) {
        printf("Error: %s failed to allocate scope node.\n", __FUNCTION__);
        return;
    }

    new_scope->table = table_new();
    new_scope->below = stack->top;
    stack->top = new_scope;
    stack->num_tables++;
}

void stack_pop(stack_t* stack)
{
    if (!stack) {
        printf("Error: %s received NULL scope stack = %p.\n", __FUNCTION__, stack);
        return;
    }

    if (!stack->top) {
        printf("Error: %s called on empty stack.\n", __FUNCTION__);
        return;
    }

    scope_node_t* popped = stack->top;
    stack->top = popped->below;
    stack->num_tables--;
    
    table_free(popped->table);
    free(popped);
}

void stack_declare_symbol(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value)
{
    if (!stack || !stack->top) {
        printf("Error: %s called with invalid stack or empty scope stack.\n", __FUNCTION__);
        return;
    }

    table_t* current_scope = stack->top->table;
    symbol_t* symbol = symbol_new(nature, data_type, lex_value);
    symbol_t* declared_symbol = table_add_symbol(current_scope, symbol);

    if (declared_symbol) {
        char error_msg[100];
        sprintf(error_msg, "Identificador '%s' já foi declarado na linha %d.", symbol->lex_value->value, symbol->lex_value->line);
        print_err(symbol->lex_value->line, ERR_DECLARED, error_msg);
        stack_free(stack);
        exit(ERR_DECLARED);
    }
}

void stack_declare_function_parameter(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value)
{
    stack_declare_symbol(stack, nature, data_type, lex_value);

    symbol_t* func_symbol = stack_get_function(stack);
    param_node_t* new_param = parameter_new(lex_value->value, data_type);
    symbol_add_parameter(func_symbol, new_param);
}

symbol_t* stack_get_function(stack_t* stack)
{
    if (!stack || stack->num_tables < 2) {
        printf("Error: %s Not enough scopes to find function symbol (expected parent scope below parameters).\n",
               __FUNCTION__);
        return NULL;
    }

    scope_node_t* current = stack->top;
    while (current->below) {
        current = current->below;
    }

    table_t* function_table = current->table;
    if (!function_table->head) {
        printf("Error: %s expected at least one symbol in the function declaration scope.\n",
               __FUNCTION__);
        return NULL;
    }

    symbol_t* func_symbol = function_table->head;
    if (func_symbol->nature != FUNCTION) {
        printf("Error: %s expected last symbol in function declaration scope to be a function.\n",
               __FUNCTION__);
        return NULL;
    }

    return func_symbol;
}

symbol_t* stack_get_symbol(stack_t* stack, const char* label, int line)
{
    if (!stack) {
        printf("Error: %s received NULL stack.\n", __FUNCTION__);
        return NULL;
    }

    scope_node_t* current = stack->top;
    while (current) {
        symbol_t* found = table_get_symbol(current->table, label);
        if (found) {
            return found;
        }
        current = current->below;
    }

    char error_msg[100];
    sprintf(error_msg, "Identificador '%s' não foi declarado.", label);
    print_err(line, ERR_UNDECLARED, error_msg);
    exit(ERR_UNDECLARED);
}