#include <stdio.h>
#include "stack.h"
#include "iloc.h"
extern int yyparse(void);
extern int yylex_destroy(void);
asd_tree_t *arvore = NULL;
stack_t *pilha = NULL;

int main (int argc, char **argv)
{
  pilha = stack_new();
  int ret = yyparse();
  
  // Passo 9.4: Imprimir código ILOC ao invés de Graphviz
  // Conforme especificação: "Ao final da execução, o compilador deve ter na raiz da árvore AST 
  // um ponteiro para todo o código intermediário ILOC do programa de entrada."
  if (arvore != NULL && arvore->iloc_code != NULL) {
      iloc_print_code(arvore->iloc_code, stdout);
  }
  
  asd_free(arvore);
  stack_free(pilha);
  yylex_destroy();
  return ret;
}