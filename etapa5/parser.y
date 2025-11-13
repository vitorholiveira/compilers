%{
#include <stdio.h>
#include "stack.h"
#include "codegen.h"
#include <string.h>
#include <stdlib.h>
int yylex(void);
void yyerror (char const *mensagem);
extern int yylineno;
extern asd_tree_t *arvore;
extern stack_t *pilha;

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

%}
%define parse.trace
%token TK_VAR
%token TK_SENAO
%token TK_DECIMAL
%token TK_SE
%token TK_INTEIRO
%token TK_ATRIB
%token TK_RETORNA
%token TK_SETA
%token TK_ENQUANTO
%token TK_COM
%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_ER
%token TK_TIPO
%code requires { #include "stack.h" }

%union {
    lex_value_t* valor_lexico;
    asd_tree_t* arvore;
    data_type_t data_type;
    args_t* argumentos;
}

%destructor {
  if ($<valor_lexico>$) {
    lex_free($<valor_lexico>$);
  }
} <valor_lexico>

%destructor {
  if ($<argumentos>$) {
    args_free($<argumentos>$);
  }
} <argumentos>

%destructor {
  if ($<arvore>$) {
    asd_free($<arvore>$);
  }
} <arvore>

%token <valor_lexico> TK_ID
%token <valor_lexico> TK_LI_INTEIRO
%token <valor_lexico> TK_LI_DECIMAL

%type <arvore> lista elemento declaracao_variavel definicao_funcao parametros_funcao lista_params
%type <arvore> comandos_simples bloco_de_comandos sequencia_comandos_simples declaracao_variavel_comando_simples
%type <arvore> literais comando_atribuicao chamada_funcao comando_retorno
%type <arvore> fluxo_condicional fluxo_iterativo cabeca_funcao corpo_funcao
%type <arvore> expr_or expr_and expr_eq expr_rel expr_add expr_mul expr_unario expr_prim expressao
%type <argumentos> argumentos
%type <data_type> opcao_tipo

%define parse.error verbose 
%start programa
%%

/*
SCOPE MANAGEMENT
*/

escopo_ini: %empty { stack_push(pilha); };
escopo_ini_reset: %empty { 
    stack_push(pilha); 
    stack_reset_local_offset();  // Resetar offset local ao entrar em nova função
};
escopo_fim: %empty { stack_pop(pilha); };

/*
CONSTANTS AND TYPE DEFINITIONS
*/

literais: TK_LI_INTEIRO {
    $$ = asd_new($1->value, $1, INTEIRO);
    lex_free($1);
};
literais: TK_LI_DECIMAL {
    $$ = asd_new($1->value, $1, DECIMAL);
    lex_free($1);
};

opcao_tipo: TK_INTEIRO { $$ = INTEIRO; };
opcao_tipo: TK_DECIMAL { $$ = DECIMAL; };

/*
MAIN STRUCTURE
*/

programa: %empty { arvore = NULL; };
programa: escopo_ini lista escopo_fim ';' { 
    arvore = $2;
    // O código ILOC já foi gerado e armazenado em definicao_funcao->iloc_code
    // Então arvore->iloc_code já deve estar preenchido
};
lista: elemento { $$ = $1; }
    | elemento ',' lista {
    if($1 == NULL){
        $$ = $3;
    }else{
        if($3 != NULL) asd_add_child($1, $3);
        $$ = $1;
    }
};

elemento: definicao_funcao { $$ = $1; };
elemento: declaracao_variavel { $$ = $1; };

declaracao_variavel: TK_VAR TK_ID TK_ATRIB opcao_tipo {
    $$ = NULL; 
    stack_declare_symbol(pilha, IDENTIFIER, $4, $2);
    lex_free($2);
};

/*
FUNCTIONS
*/

definicao_funcao: cabeca_funcao escopo_ini_reset parametros_funcao TK_ATRIB corpo_funcao escopo_fim {
    $$ = $1;
    if($3 != NULL) asd_free($3);
    if($5 != NULL) asd_add_child($$, $5);
    // Gerar código ILOC para o corpo da função
    // Copiar código do corpo para a função (não compartilhar ponteiro)
    if ($5 != NULL && $5->iloc_code != NULL) {
        $$->iloc_code = $5->iloc_code;
        // Limpar ponteiro do corpo para evitar double-free
        $5->iloc_code = NULL;
    }
};

cabeca_funcao: TK_ID TK_SETA opcao_tipo {
    $$ = asd_new($1->value, $1, $3);
    stack_declare_symbol(pilha, FUNCTION, $3, $1);
    lex_free($1);
}

corpo_funcao: '[' sequencia_comandos_simples ']' { 
    $$ = $2;
    // Gerar código ILOC para o corpo da função
    // Se sequencia_comandos_simples já tem código, usar ele
    // Caso contrário, gerar código para todos os comandos (incluindo controle de fluxo)
    if ($$ != NULL) {
        iloc_code_t* corpo_code = iloc_code_new();
        
        // Se sequencia_comandos_simples já tem código, usar ele
        if ($$->iloc_code) {
            iloc_code_concat(corpo_code, $$->iloc_code);
            iloc_code_free($$->iloc_code);
            $$->iloc_code = NULL;
        }
        
        // Gerar código para todos os comandos (incluindo controle de fluxo)
        for (int i = 0; i < $$->number_of_children; i++) {
            asd_tree_t* cmd = $$->children[i];
            if (!cmd) continue;
            
            // Se o comando já tem código e não é controle de fluxo, usar ele
            // Para comandos de controle de fluxo, sempre gerar código (não usar código já gerado)
            if (cmd->iloc_code && 
                (!cmd->label || (strcmp(cmd->label, "se") != 0 && strcmp(cmd->label, "enquanto") != 0))) {
                iloc_code_concat(corpo_code, cmd->iloc_code);
                iloc_code_free(cmd->iloc_code);
                cmd->iloc_code = NULL;
            } else {
                // Gerar código para o comando usando generate_code
                // (sempre gerar para comandos de controle de fluxo ou comandos sem código)
                codegen_result_t* cmd_result = generate_code(cmd, pilha);
                if (cmd_result && cmd_result->code) {
                    iloc_code_concat(corpo_code, cmd_result->code);
                    iloc_code_free(cmd_result->code);
                }
                if (cmd_result) {
                    free(cmd_result);
                }
            }
        }
        
        // Se gerou código, armazenar
        if (corpo_code->count > 0) {
            $$->iloc_code = corpo_code;
        } else {
            iloc_code_free(corpo_code);
        }
    }
}
corpo_funcao: '[' ']' { $$ = NULL; };

parametros_funcao: %empty { $$ = NULL; }
parametros_funcao: TK_COM lista_params { 
    $$ = NULL; 
    if($2 != NULL) asd_free($2);
}
parametros_funcao: lista_params { 
    $$ = NULL; 
    if($1 != NULL) asd_free($1);
};

lista_params: TK_ID TK_ATRIB opcao_tipo { 
    $$ = NULL;
    stack_declare_function_parameter(pilha, IDENTIFIER, $3, $1);
    lex_free($1);
};
lista_params: lista_params ',' TK_ID TK_ATRIB opcao_tipo { 
    $$ = NULL; 
    if($1 != NULL) asd_free($1);
    stack_declare_function_parameter(pilha, IDENTIFIER, $5, $3);
    lex_free($3);
};

/*
INSTRUCTIONS
*/

comandos_simples: bloco_de_comandos {$$ = $1;}
                | declaracao_variavel_comando_simples {$$ = $1;}
                | comando_atribuicao {$$ = $1;}
                | chamada_funcao {$$ = $1;}
                | comando_retorno {$$ = $1;}
                | fluxo_condicional {$$ = $1;}
                | fluxo_iterativo {$$ = $1;}
                ;

/*
BLOCKS
*/

bloco_de_comandos: '[' escopo_ini sequencia_comandos_simples escopo_fim ']' { 
    $$ = $3;
    // Gerar código ILOC para o bloco (concatenar códigos de todos os comandos)
    // MAS: não gerar código aqui se o bloco é corpo de um while/if.
    // O código será gerado por generate_block_code quando o while/if for processado.
    // Isso evita duplicação de código.
    // Por enquanto, não geramos código aqui - será gerado quando necessário
}
    | '[' ']' { $$ = NULL; }
    ;
sequencia_comandos_simples: comandos_simples sequencia_comandos_simples {
    if ($1 == NULL){
        $$ = $2;
    }else{
        if ($2 != NULL) asd_add_child($1, $2);
        $$ = $1;
        // Gerar código ILOC concatenando códigos de todos os comandos
        // MAS: não concatenar código de comandos de controle de fluxo (se, enquanto)
        // que ainda não foram completamente processados, pois isso causaria duplicação
        if ($$ != NULL) {
            iloc_code_t* seq_code = iloc_code_new();
            // Primeiro adicionar código do primeiro comando (se não for controle de fluxo)
            if ($1 && $1->iloc_code && 
                (!$1->label || (strcmp($1->label, "se") != 0 && strcmp($1->label, "enquanto") != 0))) {
                iloc_code_concat(seq_code, $1->iloc_code);
                iloc_code_free($1->iloc_code);  // Liberar código vazio após concat
                $1->iloc_code = NULL;  // Limpar ponteiro para evitar double-free
            }
            // Depois adicionar códigos dos filhos (que são os comandos subsequentes)
            // MAS: não concatenar código de comandos de controle de fluxo
            for (int i = 0; i < $$->number_of_children; i++) {
                asd_tree_t* cmd = $$->children[i];
                if (cmd && cmd->iloc_code && 
                    (!cmd->label || (strcmp(cmd->label, "se") != 0 && strcmp(cmd->label, "enquanto") != 0))) {
                    iloc_code_concat(seq_code, cmd->iloc_code);
                    iloc_code_free(cmd->iloc_code);  // Liberar código vazio após concat
                    cmd->iloc_code = NULL;  // Limpar ponteiro para evitar double-free
                }
            }
            // Se gerou código, armazenar
            if (seq_code->count > 0) {
                $$->iloc_code = seq_code;
            } else {
                iloc_code_free(seq_code);
            }
        }
    }
};
sequencia_comandos_simples: comandos_simples { 
    $$ = $1;
    // Quando há apenas um comando, sequencia_comandos_simples É o próprio comando
    // Então o código já está em $$->iloc_code (se foi gerado)
};

/*
LOCAL VARIABLES
*/

declaracao_variavel_comando_simples: declaracao_variavel {$$ = $1;};
declaracao_variavel_comando_simples: TK_VAR TK_ID TK_ATRIB opcao_tipo TK_COM literais {
    data_type_t data_type = validate_var_init_types(pilha, $2, $4, $6->data_type);
    stack_declare_symbol(pilha, IDENTIFIER, data_type, $2);
    $$ = asd_new("com", NULL, $4);
    asd_add_child($$, asd_new($2->value, $2, $4));
    if($6 != NULL) asd_add_child($$, $6);
    lex_free($2);
};

/*
ASSIGNMENT OPERATIONS
*/

comando_atribuicao: TK_ID TK_ATRIB expressao {
    data_type_t data_type = validate_assignment_types(pilha, $1, $3->data_type);
    $$ = asd_new(":=", NULL, data_type); 
    asd_add_child($$, asd_new($1->value, $1, data_type)); 
    asd_add_child($$, $3);
    lex_free($1);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result && result->code) {
        $$->iloc_code = result->code;
        free(result);
    }
};

