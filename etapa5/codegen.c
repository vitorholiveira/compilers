#include "codegen.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE ACESSO A VARIÁVEIS
 * ============================================================================ */

iloc_code_t* gen_load_variable_code(symbol_t* symbol, iloc_operand_t** result_temp) {
    if (!symbol || !result_temp) {
        return NULL;
    }
    
    // Criar código vazio
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    // Criar temporário para resultado
    *result_temp = iloc_operand_new_reg();
    if (!*result_temp) {
        iloc_code_free(code);
        return NULL;
    }
    
    // Determinar se é variável local ou global baseado no offset
    // Offset negativo = local (relativo a rfp)
    // Offset positivo/zero = global (relativo a rbss)
    const char* base_register = (symbol->offset < 0) ? "rfp" : "rbss";
    
    // Criar operação loadAI
    iloc_operation_t* load_op = iloc_operation_new("loadAI", false);
    
    // Criar operando para registrador base (rfp ou rbss)
    // Precisamos criar operandos especiais para registradores reservados
    iloc_operand_t* base_op = (iloc_operand_t*)malloc(sizeof(iloc_operand_t));
    base_op->type = REGISTER;
    base_op->value.str_value = (char*)malloc(5 * sizeof(char));
    strcpy(base_op->value.str_value, base_register);
    
    // Criar operando para offset
    iloc_operand_t* offset_op = iloc_operand_new_const(symbol->offset);
    
    // Adicionar operandos fonte
    iloc_operation_add_source(load_op, base_op);
    iloc_operation_add_source(load_op, offset_op);
    
    // Adicionar operando alvo
    iloc_operation_add_target(load_op, *result_temp);
    
    // Adicionar operação ao código
    iloc_code_append(code, load_op);
    
    return code;
}

iloc_code_t* gen_store_variable_code(symbol_t* symbol, iloc_operand_t* value_temp) {
    if (!symbol || !value_temp) {
        return NULL;
    }
    
    // Criar código vazio
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    // Determinar registrador base
    const char* base_register = (symbol->offset < 0) ? "rfp" : "rbss";
    
    // Criar operação storeAI
    iloc_operation_t* store_op = iloc_operation_new("storeAI", false);
    
    // Criar operando para registrador base
    iloc_operand_t* base_op = (iloc_operand_t*)malloc(sizeof(iloc_operand_t));
    base_op->type = REGISTER;
    base_op->value.str_value = (char*)malloc(5 * sizeof(char));
    strcpy(base_op->value.str_value, base_register);
    
    // Criar operando para offset
    iloc_operand_t* offset_op = iloc_operand_new_const(symbol->offset);
    
    // Adicionar operando fonte (valor a ser armazenado)
    iloc_operation_add_source(store_op, value_temp);
    
    // Adicionar operandos alvo (base e offset)
    iloc_operation_add_target(store_op, base_op);
    iloc_operation_add_target(store_op, offset_op);
    
    // Adicionar operação ao código
    iloc_code_append(code, store_op);
    
    return code;
}

iloc_code_t* gen_variable_address_code(symbol_t* symbol, iloc_operand_t** result_temp) {
    if (!symbol || !result_temp) {
        return NULL;
    }
    
    // Criar código vazio
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    // Criar temporário para resultado
    *result_temp = iloc_operand_new_reg();
    if (!*result_temp) {
        iloc_code_free(code);
        return NULL;
    }
    
    // Determinar registrador base
    const char* base_register = (symbol->offset < 0) ? "rfp" : "rbss";
    
    // Criar operação addI para calcular endereço
    iloc_operation_t* add_op = iloc_operation_new("addI", false);
    
    // Criar operando para registrador base
    iloc_operand_t* base_op = (iloc_operand_t*)malloc(sizeof(iloc_operand_t));
    base_op->type = REGISTER;
    base_op->value.str_value = (char*)malloc(5 * sizeof(char));
    strcpy(base_op->value.str_value, base_register);
    
    // Criar operando para offset
    iloc_operand_t* offset_op = iloc_operand_new_const(symbol->offset);
    
    // Adicionar operandos fonte
    iloc_operation_add_source(add_op, base_op);
    iloc_operation_add_source(add_op, offset_op);
    
    // Adicionar operando alvo
    iloc_operation_add_target(add_op, *result_temp);
    
    // Adicionar operação ao código
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
    // Não liberar código e temp aqui - eles podem ser compartilhados
    // A liberação deve ser feita separadamente
    free(result);
}

