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
void print_err_function(const char* label, int line_number, int declared_line_number);
void print_err_wrong_type(const char* label, int line_number, int type_label, int type_value);
const char* number_type_to_string(int nature);

#endif