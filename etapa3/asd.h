#ifndef _ASD_H_
#define _ASD_H_


#define ID 1
#define LI 2

typedef struct lex_value {
  int line;
  int type;
  char* value;
} lex_value_t;

typedef struct asd_tree {
  char *label;
  int number_of_children;
  struct asd_tree **children;
  lex_value_t *lex_value;
} asd_tree_t;

/*
 * Função asd_new, cria um nó sem filhos com o label informado.
 */
asd_tree_t *asd_new(const char *label, lex_value_t* lez_value);

/*
 * Função asd_tree, libera recursivamente o nó e seus filhos.
 */
void asd_free(asd_tree_t *tree);

/*
 * Função asd_add_child, adiciona child como filho de tree.
 */
void asd_add_child(asd_tree_t *tree, asd_tree_t *child);

/*
 * Função asd_print, imprime recursivamente a árvore.
 */
void asd_print(asd_tree_t *tree);

/*
 * Função asd_print_graphviz, idem, em formato DOT
 */
void asd_print_graphviz (asd_tree_t *tree);

#endif //_ASD_H_