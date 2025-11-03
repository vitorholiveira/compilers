#ifndef TYPE_INFER_H
#define TYPE_INFER_H

#include "stack.h"

data_type_t infer_initialization_type(stack_t* scope_stack, lex_value_t* var_id, data_type_t decl_type, data_type_t exp_type);

data_type_t infer_atribution_type(stack_t* scope_stack, lex_value_t* var_id, data_type_t exp_type);

data_type_t infer_function_call_type(stack_t* scope_stack, lex_value_t* call_id, asd_tree_t* call_args, int num_args);

data_type_t infer_return_type(stack_t* scope_stack, asd_tree_t* return_expr, data_type_t declared_type);

data_type_t infer_if_type(stack_t* scope_stack, data_type_t cond_type, asd_tree_t* if_block, asd_tree_t* else_block);

data_type_t infer_exp_type(stack_t* scope_stack, const char* op, asd_tree_t* exp_left, asd_tree_t* exp_right);

data_type_t infer_var_type(stack_t* scope_stack, lex_value_t* var_id);

#endif  // TYPE_INFER_H
