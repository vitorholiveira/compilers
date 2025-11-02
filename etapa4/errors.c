#include "errors.h"

#include <stdio.h>

void err_print_undeclared(const char* label, int line) {
    printf("[ERR] Line %d: Error %d (UNDECLARED) - Identifier '%s' was not declared.\n",
           line, ERR_UNDECLARED, label);
}

void err_print_declared(const char* label, int line, int decl_line) {
    printf("[ERR] Line %d: Error %d (REDECLARED) - Identifier '%s' already declared at line %d.\n",
           line, ERR_DECLARED, label, decl_line);
}

void err_print_variable(const char* label, int line, int decl_line) {
    printf("[ERR] Line %d: Error %d (VAR_AS_FUNC) - Identifier '%s' is used as a function but was declared as a variable at line %d.\n",
           line, ERR_VARIABLE, label, decl_line);
}

void err_print_function(const char* label, int line, int decl_line) {
    printf("[ERR] Line %d: Error %d (FUNC_AS_VAR) - Identifier '%s' is used as a variable but was declared as a function at line %d.\n",
           line, ERR_FUNCTION, label, decl_line);
}

void err_print_initialization_type(int init_line, const char* var_name, int declared_type, int initializer_type) {
    printf("[ERR] Line %d: Error %d (TYPE_MISMATCH) - Cannot initialize variable '%s' of nature '%s' with value of nature '%s'.\n",
           init_line, ERR_WRONG_TYPE, var_name, number_type_to_string(declared_type), number_type_to_string(initializer_type));
}

void err_print_atribution_type(int assign_line, int decl_line, const char* var_name, int declared_type, int assigned_type) {
    printf("[ERR] Line %d: Error %d (TYPE_MISMATCH) - Cannot assign nature '%s' to variable '%s' (declared at line %d as '%s').\n",
           assign_line, ERR_WRONG_TYPE, number_type_to_string(assigned_type), var_name, decl_line,
           number_type_to_string(declared_type));
}

void err_print_return_type(int return_line, int func_line, const char* func_name, int type_decl, int type_used) {
    printf("[ERR] Line %d: Error %d (TYPE_MISMATCH) - Return nature mismatch in function '%s' declared at line %d: expected '%s' but got '%s'.\n",
           return_line, ERR_WRONG_TYPE, func_name, func_line, number_type_to_string(type_decl), number_type_to_string(type_used));
}

void err_print_if_else_type(int line, int type_if, int type_else) {
    printf("[ERR] Line %d: Error %d (TYPE_MISMATCH) - Mismatched types between 'if' and 'else' blocks: '%s' vs '%s'.\n",
           line, ERR_WRONG_TYPE, number_type_to_string(type_if), number_type_to_string(type_else));
}

void err_print_expression_type(int line, const char* op, int type1, int type2) {
    printf("[ERR] Line %d: Error %d (TYPE_MISMATCH) - Cannot apply '%s' between '%s' and '%s'.\n",
           line, ERR_WRONG_TYPE, op, number_type_to_string(type1), number_type_to_string(type2));
}

void err_print_missing_args(int call_line, int decl_line, const char* func_name, int expected, int got) {
    printf("[ERR] Line %d: Error %d (ERR_MISSING_ARGS) - Too few arguments to function '%s' declared at line %d; expected %d but got %d.\n",
           call_line, ERR_MISSING_ARGS, func_name, decl_line, expected, got);
}

void err_print_excess_args(int call_line, int decl_line, const char* func_name, int expected, int got) {
    printf("[ERR] Line %d: Error %d (ERR_EXCESS_ARGS) - Too many arguments to function '%s' declared at line %d; expected %d but got %d.\n",
           call_line, ERR_EXCESS_ARGS, func_name, decl_line, expected, got);
}

void err_print_wrong_type_args(int call_line, int decl_line, const char* func_name, int param_index, const char* arg_label, int expected_type, int provided_type) {
    printf("[ERR] Line %d: Error %d (ERR_WRONG_TYPE_ARGS) - Expected '%s' but got '%s' for argument %d ('%s') of function '%s' declared at line %d.\n",
           call_line, ERR_WRONG_TYPE_ARGS, number_type_to_string(expected_type), number_type_to_string(provided_type), param_index, arg_label, func_name, decl_line);
}

const char* number_type_to_string(int nature) {
    if (nature == 0)
        return "inteiro";
    else if (nature == 1)
        return "decimal";
    else
        return "err";
}
