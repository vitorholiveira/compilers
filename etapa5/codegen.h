#ifndef CODEGEN_H
#define CODEGEN_H

#include "iloc.h"
#include "table.h"

/* ============================================================================
 * FUNÇÕES PARA GERAÇÃO DE CÓDIGO DE ACESSO A VARIÁVEIS
 * ============================================================================ */

iloc_code_t* gen_load_variable_code(symbol_t* symbol, iloc_operand_t** result_temp);


iloc_code_t* gen_store_variable_code(symbol_t* symbol, iloc_operand_t* value_temp);

iloc_code_t* gen_variable_address_code(symbol_t* symbol, iloc_operand_t** result_temp);

#endif // CODEGEN_H

