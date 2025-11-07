#include "type_infer.h"

data_type_t infer_initialization_type(stack_t* scope_stack, lex_value_t* var_id, data_type_t decl_type, data_type_t exp_type)
{
    /* Type mismatch between declaration and initialization */
    if (decl_type == exp_type) {
        return decl_type;
    }

    err_print_initialization_type(var_id->line, var_id->value, decl_type, exp_type);
    stack_free(scope_stack);
    exit(ERR_WRONG_TYPE);
}

data_type_t infer_atribution_type(stack_t* scope_stack, lex_value_t* var_id, data_type_t exp_type)
{
    symbol_t* var_symbol = stack_get_symbol(scope_stack, var_id->value, var_id->line);
    
    /* Verify target is a variable, not a function */
    if (var_symbol->nature == FUNCTION) {
        err_print_function(var_id->value, var_id->line, var_symbol->lex_value->line);
        stack_free(scope_stack);
        exit(ERR_FUNCTION);
    }

    /* Check type compatibility */
    if (var_symbol->data_type == exp_type) {
        return exp_type;
    }

    err_print_atribution_type(var_id->line, var_symbol->lex_value->line, var_id->value, 
                              var_symbol->data_type, exp_type);
    stack_free(scope_stack);
    exit(ERR_WRONG_TYPE);
}

data_type_t infer_function_call_type(stack_t* scope_stack, lex_value_t* call_id, asd_tree_t* call_args, int num_args)
{
    symbol_t* func_symbol = stack_get_symbol(scope_stack, call_id->value, call_id->line);

    /* Ensure symbol is actually a function */
    if (func_symbol->nature == IDENTIFIER) {
        char error_msg[100];
        sprintf(error_msg, "Identificador '%s' está sendo usado como função, mas foi declarado como variável na linha %d", call_id->value, func_symbol->lex_value->line);
        print_err(call_id->line, ERR_VARIABLE, error_msg);
        stack_free(scope_stack);
        exit(ERR_VARIABLE);
    }

    /* Count expected parameters */
    int expected_count = func_symbol->param_count;

    /* Check for too few arguments */
    if (expected_count > num_args) {
        err_print_missing_args(call_id->line, func_symbol->lex_value->line, call_id->value, 
                              expected_count, num_args);
        stack_free(scope_stack);
        exit(ERR_MISSING_ARGS);
    }

    /* Check for too many arguments */
    if (expected_count < num_args) {
        err_print_excess_args(call_id->line, func_symbol->lex_value->line, call_id->value, 
                             expected_count, num_args);
        stack_free(scope_stack);
        exit(ERR_EXCESS_ARGS);
    }

    /* Validate each argument type against parameter list */
    param_node_t* param_cursor = func_symbol->param_list;
    asd_tree_t* arg_cursor = call_args;
    int position = 1;

    while (param_cursor && arg_cursor) {
        data_type_t expected = param_cursor->data_type;
        data_type_t provided = arg_cursor->data_type;
        
        if (expected != provided) {
            err_print_wrong_type_args(call_id->line, func_symbol->lex_value->line, call_id->value, 
                                     position, param_cursor->label, expected, provided);
            stack_free(scope_stack);
            exit(ERR_WRONG_TYPE_ARGS);
        }

        param_cursor = param_cursor->next;
        position++;

        /* Navigate to next argument in tree */
        if (arg_cursor->number_of_children > 0) {
            arg_cursor = arg_cursor->children[arg_cursor->number_of_children - 1];
        } else {
            arg_cursor = NULL;
        }
    }

    return func_symbol->data_type;
}

data_type_t infer_return_type(stack_t* scope_stack, asd_tree_t* return_expr, data_type_t declared_type)
{
    /* Check expression type matches declared return type */
    if (return_expr->data_type != declared_type) {
        err_print_atribution_type(return_expr->lex_value->line, return_expr->lex_value->line, 
                                 return_expr->label, declared_type, return_expr->data_type);
        stack_free(scope_stack);
        exit(ERR_WRONG_TYPE);
    }

    /* Verify against function declaration */
    symbol_t* func_symbol = stack_get_function(scope_stack);
    if (func_symbol->data_type != declared_type) {
        err_print_return_type(func_symbol->lex_value->line, return_expr->lex_value->line, 
                             func_symbol->lex_value->value, func_symbol->data_type, declared_type);
        stack_free(scope_stack);
        exit(ERR_WRONG_TYPE);
    }

    return declared_type;
}

data_type_t infer_if_type(stack_t* scope_stack, data_type_t cond_type, asd_tree_t* if_block,
                          asd_tree_t* else_block)
{
    /* If both branches exist, they must have matching types */
    int both_branches_present = (if_block && else_block);
    
    if (both_branches_present) {
        if (if_block->data_type != else_block->data_type) {
            err_print_if_else_type(if_block->lex_value->line, if_block->data_type, 
                                  else_block->data_type);
            stack_free(scope_stack);
            exit(ERR_WRONG_TYPE);
        }
    }

    return cond_type;
}

data_type_t infer_exp_type(stack_t* scope_stack, const char* op, asd_tree_t* exp_left, asd_tree_t* exp_right)
{
    /* Binary operation requires matching operand types */
    if (exp_left->data_type == exp_right->data_type) {
        return exp_left->data_type;
    }

    err_print_expression_type(exp_right->lex_value->line, op, exp_left->data_type, 
                             exp_right->data_type);
    stack_free(scope_stack);
    exit(ERR_WRONG_TYPE);
}

data_type_t infer_var_type(stack_t* scope_stack, lex_value_t* var_id)
{
    symbol_t* var_symbol = stack_get_symbol(scope_stack, var_id->value, var_id->line);

    /* Ensure we're referencing a variable, not a function */
    if (var_symbol->nature == FUNCTION) {
        err_print_function(var_id->value, var_id->line, var_symbol->lex_value->line);
        stack_free(scope_stack);
        exit(ERR_FUNCTION);
    }

    return var_symbol->data_type;
}