#include "type_infer.h"

data_type_t infer_initialization_type(stack_t* scope_stack, lex_value_t* var_id, data_type_t decl_type, data_type_t exp_type)
{
    if (decl_type != exp_type) {
        err_print_initialization_type(var_id->line, var_id->value, decl_type, exp_type);
        stack_free(scope_stack);
        exit(ERR_WRONG_TYPE);
    }

    return decl_type;
}

data_type_t infer_atribution_type(stack_t* scope_stack, lex_value_t* var_id, data_type_t exp_type)
{
    symbol_t* var_decl = stack_get_symbol(scope_stack, var_id->value, var_id->line);
    if (var_decl->nature != IDENTIFIER) {
        err_print_function(var_id->value, var_id->line, var_decl->lex_value->line);
        stack_free(scope_stack);
        exit(ERR_FUNCTION);
    }

    if (var_decl->data_type != exp_type) {
        err_print_atribution_type(var_id->line, var_decl->lex_value->line, var_id->value, var_decl->data_type, exp_type);
        stack_free(scope_stack);
        exit(ERR_WRONG_TYPE);
    }

    return exp_type;
}

data_type_t infer_function_call_type(stack_t* scope_stack, lex_value_t* call_id, asd_tree_t* call_args, int num_args)
{
    symbol_t* function_symbol = stack_get_symbol(scope_stack, call_id->value, call_id->line);

    if (function_symbol->nature != FUNCTION) {
        err_print_variable(call_id->value, call_id->line, function_symbol->lex_value->line);
        stack_free(scope_stack);
        exit(ERR_VARIABLE);
    }

    int num_expected_args = 0;
    if (function_symbol->params != NULL) {
        num_expected_args = function_symbol->params->num_params;
    }

    if (num_expected_args > num_args) {
        err_print_missing_args(call_id->line, function_symbol->lex_value->line, call_id->value, num_expected_args, num_args);
        stack_free(scope_stack);
        exit(ERR_MISSING_ARGS);
    }

    if (num_expected_args < num_args) {
        err_print_excess_args(call_id->line, function_symbol->lex_value->line, call_id->value, num_expected_args, num_args);
        stack_free(scope_stack);
        exit(ERR_EXCESS_ARGS);
    }

    int i;
    asd_tree_t* arg = call_args;
    // Validate each argument type
    for (i = 0; i < num_expected_args; i++) {
        data_type_t expected_type = function_symbol->params->params[i]->data_type;
        data_type_t provided_type = arg->data_type;
        if (expected_type != provided_type) {
            err_print_wrong_type_args(
                call_id->line, function_symbol->lex_value->line, call_id->value, i + 1,
                function_symbol->params->params[i]->label, expected_type, provided_type);
            stack_free(scope_stack);
            exit(ERR_WRONG_TYPE_ARGS);
        }

        if (arg->number_of_children > 0) {
            arg = arg->children[arg->number_of_children - 1];
        } else {
            arg = NULL;
        }
    }

    return function_symbol->data_type;
}

data_type_t infer_return_type(stack_t* scope_stack, asd_tree_t* return_expr, data_type_t declared_type)
{
    if (return_expr->data_type != declared_type) {
        err_print_atribution_type(return_expr->lex_value->line, return_expr->lex_value->line, return_expr->label, declared_type, return_expr->data_type);
        stack_free(scope_stack);
        exit(ERR_WRONG_TYPE);
    }

    symbol_t* declared_function = stack_get_current_function(scope_stack);
    if (declared_function->data_type != declared_type) {
        err_print_return_type(
            declared_function->lex_value->line, return_expr->lex_value->line,
            declared_function->lex_value->value, declared_function->data_type, declared_type);
        stack_free(scope_stack);
        exit(ERR_WRONG_TYPE);
    }

    return declared_type;
}

data_type_t infer_if_type(stack_t* scope_stack, data_type_t cond_type, asd_tree_t* if_block,
                     asd_tree_t* else_block)
{
    if (if_block != NULL && else_block != NULL) {
        if (if_block->data_type != else_block->data_type) {
            err_print_if_else_type(if_block->lex_value->line, if_block->data_type, else_block->data_type);
            stack_free(scope_stack);
            exit(ERR_WRONG_TYPE);
        }
    }

    return cond_type;
}

data_type_t infer_exp_type(stack_t* scope_stack, const char* op, asd_tree_t* tree_left, asd_tree_t* tree_right)
{
    if (tree_left->data_type != tree_right->data_type) {
        err_print_expression_type(tree_right->lex_value->line, op, tree_left->data_type, tree_right->data_type);
        stack_free(scope_stack);
        exit(ERR_WRONG_TYPE);
    }

    return tree_left->data_type;
}

data_type_t infer_var_type(stack_t* scope_stack, lex_value_t* var_id)
{
    symbol_t* var_decl = stack_get_symbol(scope_stack, var_id->value, var_id->line);

    if (var_decl->nature != IDENTIFIER) {
        err_print_function(var_id->value, var_id->line, var_decl->lex_value->line);
        stack_free(scope_stack);
        exit(ERR_FUNCTION);
    }

    return var_decl->data_type;
}
