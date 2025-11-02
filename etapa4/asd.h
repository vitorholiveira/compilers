#ifndef _ASD_H_
#define _ASD_H_
#include <stdlib.h>
#include <stdio.h>

typedef enum { INTEIRO = 0, DECIMAL = 1 } data_type_t;
typedef enum { IDENTIFIER = 0, LITERAL = 1, FUNCTION = 2 } nature_t;

typedef struct lex_value {
  int line;
  nature_t nature;
  char* value;
} lex_value_t;

typedef struct asd_tree {
  char *label;
  int number_of_children;
  struct asd_tree **children;
  lex_value_t *lex_value;
  data_type_t data_type;
} asd_tree_t;

typedef struct args {
    asd_tree_t* args;
    int num_args;
} args_t;

/*
 * asd_new function, creates a node with no children with the given label.
 */
asd_tree_t *asd_new(const char *label, lex_value_t* lex_value, data_type_t data_type);

/*
 * asd_free function, recursively frees the node and its children.
 */
void asd_free(asd_tree_t *tree);

/*
 * asd_add_child function, adds 'child' as a child of 'tree'.
 */
void asd_add_child(asd_tree_t *tree, asd_tree_t *child);

/*
 * asd_print function, recursively prints the tree.
 */
void asd_print(asd_tree_t *tree);

/*
 * asd_print_graphviz function, same as above, but in DOT format.
 */
void asd_print_graphviz (asd_tree_t *tree);

/*
 * lex_free function, frees a variable of nature lex_value_t.
 */
void lex_free(lex_value_t* lv);

#endif //_ASD_H_