/* Passo 4.1: Geração para literais inteiros */
codegen_result_t* gen_literal_code(asd_tree_t* node) {
    if (!node || !node->lex_value || !node->lex_value->value) {
        return NULL;
    }
    
    // Converter string para inteiro
    int value = atoi(node->lex_value->value);
    
    // Criar código vazio
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    // Criar temporário para resultado
    iloc_operand_t* temp = iloc_operand_new_reg();
    if (!temp) {
        iloc_code_free(code);
        return NULL;
    }
    
    // Criar operação loadI
    iloc_operation_t* loadI_op = iloc_operation_new("loadI", false);
    
    // Criar operando constante
    iloc_operand_t* const_op = iloc_operand_new_const(value);
    
    // Adicionar operando fonte (constante)
    iloc_operation_add_source(loadI_op, const_op);
    
    // Adicionar operando alvo (temporário)
    iloc_operation_add_target(loadI_op, temp);
    
    // Adicionar operação ao código
    iloc_code_append(code, loadI_op);
    
    return codegen_result_new(code, temp);
}

/* Passo 4.2: Geração para identificadores (variáveis) */
codegen_result_t* gen_identifier_code(asd_tree_t* node, stack_t* scopes) {
    if (!node || !node->lex_value || !scopes) {
        return NULL;
    }
    
    // Buscar símbolo na tabela de símbolos
    symbol_t* symbol = stack_get_symbol(scopes, node->lex_value->value, node->lex_value->line);
    if (!symbol) {
        return NULL;
    }
    
    // Gerar código para carregar variável
    iloc_operand_t* temp = NULL;
    iloc_code_t* code = gen_load_variable_code(symbol, &temp);
    
    if (!code || !temp) {
        return NULL;
    }
    
    return codegen_result_new(code, temp);
}

/* Função auxiliar para verificar se um nó é uma constante literal */
static int is_literal_constant(asd_tree_t* node) {
    return (node && node->lex_value && node->lex_value->nature == LITERAL);
}

