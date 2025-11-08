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

/*
 * print_err function, prints a formatted error message with line number, error code, and custom message.
 */
void print_err(int line_number, int error_id , const char* msg);

/*
 * print_err_function function, prints an error when an identifier declared as a function is used as a variable.
 */
void print_err_function(const char* label, int line_number, int declared_line_number);

/*
 * print_err_wrong_type function, prints an error when there is a type mismatch between expected and actual types.
 */
void print_err_wrong_type(const char* label, int line_number, int type_label, int type_value);

/*
 * number_type_to_string function, converts a numeric data type identifier to its string representation.
 */
const char* number_type_to_string(int nature);

/*
 * error_to_string function, converts an error code to its corresponding error name string.
 */
const char* error_to_string(int error);

#endif