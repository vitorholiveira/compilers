
%{
    #include "tokens.h"
%}

%option noyywrap
%option yylineno 

%%

tipo { return TK_TIPO; }
[,=\]] { return yytext[0]; }
== { return TK_ER; }
[ \n\r\t]

%%

int get_line_number() {
    return yylineno;
}