%{
    #include "parser.tab.h"
    #include "asd.h"
    void create_lex_value(int type);
%}

%option noyywrap yylineno noinput nounput

    /* IGNORE */
%%
[ \t\n\r]+  // white spaces
\/\/.*      // comments

    /* KEYWORDS */
tipo                  { return TK_TIPO; }
senao                 { return TK_SENAO; }
decimal               { return TK_DECIMAL; }
se                    { return TK_SE; }
inteiro               { return TK_INTEIRO; }
retorna               { return TK_RETORNA; }
enquanto              { return TK_ENQUANTO; }
com                   { return TK_COM; }
var                   { return TK_VAR; }


    /* OPERADORES */
[!\*\/%+\-<>\[\]\(\)=,;&|] { return (int)yytext[0]; }
==                    { return TK_OC_EQ; }
"<="                  { return TK_OC_LE; }
">="                  { return TK_OC_GE; }
!=                    { return TK_OC_NE; }
:=                    { return TK_ATRIB; } 
->                    { return TK_SETA; }


    /* IDS E LITERAIS */
[_a-z][_a-z0-9]*      { create_lex_value(ID); return TK_ID; }
[0-9]+                { create_lex_value(LI); return TK_LI_INTEIRO; }
[0-9]+\.[0-9]+        { create_lex_value(LI); return TK_LI_DECIMAL; }   /* 3.14 */
\.[0-9]+              { create_lex_value(LI); return TK_LI_DECIMAL; }   /* .5 */

.                     { return TK_ER; }

%%

void create_lex_value(int type){
    yylval.valor_lexico = malloc(sizeof(lex_value_t));
    yylval.valor_lexico->line = yylineno;
    yylval.valor_lexico->type = type;
    yylval.valor_lexico->value = strdup(yytext);
}