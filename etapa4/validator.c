#include "validator.h"

data_type_t validate_var_init_types(stack_t* scopes, lex_value_t* identifier, data_type_t declared, data_type_t assigned)
{
    /* Validates type consistency during variable initialization */
    if (declared != assigned) {
        print_err_wrong_type(identifier->value, identifier->line, declared, assigned);
        stack_free(scopes);
        exit(ERR_WRONG_TYPE);
    }
    return declared;
}

data_type_t validate_assignment_types(stack_t* scopes, lex_value_t* identifier, data_type_t rhs_type)
{
    symbol_t* sym = stack_get_symbol(scopes, identifier->value, identifier->line);
    
    /* Functions cannot be assignment targets */
    if (sym->nature == FUNCTION) {
        print_err_function(identifier->value, identifier->line, sym->lex_value->line);
        stack_free(scopes);
        exit(ERR_FUNCTION);
    }

    /* Type compatibility verification */
    if (sym->data_type != rhs_type) {
        print_err_wrong_type(identifier->value, identifier->line, sym->data_type, rhs_type);
        stack_free(scopes);
        exit(ERR_WRONG_TYPE);
    }

    return rhs_type;
}

data_type_t validate_call_and_get_type(stack_t* scopes, lex_value_t* func_name, asd_tree_t* arguments, int arg_count)
{
    symbol_t* sym = stack_get_symbol(scopes, func_name->value, func_name->line);

    /* Verify the symbol represents a callable function */
    if (sym->nature == IDENTIFIER) {
        char msg[150];
        sprintf(msg, "Identificador '%s' está sendo usado como função mas é uma variável.", func_name->value);
        print_err(func_name->line, ERR_VARIABLE, msg);
        stack_free(scopes);
        exit(ERR_VARIABLE);
    }

    int params_expected = sym->param_count;

    /* Argument count validation - insufficient */
    if (params_expected > arg_count) {
        char msg[200];
        sprintf(msg, "A função '%s' declarada na linha %d espera %d argumentos, mas obteve apenas %d argumentos.", func_name->value, func_name->line, params_expected, arg_count);
        print_err(func_name->line, ERR_MISSING_ARGS, msg);
        stack_free(scopes);
        exit(ERR_MISSING_ARGS);
    }

    /* Argument count validation - excessive */
    if (params_expected < arg_count) {
        char msg[200];
        sprintf(msg, "A função '%s' declarada na linha %d espera %d argumentos, mas obteve %d argumentos.", func_name->value, func_name->line, params_expected, arg_count);
        print_err(func_name->line, ERR_EXCESS_ARGS, msg);
        stack_free(scopes);
        exit(ERR_EXCESS_ARGS);
    }

    /* Type checking for each argument-parameter pair */
    param_node_t* current_param = sym->param_list;
    asd_tree_t* current_arg = arguments;
    int idx = 1;

    while (current_param != NULL && current_arg != NULL) {
        data_type_t param_type = current_param->data_type;
        data_type_t arg_type = current_arg->data_type;
        
        if (param_type != arg_type) {
            char msg[200];
            sprintf(msg, "O tipo esperado era '%s', mas foi obtido '%s' para o argumento %d ('%s') da função '%s'.", number_type_to_string(param_type), number_type_to_string(arg_type), idx, current_arg->label, func_name->value);
            print_err(func_name->line, ERR_WRONG_TYPE_ARGS, msg);
            stack_free(scopes);
            exit(ERR_WRONG_TYPE_ARGS);
        }

        current_param = current_param->next;
        idx++;

        /* Advance to subsequent argument node */
        current_arg = (current_arg->number_of_children > 0) 
            ? current_arg->children[current_arg->number_of_children - 1] 
            : NULL;
    }

    return sym->data_type;
}

data_type_t validate_return_statement(stack_t* scopes, asd_tree_t* expr, data_type_t func_return_type)
{
    /* Ensure return expression matches function signature */
    if (expr->data_type != func_return_type) {
        print_err_wrong_type(expr->label, expr->lex_value->line, expr->data_type, func_return_type);
        stack_free(scopes);
        exit(ERR_WRONG_TYPE);
    }

    /* Cross-reference with function symbol in scope */
    symbol_t* current_func = stack_get_function(scopes);
    if (current_func->data_type != func_return_type) {
        print_err_wrong_type(current_func->lex_value->value, current_func->lex_value->line, current_func->data_type, func_return_type);
        stack_free(scopes);
        exit(ERR_WRONG_TYPE);
    }

    return func_return_type;
}

data_type_t validate_conditional_branches(stack_t* scopes, data_type_t condition_type, asd_tree_t* then_branch, asd_tree_t* else_branch)
{
    /* When both branches exist, they must yield compatible types */
    if (then_branch != NULL && else_branch != NULL) {
        if (then_branch->data_type != else_branch->data_type) {
            print_err_wrong_type("comando se - senão", then_branch->lex_value->line, then_branch->data_type, else_branch->data_type);
            stack_free(scopes);
            exit(ERR_WRONG_TYPE);
        }
    }

    return condition_type;
}

data_type_t deduce_binary_expr_type(stack_t* scopes, const char* operator, asd_tree_t* lhs, asd_tree_t* rhs)
{
    /* Both operands must share the same type */
    if (lhs->data_type != rhs->data_type) {
        print_err_wrong_type(operator, rhs->lex_value->line, rhs->data_type, lhs->data_type);
        stack_free(scopes);
        exit(ERR_WRONG_TYPE);
    }

    return lhs->data_type;
}

data_type_t lookup_identifier_type(stack_t* scopes, lex_value_t* identifier)
{
    symbol_t* sym = stack_get_symbol(scopes, identifier->value, identifier->line);

    /* Variable references only - functions require explicit calls */
    if (sym->nature == FUNCTION) {
        print_err_function(identifier->value, identifier->line, sym->lex_value->line);
        stack_free(scopes);
        exit(ERR_FUNCTION);
    }

    return sym->data_type;
}