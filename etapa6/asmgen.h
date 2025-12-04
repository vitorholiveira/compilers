#ifndef ASMGEN_H
#define ASMGEN_H

#include <stdio.h>
#include "asd.h"
#include "stack.h"

/* Ponto de entrada da geração de código assembly da etapa 6.
 * Recebe a AST raiz (`program_root`), a pilha de escopos `scopes`
 * (para acesso às tabelas de símbolos, em especial a global),
 * e o arquivo de saída `out` (normalmente `stdout`).
 */
void generate_assembly_program(asd_tree_t* program_root, stack_t* scopes, FILE* out);

#endif /* ASMGEN_H */


