%{
#include <stdio.h>
#include "type_infer.h"
#include "stack.h"
#include <string.h>
#include <stdlib.h>
int yylex(void);
void yyerror (char const *mensagem);
extern int yylineno;
extern asd_tree_t *arvore;
extern stack_t *pilha;

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
%code requires { #include "type_infer.h" }

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

%destructor { free($$); } <argumentos> 

%token <valor_lexico> TK_ID
%token <valor_lexico> TK_LI_INTEIRO
%token <valor_lexico> TK_LI_DECIMAL

%type <arvore> lista elemento declaracao_variavel definicao_funcao parametros_funcao lista_params
%type <arvore> comandos_simples bloco_de_comandos sequencia_comandos_simples declaracao_variavel_comando_simples
%type <arvore> literais comando_atribuicao chamada_funcao comando_retorno
%type <arvore> fluxo_condicional fluxo_iterativo cabeca_funcao corpo_funcao
%type <arvore> expr_or expr_and expr_eq expr_rel expr_add expr_mul expr_unario expr_prim expressao
%type <data_type> opcao_tipo
%type <argumentos> argumentos

%define parse.error verbose 
%start programa
%%

/*
GERENCIAMENTO DE ESCOPO
*/

escopo_ini: %empty { stack_push(pilha); };
escopo_fim: %empty { stack_pop(pilha); };

/*
CONSTANTES E DEFINIÇÕES DE TIPO
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
ESTRUTURA PRINCIPAL
*/

programa: %empty { arvore = NULL; };
programa: escopo_ini lista escopo_fim ';' { arvore = $2; };
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
FUNÇÕES
*/

definicao_funcao: cabeca_funcao escopo_ini parametros_funcao TK_ATRIB corpo_funcao escopo_fim {
    $$ = $1;
    if($5 != NULL) asd_add_child($$, $5);
};

cabeca_funcao: TK_ID TK_SETA opcao_tipo {
    $$ = asd_new($1->value, $1, $3);
    stack_declare_symbol(pilha, FUNCTION, $3, $1);
    lex_free($1);
}

corpo_funcao: '[' sequencia_comandos_simples ']' { $$ = $2; }
corpo_funcao: '[' ']' { $$ = NULL; };

parametros_funcao: %empty { $$ = NULL; }
parametros_funcao: TK_COM lista_params { $$ = NULL; }
parametros_funcao: lista_params { $$ = NULL; };

lista_params: TK_ID TK_ATRIB opcao_tipo { 
    $$ = NULL;
    stack_declare_function_parameter(pilha, IDENTIFIER, $3, $1);
    lex_free($1);
};
lista_params: lista_params ',' TK_ID TK_ATRIB opcao_tipo { 
    $$ = NULL; 
    stack_declare_function_parameter(pilha, IDENTIFIER, $5, $3);
    lex_free($3);
};

/*
INSTRUÇÕES
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
BLOCOS
*/

bloco_de_comandos: '[' escopo_ini sequencia_comandos_simples escopo_fim ']' { $$ = $3; }
    | '[' ']' { $$ = NULL; }
    ;
sequencia_comandos_simples: comandos_simples sequencia_comandos_simples {
    if ($1 == NULL){
        $$ = $2;
    }else{
        if ($2 != NULL) asd_add_child($1, $2);
        $$ = $1;
    }
};
sequencia_comandos_simples: comandos_simples { $$ = $1; };

/*
VARIÁVEIS LOCAIS
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
OPERAÇÕES DE ATRIBUIÇÃO
*/

comando_atribuicao: TK_ID TK_ATRIB expressao {
    data_type_t data_type = validate_assignment_types(pilha, $1, $3->data_type);
    $$ = asd_new(":=", NULL, data_type); 
    asd_add_child($$, asd_new($1->value, $1, data_type)); 
    asd_add_child($$, $3);
    lex_free($1);
};

/*
INVOCAÇÃO E RETORNO DE FUNÇÃO
*/

chamada_funcao: TK_ID '(' argumentos ')' {
    data_type_t data_type = validate_call_and_get_type(pilha, $1, $3->args, $3->num_args);
    int len = strlen("call ") + strlen($1->value) + 1;
    char *buffer = malloc(len);
    snprintf(buffer, len, "call %s", $1->value);

    $$ = asd_new(buffer, $1, data_type);
    free(buffer);

    if($3 != NULL) asd_add_child($$, $3->args);
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
CONDICIONAIS
*/

fluxo_condicional: TK_SE '(' expressao ')' bloco_de_comandos {
    data_type_t data_type = validate_conditional_branches(pilha, $3->data_type, $5, NULL);
    $$ = asd_new("se", NULL, data_type);
    if($3 != NULL) asd_add_child($$, $3);
    if($5 != NULL) asd_add_child($$, $5);
};

fluxo_condicional: TK_SE '(' expressao ')' bloco_de_comandos TK_SENAO bloco_de_comandos{
    data_type_t data_type = validate_conditional_branches(pilha, $3->data_type, $5, $7);
    $$ = asd_new("se", NULL, data_type);
    if($3 != NULL) asd_add_child($$, $3);
    if($5 != NULL) asd_add_child($$, $5);
    if($7 != NULL) asd_add_child($$, $7);
};

/*
LAÇOS
*/

fluxo_iterativo: TK_ENQUANTO '(' expressao ')' bloco_de_comandos { 
    $$ = asd_new("enquanto", NULL, $3->data_type); 
    if($3 != NULL) asd_add_child($$, $3);
    if($5 != NULL) asd_add_child($$, $5);
};

/*
ANÁLISE DE EXPRESSÕES
*/

/*
Disjunção lógica OR '|'
*/
expr_or: expr_or '|' expr_and { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "|", $1, $3);
    $$ = asd_new("|", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_or: expr_and { $$ = $1; };

/*
Conjunção lógica AND '&'
*/
expr_and: expr_and '&' expr_eq { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "&", $1, $3);
    $$ = asd_new("&", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_and: expr_eq { $$ = $1; };

/*
Operadores de comparação ==, != 
*/
expr_eq: expr_eq TK_OC_EQ expr_rel {
    data_type_t data_type = deduce_binary_expr_type(pilha, "==", $1, $3); 
    $$ = asd_new("==", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
}
expr_eq: expr_eq TK_OC_NE expr_rel { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "!=", $1, $3); 
    $$ = asd_new("!=", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_eq: expr_rel { $$ = $1; };

/*
Comparações relacionais <, >, <=, >=
*/
expr_rel: expr_rel '<' expr_add { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "<", $1, $3);
    $$ = asd_new("<", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_rel: expr_rel '>' expr_add { 
    data_type_t data_type = deduce_binary_expr_type(pilha, ">", $1, $3);
    $$ = asd_new(">", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_rel: expr_rel TK_OC_LE expr_add { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "<=", $1, $3);
    $$ = asd_new("<=", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_rel: expr_rel TK_OC_GE expr_add { 
    data_type_t data_type = deduce_binary_expr_type(pilha, ">=", $1, $3);
    $$ = asd_new(">=", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_rel: expr_add { $$ = $1; };

/*
Operações aritméticas +, -
*/
expr_add: expr_add '+' expr_mul { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "+", $1, $3);
    $$ = asd_new("+", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_add: expr_add '-' expr_mul { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "-", $1, $3);
    $$ = asd_new("-", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_add: expr_mul { $$ = $1; };

/*
Operações multiplicativas * / %
*/
expr_mul: expr_mul '*' expr_unario { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "*", $1, $3);
    $$ = asd_new("*", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_mul: expr_mul '/' expr_unario { 
    data_type_t data_type = deduce_binary_expr_type(pilha, "/", $1, $3);
    $$ = asd_new("/", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_mul: expr_mul '%' expr_unario {
    data_type_t data_type = deduce_binary_expr_type(pilha, "%", $1, $3);
    $$ = asd_new("%", NULL, data_type);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
};
expr_mul: expr_unario { $$ = $1; };

/*
Operadores de prefixo
*/
expr_unario: '+' expr_unario { 
    $$ = asd_new("+", NULL, $2->data_type);
    asd_add_child($$, $2);
}
expr_unario: '-' expr_unario { 
    $$ = asd_new("-", NULL, $2->data_type);
    asd_add_child($$, $2);
}
expr_unario: '!' expr_unario { 
    $$ = asd_new("!", NULL, $2->data_type);
    asd_add_child($$, $2);
}
expr_unario: expr_prim { $$ = $1; };

/*
Elementos primários: variáveis, constantes, invocações, parênteses
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
Ponto de entrada para avaliação de expressões
*/
expressao: expr_or;

%%
void yyerror(const char *msg) {
    printf( "Erro de sintaxe na linha %d: %s\n", yylineno, msg);
}