/* Passo 4.3: Geração para operações aritméticas binárias (com otimização) */
codegen_result_t* gen_binary_arithmetic_code(const char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes) {
    if (!op || !left || !right || !scopes) {
        return NULL;
    }
    
    // Determinar opcode ILOC baseado no operador
    const char* iloc_opcode = NULL;
    const char* iloc_opcode_immediate = NULL;
    
    if (strcmp(op, "+") == 0) {
        iloc_opcode = "add";
        iloc_opcode_immediate = "addI";
    } else if (strcmp(op, "-") == 0) {
        iloc_opcode = "sub";
        iloc_opcode_immediate = "subI";
    } else if (strcmp(op, "*") == 0) {
        iloc_opcode = "mult";
        iloc_opcode_immediate = "multI";
    } else if (strcmp(op, "/") == 0) {
        iloc_opcode = "div";
        iloc_opcode_immediate = "divI";
    } else {
        return NULL; // Operador não suportado
    }
    
    // Gerar código para operandos (recursivamente)
    codegen_result_t* left_result = NULL;
    codegen_result_t* right_result = NULL;
    
    // Verificar se left é literal ou precisa gerar código
    if (is_literal_constant(left)) {
        left_result = gen_literal_code(left);
    } else {
        // Chamar função genérica de geração (será implementada na Fase 8)
        // Por enquanto, assumir que é identificador ou expressão complexa
        if (left->lex_value && left->lex_value->nature == IDENTIFIER) {
            left_result = gen_identifier_code(left, scopes);
        }
        // TODO: Chamar generate_code recursivamente quando implementar Fase 8
    }
    
    // Verificar se right é literal ou precisa gerar código
    if (is_literal_constant(right)) {
        right_result = gen_literal_code(right);
    } else {
        // Chamar função genérica de geração (será implementada na Fase 8)
        if (right->lex_value && right->lex_value->nature == IDENTIFIER) {
            right_result = gen_identifier_code(right, scopes);
        }
        // TODO: Chamar generate_code recursivamente quando implementar Fase 8
    }
    
    if (!left_result || !right_result) {
        if (left_result) codegen_result_free(left_result);
        if (right_result) codegen_result_free(right_result);
        return NULL;
    }
    
    // Criar código combinado
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    // Concatenar códigos dos operandos
    iloc_code_concat(code, left_result->code);
    iloc_code_concat(code, right_result->code);
    
    // Criar temporário para resultado
    iloc_operand_t* result_temp = iloc_operand_new_reg();
    if (!result_temp) {
        iloc_code_free(code);
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    // Passo 4.4: Otimização - usar versão imediata se um operando é constante
    iloc_operation_t* op_iloc = NULL;
    
    if (is_literal_constant(right)) {
        // Usar versão imediata: opI left_temp, constante => result_temp
        op_iloc = iloc_operation_new(iloc_opcode_immediate, false);
        iloc_operation_add_source(op_iloc, left_result->temp);
        iloc_operation_add_source(op_iloc, right_result->temp); // Usa a constante já carregada
        iloc_operation_add_target(op_iloc, result_temp);
    } else if (is_literal_constant(left) && (strcmp(op, "+") == 0 || strcmp(op, "*") == 0)) {
        // Para operações comutativas, podemos usar versão imediata com left constante
        if (strcmp(op, "+") == 0) {
            op_iloc = iloc_operation_new("addI", false);
            iloc_operation_add_source(op_iloc, right_result->temp);
            iloc_operation_add_source(op_iloc, left_result->temp);
            iloc_operation_add_target(op_iloc, result_temp);
        } else if (strcmp(op, "*") == 0) {
            op_iloc = iloc_operation_new("multI", false);
            iloc_operation_add_source(op_iloc, right_result->temp);
            iloc_operation_add_source(op_iloc, left_result->temp);
            iloc_operation_add_target(op_iloc, result_temp);
        } else {
            // Operação não-comutativa com constante à esquerda - usar versão normal
            op_iloc = iloc_operation_new(iloc_opcode, false);
            iloc_operation_add_source(op_iloc, left_result->temp);
            iloc_operation_add_source(op_iloc, right_result->temp);
            iloc_operation_add_target(op_iloc, result_temp);
        }
    } else {
        // Usar versão normal: op left_temp, right_temp => result_temp
        op_iloc = iloc_operation_new(iloc_opcode, false);
        iloc_operation_add_source(op_iloc, left_result->temp);
        iloc_operation_add_source(op_iloc, right_result->temp);
        iloc_operation_add_target(op_iloc, result_temp);
    }
    
    // Adicionar operação ao código
    iloc_code_append(code, op_iloc);
    
    // Liberar resultados intermediários (mas não o código/temp que foram movidos)
    free(left_result);
    free(right_result);
    
    return codegen_result_new(code, result_temp);
}

/* Passo 4.5: Geração para operadores unários */
codegen_result_t* gen_unary_code(const char* op, asd_tree_t* operand, stack_t* scopes) {
    if (!op || !operand || !scopes) {
        return NULL;
    }
    
    // Gerar código para o operando
    codegen_result_t* operand_result = NULL;
    
    if (is_literal_constant(operand)) {
        operand_result = gen_literal_code(operand);
    } else if (operand->lex_value && operand->lex_value->nature == IDENTIFIER) {
        operand_result = gen_identifier_code(operand, scopes);
    } else {
        // TODO: Chamar generate_code recursivamente quando implementar Fase 8
        return NULL;
    }
    
    if (!operand_result) {
        return NULL;
    }
    
    // Criar código combinado
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(operand_result);
        return NULL;
    }
    
    // Concatenar código do operando
    iloc_code_concat(code, operand_result->code);
    
    // Criar temporário para resultado
    iloc_operand_t* result_temp = iloc_operand_new_reg();
    if (!result_temp) {
        iloc_code_free(code);
        codegen_result_free(operand_result);
        return NULL;
    }
    
    // Processar operador unário
    if (strcmp(op, "+") == 0) {
        // Operador unário +: apenas copiar valor (i2i)
        iloc_operation_t* copy_op = iloc_operation_new("i2i", false);
        iloc_operation_add_source(copy_op, operand_result->temp);
        iloc_operation_add_target(copy_op, result_temp);
        iloc_code_append(code, copy_op);
    } else if (strcmp(op, "-") == 0) {
        // Operador unário -: multiplicar por -1
        iloc_operation_t* mult_op = iloc_operation_new("multI", false);
        iloc_operand_t* minus_one = iloc_operand_new_const(-1);
        iloc_operation_add_source(mult_op, operand_result->temp);
        iloc_operation_add_source(mult_op, minus_one);
        iloc_operation_add_target(mult_op, result_temp);
        iloc_code_append(code, mult_op);
    } else if (strcmp(op, "!") == 0) {
        // Operador !: XOR com 1 (0=false, 1=true)
        iloc_operation_t* xor_op = iloc_operation_new("xorI", false);
        iloc_operand_t* one = iloc_operand_new_const(1);
        iloc_operation_add_source(xor_op, operand_result->temp);
        iloc_operation_add_source(xor_op, one);
        iloc_operation_add_target(xor_op, result_temp);
        iloc_code_append(code, xor_op);
    } else {
        // Operador não suportado
        iloc_code_free(code);
        iloc_operand_free(result_temp);
        codegen_result_free(operand_result);
        return NULL;
    }
    
    // Liberar resultado intermediário
    free(operand_result);
    
    return codegen_result_new(code, result_temp);
}

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE EXPRESSÕES LÓGICAS
 * ============================================================================ */

