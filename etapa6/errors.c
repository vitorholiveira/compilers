#include "errors.h"
#include <stdio.h>

void print_err(int line_number, int error_id, const char* msg) {
    printf("[ERRO] Linha: %d | Erro: %s | Mensagem: %s\n", line_number, error_to_string(error_id), msg);
}

void print_err_function(const char* label, int line_number, int declared_line_number) {
    char buffer[200];
    sprintf(buffer, "Identificador '%s' está sendo usado como variável, mas foi declarado como função na linha %d.", label, declared_line_number);
    print_err(line_number, ERR_FUNCTION, buffer);
}

void print_err_wrong_type(const char* label, int line_number, int type_label, int type_value) {
    char buffer[200];
    sprintf(buffer, "Incompatibilidade de tipo em '%s'. O tipos '%s' e '%s' são diferentes.", label, number_type_to_string(type_label), number_type_to_string(type_value));
    print_err(line_number, ERR_WRONG_TYPE, buffer);
}

const char* number_type_to_string(int data_type) {
    switch (data_type) {
        case 0:  return "inteiro";
        case 1:  return "decimal";
        default: return "erro";
    }
}

const char* error_to_string(int error) {
    switch (error) {
        case ERR_UNDECLARED:      return "ERR_UNDECLARED";
        case ERR_DECLARED:        return "ERR_DECLARED";
        case ERR_VARIABLE:        return "ERR_VARIABLE";
        case ERR_FUNCTION:        return "ERR_FUNCTION";
        case ERR_WRONG_TYPE:      return "ERR_WRONG_TYPE";
        case ERR_MISSING_ARGS:    return "ERR_MISSING_ARGS";
        case ERR_EXCESS_ARGS:     return "ERR_EXCESS_ARGS";
        case ERR_WRONG_TYPE_ARGS: return "ERR_WRONG_TYPE_ARGS";
        default:                  return "ERR_UNKNOWN";
    }
}