#ifndef TYPE_INFER_H
#define TYPE_INFER_H

#include "stack.h"

/**
 * @brief Infers and validates the type of a variable initialization.
 *
 * Checks that the declared type of a variable matches the type of the expression
 * used during its initialization.
 *
 * @param scope_stack Pointer to the current scope stack.
 * @param var_id      Lexical value of the declared variable.
 * @param decl_type   The declared type of the variable.
 * @param exp_type    The type of the initializer expression.
 * @return The declared type if it matches the initializer expression type.
 *
 * @note Exits with ERR_WRONG_TYPE if the initializer type does not match the declared type.
 */
data_type_t infer_initialization_type(stack_t* scope_stack, lex_value_t* var_id,
                                 data_type_t decl_type, data_type_t exp_type);

/**
 * @brief Infers and validates the type of a variable assignment.
 *
 * Checks that the identifier refers to a declared variable (not a function),
 * and that the type of the expression assigned matches the declared type of the variable.
 *
 * @param scope_stack Pointer to the scope stack for symbol lookup.
 * @param var_id      Lexical value of the variable receiving the assignment.
 * @param exp_type    Type of the expression being assigned to the variable.
 * @return The expression type if the assignment is valid.
 *
 * @note Exits with ERR_UNDECLARED if the variable is not declared.
 * @note Exits with ERR_FUNCTION if the identifier refers to a function.
 * @note Exits with ERR_WRONG_TYPE if the assigned type does not match the declared type.
 */
data_type_t infer_atribution_type(stack_t* scope_stack, lex_value_t* var_id, data_type_t exp_type);

/**
 * @brief Infers and validates the return type of a function call expression.
 *
 * Checks if the called function exists, and verifies the number and types
 * of arguments against the function's parameter list as registered in the symbol table.
 *
 * @param scope_stack Pointer to the scope stack.
 * @param call_id     Lexical value of the function being called.
 * @param call_args   AST node representing the list of arguments in the function call.
 * @param num_args    Number of arguments passed to the function call.
 * @return The return type of the function if the call is valid.
 *
 * @note Exits with ERR_UNDECLARED if the function is not declared.
 * @note Exits with ERR_VARIABLE if the identifier refers to a variable.
 * @note Exits with ERR_MISSING_ARGS if too few arguments are provided.
 * @note Exits with ERR_EXCESS_ARGS if too many arguments are provided.
 * @note Exits with ERR_WRONG_TYPE_ARGS if any argument type does not match the expected parameter
 * type.
 */
data_type_t infer_function_call_type(stack_t* scope_stack, lex_value_t* call_id,
                                asd_tree_t* call_args, int num_args);

/**
 * @brief Infers and validates the return expression type considering user declaration and function
 * return type.
 *
 * Checks that the type of the return expression matches both the explicitly declared return type
 * and the function’s actual return type.
 *
 * @param scope_stack    Current scope for semantic info.
 * @param return_expr    AST node for the return expression.
 * @param declared_type  Type explicitly declared on the return expression by the user.
 *
 * @return The final resolved type of the return expression if compatible.
 * @note Exits with ERR_WRONG_TYPE if the expression type doesn't match the declared return type.
 * @note Exits with ERR_WRONG_TYPE if the expression type doesn't match the function's return type.
 */
data_type_t infer_return_type(stack_t* scope_stack, asd_tree_t* return_expr, data_type_t declared_type);

/**
 * @brief Infers and validates the resulting type of an if-else command.
 *
 * Compares the data types of the `if` and `else` blocks (if `else_block` is not NULL).
 *
 * @param scope_stack Pointer to the scope_stack.
 * @param cond_type   The type inferred from the if-else condition expression.
 * @param if_block    AST node representing the `if` block.
 * @param else_block  AST node representing the `else` block (may be NULL).
 * @return The condition expression type, if blocks are compatible.
 *
 * @note Exits with ERR_WRONG_TYPE if the block types do not match.
 */
data_type_t infer_if_type(stack_t* scope_stack, data_type_t cond_type, asd_tree_t* if_block,
                     asd_tree_t* else_block);

/**
 * @brief Infers and validates the resulting type of a binary expression.
 *
 * Checks the data types of the left and right expression trees.
 *
 * @param scope_stack Pointer to the scope stack.
 * @param op          The operator being applied (e.g., "+", "==", "|").
 * @param exp_left    Pointer to the left operand's AST node.
 * @param exp_right   Pointer to the right operand's AST node.
 * @return The common data type if both operands have the same type.
 *
 * @note Exits with ERR_WRONG_TYPE if the types do not match.
 */
data_type_t infer_exp_type(stack_t* scope_stack, const char* op, asd_tree_t* exp_left,
                      asd_tree_t* exp_right);

/**
 * @brief Infers and validates the type of a variable identifier.
 *
 * Checks if the given identifier corresponds to a variable in the current
 * or visible scopes.
 *
 * @param scope_stack    Current scope stack for semantic information.
 * @param var_id         Lexical value of the variable being used.
 *
 * @return The data type of the variable if valid.
 *
 * @note Exits with ERR_UNDECLARED if the identifier is not declared.
 * @note Exits with ERR_FUNCTION if the identifier is a function.
 */
data_type_t infer_var_type(stack_t* scope_stack, lex_value_t* var_id);

#endif  // TYPE_INFER_H
