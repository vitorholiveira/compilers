#include "codegen.h"
#include <stdlib.h>
#include <string.h>

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

