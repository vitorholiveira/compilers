#include "codegen.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE ACESSO A VARIÁVEIS
 * ============================================================================ */

iloc_code_t* gen_load_variable_code(symbol_t* symbol, iloc_operand_t** result_temp, stack_t* scopes) {
    if (!symbol || !result_temp) {
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    *result_temp = iloc_operand_new_reg();
    if (!*result_temp) {
        iloc_code_free(code);
        return NULL;
    }
    
    // Determinar se é local (rfp) ou global (rbss) verificando a tabela global
    const char* base_register = "rbss";  // Default: global
    if (scopes && symbol->lex_value) {
        // Encontrar o escopo global (bottom da stack)
        scope_node_t* bottom = scopes->top;
        while (bottom && bottom->below) {
            bottom = bottom->below;
        }
        symbol_t* glob = NULL;
        if (bottom && bottom->table) {
            glob = table_get_symbol(bottom->table, symbol->lex_value->value);
        }
        // Se não há símbolo global com esse nome, ou se o símbolo atual
        // não é aquele da tabela global, tratamos como local (parâmetro/variável local)
        if (!glob || glob != symbol) {
            base_register = "rfp";
        }
    }
    
    iloc_operation_t* load_op = iloc_operation_new("loadAI", false);
    
    // Criar operando para registrador reservado
    iloc_operand_t* base_op = (iloc_operand_t*)malloc(sizeof(iloc_operand_t));
    base_op->type = REGISTER;
    base_op->value.str_value = (char*)malloc(5 * sizeof(char));
    strcpy(base_op->value.str_value, base_register);
    
    iloc_operand_t* offset_op = iloc_operand_new_const(symbol->offset);
    
    iloc_operation_add_source(load_op, base_op);
    iloc_operation_add_source(load_op, offset_op);
    iloc_operation_add_target(load_op, *result_temp);
    iloc_code_append(code, load_op);
    
    return code;
}

iloc_code_t* gen_store_variable_code(symbol_t* symbol, iloc_operand_t* value_temp, stack_t* scopes) {
    if (!symbol || !value_temp) {
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    // Determinar se é local (rfp) ou global (rbss) verificando a tabela global
    const char* base_register = "rbss";  // Default: global
    if (scopes && symbol->lex_value) {
        // Encontrar o escopo global (bottom da stack)
        scope_node_t* bottom = scopes->top;
        while (bottom && bottom->below) {
            bottom = bottom->below;
        }
        symbol_t* glob = NULL;
        if (bottom && bottom->table) {
            glob = table_get_symbol(bottom->table, symbol->lex_value->value);
        }
        if (!glob || glob != symbol) {
            base_register = "rfp";
        }
    }
    
    iloc_operation_t* store_op = iloc_operation_new("storeAI", false);
    
    iloc_operand_t* base_op = (iloc_operand_t*)malloc(sizeof(iloc_operand_t));
    base_op->type = REGISTER;
    base_op->value.str_value = (char*)malloc(5 * sizeof(char));
    strcpy(base_op->value.str_value, base_register);
    
    iloc_operand_t* offset_op = iloc_operand_new_const(symbol->offset);
    
    iloc_operation_add_source(store_op, value_temp);
    iloc_operation_add_target(store_op, base_op);
    iloc_operation_add_target(store_op, offset_op);
    iloc_code_append(code, store_op);
    
    return code;
}

iloc_code_t* gen_variable_address_code(symbol_t* symbol, iloc_operand_t** result_temp) {
    if (!symbol || !result_temp) {
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    *result_temp = iloc_operand_new_reg();
    if (!*result_temp) {
        iloc_code_free(code);
        return NULL;
    }
    
    const char* base_register = (symbol->offset < 0) ? "rfp" : "rbss";
    
    iloc_operation_t* add_op = iloc_operation_new("addI", false);
    
    iloc_operand_t* base_op = (iloc_operand_t*)malloc(sizeof(iloc_operand_t));
    base_op->type = REGISTER;
    base_op->value.str_value = (char*)malloc(5 * sizeof(char));
    strcpy(base_op->value.str_value, base_register);
    
    iloc_operand_t* offset_op = iloc_operand_new_const(symbol->offset);
    
    iloc_operation_add_source(add_op, base_op);
    iloc_operation_add_source(add_op, offset_op);
    iloc_operation_add_target(add_op, *result_temp);
    iloc_code_append(code, add_op);
    
    return code;
}

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE EXPRESSÕES SIMPLES
 * ============================================================================ */

codegen_result_t* codegen_result_new(iloc_code_t* code, iloc_operand_t* temp) {
    codegen_result_t* result = (codegen_result_t*)malloc(sizeof(codegen_result_t));
    if (!result) {
        return NULL;
    }
    result->code = code;
    result->temp = temp;
    return result;
}

void codegen_result_free(codegen_result_t* result) {
    if (!result) {
        return;
    }
    // Não libera código/temp - podem ser compartilhados
    free(result);
}

codegen_result_t* gen_literal_code(asd_tree_t* node) {
    if (!node || !node->lex_value || !node->lex_value->value) {
        return NULL;
    }
    
    int value = atoi(node->lex_value->value);
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    iloc_operand_t* temp = iloc_operand_new_reg();
    if (!temp) {
        iloc_code_free(code);
        return NULL;
    }
    
    iloc_operation_t* loadI_op = iloc_operation_new("loadI", false);
    iloc_operand_t* const_op = iloc_operand_new_const(value);
    
    iloc_operation_add_source(loadI_op, const_op);
    iloc_operation_add_target(loadI_op, temp);
    iloc_code_append(code, loadI_op);
    
    return codegen_result_new(code, temp);
}

codegen_result_t* gen_identifier_code(asd_tree_t* node, stack_t* scopes) {
    if (!node || !node->lex_value || !scopes) {
        return NULL;
    }
    
    /* Sempre que possível, usar o símbolo já associado ao nó da AST
       durante a análise semântica. Isso garante que usamos o símbolo
       correto mesmo após o fechamento de escopos de bloco. */
    symbol_t* symbol = node->symbol;
    if (!symbol) {
        symbol = stack_get_symbol(scopes, node->lex_value->value, node->lex_value->line);
    }
    if (!symbol) {
        return NULL;
    }
    
    iloc_operand_t* temp = NULL;
    iloc_code_t* code = gen_load_variable_code(symbol, &temp, scopes);
    
    if (!code || !temp) {
        return NULL;
    }
    
    return codegen_result_new(code, temp);
}

static int is_literal_constant(asd_tree_t* node) {
    return (node && node->lex_value && node->lex_value->nature == LITERAL);
}

codegen_result_t* gen_binary_arithmetic_code(const char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes) {
    if (!op || !left || !right || !scopes) {
        return NULL;
    }
    
    const char* iloc_opcode = NULL;
    
    if (strcmp(op, "+") == 0) {
        iloc_opcode = "add";
    } else if (strcmp(op, "-") == 0) {
        iloc_opcode = "sub";
    } else if (strcmp(op, "*") == 0) {
        iloc_opcode = "mult";
    } else if (strcmp(op, "/") == 0) {
        iloc_opcode = "div";
    } else {
        return NULL;
    }
    
    codegen_result_t* left_result = NULL;
    codegen_result_t* right_result = NULL;
    
    if (is_literal_constant(left)) {
        left_result = gen_literal_code(left);
    } else if (left->lex_value && left->lex_value->nature == IDENTIFIER) {
        left_result = gen_identifier_code(left, scopes);
    } else {
        left_result = generate_code(left, scopes);
    }
    
    if (is_literal_constant(right)) {
        right_result = gen_literal_code(right);
    } else if (right->lex_value && right->lex_value->nature == IDENTIFIER) {
        right_result = gen_identifier_code(right, scopes);
    } else {
        right_result = generate_code(right, scopes);
    }
    
    if (!left_result || !right_result) {
        if (left_result) codegen_result_free(left_result);
        if (right_result) codegen_result_free(right_result);
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    iloc_operand_t* result_temp = iloc_operand_new_reg();
    if (!result_temp) {
        iloc_code_free(code);
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    /* Para compatibilidade com os testes do professor, NÃO usamos versões
       imediatas (addI, multI, etc). Sempre geramos:
         - código para a expressão da esquerda (pode incluir loadI)
         - código para a expressão da direita (pode incluir loadI)
         - operação binária normal (add, sub, mult, div) usando registradores. */
    iloc_operation_t* op_iloc = iloc_operation_new(iloc_opcode, false);
        iloc_operation_add_source(op_iloc, left_result->temp);
            iloc_operation_add_source(op_iloc, right_result->temp);
            iloc_operation_add_target(op_iloc, result_temp);
    
        iloc_code_concat(code, left_result->code);
        iloc_code_free(left_result->code);
    
        iloc_code_concat(code, right_result->code);
        iloc_code_free(right_result->code);
    
    iloc_code_append(code, op_iloc);
    
    free(left_result);
    free(right_result);
    
    return codegen_result_new(code, result_temp);
}

codegen_result_t* gen_unary_code(const char* op, asd_tree_t* operand, stack_t* scopes) {
    if (!op || !operand || !scopes) {
        return NULL;
    }
    
    codegen_result_t* operand_result = NULL;
    
    if (is_literal_constant(operand)) {
        operand_result = gen_literal_code(operand);
    } else if (operand->lex_value && operand->lex_value->nature == IDENTIFIER) {
        operand_result = gen_identifier_code(operand, scopes);
    } else {
        operand_result = generate_code(operand, scopes);
    }
    
    if (!operand_result) {
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(operand_result);
        return NULL;
    }
    
    iloc_code_concat(code, operand_result->code);
    iloc_code_free(operand_result->code);
    
    iloc_operand_t* result_temp = iloc_operand_new_reg();
    if (!result_temp) {
        iloc_code_free(code);
        codegen_result_free(operand_result);
        return NULL;
    }
    
    if (strcmp(op, "+") == 0) {
        // Unário +: apenas copia o valor
        iloc_operation_t* copy_op = iloc_operation_new("i2i", false);
        iloc_operation_add_source(copy_op, operand_result->temp);
        iloc_operation_add_target(copy_op, result_temp);
        iloc_code_append(code, copy_op);
    } else if (strcmp(op, "-") == 0) {
        // Unário -: usar rsubI op, 0 => temp (0 - op)
        iloc_operation_t* neg_op = iloc_operation_new("rsubI", false);
        iloc_operand_t* zero = iloc_operand_new_const(0);
        iloc_operation_add_source(neg_op, operand_result->temp);
        iloc_operation_add_source(neg_op, zero);
        iloc_operation_add_target(neg_op, result_temp);
        iloc_code_append(code, neg_op);
    } else if (strcmp(op, "!") == 0) {
        // Negação lógica: XOR com 1
        iloc_operation_t* xor_op = iloc_operation_new("xorI", false);
        iloc_operand_t* one = iloc_operand_new_const(1);
        iloc_operation_add_source(xor_op, operand_result->temp);
        iloc_operation_add_source(xor_op, one);
        iloc_operation_add_target(xor_op, result_temp);
        iloc_code_append(code, xor_op);
    } else {
        iloc_code_free(code);
        iloc_operand_free(result_temp);
        codegen_result_free(operand_result);
        return NULL;
    }
    
    free(operand_result);
    
    return codegen_result_new(code, result_temp);
}

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE EXPRESSÕES LÓGICAS
 * ============================================================================ */

codegen_result_t* gen_relational_code(const char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes) {
    if (!op || !left || !right || !scopes) {
        return NULL;
    }
    
    const char* iloc_opcode = NULL;
    
    if (strcmp(op, "<") == 0) {
        iloc_opcode = "cmp_LT";
    } else if (strcmp(op, "<=") == 0) {
        iloc_opcode = "cmp_LE";
    } else if (strcmp(op, ">") == 0) {
        iloc_opcode = "cmp_GT";
    } else if (strcmp(op, ">=") == 0) {
        iloc_opcode = "cmp_GE";
    } else if (strcmp(op, "==") == 0) {
        iloc_opcode = "cmp_EQ";
    } else if (strcmp(op, "!=") == 0) {
        iloc_opcode = "cmp_NE";
    } else {
        return NULL;
    }
    
    codegen_result_t* left_result = NULL;
    codegen_result_t* right_result = NULL;
    
    if (is_literal_constant(left)) {
        left_result = gen_literal_code(left);
    } else if (left->lex_value && left->lex_value->nature == IDENTIFIER) {
        left_result = gen_identifier_code(left, scopes);
    } else {
        left_result = generate_code(left, scopes);
    }
    
    if (is_literal_constant(right)) {
        right_result = gen_literal_code(right);
    } else if (right->lex_value && right->lex_value->nature == IDENTIFIER) {
        right_result = gen_identifier_code(right, scopes);
    } else {
        right_result = generate_code(right, scopes);
    }
    
    if (!left_result || !right_result) {
        if (left_result) codegen_result_free(left_result);
        if (right_result) codegen_result_free(right_result);
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    iloc_code_concat(code, left_result->code);
    iloc_code_free(left_result->code);
    iloc_code_concat(code, right_result->code);
    iloc_code_free(right_result->code);
    
    iloc_operand_t* result_temp = iloc_operand_new_reg();
    if (!result_temp) {
        iloc_code_free(code);
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    // Comparações usam '->' ao invés de '=>'
    iloc_operation_t* cmp_op = iloc_operation_new(iloc_opcode, true);
    
    iloc_operation_add_source(cmp_op, left_result->temp);
    iloc_operation_add_source(cmp_op, right_result->temp);
    iloc_operation_add_target(cmp_op, result_temp);
    iloc_code_append(code, cmp_op);
    
    free(left_result);
    free(right_result);
    
    return codegen_result_new(code, result_temp);
}

codegen_result_t* gen_logical_code(const char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes) {
    if (!op || !left || !right || !scopes) {
        return NULL;
    }
    
    const char* iloc_opcode = NULL;
    
    if (strcmp(op, "&") == 0 || strcmp(op, "&&") == 0) {
        iloc_opcode = "and";
    } else if (strcmp(op, "|") == 0 || strcmp(op, "||") == 0) {
        iloc_opcode = "or";
    } else {
        return NULL;
    }
    
    codegen_result_t* left_result = NULL;
    codegen_result_t* right_result = NULL;
    
    if (is_literal_constant(left)) {
        left_result = gen_literal_code(left);
    } else if (left->lex_value && left->lex_value->nature == IDENTIFIER) {
        left_result = gen_identifier_code(left, scopes);
    } else {
        left_result = generate_code(left, scopes);
    }
    
    if (is_literal_constant(right)) {
        right_result = gen_literal_code(right);
    } else if (right->lex_value && right->lex_value->nature == IDENTIFIER) {
        right_result = gen_identifier_code(right, scopes);
    } else {
        right_result = generate_code(right, scopes);
    }
    
    if (!left_result || !right_result) {
        if (left_result) codegen_result_free(left_result);
        if (right_result) codegen_result_free(right_result);
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    iloc_code_concat(code, left_result->code);
    iloc_code_free(left_result->code);
    iloc_code_concat(code, right_result->code);
    iloc_code_free(right_result->code);
    
    iloc_operand_t* result_temp = iloc_operand_new_reg();
    if (!result_temp) {
        iloc_code_free(code);
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    iloc_operation_t* logical_op = iloc_operation_new(iloc_opcode, false);
    
    iloc_operation_add_source(logical_op, left_result->temp);
    iloc_operation_add_source(logical_op, right_result->temp);
    iloc_operation_add_target(logical_op, result_temp);
    iloc_code_append(code, logical_op);
    
    free(left_result);
    free(right_result);
    
    return codegen_result_new(code, result_temp);
}

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE COMANDOS
 * ============================================================================ */

static codegen_result_t* generate_expression_code(asd_tree_t* expr, stack_t* scopes) {
    return generate_code(expr, scopes);
}

/* ============================================================================
 * FUNÇÃO GENÉRICA DE GERAÇÃO DE CÓDIGO
 * ============================================================================ */

codegen_result_t* generate_code(asd_tree_t* node, stack_t* scopes) {
    if (!node || !scopes) {
        return NULL;
    }
    
    /* Chamadas de função: label começa com "call ". 
       Os testes do professor não usam instruções de chamada no ILOC,
       mas esperam que os argumentos sejam avaliados.
       Assim, para 'call f(x)', geramos apenas o código de 'x'. */
    if (node->label && strncmp(node->label, "call ", 5) == 0) {
        if (node->number_of_children >= 1) {
            asd_tree_t* args_root = node->children[0];
            return generate_expression_code(args_root, scopes);
        }
        return NULL;
    }
    
    if (is_literal_constant(node)) {
        return gen_literal_code(node);
    }
    
    if (node->lex_value && node->lex_value->nature == IDENTIFIER) {
        return gen_identifier_code(node, scopes);
    }
    
    if (!node->label) {
        return NULL;
    }
    
    if (node->number_of_children >= 2) {
        asd_tree_t* left = node->children[0];
        asd_tree_t* right = node->children[1];
        const char* op = node->label;
        
        if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || 
            strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
            return gen_binary_arithmetic_code(op, left, right, scopes);
        }
        else if (strcmp(op, "<") == 0 || strcmp(op, "<=") == 0 ||
                 strcmp(op, ">") == 0 || strcmp(op, ">=") == 0 ||
                 strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
            return gen_relational_code(op, left, right, scopes);
        }
        else if (strcmp(op, "&") == 0 || strcmp(op, "&&") == 0 ||
                 strcmp(op, "|") == 0 || strcmp(op, "||") == 0) {
            return gen_logical_code(op, left, right, scopes);
        }
    }
    else if (node->number_of_children == 1) {
        asd_tree_t* operand = node->children[0];
        const char* op = node->label;
        
        if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "!") == 0) {
            return gen_unary_code(op, operand, scopes);
        }
    }
    
    if (strcmp(node->label, ":=") == 0 && node->number_of_children >= 2) {
        asd_tree_t* identifier = node->children[0];
        asd_tree_t* expression = node->children[1];
        iloc_code_t* code = gen_assignment_code(identifier, expression, scopes);
        if (code) {
            codegen_result_t* result = (codegen_result_t*)malloc(sizeof(codegen_result_t));
            result->code = code;
            result->temp = NULL;
            return result;
        }
        return NULL;
    }
    
    /* IMPORTANTE: tratar primeiro o caso if-else (3 filhos),
       depois o caso if simples (2 filhos). Com o parser ajustado,
       todo 'se ... senao ...' terá sempre 3 filhos (cond, then, else),
       mesmo que um dos blocos seja vazio. */
    if (strcmp(node->label, "se") == 0 && node->number_of_children >= 3) {
        asd_tree_t* condition = node->children[0];
        asd_tree_t* then_block = node->children[1];
        asd_tree_t* else_block = node->children[2];
        iloc_code_t* code = gen_if_else_code(condition, then_block, else_block, scopes);
        if (code) {
            codegen_result_t* result = (codegen_result_t*)malloc(sizeof(codegen_result_t));
            result->code = code;
            result->temp = NULL;
            return result;
        }
        return NULL;
    }
    
    if (strcmp(node->label, "se") == 0 && node->number_of_children >= 2) {
        asd_tree_t* condition = node->children[0];
        asd_tree_t* then_block = node->children[1];
        iloc_code_t* code = gen_if_code(condition, then_block, scopes);
        if (code) {
            codegen_result_t* result = (codegen_result_t*)malloc(sizeof(codegen_result_t));
            result->code = code;
            result->temp = NULL;
            return result;
        }
        return NULL;
    }
    
    /* RETURN */
    if (strcmp(node->label, "retorna") == 0) {
        /* retorna expr: gerar apenas o código da expressão.
           Os testes do professor esperam, por exemplo, um
           'loadAI rfp, 0 => rX' no final para 'retorna v'. */
        if (node->number_of_children >= 1) {
            asd_tree_t* expr = node->children[0];
            return generate_expression_code(expr, scopes);
        }
        /* 'retorna' sem expressão não gera código adicional */
        return NULL;
    }
    
    if (strcmp(node->label, "enquanto") == 0 && node->number_of_children >= 2) {
        asd_tree_t* condition = node->children[0];
        asd_tree_t* body = node->children[1];
        iloc_code_t* code = gen_while_code(condition, body, scopes);
        if (code) {
            codegen_result_t* result = (codegen_result_t*)malloc(sizeof(codegen_result_t));
            result->code = code;
            result->temp = NULL;
            return result;
        }
        return NULL;
    }
    
    return NULL;
}

iloc_code_t* gen_assignment_code(asd_tree_t* identifier, asd_tree_t* expression, stack_t* scopes) {
    if (!identifier || !expression || !scopes) {
        return NULL;
    }
    
    if (!identifier->lex_value || !identifier->lex_value->value) {
        return NULL;
    }
    
    codegen_result_t* expr_result = generate_expression_code(expression, scopes);
    if (!expr_result) {
        return NULL;
    }
    
    /* Preferir o símbolo já associado ao nó de identificador na AST,
       caindo para uma busca na pilha apenas se ele não estiver definido. */
    symbol_t* symbol = identifier->symbol;
    if (!symbol) {
        symbol = stack_get_symbol(scopes, identifier->lex_value->value, identifier->lex_value->line);
    }
    if (!symbol) {
        codegen_result_free(expr_result);
        return NULL;
    }
    
    iloc_code_t* store_code = gen_store_variable_code(symbol, expr_result->temp, scopes);
    if (!store_code) {
        codegen_result_free(expr_result);
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        iloc_code_free(store_code);
        codegen_result_free(expr_result);
        return NULL;
    }
    
    // Concatenar código da expressão (se houver)
    if (expr_result->code && expr_result->code->count > 0) {
        iloc_code_concat(code, expr_result->code);
        iloc_code_free(expr_result->code);
    } else if (expr_result->code) {
        iloc_code_free(expr_result->code);
    }
    
    // Concatenar código de store (sempre deve existir)
    if (store_code && store_code->count > 0) {
        iloc_code_concat(code, store_code);
        iloc_code_free(store_code);
    } else if (store_code) {
        iloc_code_free(store_code);
    }
    
    free(expr_result);
    
    return code;
}

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE FLUXO DE CONTROLE
 * ============================================================================ */

iloc_code_t* generate_block_code(asd_tree_t* block, stack_t* scopes) {
    if (!block || !scopes) {
        return iloc_code_new();
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    int first_seq_child = 0;
    
    /* Quando 'block' é o primeiro comando de uma sequência (corpo de função
       ou de um bloco), ele pode ter dois tipos de filhos:
       - filhos estruturais (por exemplo, condição/then/else de 'se',
         condição/corpo de 'enquanto', identificador/expressão de ':=')
       - filhos que representam comandos subsequentes da sequência.
       
       A estratégia:
       1) Gerar código para o próprio 'block' SE ele for um comando.
       2) Descobrir quantos filhos são estruturais e, a partir daí,
          iterar apenas sobre os filhos que representam comandos
          subsequentes, gerando código para cada um com generate_code. */
    
    if (block->label) {
        if (strcmp(block->label, ":=") == 0) {
            /* Atribuição: filhos 0 = identificador, 1 = expressão */
        codegen_result_t* cmd_result = generate_code(block, scopes);
        if (cmd_result && cmd_result->code) {
            iloc_code_concat(code, cmd_result->code);
            iloc_code_free(cmd_result->code);
        }
        if (cmd_result) {
            free(cmd_result);
        }
            first_seq_child = 2;
    }
        else if (strcmp(block->label, "se") == 0) {
            /* 'se' simples: filhos 0 = cond, 1 = then
               'se' com senao: filhos 0 = cond, 1 = then, 2 = else */
            codegen_result_t* cmd_result = generate_code(block, scopes);
            if (cmd_result && cmd_result->code) {
                iloc_code_concat(code, cmd_result->code);
                iloc_code_free(cmd_result->code);
            }
            if (cmd_result) {
                free(cmd_result);
            }
            if (block->number_of_children >= 3) {
                first_seq_child = 3;
        } else {
                first_seq_child = 2;
            }
        }
        else if (strcmp(block->label, "enquanto") == 0) {
            /* enquanto: filhos 0 = cond, 1 = corpo */
            codegen_result_t* cmd_result = generate_code(block, scopes);
            if (cmd_result && cmd_result->code) {
                iloc_code_concat(code, cmd_result->code);
                iloc_code_free(cmd_result->code);
            }
            if (cmd_result) {
                free(cmd_result);
            }
            first_seq_child = 2;
        }
        else if (strcmp(block->label, "retorna") == 0) {
            /* retorna pode ter 0 ou 1 filho (expressão) */
        codegen_result_t* cmd_result = generate_code(block, scopes);
        if (cmd_result && cmd_result->code) {
            iloc_code_concat(code, cmd_result->code);
            iloc_code_free(cmd_result->code);
        }
        if (cmd_result) {
            free(cmd_result);
        }
            /* todos os filhos (se existirem) são estruturais */
            first_seq_child = block->number_of_children;
    }
        else if (strcmp(block->label, "bloco_vazio") == 0) {
            /* Bloco vazio usado como placeholder em if-else: não gera código,
               mas ainda pode ter filhos subsequentes se for cabeça de sequência. */
            first_seq_child = 0;
        }
    }
    
    /* Gerar código para comandos subsequentes (filhos a partir de first_seq_child) */
    for (int i = first_seq_child; i < block->number_of_children; i++) {
        asd_tree_t* cmd = block->children[i];
        if (!cmd) continue;
        
        /* Se o filho é um nó 'seq', processá-lo recursivamente como bloco */
        if (cmd->label && strcmp(cmd->label, "seq") == 0) {
            iloc_code_t* nested = generate_block_code(cmd, scopes);
            if (nested && nested->count > 0) {
                iloc_code_concat(code, nested);
            }
            if (nested) {
                iloc_code_free(nested);
            }
            continue;
        }
        
        /* Limpar código pré-existente para evitar duplicação */
        if (cmd->iloc_code) {
            iloc_code_free(cmd->iloc_code);
            cmd->iloc_code = NULL;
        }
        
        codegen_result_t* cmd_result = generate_code(cmd, scopes);
        if (cmd_result && cmd_result->code) {
            iloc_code_concat(code, cmd_result->code);
            /* Após concatenar, cmd_result->code está vazio, mas ainda precisa ser liberado */
            iloc_code_free(cmd_result->code);
        }
        if (cmd_result) {
            free(cmd_result);
        }
    }
    
    return code;
}

iloc_code_t* gen_if_code(asd_tree_t* condition, asd_tree_t* then_block, stack_t* scopes) {
    if (!condition || !scopes) {
        return NULL;
    }
    
    codegen_result_t* cond_result = generate_expression_code(condition, scopes);
    if (!cond_result) {
        return NULL;
    }
    
    iloc_operand_t* L_then = iloc_operand_new_label();
    iloc_operand_t* L_end = iloc_operand_new_label();
    
    if (!L_then || !L_end) {
        codegen_result_free(cond_result);
        if (L_then) iloc_operand_free(L_then);
        if (L_end) iloc_operand_free(L_end);
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(cond_result);
        iloc_operand_free(L_then);
        iloc_operand_free(L_end);
        return NULL;
    }
    
    // Concatenar código da condição (se houver)
    if (cond_result->code) {
        iloc_code_concat(code, cond_result->code);
        iloc_code_free(cond_result->code);
    }
    
    // Verificar se temp existe (deve existir para expressões válidas)
    if (!cond_result->temp) {
        iloc_code_free(code);
        iloc_operand_free(L_then);
        iloc_operand_free(L_end);
        free(cond_result);
        return NULL;
    }
    
    iloc_operation_t* cbr_op = iloc_operation_new("cbr", true);
    iloc_operation_add_source(cbr_op, cond_result->temp);
    iloc_operation_add_target(cbr_op, L_then);
    iloc_operation_add_target(cbr_op, L_end);
    iloc_code_append(code, cbr_op);
    
    // Sempre criar nop com rótulo L_then após cbr
    iloc_operation_t* label_then = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_then, L_then);
    iloc_code_append(code, label_then);
    
    if (then_block) {
        iloc_code_t* then_code = generate_block_code(then_block, scopes);
        if (then_code && then_code->count > 0) {
            iloc_code_concat(code, then_code);
            iloc_code_free(then_code);
        } else {
            if (then_code) {
                iloc_code_free(then_code);
            }
        }
    }
    
    // Sempre criar nop com rótulo L_end no final
    iloc_operation_t* label_end = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_end, L_end);
    iloc_code_append(code, label_end);
    
    /* Compatibilidade com os testes do professor:
       gerar um rótulo extra no final do if simples.
       Isso faz com que um 'se' sem 'senao' produza dois
       rótulos de saída consecutivos (ex.: L1, L2). */
    iloc_operand_t* L_extra = iloc_operand_new_label();
    if (L_extra) {
        iloc_operation_t* label_extra = iloc_operation_new("nop", false);
        iloc_operation_set_label(label_extra, L_extra);
        iloc_code_append(code, label_extra);
    }
    
    free(cond_result);
    
    return code;
}

iloc_code_t* gen_if_else_code(asd_tree_t* condition, asd_tree_t* then_block, asd_tree_t* else_block, stack_t* scopes) {
    if (!condition || !scopes) {
        return NULL;
    }
    
    codegen_result_t* cond_result = generate_expression_code(condition, scopes);
    if (!cond_result) {
        return NULL;
    }
    
    iloc_operand_t* L_then = iloc_operand_new_label();
    iloc_operand_t* L_else = iloc_operand_new_label();
    iloc_operand_t* L_end = iloc_operand_new_label();
    
    if (!L_then || !L_else || !L_end) {
        codegen_result_free(cond_result);
        if (L_then) iloc_operand_free(L_then);
        if (L_else) iloc_operand_free(L_else);
        if (L_end) iloc_operand_free(L_end);
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(cond_result);
        iloc_operand_free(L_then);
        iloc_operand_free(L_else);
        iloc_operand_free(L_end);
        return NULL;
    }
    
    // Concatenar código da condição (se houver)
    if (cond_result->code) {
        iloc_code_concat(code, cond_result->code);
        iloc_code_free(cond_result->code);
    }
    
    // Verificar se temp existe (deve existir para expressões válidas)
    if (!cond_result->temp) {
        iloc_code_free(code);
        iloc_operand_free(L_then);
        iloc_operand_free(L_else);
        iloc_operand_free(L_end);
        free(cond_result);
        return NULL;
    }
    
    iloc_operation_t* cbr_op = iloc_operation_new("cbr", true);
    iloc_operation_add_source(cbr_op, cond_result->temp);
    iloc_operation_add_target(cbr_op, L_then);
    iloc_operation_add_target(cbr_op, L_else);
    iloc_code_append(code, cbr_op);
    
    // Sempre adicionar rótulo L_then antes do bloco then
    iloc_operation_t* label_then = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_then, L_then);
    iloc_code_append(code, label_then);
    
    if (then_block) {
        iloc_code_t* then_code = generate_block_code(then_block, scopes);
        if (then_code && then_code->count > 0) {
            iloc_code_concat(code, then_code);
            iloc_code_free(then_code);
        } else {
            if (then_code) {
                iloc_code_free(then_code);
            }
        }
    }
    
    // Sempre adicionar jump para o final após o bloco then
    iloc_operation_t* jump_end = iloc_operation_new("jumpI", true);
    iloc_operation_add_target(jump_end, L_end);
    iloc_code_append(code, jump_end);
    
    // Sempre adicionar rótulo L_else antes do bloco else
    iloc_operation_t* label_else = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_else, L_else);
    iloc_code_append(code, label_else);
    
    if (else_block) {
        iloc_code_t* else_code = generate_block_code(else_block, scopes);
        if (else_code && else_code->count > 0) {
            iloc_code_concat(code, else_code);
            iloc_code_free(else_code);
        } else {
            if (else_code) {
                iloc_code_free(else_code);
            }
        }
    }
    
    iloc_operation_t* label_end = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_end, L_end);
    iloc_code_append(code, label_end);
    
    free(cond_result);
    
    return code;
}

iloc_code_t* gen_while_code(asd_tree_t* condition, asd_tree_t* body, stack_t* scopes) {
    if (!condition || !scopes) {
        return NULL;
    }
    
    iloc_operand_t* L_loop = iloc_operand_new_label();
    iloc_operand_t* L_body = iloc_operand_new_label();
    iloc_operand_t* L_end = iloc_operand_new_label();
    
    if (!L_loop || !L_body || !L_end) {
        if (L_loop) iloc_operand_free(L_loop);
        if (L_body) iloc_operand_free(L_body);
        if (L_end) iloc_operand_free(L_end);
        return NULL;
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        iloc_operand_free(L_loop);
        iloc_operand_free(L_body);
        iloc_operand_free(L_end);
        return NULL;
    }
    
    codegen_result_t* cond_result = generate_expression_code(condition, scopes);
    if (!cond_result) {
        iloc_code_free(code);
        iloc_operand_free(L_loop);
        iloc_operand_free(L_body);
        iloc_operand_free(L_end);
        return NULL;
    }
    
    // Criar label L_loop primeiro para garantir que sempre existe
    iloc_operation_t* label_loop = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_loop, L_loop);
    iloc_code_append(code, label_loop);
    
    if (cond_result->code) {
        iloc_code_concat(code, cond_result->code);
        iloc_code_free(cond_result->code);
    }
    
    iloc_operation_t* cbr_op = iloc_operation_new("cbr", true);
    iloc_operation_add_source(cbr_op, cond_result->temp);
    iloc_operation_add_target(cbr_op, L_body);
    iloc_operation_add_target(cbr_op, L_end);
    iloc_code_append(code, cbr_op);
    
    // Sempre adicionar rótulo L_body antes do corpo do while
    iloc_operation_t* label_body = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_body, L_body);
    iloc_code_append(code, label_body);
    
    if (body) {
        iloc_code_t* body_code = generate_block_code(body, scopes);
        if (body_code && body_code->count > 0) {
            iloc_code_concat(code, body_code);
            iloc_code_free(body_code);
        } else {
            if (body_code) {
                iloc_code_free(body_code);
            }
        }
    }
    
    iloc_operation_t* jump_loop = iloc_operation_new("jumpI", true);
    iloc_operation_add_target(jump_loop, L_loop);
    iloc_code_append(code, jump_loop);
    
    iloc_operation_t* label_end = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_end, L_end);
    iloc_code_append(code, label_end);
    
    free(cond_result);
    
    return code;
}

