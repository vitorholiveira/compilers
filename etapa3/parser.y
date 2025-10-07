%{
#include <stdio.h>
#include "asd.h"
#include <string.h>
#include <stdlib.h>
int yylex(void);
void yyerror (char const *mensagem);
extern int yylineno;
extern asd_tree_t *arvore;

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
%code requires { #include "asd.h" }

%union {
    lex_value_t* valor_lexico;
    asd_tree_t* arvore;
}

%token <valor_lexico> TK_ID
%token <valor_lexico> TK_LI_INTEIRO
%token <valor_lexico> TK_LI_DECIMAL

%type <arvore> lista elemento opcao_tipo declaracao_variavel definicao_funcao parametros_funcao lista_params
%type <arvore> comandos_simples bloco_de_comandos sequencia_comandos_simples declaracao_variavel_comando_simples
%type <arvore> literais comando_atribuicao argumentos chamada_funcao comando_retorno
%type <arvore> fluxo_condicional fluxo_iterativo
%type <arvore> expr_or expr_and expr_eq expr_rel expr_add expr_mul expr_unario expr_prim expressao

%define parse.error verbose 
%start programa
%%

/*
COMMANDS
*/

programa: %empty { arvore = NULL; };
programa: lista ';' { arvore = $1; };
lista: elemento { $$ = $1; }
    | elemento ',' lista { 
    if($1 == NULL){
        $$ = $3;
    }else {
        if($3 != NULL){
            asd_add_child($1, $3);
        }
        $$ = $1;
    }
};

elemento: definicao_funcao { $$ = $1; }
    | declaracao_variavel { $$ = $1; };

opcao_tipo: TK_INTEIRO { $$ = NULL; }
    | TK_DECIMAL { $$ = NULL; };

declaracao_variavel: TK_VAR TK_ID TK_ATRIB opcao_tipo { 
    $$ = NULL; 
    lex_free($2);
};

/*
FUNCTION DECLARATION
*/
definicao_funcao: TK_ID TK_SETA opcao_tipo parametros_funcao TK_ATRIB bloco_de_comandos {
    $$ = asd_new($1->value, $1);
    // If there are parameters, add them (if you implement this in the future)
    // if($4 != NULL) asd_add_child($$, $4);
    if($6 != NULL){ 
        asd_add_child($$, $6);
    }
};

parametros_funcao: %empty { $$ = NULL; }
    | TK_COM lista_params { $$ = NULL; }
    | lista_params { $$ = NULL; }; // without optional TK_COM token

lista_params: TK_ID TK_ATRIB opcao_tipo { 
    $$ = NULL;
    lex_free($1);
};
lista_params: lista_params ',' TK_ID TK_ATRIB opcao_tipo { 
    $$ = NULL; 
    lex_free($3);
};

/*
SIMPLE COMMANDS
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
COMMAND BLOCK
*/

bloco_de_comandos: '[' sequencia_comandos_simples ']' { $$ = $2; }
    | '[' ']' { $$ = NULL; }
    ;
sequencia_comandos_simples: comandos_simples sequencia_comandos_simples {
    if ($1 == NULL){
        $$ = $2;
    } else {
        if ($2 != NULL){
            asd_add_child($1, $2);
        }
        $$ = $1;
    }
};
sequencia_comandos_simples: comandos_simples { $$ = $1; };

/*
VARIABLE DECLARATION
*/

declaracao_variavel_comando_simples: declaracao_variavel {$$ = $1;};
declaracao_variavel_comando_simples: TK_VAR TK_ID TK_ATRIB opcao_tipo TK_COM literais {
    $$ = asd_new("com", NULL);
    asd_add_child($$, asd_new($2->value, $2));
    if($6 != NULL) {
        asd_add_child($$, $6);
    }
};

literais : TK_LI_INTEIRO {
    $$ = asd_new($1->value, $1);
}
    | TK_LI_DECIMAL {
        $$ = asd_new($1->value, $1);
    };

/*
ASSIGNMENT
*/

comando_atribuicao: TK_ID TK_ATRIB expressao {
    $$ = asd_new(":=", NULL); 
    asd_add_child($$, asd_new($1->value, $1)); 
    asd_add_child($$, $3);
};

/*
FUNCTION CALL AND RETURN
*/

chamada_funcao: TK_ID '(' argumentos ')' {
    // "call TK_ID" string
    int len = strlen("call ") + strlen($1->value) + 1;
    char *buffer = malloc(len);
    snprintf(buffer, len, "call %s", $1->value);

    $$ = asd_new(buffer, $1);
    free(buffer);

    if($3 != NULL) {
        asd_add_child($$, $3);
    }
};
chamada_funcao: TK_ID '(' ')' {
    // "call TK_ID" string
    int len = strlen("call ") + strlen($1->value) + 1;
    char *buffer = malloc(len);
    snprintf(buffer, len, "call %s", $1->value);

    $$ = asd_new(buffer, $1);
    free(buffer);
}; 

argumentos: expressao ',' argumentos { 
    if($1 != NULL && $3 != NULL) {
        asd_add_child($1, $3);
    }
    $$ = $1; 
}
    | expressao { $$ = $1; };


comando_retorno: TK_RETORNA expressao TK_ATRIB opcao_tipo { 
    $$ = asd_new("retorna", NULL); 
    if($2 != NULL) {
        asd_add_child($$, $2);
    }
}; 

/*
IF ELSE
*/

fluxo_condicional: TK_SE '(' expressao ')' bloco_de_comandos {
    $$ = asd_new("se", NULL);
    if($3 != NULL) {
        asd_add_child($$, $3);
    }
    if($5 != NULL){
        asd_add_child($$, $5);
    }
};

fluxo_condicional: TK_SE '(' expressao ')' bloco_de_comandos TK_SENAO bloco_de_comandos{
    $$ = asd_new("se", NULL);
    if($3 != NULL) {
        asd_add_child($$, $3);
    }
    if($5 != NULL){
        asd_add_child($$, $5);
    }
    if($7 != NULL) {
        asd_add_child($$, $7);
    }
};

/*
WHILE
*/

fluxo_iterativo: TK_ENQUANTO '(' expressao ')' bloco_de_comandos { 
    $$ = asd_new("enquanto", NULL); 
    if($3 != NULL) {
        asd_add_child($$, $3);
    }
    if($5 != NULL){
        asd_add_child($$, $5);
    }
};

/*
EXPRESSIONS
*/

/* Highest level: OR operator '|' */
expr_or: expr_or '|' expr_and { 
    $$ = asd_new("|", NULL);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
}
    | expr_and { $$ = $1; };

/* AND level '&' */
expr_and: expr_and '&' expr_eq { 
    $$ = asd_new("&", NULL);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
}
    | expr_eq { $$ = $1; };

/* Equality level ==, != */
expr_eq: expr_eq TK_OC_EQ expr_rel { 
    $$ = asd_new("==", NULL);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
}
    | expr_eq TK_OC_NE expr_rel { 
        $$ = asd_new("!=", NULL);
        if($1 != NULL) asd_add_child($$, $1);
        if($3 != NULL) asd_add_child($$, $3);
    }
    | expr_rel { $$ = $1; };

/* Relational level <, >, <=, >= */
expr_rel: expr_rel '<' expr_add { 
    $$ = asd_new("<", NULL);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
}
    | expr_rel '>' expr_add { 
        $$ = asd_new(">", NULL);
        if($1 != NULL) asd_add_child($$, $1);
        if($3 != NULL) asd_add_child($$, $3);
    }
    | expr_rel TK_OC_LE expr_add { 
        $$ = asd_new("<=", NULL);
        if($1 != NULL) asd_add_child($$, $1);
        if($3 != NULL) asd_add_child($$, $3);
    }
    | expr_rel TK_OC_GE expr_add { 
        $$ = asd_new(">=", NULL);
        if($1 != NULL) asd_add_child($$, $1);
        if($3 != NULL) asd_add_child($$, $3);
    }
    | expr_add { $$ = $1; };

/* Binary addition and subtraction level +, - */
expr_add: expr_add '+' expr_mul { 
    $$ = asd_new("+", NULL);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
}
    | expr_add '-' expr_mul { 
        $$ = asd_new("-", NULL);
        if($1 != NULL) asd_add_child($$, $1);
        if($3 != NULL) asd_add_child($$, $3);
    }
    | expr_mul { $$ = $1; };

/* Multiplication, division, and remainder level * / % */
expr_mul: expr_mul '*' expr_unario { 
    $$ = asd_new("*", NULL);
    if($1 != NULL) asd_add_child($$, $1);
    if($3 != NULL) asd_add_child($$, $3);
}
    | expr_mul '/' expr_unario { 
        $$ = asd_new("/", NULL);
        if($1 != NULL) asd_add_child($$, $1);
        if($3 != NULL) asd_add_child($$, $3);
    }
    | expr_mul '%' expr_unario { 
        $$ = asd_new("%", NULL);
        if($1 != NULL) asd_add_child($$, $1);
        if($3 != NULL) asd_add_child($$, $3);
    }
    | expr_unario { $$ = $1; };

/* Prefixed unary operators */
expr_unario: '+' expr_unario { 
    $$ = asd_new("+", NULL);
    if($2 != NULL) asd_add_child($$, $2);
}
    | '-' expr_unario { 
        $$ = asd_new("-", NULL);
        if($2 != NULL) asd_add_child($$, $2);
    }
    | '!' expr_unario { 
        $$ = asd_new("!", NULL);
        if($2 != NULL) asd_add_child($$, $2);
    }
    | expr_prim { $$ = $1; };

/* Operands: identifiers, literals, function call, parentheses */
expr_prim: TK_ID { $$ = asd_new($1->value, $1); }
    | TK_LI_INTEIRO { $$ = asd_new($1->value, $1); }
    | TK_LI_DECIMAL { $$ = asd_new($1->value, $1); }
    | chamada_funcao { $$ = $1; }
    | '(' expressao ')' { $$ = $2; }
    ;


/* Main expression pointing to the highest level */
expressao: expr_or;

%%
void yyerror(const char *msg) {
    fprintf(stderr, "Syntax Error on Line %d: %s\n", yylineno, msg);
}