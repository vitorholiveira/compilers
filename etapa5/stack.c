#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

/* Variáveis estáticas para rastrear offsets */
static int global_offset = 0;  // Offset para variáveis globais (relativo a rbss)
static int local_offset = 0;   // Offset para variáveis locais (relativo a rfp)

stack_t* stack_new(void)
{
    stack_t* stack = calloc(1, sizeof(stack_t));
    if (stack) {
        stack->top = NULL;
        stack->archived = NULL;
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

    /* Liberar também escopos arquivados (por exemplo, blocos internos
       cujo tempo de vida foi estendido até o final da compilação para
       permitir geração de código). */
    current = stack->archived;
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
        printf("Erro: %s recebeu pilha de escopo NULL = %p.\n", __FUNCTION__, stack);
        return;
    }

    scope_node_t* new_scope = malloc(sizeof(scope_node_t));
    if (!new_scope) {
        printf("Erro: %s falhou ao alocar nó de escopo.\n", __FUNCTION__);
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
        printf("Erro: %s recebeu pilha de escopo NULL = %p.\n", __FUNCTION__, stack);
        return;
    }

    if (!stack->top) {
        printf("Erro: %s chamado em pilha vazia.\n", __FUNCTION__);
        return;
    }

    scope_node_t* popped = stack->top;
    stack->top = popped->below;
    stack->num_tables--;

    /* Não liberamos imediatamente a tabela de símbolos ao fazer pop,
       pois informações de variáveis locais de blocos internos ainda
       podem ser necessárias durante a geração de código. Em vez disso,
       movemos o escopo para uma lista de arquivados, que será liberada
       apenas em stack_free. */
    popped->below = stack->archived;
    stack->archived = popped;
}

void stack_declare_symbol(stack_t* stack, nature_t nature, data_type_t data_type, lex_value_t* lex_value)
{
    if (!stack || !stack->top) {
        printf("Erro: %s chamado com pilha inválida ou pilha de escopo vazia.\n", __FUNCTION__);
        return;
    }

    table_t* current_scope = stack->top->table;
    symbol_t* symbol = symbol_new(nature, data_type, lex_value);
    
    // Calcular offset baseado no escopo (apenas para variáveis, não funções)
    if (nature == IDENTIFIER) {
        if (stack_is_global_scope(stack)) {
            // Variável global: offset positivo em relação a rbss
            symbol->offset = stack_get_global_offset();
        } else {
            // Variável local: offset negativo em relação a rfp
            symbol->offset = stack_get_local_offset();
        }
    } else {
        // Funções não têm offset
        symbol->offset = 0;
    }
    
    symbol_t* declared_symbol = table_add_symbol(current_scope, symbol);

    if (declared_symbol) {
        char error_msg[100];
        sprintf(error_msg, "Identificador '%s' já foi declarado.", symbol->lex_value->value);
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
        printf("Erro: %s Não há escopos suficientes para encontrar símbolo de função (esperado escopo pai abaixo dos parâmetros).\n",
               __FUNCTION__);
        return NULL;
    }

    scope_node_t* current = stack->top;
    while (current->below) {
        current = current->below;
    }

    table_t* function_table = current->table;
    if (!function_table->head) {
        printf("Erro: %s esperado pelo menos um símbolo no escopo de declaração da função.\n",
               __FUNCTION__);
        return NULL;
    }

    symbol_t* func_symbol = function_table->head;
    if (func_symbol->nature != FUNCTION) {
        printf("Erro: %s esperado que o último símbolo no escopo de declaração da função seja uma função.\n",
               __FUNCTION__);
        return NULL;
    }

    return func_symbol;
}

symbol_t* stack_get_symbol(stack_t* stack, const char* label, int line)
{
    if (!stack) {
        printf("Erro: %s recebeu pilha NULL.\n", __FUNCTION__);
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

/* ============================================================================
 * FUNÇÕES PARA GERENCIAMENTO DE OFFSETS
 * ============================================================================ */

void stack_reset_local_offset(void) {
    local_offset = 0;
}

int stack_get_local_offset(void) {
    // Offset positivo cresce para cima no frame
    // Primeira variável local: 0, segunda: 4, etc.
    int offset = local_offset;
    local_offset += 4;  // Assumindo que int ocupa 4 bytes
    return offset;
}

int stack_get_global_offset(void) {
    // Offset positivo cresce para cima no segmento de dados
    // Primeira variável global: 0, segunda: 4, etc.
    int offset = global_offset;
    global_offset += 4;  // Assumindo que int ocupa 4 bytes
    return offset;
}

int stack_is_global_scope(stack_t* stack) {
    if (!stack || stack->num_tables == 0) {
        return 0;
    }
    
    // O escopo global é sempre o primeiro escopo criado (bottom da pilha)
    // Se há apenas 1 escopo, é global
    if (stack->num_tables == 1) {
        return 1;
    }
    
    // Se há múltiplos escopos, encontrar o bottom
    scope_node_t* bottom = stack->top;
    while (bottom && bottom->below) {
        bottom = bottom->below;
    }
    
    // O escopo atual é global se for o bottom
    return (stack->top == bottom);
}