/*
FUNCTION INVOCATION AND RETURN
*/

chamada_funcao: TK_ID '(' argumentos ')' {
    data_type_t data_type = validate_call_and_get_type(pilha, $1, $3->args, $3->num_args);
    int len = strlen("call ") + strlen($1->value) + 1;
    char *buffer = malloc(len);
    snprintf(buffer, len, "call %s", $1->value);

    $$ = asd_new(buffer, $1, data_type);
    free(buffer);

    if($3 != NULL) {
        asd_add_child($$, $3->args);
        free($3);  // Free the args_t wrapper structure
    }
    lex_free($1);
};
chamada_funcao: TK_ID '(' ')' {
    data_type_t data_type = validate_call_and_get_type(pilha, $1, NULL, 0);
    int len = strlen("call ") + strlen($1->value) + 1;
    char *buffer = malloc(len);
    snprintf(buffer, len, "call %s", $1->value);

    $$ = asd_new(buffer, $1, data_type);
    free(buffer);
    lex_free($1);
}; 

argumentos: expressao ',' argumentos { 
    if($1 != NULL && $3 != NULL) asd_add_child($1, $3->args);
    args_t* args = malloc(sizeof(args_t));
    args->num_args = 1 + $3->num_args;
    args->args = $1;
    free($3);  // Free the previous args_t wrapper
    $$ = args;
};
argumentos: expressao { 
    args_t* args = malloc(sizeof(args_t));
    args->num_args = 1;
    args->args = $1;
    $$ = args;
};


