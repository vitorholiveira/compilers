#include <stdio.h>
#include "stack.h"
#include "iloc.h"
#include "asd.h"

extern int yyparse(void);
extern int yylex_destroy(void);
asd_tree_t *arvore = NULL;
stack_t *pilha = NULL;

/* Retorna 1 se o rótulo corresponde a um nó interno (expressão/comando),
   cujo iloc_code não deve ser concatenado no código final do programa. */
static int is_internal_label(const char* label) {
    if (!label) return 0;

    /* Operadores aritméticos e lógicos */
    const char* internal_ops[] = {
        ":=", "se", "enquanto", "retorna", "seq", "bloco_vazio",
        "+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=",
        "&", "&&", "|", "||", "!", NULL
    };

    for (int i = 0; internal_ops[i] != NULL; i++) {
        if (strcmp(label, internal_ops[i]) == 0) {
            return 1;
        }
    }

    /* Nós de chamada de função ("call f") também são internos */
    if (strncmp(label, "call ", 5) == 0) {
        return 1;
    }

    return 0;
}

/* Percorre a AST e concatena apenas os códigos ILOC de nós "de topo"
   (tipicamente definições de função). Nós internos de expressão/comando
   são ignorados aqui, pois seu código já foi incorporado ao corpo
   das funções correspondentes. */
static void collect_iloc(asd_tree_t* node, iloc_code_t* acc) {
    if (!node || !acc) return;

    if (node->iloc_code && node->iloc_code->count > 0 && !is_internal_label(node->label)) {
        iloc_code_concat(acc, node->iloc_code);
    }

    for (int i = 0; i < node->number_of_children; i++) {
        collect_iloc(node->children[i], acc);
    }
}

int main (int argc, char **argv)
{
  pilha = stack_new();

  /* Garantir que os contadores de registradores/rótulos começam em 0
     no início de cada execução do compilador. */
  iloc_reset_counters();

  int ret = yyparse();
  
  /* Em vez de depender apenas de arvore->iloc_code, coletamos o código
     de todos os nós relevantes (ex.: múltiplas funções no mesmo programa). */
  if (arvore != NULL) {
      iloc_code_t* program_code = iloc_code_new();
      if (program_code) {
          collect_iloc(arvore, program_code);
          iloc_print_code(program_code, stdout);
          iloc_code_free(program_code);
      }
  }
  
  asd_free(arvore);
  stack_free(pilha);
  yylex_destroy();
  return ret;
}