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
 * asd_new function, creates a node with no children with the given label.
 */
asd_tree_t *asd_new(const char *label, lex_value_t* lez_value);

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
 * lex_free function, frees a variable of type lex_value_t.
 */
void lex_free(lex_value_t* lv);

#endif //_ASD_H_