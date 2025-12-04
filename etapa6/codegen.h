#ifndef CODEGEN_H
#define CODEGEN_H

#include "iloc.h"
#include "table.h"
#include "stack.h"
#include "asd.h"

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE ACESSO A VARIÁVEIS
 * ============================================================================ */

iloc_code_t* gen_load_variable_code(symbol_t* symbol, iloc_operand_t** result_temp, stack_t* scopes);
iloc_code_t* gen_store_variable_code(symbol_t* symbol, iloc_operand_t* value_temp, stack_t* scopes);
iloc_code_t* gen_variable_address_code(symbol_t* symbol, iloc_operand_t** result_temp);

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE EXPRESSÕES SIMPLES
 * ============================================================================ */

/* Estrutura para retornar código ILOC e temporário resultante */
typedef struct {
    iloc_code_t* code;
    iloc_operand_t* temp;
} codegen_result_t;

codegen_result_t* gen_literal_code(asd_tree_t* node);

codegen_result_t* gen_identifier_code(asd_tree_t* node, stack_t* scopes);

codegen_result_t* gen_binary_arithmetic_code(const char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes);

codegen_result_t* gen_unary_code(const char* op, asd_tree_t* operand, stack_t* scopes);

/* Função auxiliar para liberar resultado */
void codegen_result_free(codegen_result_t* result);

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE EXPRESSÕES LÓGICAS
 * ============================================================================ */

codegen_result_t* gen_relational_code(const char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes);

codegen_result_t* gen_logical_code(const char* op, asd_tree_t* left, asd_tree_t* right, stack_t* scopes);

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE COMANDOS
 * ============================================================================ */

iloc_code_t* gen_assignment_code(asd_tree_t* identifier, asd_tree_t* expression, stack_t* scopes);

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE FLUXO DE CONTROLE
 * ============================================================================ */

iloc_code_t* gen_if_code(asd_tree_t* condition, asd_tree_t* then_block, stack_t* scopes);

iloc_code_t* gen_if_else_code(asd_tree_t* condition, asd_tree_t* then_block, asd_tree_t* else_block, stack_t* scopes);

iloc_code_t* gen_while_code(asd_tree_t* condition, asd_tree_t* body, stack_t* scopes);

iloc_code_t* generate_block_code(asd_tree_t* block, stack_t* scopes);

/* ============================================================================
 * FUNÇÃO GENÉRICA DE GERAÇÃO DE CÓDIGO
 * ============================================================================ */

/* Função genérica que gera código ILOC para qualquer nó da AST */
codegen_result_t* generate_code(asd_tree_t* node, stack_t* scopes);

#endif // CODEGEN_H