/* Passo 5.1: Geração para comparações relacionais */
codegen_result_t* gen_relational_code(const char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes) {
    if (!op || !left || !right || !scopes) {
        return NULL;
    }
    
    // Determinar opcode ILOC baseado no operador
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
        return NULL; // Operador não suportado
    }
    
    // Gerar código para operandos (recursivamente)
    codegen_result_t* left_result = NULL;
    codegen_result_t* right_result = NULL;
    
    // Verificar se left é literal ou precisa gerar código
    if (is_literal_constant(left)) {
        left_result = gen_literal_code(left);
    } else {
        // Chamar função genérica de geração (será implementada na Fase 8)
        if (left->lex_value && left->lex_value->nature == IDENTIFIER) {
            left_result = gen_identifier_code(left, scopes);
        }
        // TODO: Chamar generate_code recursivamente quando implementar Fase 8
    }
    
    // Verificar se right é literal ou precisa gerar código
    if (is_literal_constant(right)) {
        right_result = gen_literal_code(right);
    } else {
        // Chamar função genérica de geração (será implementada na Fase 8)
        if (right->lex_value && right->lex_value->nature == IDENTIFIER) {
            right_result = gen_identifier_code(right, scopes);
        }
        // TODO: Chamar generate_code recursivamente quando implementar Fase 8
    }
    
    if (!left_result || !right_result) {
        if (left_result) codegen_result_free(left_result);
        if (right_result) codegen_result_free(right_result);
        return NULL;
    }
    
    // Criar código combinado
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    // Concatenar códigos dos operandos
    iloc_code_concat(code, left_result->code);
    iloc_code_concat(code, right_result->code);
    
    // Criar temporário para resultado booleano
    iloc_operand_t* result_temp = iloc_operand_new_reg();
    if (!result_temp) {
        iloc_code_free(code);
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    // Criar operação de comparação
    // Operações de comparação usam '->' ao invés de '=>'
    iloc_operation_t* cmp_op = iloc_operation_new(iloc_opcode, true);
    
    // Adicionar operandos fonte
    iloc_operation_add_source(cmp_op, left_result->temp);
    iloc_operation_add_source(cmp_op, right_result->temp);
    
    // Adicionar operando alvo (temporário booleano)
    iloc_operation_add_target(cmp_op, result_temp);
    
    // Adicionar operação ao código
    iloc_code_append(code, cmp_op);
    
    // Liberar resultados intermediários
    free(left_result);
    free(right_result);
    
    return codegen_result_new(code, result_temp);
}

