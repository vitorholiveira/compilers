#ifndef ERROR_H
#define ERROR_H

#define ERR_UNDECLARED       10 //2.2
#define ERR_DECLARED         11 //2.2
#define ERR_VARIABLE         20 //2.3
#define ERR_FUNCTION         21 //2.3
#define ERR_WRONG_TYPE       30 //2.4
#define ERR_MISSING_ARGS     40 //2.5
#define ERR_EXCESS_ARGS      41 //2.5
#define ERR_WRONG_TYPE_ARGS  42 //2.5

void print_err(int line_number, int error_id , const char* msg);
void err_print_function(const char* label, int used_line_number, int declared_line_number);
void err_print_initialization_type(int init_line, const char* var_name, int declared_type, int initializer_type);
void err_print_atribution_type(int assign_line, int decl_line, const char* var_name, int declared_type, int assigned_type);
void err_print_return_type(int return_line, int func_line, const char* func_name, int type_decl, int type_used);
void err_print_if_else_type(int if_line, int type_if, int type_else);
void err_print_expression_type(int line, const char* op, int type_left, int type_right);
void err_print_missing_args(int call_line, int decl_line, const char* func_name, int expected, int got);
void err_print_excess_args(int call_line, int decl_line, const char* func_name, int expected, int got);
void err_print_wrong_type_args(int call_line, int decl_line, const char* func_name, int param_index, const char* param_name, int expected_type, int provided_type);
const char* number_type_to_string(int nature);

#endif