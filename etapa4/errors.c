#include "errors.h"

#include <stdio.h>


void print_err(int line_number, int error_id , const char* msg) {
    printf("[ERRO] Linha: %d | Erro: %s | Mensagem: %s\n", line_number, error_to_string(error_id), msg);
}

void print_err_function(const char* label, int line_number, int declared_line_number) {
    char error_msg[100];
    sprintf(error_msg, "Identificador '%s' está sendo usado como variável, mas foi declarado como função na linha %d.", label, declared_line_number);
    print_err(line_number, ERR_FUNCTION, error_msg);
}

void print_err_wrong_type(const char* label, int line_number, int type_label, int type_value) {
    char error_msg[100];
    sprintf(error_msg, "Incompatibilidade de tipo em '%s'. O tipo '%s' deveria ser o mesmo de '%s'.", label, number_type_to_string(type_label), number_type_to_string(type_value));
    print_err(line_number, ERR_WRONG_TYPE, error_msg);
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