/* Passo 5.2: Geração para operações lógicas AND e OR */
codegen_result_t* gen_logical_code(const char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes) {
    if (!op || !left || !right || !scopes) {
        return NULL;
    }
    
    // Determinar opcode ILOC baseado no operador
    const char* iloc_opcode = NULL;
    
    if (strcmp(op, "&") == 0 || strcmp(op, "&&") == 0) {
        iloc_opcode = "and";
    } else if (strcmp(op, "|") == 0 || strcmp(op, "||") == 0) {
        iloc_opcode = "or";
    } else {
        return NULL; // Operador não suportado
    }
    
    // Gerar código para operandos (recursivamente)
    codegen_result_t* left_result = NULL;
    codegen_result_t* right_result = NULL;
    
    // Verificar se left é literal ou precisa gerar código
    if (is_literal_constant(left)) {
        left_result = gen_literal_code(left);
    } else {
        // Chamar função genérica de geração (será implementada na Fase 8)
        if (left->lex_value && left->lex_value->nature == IDENTIFIER) {
            left_result = gen_identifier_code(left, scopes);
        }
        // TODO: Chamar generate_code recursivamente quando implementar Fase 8
    }
    
    // Verificar se right é literal ou precisa gerar código
    if (is_literal_constant(right)) {
        right_result = gen_literal_code(right);
    } else {
        // Chamar função genérica de geração (será implementada na Fase 8)
        if (right->lex_value && right->lex_value->nature == IDENTIFIER) {
            right_result = gen_identifier_code(right, scopes);
        }
        // TODO: Chamar generate_code recursivamente quando implementar Fase 8
    }
    
    if (!left_result || !right_result) {
        if (left_result) codegen_result_free(left_result);
        if (right_result) codegen_result_free(right_result);
        return NULL;
    }
    
    // Criar código combinado
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    // Concatenar códigos dos operandos
    iloc_code_concat(code, left_result->code);
    iloc_code_concat(code, right_result->code);
    
    // Criar temporário para resultado
    iloc_operand_t* result_temp = iloc_operand_new_reg();
    if (!result_temp) {
        iloc_code_free(code);
        codegen_result_free(left_result);
        codegen_result_free(right_result);
        return NULL;
    }
    
    // Criar operação lógica
    iloc_operation_t* logical_op = iloc_operation_new(iloc_opcode, false);
    
    // Adicionar operandos fonte
    iloc_operation_add_source(logical_op, left_result->temp);
    iloc_operation_add_source(logical_op, right_result->temp);
    
    // Adicionar operando alvo
    iloc_operation_add_target(logical_op, result_temp);
    
    // Adicionar operação ao código
    iloc_code_append(code, logical_op);
    
    // Liberar resultados intermediários
    free(left_result);
    free(right_result);
    
    return codegen_result_new(code, result_temp);
}

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE COMANDOS
 * ============================================================================ */

/* Função auxiliar para gerar código de expressão (usa generate_code recursivamente) */
static codegen_result_t* generate_expression_code(asd_tree_t* expr, stack_t* scopes) {
    // Chama generate_code recursivamente
    return generate_code(expr, scopes);
}

/* ============================================================================
 * FUNÇÃO GENÉRICA DE GERAÇÃO DE CÓDIGO
 * ============================================================================ */

/* Passo 8.2: Função genérica que gera código ILOC para qualquer nó da AST */
codegen_result_t* generate_code(asd_tree_t* node, stack_t* scopes) {
    if (!node || !scopes) {
        return NULL;
    }
    
    // Se o nó já tem código gerado, retornar NULL (evitar regeneração)
    // Mas na verdade, vamos sempre gerar quando chamado
    
    // Se é literal, usar gen_literal_code
    if (is_literal_constant(node)) {
        return gen_literal_code(node);
    }
    
    // Se é identificador, usar gen_identifier_code
    if (node->lex_value && node->lex_value->nature == IDENTIFIER) {
        return gen_identifier_code(node, scopes);
    }
    
    // Verificar label do nó para determinar tipo de operação/comando
    if (!node->label) {
        return NULL;
    }
    
    // EXPRESSÕES BINÁRIAS
    if (node->number_of_children >= 2) {
        asd_tree_t* left = node->children[0];
        asd_tree_t* right = node->children[1];
        const char* op = node->label;
        
        // Operações aritméticas
        if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || 
            strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
            return gen_binary_arithmetic_code(op, left, right, scopes);
        }
        // Operações relacionais
        else if (strcmp(op, "<") == 0 || strcmp(op, "<=") == 0 ||
                 strcmp(op, ">") == 0 || strcmp(op, ">=") == 0 ||
                 strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
            return gen_relational_code(op, left, right, scopes);
        }
        // Operações lógicas
        else if (strcmp(op, "&") == 0 || strcmp(op, "&&") == 0 ||
                 strcmp(op, "|") == 0 || strcmp(op, "||") == 0) {
            return gen_logical_code(op, left, right, scopes);
        }
    }
    // EXPRESSÕES UNÁRIAS
    else if (node->number_of_children == 1) {
        asd_tree_t* operand = node->children[0];
        const char* op = node->label;
        
        if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "!") == 0) {
            return gen_unary_code(op, operand, scopes);
        }
    }
    
    // COMANDOS E CONSTRUÇÕES ESPECIAIS
    // Atribuição
    if (strcmp(node->label, ":=") == 0 && node->number_of_children >= 2) {
        asd_tree_t* identifier = node->children[0];
        asd_tree_t* expression = node->children[1];
        iloc_code_t* code = gen_assignment_code(identifier, expression, scopes);
        if (code) {
            // Para comandos, retornar código sem temporário
            codegen_result_t* result = (codegen_result_t*)malloc(sizeof(codegen_result_t));
            result->code = code;
            result->temp = NULL;  // Comandos não retornam temporário
            return result;
        }
        return NULL;
    }
    
    // IF-THEN
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
    
    // IF-THEN-ELSE
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
    
    // WHILE
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
    
    // Bloco de comandos - concatenar códigos de todos os comandos
    // (será tratado no parser, mas podemos gerar aqui também)
    
    // Expressão não suportada ou nó desconhecido
    return NULL;
}

