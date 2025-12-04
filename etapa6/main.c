#include <stdio.h>
#include "stack.h"
#include "iloc.h"
#include "asd.h"
#include "asmgen.h"

extern int yyparse(void);
extern int yylex_destroy(void);
asd_tree_t *arvore = NULL;
stack_t *pilha = NULL;

int main (int argc, char **argv)
{
  pilha = stack_new();

  /* Garantir que os contadores de registradores/rótulos começam em 0
     no início de cada execução do compilador. */
  iloc_reset_counters();

  int ret = yyparse();

  /* A partir da etapa 6, não imprimimos mais o código ILOC.
     Em vez disso, geramos diretamente código assembly x86_64
     na saída padrão a partir da AST completa. */
  if (ret == 0 && arvore != NULL) {
      generate_assembly_program(arvore, pilha, stdout);
  }

  asd_free(arvore);
  stack_free(pilha);
  yylex_destroy();
  return ret;
}