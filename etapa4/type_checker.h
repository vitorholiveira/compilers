#ifndef TYPE_INFER_H
#define TYPE_INFER_H

#include "stack.h"

data_type_t validate_var_init_types(stack_t* scopes, lex_value_t* identifier, data_type_t declared, data_type_t assigned);

data_type_t validate_assignment_types(stack_t* scopes, lex_value_t* identifier, data_type_t rhs_type);

data_type_t validate_call_and_get_type(stack_t* scopes, lex_value_t* func_name, asd_tree_t* arguments, int arg_count);

data_type_t validate_return_statement(stack_t* scopes, asd_tree_t* expr, data_type_t func_return_type);

data_type_t validate_conditional_branches(stack_t* scopes, data_type_t condition_type, asd_tree_t* then_branch, asd_tree_t* else_branch);

data_type_t deduce_binary_expr_type(stack_t* scopes, const char* operator, asd_tree_t* lhs, asd_tree_t* rhs);

data_type_t lookup_identifier_type(stack_t* scopes, lex_value_t* identifier);

#endif  // TYPE_INFER_H