/* Passo 6.1: Geração básica de atribuição */
iloc_code_t* gen_assignment_code(asd_tree_t* identifier, asd_tree_t* expression, stack_t* scopes) {
    if (!identifier || !expression || !scopes) {
        return NULL;
    }
    
    // Verificar se identifier tem lex_value válido
    if (!identifier->lex_value || !identifier->lex_value->value) {
        return NULL;
    }
    
    // Gerar código para a expressão
    codegen_result_t* expr_result = generate_expression_code(expression, scopes);
    if (!expr_result) {
        return NULL;
    }
    
    // Buscar símbolo do identificador
    symbol_t* symbol = stack_get_symbol(scopes, identifier->lex_value->value, identifier->lex_value->line);
    if (!symbol) {
        codegen_result_free(expr_result);
        return NULL;
    }
    
    // Gerar código de armazenamento
    iloc_code_t* store_code = gen_store_variable_code(symbol, expr_result->temp);
    if (!store_code) {
        codegen_result_free(expr_result);
        return NULL;
    }
    
    // Criar código completo: código da expressão + código de store
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        iloc_code_free(store_code);
        codegen_result_free(expr_result);
        return NULL;
    }
    
    // Concatenar código da expressão
    iloc_code_concat(code, expr_result->code);
    
    // Concatenar código de store
    iloc_code_concat(code, store_code);
    
    // Liberar resultado da expressão (mas não o código/temp que foram movidos)
    free(expr_result);
    
    return code;
}

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE FLUXO DE CONTROLE
 * ============================================================================ */

/* Função auxiliar para gerar código de bloco de comandos */
static iloc_code_t* generate_block_code(asd_tree_t* block, stack_t* scopes) {
    if (!block || !scopes) {
        return iloc_code_new(); // Retorna código vazio se não há bloco
    }
    
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        return NULL;
    }
    
    // Se o bloco tem filhos, processar cada comando usando generate_code
    for (int i = 0; i < block->number_of_children; i++) {
        asd_tree_t* cmd = block->children[i];
        if (!cmd) continue;
        
        // Gerar código para o comando usando generate_code
        codegen_result_t* cmd_result = generate_code(cmd, scopes);
        if (cmd_result && cmd_result->code) {
            iloc_code_concat(code, cmd_result->code);
            // Armazenar código no nó também
            cmd->iloc_code = cmd_result->code;
        }
        if (cmd_result) {
            free(cmd_result);  // Liberar estrutura, mas não o código que foi movido
        }
    }
    
    return code;
}

