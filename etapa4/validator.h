#ifndef VALIDATOR
#define VALIDATOR

#include "stack.h"

/*
 * validate_var_init_types function, validates that the declared type matches the assigned type during variable initialization.
 */
data_type_t validate_var_init_types(stack_t* scopes, lex_value_t* identifier, data_type_t declared, data_type_t assigned);

/*
 * validate_assignment_types function, validates that the assignment target type matches the expression type and ensures the target is a variable.
 */
data_type_t validate_assignment_types(stack_t* scopes, lex_value_t* identifier, data_type_t rhs_type);

/*
 * validate_call_and_get_type function, validates function call arguments against parameter types and counts, returning the function's return type.
 */
data_type_t validate_call_and_get_type(stack_t* scopes, lex_value_t* func_name, asd_tree_t* arguments, int arg_count);

/*
 * validate_return_statement function, validates that the return expression type matches the declared function return type.
 */
data_type_t validate_return_statement(stack_t* scopes, asd_tree_t* expr, data_type_t func_return_type);

/*
 * validate_conditional_branches function, validates that both branches of a conditional statement have compatible types if both exist.
 */
data_type_t validate_conditional_branches(stack_t* scopes, data_type_t condition_type, asd_tree_t* then_branch, asd_tree_t* else_branch);

/*
 * deduce_binary_expr_type function, validates that both operands of a binary expression have matching types and returns the result type.
 */
data_type_t deduce_binary_expr_type(stack_t* scopes, const char* operator, asd_tree_t* lhs, asd_tree_t* rhs);

/*
 * lookup_identifier_type function, retrieves the data type of a variable identifier, ensuring it is not a function.
 */
data_type_t lookup_identifier_type(stack_t* scopes, lex_value_t* identifier);

#endif  // VALIDATOR