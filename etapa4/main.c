#include <stdio.h>
#include "stack.h"
extern int yyparse(void);
extern int yylex_destroy(void);
asd_tree_t *arvore = NULL;
stack_t *pilha = NULL;

int main (int argc, char **argv)
{
  pilha = stack_new();
  int ret = yyparse();
  asd_print_graphviz(arvore);
  asd_free(arvore);
  stack_free(pilha);
  yylex_destroy();
  return ret;
}