%{
    #include "parser.tab.h"
%}

%option noyywrap
%option yylineno 

%%
[ \t\n\r]+            /* ignora espaços em branco */

\/\/.*                /* ignora comentários de linha */
[0-9]+                { return TK_LI_INTEIRO; }
[0-9]+\.[0-9]+        { return TK_LI_DECIMAL; }   /* 3.14 */
\.[0-9]+              { return TK_LI_DECIMAL; }   /* .5 */
tipo                  { return TK_TIPO; }
senao                 { return TK_SENAO; }
decimal               { return TK_DECIMAL; }
se                    { return TK_SE; }
inteiro               { return TK_INTEIRO; }
retorna               { return TK_RETORNA; }
enquanto              { return TK_ENQUANTO; }
com                   { return TK_COM; }
[!\*\/%+\-<>\[\]\(\)=,;&|] { return (int)yytext[0]; }
==                    { return TK_OC_EQ; }
"<="                  { return TK_OC_LE; }
">="                  { return TK_OC_GE; }
!=                    { return TK_OC_NE; }
:=                    { return TK_ATRIB; } 
->                    { return TK_SETA; }
var                   { return TK_VAR; }
[_a-z][_a-z0-9]*      { return TK_ID; }
.                     { return TK_ER; }
%%

int get_line_number() {
    return yylineno;
}