/* Passo 7.1: Geração para IF-THEN */
iloc_code_t* gen_if_code(asd_tree_t* condition, asd_tree_t* then_block, stack_t* scopes) {
    if (!condition || !scopes) {
        return NULL;
    }
    
    // Gerar código para condição
    codegen_result_t* cond_result = generate_expression_code(condition, scopes);
    if (!cond_result) {
        return NULL;
    }
    
    // Criar rótulos
    iloc_operand_t* L_then = iloc_operand_new_label();
    iloc_operand_t* L_end = iloc_operand_new_label();
    
    if (!L_then || !L_end) {
        codegen_result_free(cond_result);
        if (L_then) iloc_operand_free(L_then);
        if (L_end) iloc_operand_free(L_end);
        return NULL;
    }
    
    // Criar código completo
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(cond_result);
        iloc_operand_free(L_then);
        iloc_operand_free(L_end);
        return NULL;
    }
    
    // Concatenar código da condição
    iloc_code_concat(code, cond_result->code);
    
    // Criar operação cbr (conditional branch)
    iloc_operation_t* cbr_op = iloc_operation_new("cbr", true);
    iloc_operation_add_source(cbr_op, cond_result->temp);
    iloc_operation_add_target(cbr_op, L_then);
    iloc_operation_add_target(cbr_op, L_end);
    iloc_code_append(code, cbr_op);
    
    // Gerar código do bloco then
    iloc_code_t* then_code = NULL;
    if (then_block) {
        then_code = generate_block_code(then_block, scopes);
        if (then_code && then_code->first) {
            // Adicionar rótulo L_then à primeira operação do bloco then
            iloc_operation_set_label(then_code->first, L_then);
            iloc_code_concat(code, then_code);
        } else if (then_code) {
            // Se o bloco está vazio, criar nop com rótulo
            iloc_operation_t* label_then = iloc_operation_new("nop", false);
            iloc_operation_set_label(label_then, L_then);
            iloc_code_append(code, label_then);
            iloc_code_free(then_code);
        }
    } else {
        // Se não há bloco then, criar nop com rótulo
        iloc_operation_t* label_then = iloc_operation_new("nop", false);
        iloc_operation_set_label(label_then, L_then);
        iloc_code_append(code, label_then);
    }
    
    // Adicionar rótulo L_end - criar nop com rótulo
    iloc_operation_t* label_end = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_end, L_end);
    iloc_code_append(code, label_end);
    
    // Liberar recursos
    free(cond_result);
    iloc_operand_free(L_then);
    iloc_operand_free(L_end);
    
    return code;
}

/* Passo 7.2: Geração para IF-THEN-ELSE */
iloc_code_t* gen_if_else_code(asd_tree_t* condition, asd_tree_t* then_block, asd_tree_t* else_block, stack_t* scopes) {
    if (!condition || !scopes) {
        return NULL;
    }
    
    // Gerar código para condição
    codegen_result_t* cond_result = generate_expression_code(condition, scopes);
    if (!cond_result) {
        return NULL;
    }
    
    // Criar rótulos
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
    
    // Criar código completo
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        codegen_result_free(cond_result);
        iloc_operand_free(L_then);
        iloc_operand_free(L_else);
        iloc_operand_free(L_end);
        return NULL;
    }
    
    // Concatenar código da condição
    iloc_code_concat(code, cond_result->code);
    
    // Criar operação cbr
    iloc_operation_t* cbr_op = iloc_operation_new("cbr", true);
    iloc_operation_add_source(cbr_op, cond_result->temp);
    iloc_operation_add_target(cbr_op, L_then);
    iloc_operation_add_target(cbr_op, L_else);
    iloc_code_append(code, cbr_op);
    
    // Gerar código do bloco then
    if (then_block) {
        iloc_code_t* then_code = generate_block_code(then_block, scopes);
        if (then_code && then_code->first) {
            // Adicionar rótulo L_then à primeira operação do bloco then
            iloc_operation_set_label(then_code->first, L_then);
            iloc_code_concat(code, then_code);
        } else if (then_code) {
            // Se o bloco está vazio, criar nop com rótulo
            iloc_operation_t* label_then = iloc_operation_new("nop", false);
            iloc_operation_set_label(label_then, L_then);
            iloc_code_append(code, label_then);
            iloc_code_free(then_code);
        }
    } else {
        // Se não há bloco then, criar nop com rótulo
        iloc_operation_t* label_then = iloc_operation_new("nop", false);
        iloc_operation_set_label(label_then, L_then);
        iloc_code_append(code, label_then);
    }
    
    // Gerar jump para L_end após o bloco then
    iloc_operation_t* jump_end = iloc_operation_new("jumpI", true);
    iloc_operation_add_target(jump_end, L_end);
    iloc_code_append(code, jump_end);
    
    // Gerar código do bloco else
    if (else_block) {
        iloc_code_t* else_code = generate_block_code(else_block, scopes);
        if (else_code && else_code->first) {
            // Adicionar rótulo L_else à primeira operação do bloco else
            iloc_operation_set_label(else_code->first, L_else);
            iloc_code_concat(code, else_code);
        } else if (else_code) {
            // Se o bloco está vazio, criar nop com rótulo
            iloc_operation_t* label_else = iloc_operation_new("nop", false);
            iloc_operation_set_label(label_else, L_else);
            iloc_code_append(code, label_else);
            iloc_code_free(else_code);
        }
    } else {
        // Se não há bloco else, criar nop com rótulo
        iloc_operation_t* label_else = iloc_operation_new("nop", false);
        iloc_operation_set_label(label_else, L_else);
        iloc_code_append(code, label_else);
    }
    
    // Adicionar rótulo L_end - criar nop com rótulo
    iloc_operation_t* label_end = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_end, L_end);
    iloc_code_append(code, label_end);
    
    // Liberar recursos
    free(cond_result);
    iloc_operand_free(L_then);
    iloc_operand_free(L_else);
    iloc_operand_free(L_end);
    
    return code;
}