comando_retorno: TK_RETORNA expressao TK_ATRIB opcao_tipo { 
    data_type_t data_type = validate_return_statement(pilha, $2, $4);
    $$ = asd_new("retorna", NULL, data_type); 
    if($2 != NULL) asd_add_child($$, $2);
}; 

/*
CONDITIONALS
*/

fluxo_condicional: TK_SE '(' expressao ')' bloco_de_comandos {
    data_type_t data_type = validate_conditional_branches(pilha, $3->data_type, $5, NULL);
    $$ = asd_new("se", NULL, data_type);
    if($3 != NULL) asd_add_child($$, $3);
    if($5 != NULL) asd_add_child($$, $5);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};

fluxo_condicional: TK_SE '(' expressao ')' bloco_de_comandos TK_SENAO bloco_de_comandos{
    data_type_t data_type = validate_conditional_branches(pilha, $3->data_type, $5, $7);
    $$ = asd_new("se", NULL, data_type);
    if($3 != NULL) asd_add_child($$, $3);
    if($5 != NULL) asd_add_child($$, $5);
    if($7 != NULL) asd_add_child($$, $7);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};

/*
LOOPS
*/

fluxo_iterativo: TK_ENQUANTO '(' expressao ')' bloco_de_comandos { 
    $$ = asd_new("enquanto", NULL, $3->data_type); 
    if($3 != NULL) asd_add_child($$, $3);
    if($5 != NULL) asd_add_child($$, $5);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};

