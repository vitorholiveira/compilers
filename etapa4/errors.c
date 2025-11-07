#include "errors.h"

#include <stdio.h>


void print_err(int line_number, int error_id , const char* msg) {
    printf("[ERR] Linha %d: Erro %s - %s.\n",
           line_number, error_to_string(error_id), msg);
}

void err_print_function(const char* label, int line, int decl_line) {
    printf("[ERR] Linha %d: Erro %d (FUNC_AS_VAR) - Identificador '%s' está sendo usado como variável, mas foi declarado como função na linha %d.\n",
           line, ERR_FUNCTION, label, decl_line);
}

void err_print_initialization_type(int init_line, const char* var_name, int declared_type, int initializer_type) {
    printf("[ERR] Linha %d: Erro %d (TYPE_MISMATCH) - Não é possível inicializar a variável '%s' de natureza '%s' com um valor de natureza '%s'.\n",
           init_line, ERR_WRONG_TYPE, var_name, number_type_to_string(declared_type), number_type_to_string(initializer_type));
}

void err_print_atribution_type(int assign_line, int decl_line, const char* var_name, int declared_type, int assigned_type) {
    printf("[ERR] Linha %d: Erro %d (TYPE_MISMATCH) - Não é possível atribuir natureza '%s' à variável '%s' (declarada na linha %d como '%s').\n",
           assign_line, ERR_WRONG_TYPE, number_type_to_string(assigned_type), var_name, decl_line,
           number_type_to_string(declared_type));
}

void err_print_return_type(int return_line, int func_line, const char* func_name, int type_decl, int type_used) {
    printf("[ERR] Linha %d: Erro %d (TYPE_MISMATCH) - Incompatibilidade de tipo de retorno na função '%s' declarada na linha %d: esperado '%s', mas obtido '%s'.\n",
           return_line, ERR_WRONG_TYPE, func_name, func_line, number_type_to_string(type_decl), number_type_to_string(type_used));
}

void err_print_if_else_type(int line, int type_if, int type_else) {
    printf("[ERR] Linha %d: Erro %d (TYPE_MISMATCH) - Tipos incompatíveis entre os blocos 'if' e 'else': '%s' vs '%s'.\n",
           line, ERR_WRONG_TYPE, number_type_to_string(type_if), number_type_to_string(type_else));
}

void err_print_expression_type(int line, const char* op, int type1, int type2) {
    printf("[ERR] Linha %d: Erro %d (TYPE_MISMATCH) - Não é possível aplicar o operador '%s' entre '%s' e '%s'.\n",
           line, ERR_WRONG_TYPE, op, number_type_to_string(type1), number_type_to_string(type2));
}

void err_print_missing_args(int call_line, int decl_line, const char* func_name, int expected, int got) {
    printf("[ERR] Linha %d: Erro %d (ERR_MISSING_ARGS) - Poucos argumentos para a função '%s' declarada na linha %d; esperados %d, mas obtidos %d.\n",
           call_line, ERR_MISSING_ARGS, func_name, decl_line, expected, got);
}

void err_print_excess_args(int call_line, int decl_line, const char* func_name, int expected, int got) {
    printf("[ERR] Linha %d: Erro %d (ERR_EXCESS_ARGS) - Muitos argumentos para a função '%s' declarada na linha %d; esperados %d, mas obtidos %d.\n",
           call_line, ERR_EXCESS_ARGS, func_name, decl_line, expected, got);
}

void err_print_wrong_type_args(int call_line, int decl_line, const char* func_name, int param_index, const char* arg_label, int expected_type, int provided_type) {
    printf("[ERR] Linha %d: Erro %d (ERR_WRONG_TYPE_ARGS) - Esperado '%s', mas obtido '%s' para o argumento %d ('%s') da função '%s' declarada na linha %d.\n",
           call_line, ERR_WRONG_TYPE_ARGS, number_type_to_string(expected_type), number_type_to_string(provided_type), param_index, arg_label, func_name, decl_line);
}

const char* number_type_to_string(int nature) {
    if (nature == 0)
        return "inteiro";
    else if (nature == 1)
        return "decimal";
    else
        return "erro";
}

const char* error_to_string(int error) {
    if(error == ERR_UNDECLARED)
        return "ERR_UNDECLARED";
    else if (ERR_DECLARED)
        return "ERR_DECLARED";
    else if (ERR_VARIABLE)
        return "ERR_VARIABLE";
    else if (ERR_FUNCTION)
        return "ERR_FUNCTION";
    else if (ERR_WRONG_TYPE)
        return "ERR_WRONG_TYPE";
    else if (ERR_MISSING_ARGS)
        return "ERR_WRONG_TYPE";
    else if (ERR_EXCESS_ARGS)
        return "ERR_EXCESS_ARGS";
    else if (ERR_WRONG_TYPE_ARGS)
        return "ERR_WRONG_TYPE_ARGS";
}
