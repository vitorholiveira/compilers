
%{
    #include "tokens.h"
%}

%option noyywrap
%option yylineno 

%%
//*
[1-9]+[0-9]* { return TK_LI_INTEIRO; }
[0-9]*.[0-9]+ { return TK_LI_DECIMAL; }
tipo { return TK_TIPO; }
[,=\]] { return yytext[0]; }
== {return TK_OC_EQ; }
<= { return TK_OC_LE; }
>= { return TK_OC_GE; }
!= { return TK_OC_NE; }
:= { return TK_ATRIB; } 
-> { return TK_SETA; }
var { return TK_VAR; }
senao { return TK_SENAO; }
decimal { return TK_DECIMAL; }
se { return TK_DECIMAL; }
inteiro { return TK_INTEIRO; }
retorna { return TK_RETORNA; }
enquanto { return TK_ENQUANTO; }
com { return TK_ENQUANTO; }
[ \n\r\t]
. { return TK_ER; }
%%

int get_line_number() {
    return yylineno;
}