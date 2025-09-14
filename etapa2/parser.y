%{
#include <stdio.h>
int yylex(void);
void yyerror (char const *mensagem);
extern int yylineno;

%}
%define parse.trace
%define parse.error verbose 
%token TK_TIPO
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
%token TK_ID
%token TK_LI_INTEIRO
%token TK_LI_DECIMAL
%token TK_ER

%%
/*
Um programa na linguagem é composto por uma
lista opcional de elementos. Os elementos da lista
são separados pelo operador vírgula e a lista é ter-
minada pelo operador ponto-e-vírgula
*/

programa: %empty;
programa: lista ';' ;
lista: elemento | lista ',' elemento;

/*
Cada elemento dessa lista é ou uma definição 
de função ou uma declaração de variável
*/

elemento: definicao_funcao | declaracao_variavel;

/*
Aqui temos a opção de tipo, algo que se repete muito
na gramática. Usamos ela quando se espera um token
inteiro ou um token decimal.
*/
opcao_tipo: TK_INTEIRO | TK_DECIMAL;

/*
Declaração variável: Consiste no token
TK_VAR seguido do token TK_ID, que é por sua
vez seguido do token TK_ATRIB e enfim seguido
do tipo. O tipo pode ser ou o token TK_DECIMAL
ou o token TK_INTEIRO.
*/

declaracao_variavel: TK_VAR TK_ID TK_ATRIB opcao_tipo;

/*
Definição de Função: Ela possui um cabeçalho e
um corpo. O cabeçalho consiste no token TK_ID
seguido do token TK_SETA seguido ou do token
TK_DECIMAL ou do token TK_INTEIRO, seguido
por uma lista opcional de parâmetros seguido do
token TK_ATRIB.
*/

definicao_funcao: TK_ID TK_SETA opcao_tipo parametros_funcao TK_ATRIB bloco_de_comandos;

/*
A lista de parâmetros, quando
presente, consiste no token opcional TK_COM se-
guido de uma lista, separada por vírgula, de parâ-
metros
*/

parametros_funcao: %empty; 
parametros_funcao: TK_COM lista_params ;
parametros_funcao: lista_params; // sem token opcional TK_COM

/*
Cada parâmetro consiste no token TK_ID
seguido do token TK_ATRIB seguido ou do to-
ken TK_INTEIRO ou do token TK_DECIMAL
*/

lista_params: param | lista_params ',' param;
param: TK_ID TK_ATRIB opcao_tipo;

// Comandos simples
/*
Os comandos simples da linguagem podem ser:
bloco de comandos, declaração de variável, 
comando de atribuição, chamada de função, 
comando de retorno, e construções de fluxo de controle.
*/

comandos_simples: bloco_de_comandos
                | declaracao_variavel_comando_simples
                | comando_atribuicao
                | chamada_funcao
                | comando_retorno
                | fluxo_condicional
                | fluxo_iterativo;

/*
Bloco de Comandos: Definido entre colchetes, e
consiste em uma sequência, possivelmente vazia,
de comandos simples. Um bloco de comandos
é considerado como um comando único simples
e pode ser utilizado em qualquer construção que
aceite um comando simples.
*/

bloco_de_comandos: '[' sequencia_comandos_simples ']';
sequencia_comandos_simples: %empty | sequencia_comandos_simples comandos_simples;

/*
Declaração de Variável: Uma variável pode ser
opcionalmente inicializada caso sua declaração
seja seguida do token TK_COM e de um literal. Um
literal pode ser ou o token TK_LI_INTEIRO ou o
token TK_LI_DECIMAL.
*/

// Outra possível inicializacao
declaracao_variavel_comando_simples: declaracao_variavel
                                   | TK_VAR TK_ID TK_ATRIB TK_INTEIRO TK_COM TK_LI_INTEIRO;
                                   | TK_VAR TK_ID TK_ATRIB TK_DECIMAL TK_COM TK_LI_DECIMAL;

/*
Comando de Atribuição: O comando de atribuição 
consiste em um token TK_ID, seguido do token 
TK_ATRIB e enfim seguido por uma expressão.
*/

comando_atribuicao: TK_ID TK_ATRIB expressao;

/* 
Chamada de Função: Uma chamada de função
consiste no token TK_ID, seguida de argumentos
entre parênteses, sendo que cada argumento é 
separado do outro por vírgula. Um argumento é
uma expressão. Uma chamada de função pode
existir sem argumentos.
*/

chamada_funcao: TK_ID '(' argumentos ')' | TK_ID '('')';
argumentos: expressao ',' argumentos | expressao;

/*
Comando de Retorno: Trata-se do token
TK_RETORNA seguido de uma expressão, se-
guido do token TK_ATRIB e terminado ou pelo
token TK_DECIMAL ou pelo token TK_INTEIRO.
*/

comando_retorno: TK_RETORNA expressao TK_ATRIB opcao_tipo; 

/*
A condicional consiste no token TK_SE seguido 
de uma expressão entre parênteses e então por 
um bloco de comandos obrigatório
*/

fluxo_condicional: TK_SE '(' expressao ')' bloco_de_comandos fluxo_condicional_senao;
fluxo_condicional_senao: %empty | TK_SENAO bloco_de_comandos

/*
Temos apenas uma construção de repetição que é o 
token TK_ENQUANTO seguido de uma expressão entre 
parênteses e de um bloco de comandos.
*/

fluxo_iterativo: TK_ENQUANTO '(' expressao ')' bloco_de_comandos;

// EXPRESSOES
/*
Expressões envolvem operandos e operadores,
sendo este opcional.
Elas também permitem o uso de parênteses para forçar
uma associatividade ou precedência diferente daquela 
tradicional.
*/

/* Nível mais alto: operador OR '|' */
expr_or: expr_or '|' expr_and | expr_and;

/* Nível AND '&' */
expr_and: expr_and '&' expr_eq | expr_eq;

/* Nível de igualdade ==, != */
expr_eq: expr_eq TK_OC_EQ expr_rel
    | expr_eq TK_OC_NE expr_rel
    | expr_rel;

/* Nível relacional <, >, <=, >= */
expr_rel: expr_rel '<' expr_add
    | expr_rel '>' expr_add
    | expr_rel TK_OC_LE expr_add
    | expr_rel TK_OC_GE expr_add
    | expr_add;

/* Nível adição e subtração binária +, - com operadores compostos */
expr_add: expr_add '+' expr_mul aux_atrib
    | expr_add '-' expr_mul aux_atrib
    | expr_mul;

/* Nível multiplicação, divisão e resto * / % com operadores compostos */
expr_mul: expr_mul '*' expr_unario aux_atrib
    | expr_mul '/' expr_unario aux_atrib
    | expr_mul '%' expr_unario aux_atrib
    | expr_unario;

/* Operadores unários prefixados */
expr_unario: '+' expr_unario
    | '-' expr_unario
    | '!' expr_unario
    | expr_prim;

/* Operandos: identificadores, literais, chamada de função, parênteses */
expr_prim: TK_ID
    | TK_LI_INTEIRO
    | TK_LI_DECIMAL
    | chamada_funcao
    | '(' expressao ')'   /* força precedência */
    ;

/* Truque para operadores compostos */
aux_atrib: %empty | TK_ATRIB;

/* Expressão principal apontando para o nível mais alto */
expressao: expr_or;

%%
void yyerror(const char *msg) {
    fprintf(stderr, "Erro de Sintaxe na Linha %d: %s\n", yylineno, msg);
}