/*
EXPRESSION ANALYSIS
*/

/*
Logical disjunction OR '|'
*/
expr_or: expr_or '|' expr_and { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "|", $1, $3);
    $$ = asd_new("|", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_or: expr_and { $$ = $1; };

/*
Logical conjunction AND '&'
*/
expr_and: expr_and '&' expr_eq { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "&", $1, $3);
    $$ = asd_new("&", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_and: expr_eq { $$ = $1; };

/*
Comparison operators ==, != 
*/
expr_eq: expr_eq TK_OC_EQ expr_rel {
    data_type_t data_type = deduce_binary_expr_type(pilha, "==", $1, $3); 
    $$ = asd_new("==", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
}
expr_eq: expr_eq TK_OC_NE expr_rel { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "!=", $1, $3); 
    $$ = asd_new("!=", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_eq: expr_rel { $$ = $1; };

/*
Relational comparisons <, >, <=, >=
*/
expr_rel: expr_rel '<' expr_add { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "<", $1, $3);
    $$ = asd_new("<", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_rel: expr_rel '>' expr_add { 
    data_type_t data_type = deduce_binary_expr_type(pilha, ">", $1, $3);
    $$ = asd_new(">", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_rel: expr_rel TK_OC_LE expr_add { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "<=", $1, $3);
    $$ = asd_new("<=", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_rel: expr_rel TK_OC_GE expr_add { 
    data_type_t data_type = deduce_binary_expr_type(pilha, ">=", $1, $3);
    $$ = asd_new(">=", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_rel: expr_add { $$ = $1; };

/*
Arithmetic operations +, -
*/
expr_add: expr_add '+' expr_mul { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "+", $1, $3);
    $$ = asd_new("+", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_add: expr_add '-' expr_mul { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "-", $1, $3);
    $$ = asd_new("-", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_add: expr_mul { $$ = $1; };

/*
Multiplicative operations * / %
*/
expr_mul: expr_mul '*' expr_unario { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "*", $1, $3);
    $$ = asd_new("*", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_mul: expr_mul '/' expr_unario { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "/", $1, $3);
    $$ = asd_new("/", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
};
expr_mul: expr_mul '%' expr_unario {
    data_type_t data_type = deduce_binary_expr_type(pilha, "%", $1, $3);
    $$ = asd_new("%", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_mul: expr_unario { $$ = $1; };

/*
Prefix operators
*/
expr_unario: '+' expr_unario { 
    $$ = asd_new("+", NULL, $2->data_type);
    asd_add_child($$, $2);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
}
expr_unario: '-' expr_unario { 
    $$ = asd_new("-", NULL, $2->data_type);
    asd_add_child($$, $2);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
}
expr_unario: '!' expr_unario { 
    $$ = asd_new("!", NULL, $2->data_type);
    asd_add_child($$, $2);
    // Gerar código ILOC
    codegen_result_t* result = generate_code($$, pilha);
    if (result) {
        $$->iloc_code = result->code;
        free(result);
    }
}
expr_unario: expr_prim { $$ = $1; };

/*
Primary elements: variables, constants, invocations, parentheses
*/
expr_prim: TK_ID { 
    data_type_t data_type = lookup_identifier_type(pilha, $1);
    $$ = asd_new($1->value, $1, data_type);
    lex_free($1);
};
expr_prim: literais { $$ = $1; };
expr_prim: chamada_funcao { $$ = $1; };
expr_prim: '(' expressao ')' { $$ = $2; };


/*
Entry point for expression evaluation
*/
expressao: expr_or;

%%
void yyerror(const char *msg) {
    printf( "Erro de sintaxe na linha %d: %s\n", yylineno, msg);
}

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