/* Passo 7.3: Geração para WHILE */
iloc_code_t* gen_while_code(asd_tree_t* condition, asd_tree_t* body, stack_t* scopes) {
    if (!condition || !scopes) {
        return NULL;
    }
    
    // Criar rótulos
    iloc_operand_t* L_loop = iloc_operand_new_label();
    iloc_operand_t* L_body = iloc_operand_new_label();
    iloc_operand_t* L_end = iloc_operand_new_label();
    
    if (!L_loop || !L_body || !L_end) {
        if (L_loop) iloc_operand_free(L_loop);
        if (L_body) iloc_operand_free(L_body);
        if (L_end) iloc_operand_free(L_end);
        return NULL;
    }
    
    // Criar código completo
    iloc_code_t* code = iloc_code_new();
    if (!code) {
        iloc_operand_free(L_loop);
        iloc_operand_free(L_body);
        iloc_operand_free(L_end);
        return NULL;
    }
    
    // Gerar código da condição primeiro (para poder adicionar rótulo depois)
    codegen_result_t* cond_result = generate_expression_code(condition, scopes);
    if (!cond_result) {
        iloc_code_free(code);
        iloc_operand_free(L_loop);
        iloc_operand_free(L_body);
        iloc_operand_free(L_end);
        return NULL;
    }
    
    // Adicionar rótulo L_loop à primeira operação do código da condição
    if (cond_result->code && cond_result->code->first) {
        iloc_operation_set_label(cond_result->code->first, L_loop);
    } else {
        // Se não há código de condição, criar nop com rótulo
        iloc_operation_t* label_loop = iloc_operation_new("nop", false);
        iloc_operation_set_label(label_loop, L_loop);
        iloc_code_append(code, label_loop);
    }
    
    // Concatenar código da condição
    iloc_code_concat(code, cond_result->code);
    
    // Criar operação cbr
    iloc_operation_t* cbr_op = iloc_operation_new("cbr", true);
    iloc_operation_add_source(cbr_op, cond_result->temp);
    iloc_operation_add_target(cbr_op, L_body);
    iloc_operation_add_target(cbr_op, L_end);
    iloc_code_append(code, cbr_op);
    
    // Gerar código do corpo
    if (body) {
        iloc_code_t* body_code = generate_block_code(body, scopes);
        if (body_code && body_code->first) {
            // Adicionar rótulo L_body à primeira operação do corpo
            iloc_operation_set_label(body_code->first, L_body);
            iloc_code_concat(code, body_code);
        } else if (body_code) {
            // Se o bloco está vazio, criar nop com rótulo
            iloc_operation_t* label_body = iloc_operation_new("nop", false);
            iloc_operation_set_label(label_body, L_body);
            iloc_code_append(code, label_body);
            iloc_code_free(body_code);
        }
    } else {
        // Se não há corpo, criar nop com rótulo
        iloc_operation_t* label_body = iloc_operation_new("nop", false);
        iloc_operation_set_label(label_body, L_body);
        iloc_code_append(code, label_body);
    }
    
    // Gerar jump de volta para L_loop
    iloc_operation_t* jump_loop = iloc_operation_new("jumpI", true);
    iloc_operation_add_target(jump_loop, L_loop);
    iloc_code_append(code, jump_loop);
    
    // Adicionar rótulo L_end - criar nop com rótulo
    iloc_operation_t* label_end = iloc_operation_new("nop", false);
    iloc_operation_set_label(label_end, L_end);
    iloc_code_append(code, label_end);
    
    // Liberar recursos
    free(cond_result);
    iloc_operand_free(L_loop);
    iloc_operand_free(L_body);
    iloc_operand_free(L_end);
    
    return code;
}
